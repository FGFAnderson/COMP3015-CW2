#pragma once
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

// AI USAGE: refer to ai_transcript/physics_world_questions.md

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void loadMap(const char* path, float scale);
    void step(float dt);
    bool rayTest(glm::vec3 from, glm::vec3 to) const;
    bool inMapBounds(glm::vec3 pos) const;
    void setWalkDirection(glm::vec2 dir);
    void jump();
    glm::vec3 getPlayerPosition() const;
    void warpPlayer(glm::vec3 pos);

private:
    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btDbvtBroadphase* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;

    btCapsuleShape* capsule;
    btPairCachingGhostObject* ghost;
    btKinematicCharacterController* controller;

    btTriangleMesh* mapTriMesh = nullptr;
    btBvhTriangleMeshShape* mapShape = nullptr;
    btRigidBody* mapBody = nullptr;
    btGhostPairCallback* ghostPairCb = nullptr;

    btVector3 mapAabbMin{0,0,0};
    btVector3 mapAabbMax{0,0,0};
};
