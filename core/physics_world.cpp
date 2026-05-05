#include "physics_world.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <string>

static constexpr float CAPSULE_RADIUS = 1.5f;
static constexpr float CAPSULE_HEIGHT = 12.0f;
static constexpr float GRAVITY = 20.0f;
static constexpr float STEP_HEIGHT = 3.0f;
static constexpr float EYE_OFFSET = 10.0f;

PhysicsWorld::PhysicsWorld() {
    collisionConfig = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfig);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    world->setGravity(btVector3(0, -GRAVITY, 0));

    ghostPairCb = new btGhostPairCallback();
    broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCb);

    capsule = new btCapsuleShape(CAPSULE_RADIUS, CAPSULE_HEIGHT);
    ghost = new btPairCachingGhostObject();

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 5, 0));
    ghost->setWorldTransform(t);
    ghost->setCollisionShape(capsule);
    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    controller = new btKinematicCharacterController(ghost, capsule, STEP_HEIGHT);
    controller->setGravity(btVector3(0, -GRAVITY, 0));
    controller->setMaxSlope(btRadians(50.0f));

    world->addCollisionObject(ghost,
        btBroadphaseProxy::CharacterFilter,
        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
    world->addAction(controller);
}

void PhysicsWorld::loadMap(const char* path, float scale) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_JoinIdenticalVertices);

    if (!scene)
        throw std::runtime_error(std::string("PhysicsWorld: failed to load map '") + path + "'");

    mapTriMesh = new btTriangleMesh();

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        const aiMesh* mesh = scene->mMeshes[m];
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices != 3) continue;

            auto vert = [&](unsigned int i) {
                const aiVector3D& v = mesh->mVertices[face.mIndices[i]];
                return btVector3(v.x * scale, v.y * scale, v.z * scale);
            };
            btVector3 v0 = vert(0), v1 = vert(1), v2 = vert(2);
            // Skip degenerate triangles
            // AI USAGE: refer to bulletx_collision_crashing.md
            if ((v1 - v0).cross(v2 - v0).length2() < 0.01f) continue;
            mapTriMesh->addTriangle(v0, v1, v2);
        }
    }

    mapShape = new btBvhTriangleMeshShape(mapTriMesh, false);
    mapShape->getAabb(btTransform::getIdentity(), mapAabbMin, mapAabbMax);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, nullptr, mapShape);
    mapBody = new btRigidBody(rbInfo);
    world->addRigidBody(mapBody, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::AllFilter);
}

bool PhysicsWorld::inMapBounds(glm::vec3 pos) const {
    glm::vec3 mn(mapAabbMin.x(), mapAabbMin.y(), mapAabbMin.z());
    glm::vec3 mx(mapAabbMax.x(), mapAabbMax.y(), mapAabbMax.z());
    return glm::all(glm::greaterThanEqual(pos, mn)) && glm::all(glm::lessThanEqual(pos, mx));
}

bool PhysicsWorld::rayTest(glm::vec3 from, glm::vec3 to) const {
    btVector3 btFrom(from.x, from.y, from.z);
    btVector3 btTo(to.x, to.y, to.z);

    // Bullet's BVH tree crashes when either endpoint is outside the mesh AABB
    if (!inMapBounds(from) || !inMapBounds(to))
        return false;

    btCollisionWorld::ClosestRayResultCallback cb(btFrom, btTo);
    cb.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
    cb.m_collisionFilterMask = btBroadphaseProxy::StaticFilter;
    world->rayTest(btFrom, btTo, cb);
    return cb.hasHit();
}

void PhysicsWorld::step(float dt) {
    world->stepSimulation(dt, 10, 1.0f / 120.0f);
}

void PhysicsWorld::setWalkDirection(glm::vec2 dir) {
    controller->setWalkDirection(btVector3(dir.x, 0.0f, dir.y));
}

void PhysicsWorld::jump() {
    if (controller->canJump())
        controller->jump(btVector3(0, 8.0f, 0));
}

glm::vec3 PhysicsWorld::getPlayerPosition() const {
    btVector3 o = ghost->getWorldTransform().getOrigin();
    return glm::vec3(o.x(), o.y() + EYE_OFFSET, o.z());
}

void PhysicsWorld::warpPlayer(glm::vec3 pos) {
    controller->warp(btVector3(pos.x, pos.y - EYE_OFFSET, pos.z));
}


PhysicsWorld::~PhysicsWorld() {
    world->removeAction(controller);
    world->removeCollisionObject(ghost);
    if (mapBody) world->removeRigidBody(mapBody);

    delete controller;
    delete ghost;
    delete capsule;
    delete mapBody;
    delete mapShape;
    delete mapTriMesh;

    delete world;
    delete solver;
    delete broadphase;
    delete dispatcher;
    delete collisionConfig;
    delete ghostPairCb;
}
