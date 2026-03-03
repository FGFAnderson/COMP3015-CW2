#!/usr/bin/env python3
"""Split tree OBJ files by material group into separate OBJ files."""
import os

def split_obj_by_material(input_path, output_dir):
    with open(input_path, 'r') as f:
        lines = f.readlines()

    all_verts, all_uvs, all_norms = [], [], []
    groups = {}
    current_material = None

    for line in lines:
        line = line.strip()
        if line.startswith('v '):
            all_verts.append(tuple(line.split()[1:4]))
        elif line.startswith('vt '):
            all_uvs.append(tuple(line.split()[1:3]))
        elif line.startswith('vn '):
            all_norms.append(tuple(line.split()[1:4]))
        elif line.startswith('usemtl '):
            current_material = line.split()[1]
            if current_material not in groups:
                groups[current_material] = []
        elif line.startswith('f ') and current_material is not None:
            parts = line.split()[1:]
            face = []
            for p in parts:
                idx = p.split('/')
                v  = int(idx[0])
                vt = int(idx[1]) if len(idx) > 1 and idx[1] else None
                vn = int(idx[2]) if len(idx) > 2 and idx[2] else None
                face.append((v, vt, vn))
            groups[current_material].append(face)

    base_name = os.path.splitext(os.path.basename(input_path))[0]
    written = []

    for material, faces in groups.items():
        if not faces:
            continue

        used_v  = sorted({v  for face in faces for (v, vt, vn) in face})
        used_vt = sorted({vt for face in faces for (v, vt, vn) in face if vt})
        used_vn = sorted({vn for face in faces for (v, vt, vn) in face if vn})

        v_map  = {old: new for new, old in enumerate(used_v,  1)}
        vt_map = {old: new for new, old in enumerate(used_vt, 1)}
        vn_map = {old: new for new, old in enumerate(used_vn, 1)}

        out_path = os.path.join(output_dir, f"{base_name}_{material}.obj")
        with open(out_path, 'w') as out:
            out.write(f"# Split from {base_name}.obj - material: {material}\n")
            for i in used_v:
                out.write(f"v {' '.join(all_verts[i-1])}\n")
            for i in used_vt:
                out.write(f"vt {' '.join(all_uvs[i-1])}\n")
            for i in used_vn:
                out.write(f"vn {' '.join(all_norms[i-1])}\n")
            out.write(f"usemtl {material}\n")
            for face in faces:
                parts = []
                for (v, vt, vn) in face:
                    nv = v_map[v]
                    if vt and vn:
                        parts.append(f"{nv}/{vt_map[vt]}/{vn_map[vn]}")
                    elif vt:
                        parts.append(f"{nv}/{vt_map[vt]}")
                    elif vn:
                        parts.append(f"{nv}//{vn_map[vn]}")
                    else:
                        parts.append(str(nv))
                out.write("f " + " ".join(parts) + "\n")
        written.append(out_path)
        print(f"  -> {out_path} ({len(faces)} faces)")

    return written

models_dir = "./media/models"
trees = [
    ("CommonTree_1", "Bark_NormalTree", "Leaves_NormalTree"),
    ("CommonTree_2", "Bark_NormalTree", "Leaves_NormalTree"),
    ("CommonTree_3", "Bark_NormalTree", "Leaves_NormalTree"),
    ("CommonTree_4", "Bark_NormalTree", "Leaves_NormalTree"),
    ("Pine_1",       "Bark_NormalTree", "Leaves_Pine"),
    ("Pine_2",       "Bark_NormalTree", "Leaves_Pine"),
    ("TwistedTree_1","Bark_TwistedTree","Leaves_TwistedTree"),
    ("TwistedTree_2","Bark_TwistedTree","Leaves_TwistedTree"),
]

for (model, bark_mat, leaf_mat) in trees:
    path = os.path.join(models_dir, f"{model}.obj")
    if os.path.exists(path):
        print(f"Splitting {model}...")
        split_obj_by_material(path, models_dir)
    else:
        print(f"NOT FOUND: {path}")
