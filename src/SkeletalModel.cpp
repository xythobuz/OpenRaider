/*!
 * \file src/SkeletalModel.cpp
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#include <assert.h>

#include "main.h"
#include "SkeletalModel.h"

BoneTag::BoneTag(TombRaider &tr, unsigned int index, int j, unsigned int *l) {
    tr2_moveable_t *moveable = mTombRaider.Moveable();
    tr2_meshtree_t *meshtree = mTombRaider.MeshTree();

    off[0] = 0.0;
    off[1] = 0.0;
    off[2] = 0.0;
    flag = 0x00;
    rot[0] = 0.0;
    rot[1] = 0.0;
    rot[2] = 0.0;
    mesh = moveable[index].starting_mesh + j;

    // Setup offsets to produce skeleton
    if (j == 0) {
        // Always push tag[0], this isn't really used either
        tag->flag = 0x02;
    } else { // Nonprimary tag - position relative to first tag

        // Hack: moveable[index].mesh_tree is a byte offset
        //       into mesh_tree[], so we have to convert to index
        int *tree = (int *)(void *)meshtree;
        tr2_meshtree_t *mesh_tree = (tr2_meshtree_t *)(tree
                + moveable[index].mesh_tree + ((j - 1) * 4));

        off[0] = mesh_tree->x;
        off[1] = mesh_tree->y;
        off[2] = mesh_tree->z;
        flag = (char)mesh_tree->flags;
    }

    // Setup tag rotations
    tr.computeRotationAngles(&frame, &frame_offset, l, rot, rot+1, rot+2);
}

BoneFrame::BoneFrame(TombRaider &tr, unsigned int index, unsigned int i, unsigned int frame_offset) {
    tr2_moveable_t *moveable = mTombRaider.Moveable();
    unsigned short *frame = mTombRaider.Frame();

    pos[0] = (short)frame[frame_offset + 6];
    pos[1] = (short)frame[frame_offset + 7];
    pos[2] = (short)frame[frame_offset + 8];

    yaw = ((item[i].angle >> 14) & 0x03);
    yaw *= 90;

    unsigned int l = 9; // First angle offset in this Frame

    // Run through the tag and calculate the rotation and offset
    for (int j = 0; j < (int)moveable[index].num_meshes; ++j)
        tag.push_back(*new BoneTag(tr, index, j, &l));
}

BoneFrame::~BoneFrame() {
    for (unsigned int i = 0; i < tag.size(); i++)
        delete tag[i];
}

unsigned int BoneFrame::size() {
    return tag.size();
}

BoneTag &BoneFrame::get(unsigned int i) {
    assert(i < tag.size());
    return *tag.at(i);
}

void BoneFrame::add(BoneTag &b) {
    tag.push_back(&b);
}

AnimationFrame::AnimationFrame(TombRaider &tr, unsigned int index, unsigned int i, int a) {
    tr2_moveable_t *moveable = mTombRaider.Moveable();
    tr2_animation_t *animation = mTombRaider.Animation();

    unsigned int frame_offset = animation[a].frame_offset / 2;
    int frame_step = animation[a].frame_size;
    unsigned int frame_count = (animation[a].frame_end - animation[a].frame_start) + 1;
    rate = animation[a].frame_rate;

    for (unsigned int f = 0; f < frame_count; f++, frame_offset += frame_step) {
        // HACK: Lara's ObjectID is 315, but her meshes start at 0, so make a
        // quick substitution (so she doesn't appear as a bunch of thighs)
        if ((index == 0) && (tr.Engine() == TR_VERSION_3)) {
            for (int j = 0; (j < (int)tr.NumMoveables()) && (index == 0); j++) {
                if (moveable[j].object_id == 315)
                    index = j;
            }
        }

        // Fix Lara in TR4
        // Body is ItemID 8, joints are ItemID 9
        // (TR4 demo: body is ItemID 10, joints are ItemID 11)
        if ((index == 0) && (tr.Engine() == TR_VERSION_4)) {
            for (int j = 0; (j < (int)tr.NumMoveables()) && (index == 0); j++) {
                if (moveable[j].object_id == 8)
                    index = j;
            }
        } else if ((moveable[index].object_id == 8) && (tr.Engine() == TR_VERSION_4)) {
            // KLUDGE to do "skinning"
            index = 0;
            for (int j = 0; (j < (int)tr.NumMoveables()) && (index == 0); j++) {
                if (moveable[j].object_id == 9)
                    index = j;
            }
        }

        if (frame_offset > tr.NumFrames()) {
            getConsole().print("WARNING: Bad animation frame %i > %i",
                    frame_offset, tr.NumFrames());
            return;
        }

        frame.push_back(*new BoneFrame(tr, index, i, frame_offset));
    }
}

AnimationFrame::~AnimationFrame() {
    for (unsigned int i = 0; i < frame.size(); i++)
        delete frame[i];
}

unsigned int AnimationFrame::size() {
    return frame.size();
}

BoneFrame &AnimationFrame::get(unsigned int i) {
    assert(i < frame.size());
    return *frame.at(i);
}

void AnimationFrame::add(BoneFrame &b) {
    frame.push_back(&b);
}

SkeletalModel::SkeletalModel(TombRaider &tr, unsigned int index, unsigned int i) {
    tr2_moveable_t *moveable = mTombRaider.Moveable();
    tr2_animation_t *anim = mTombRaider.Animation();

    id = moveable[index].object_id;

    // Gather more info if this is lara
    if (id == 0) {
        // Only TR4 lara has 2 layer bone tags/meshes per bone frame
        tr4Overlay = (mTombRaider.Engine() == TR_VERSION_4);
        ponytailId = 0;
    } else {
        tr4Overlay = false;
        ponytailId = -1;
    }

    switch (mTombRaider.Engine()) {
        case TR_VERSION_4:
            if (moveable[index].object_id == 30) {
                ponytailId = getWorld().sizeSkeletalModel(); //! \fixme Why is this even needed?
                ponytailMeshId = moveable[index].starting_mesh;
                ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                        moveable[index].num_meshes : 0);
                ponytailAngle = -90.0f;
                ponytail[0] = -3;
                ponytail[1] = -22;
                ponytail[2] = -20;
                ponyOff = 40;
                ponyOff2 = 32;
                pigtails = false;

                // Try to guess pigtails by looking for certian num verts in head
                if (mesh[moveable[0].starting_mesh].num_vertices > 80) {
                    pigtails = true;
                    ponyOff -= 20;
                    ponytail[1] -= 32;
                }

                getRender().setFlags(Render::fRenderPonytail);
                getConsole().print("Found known ponytail");
            }
            break;

        case TR_VERSION_1:
        case TR_VERSION_2:
        case TR_VERSION_3:
        case TR_VERSION_5:
        case TR_VERSION_UNKNOWN:
            if (moveable[index].object_id == 2) {
                ponytailId = k;
                ponytailMeshId = moveable[index].starting_mesh;
                ponytailNumMeshes = ((moveable[index].num_meshes > 0) ?
                        moveable[index].num_meshes : 0);
                ponytailAngle = -90.0f;
                ponytail[0] = 0;
                ponytail[1] = -20;
                ponytail[2] = -20;
                ponyOff = 40;
                ponyOff2 = 0;

                getRender().setFlags(Render::fRenderPonytail);
                getConsole().print("Found ponytail?");
            }
            break;
    }

    // Animations
    int a = moveable[index].animation;
    unsigned int frame_offset = anim[a].frame_offset / 2;
    int frame_step = anim[a].frame_size;

    if (a >= (int)tr.NumAnimations())
        a = tr.NumFrames() - frame_offset; //! \fixme Couldn't a be already used out of range?!
    else
        a = (anim[a].frame_offset / 2) - frame_offset;

    if (frame_step != 0) // prevent divide-by-zero errors
        a /= frame_step;

    if (a < 0) {
        getConsole().print("Invalid animation data for model %d. Skip!", index);
        return;
    } else {
        for (; a < tr.getNumAnimsForMoveable(index); a++) {
            animation.push_back(new AnimationFrame(tr, index, i, a))
        }
    }
}

SkeletalModel::~SkeletalModel() {
    for (unsigned int i = 0; i < animation.size(); i++)
        delete animation[i];
}

int SkeletalModel::getId() {
    return id;
}

unsigned int SkeletalModel::size() {
    return animation.size();
}

AnimationFrame &SkeletalModel::get(unsigned int i) {
    assert(i < animation.size());
    return *animation.at(i);
}

