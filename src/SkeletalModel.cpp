/*!
 * \file src/SkeletalModel.cpp
 * \brief This is the factored out skeletal model class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif defined WIN32
#include <gl/glew.h>
#include <gl/wglew.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <assert.h>

#include "main.h"
#include "SkeletalModel.h"

BoneTag::BoneTag(TombRaider &tr, unsigned int index, int j, unsigned int *l, unsigned short **frame, unsigned int *frame_offset) {
    tr2_moveable_t *moveable = tr.Moveable();
    tr2_meshtree_t *meshtree = tr.MeshTree();

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
        flag = 0x02;
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
    tr.computeRotationAngles(frame, frame_offset, l, rot, rot+1, rot+2);
}

void BoneTag::display() {
    getRender().drawModelMesh(getWorld().getMesh(mesh));
}

void BoneTag::getOffset(vec3_t o) {
    o[0] = off[0];
    o[1] = off[1];
    o[2] = off[2];
}

void BoneTag::getRotation(vec3_t r) {
    r[0] = rot[0];
    r[1] = rot[1];
    r[2] = rot[2];
}

char BoneTag::getFlag() {
    return flag;
}

BoneFrame::BoneFrame(TombRaider &tr, unsigned int index, unsigned int i, unsigned short **frame, unsigned int *frame_offset) {
    tr2_moveable_t *moveable = tr.Moveable();
    tr2_item_t *item = tr.Item();

    pos[0] = (short)*frame[*frame_offset + 6];
    pos[1] = (short)*frame[*frame_offset + 7];
    pos[2] = (short)*frame[*frame_offset + 8];

    yaw = ((item[i].angle >> 14) & 0x03);
    yaw *= 90;

    unsigned int l = 9; // First angle offset in this Frame

    // Run through the tag and calculate the rotation and offset
    for (int j = 0; j < (int)moveable[index].num_meshes; ++j)
        tag.push_back(new BoneTag(tr, index, j, &l, frame, frame_offset));
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

void BoneFrame::getPosition(vec3_t p) {
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
}

AnimationFrame::AnimationFrame(TombRaider &tr, unsigned int index, unsigned int i, int a) {
    tr2_moveable_t *moveable = tr.Moveable();
    tr2_animation_t *animation = tr.Animation();
    unsigned short *fr = tr.Frame();

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

        frame.push_back(new BoneFrame(tr, index, i, &fr, &frame_offset));
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

SkeletalModel::SkeletalModel(TombRaider &tr, unsigned int index, unsigned int i, int objectId) {
    tr2_moveable_t *moveable = tr.Moveable();
    tr2_animation_t *anim = tr.Animation();
    tr2_mesh_t *mesh = tr.Mesh();

    id = objectId;

    // Gather more info if this is lara
    if (id == 0) {
        // Only TR4 lara has 2 layer bone tags/meshes per bone frame
        tr4Overlay = (tr.Engine() == TR_VERSION_4);
        ponytailId = 0;
    } else {
        tr4Overlay = false;
        ponytailId = -1;
    }

    switch (tr.Engine()) {
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
                ponytailId = getWorld().sizeSkeletalModel(); //! \fixme Why is this even needed?
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
            animation.push_back(new AnimationFrame(tr, index, i, a));
        }
    }
}

SkeletalModel::~SkeletalModel() {
    for (unsigned int i = 0; i < animation.size(); i++)
        delete animation[i];
}

void SkeletalModel::display(unsigned int aframe, unsigned int bframe) {
    assert(aframe < size());
    assert(bframe < get(aframe).size());

    AnimationFrame &anim = get(aframe);
    BoneFrame &boneframe = anim.get(bframe);

    if (boneframe.size() == 0) {
        printf("Empty bone frame?!?!\n");
        return;
    }

    vec3_t pos;
    boneframe.getPosition(pos);
    glTranslatef(pos[0], pos[1], pos[2]);

    for (unsigned int a = 0; a < boneframe.size(); a++) {
        BoneTag &tag = boneframe.get(a);
        vec3_t rot, off;
        tag.getRotation(rot);
        tag.getOffset(off);

        if (a == 0) {
            if (!equalEpsilon(rot[1], 0.0f))
                glRotatef(rot[1], 0, 1, 0);

            if (!equalEpsilon(rot[0], 0.0f))
                glRotatef(rot[0], 1, 0, 0);

            if (!equalEpsilon(rot[2], 0.0f))
                glRotatef(rot[2], 0, 0, 1);
        } else {
            if (tag.getFlag() & 0x01)
                glPopMatrix();

            if (tag.getFlag() & 0x02)
                glPushMatrix();

            glTranslatef(off[0], off[1], off[2]);

            if (!equalEpsilon(rot[1], 0.0f))
                glRotatef(rot[1], 0, 1, 0);

            if (!equalEpsilon(rot[0], 0.0f))
                glRotatef(rot[0], 1, 0, 0);

            if (!equalEpsilon(rot[2], 0.0f))
                glRotatef(rot[2], 0, 0, 1);
        }

        // Draw layered lara in TR4 (2 meshes per tag)
        if (tr4Overlay) {
            BoneFrame &boneframe2 = get(0).get(0); //! \fixme Woot?
            if (a < boneframe2.size())
                boneframe2.get(a).display();
        }

        if (getRender().getFlags() & Render::fRenderPonytail) {
            if ((ponytailId > 0) && (a == 14)) {
                glPushMatrix();

                // Mongoose 2002.08.30, TEST to align offset
                glTranslatef(ponytail[0], ponytail[1], ponytail[2]);
                glRotatef(ponytailAngle, 1, 0, 0);

                // HACK: To fill TR4 void between ponytail/head
                //   since no vertex welds are implemented yet
                if (tr4Overlay)
                    glScalef(1.20f, 1.20f, 1.20f);

#ifdef EXPERIMENTAL_NON_ITEM_RENDER
                getWorld().getSkeletalModel(ponytail).display(0, 0);
#else
                for (unsigned int i = 0; i < ponytailNumMeshes; i++) {
                    glPushMatrix();

                    if (i > 0) {
                        glRotatef(randomNum(-8.0f, -10.0f), 1, 0, 0);
                        glRotatef(randomNum(-5.0f, 5.0f), 0, 1, 0);
                        glRotatef(randomNum(-5.0f, 5.0f), 0, 0, 1);
                        glTranslatef(0.0, 0.0, ponyOff);
                    }

                    if (pigtails) {
                        glPushMatrix();
                        glTranslatef(ponyOff2, 0.0, 0.0);
                        getRender().drawModelMesh(getWorld().getMesh(ponytailMeshId + i));
                        glPopMatrix();

                        glPushMatrix();
                        glTranslatef(-ponyOff2, 0.0, 0.0);
                        getRender().drawModelMesh(getWorld().getMesh(ponytailMeshId + i));
                        glPopMatrix();
                    } else {
                        getRender().drawModelMesh(getWorld().getMesh(ponytailMeshId + i));
                    }
                }

                for (unsigned int i = 0; i < ponytailNumMeshes; i++)
                    glPopMatrix();
#endif
                glPopMatrix();
            }
        }

        tag.display();
    }
}

int SkeletalModel::getId() {
    return id;
}

void SkeletalModel::setPigTail(bool b) {
    pigtails = b;
    if (b) {
        ponyOff -= 20;
        ponytail[1] -= 32;
    } else {
        ponyOff += 20;
        ponytail[1] += 32;
    }
}

void SkeletalModel::setPonyPos(vec_t x, vec_t y, vec_t z, vec_t angle) {
    ponytail[0] = x;
    ponytail[1] = y;
    ponytail[2] = z;
    ponytailAngle = angle;
}

unsigned int SkeletalModel::size() {
    return animation.size();
}

AnimationFrame &SkeletalModel::get(unsigned int i) {
    assert(i < animation.size());
    return *animation.at(i);
}

