/*!
 * \file src/Emitter.cpp
 * \brief Particle emitter class.
 *
 * \author Mongoose
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "Emitter.h"

vec_t Emitter::mFrustum[6][4];

int Emitter::compareParticleDist(const void *voidA, const void *voidB) {
    Particle *a = const_cast<Particle *>(static_cast<const Particle *>(voidA));
    Particle *b = const_cast<Particle *>(static_cast<const Particle *>(voidB));
    float x, y, z, distA, distB;

    if (!a || !b)
        return -1; // error really

    a->Pos(&x, &y, &z);
    distA = (Emitter::mFrustum[5][0] * x +
            Emitter::mFrustum[5][1] * y +
            Emitter::mFrustum[5][2] * z +
            Emitter::mFrustum[5][3]);

    b->Pos(&x, &y, &z);
    distB = (Emitter::mFrustum[5][0] * x +
            Emitter::mFrustum[5][1] * y +
            Emitter::mFrustum[5][2] * z +
            Emitter::mFrustum[5][3]);

    // reverse less/greater than
    // less than
    if (distA > distB)
        return -1;

    // greater than ( no need for equal )
    return 1;
}

Emitter::Emitter(const char *name, int n) {
    _name = NULL;
    _flags = 0;
    _particle = NULL;
    _count = 0;
    _pos[0] = _pos[1] = _pos[2] = 0.0;
    _mangle[0] = _mangle[1] = _mangle[2] = 0.0;

    Name(name);
    ParticleArray(n);
}

Emitter::~Emitter() {
    if (_name)
        delete [] _name;

    if (_particle)
        delete [] _particle;

    _count = 0;
}

Particle *Emitter::Particles() {
    return _particle;
}


int Emitter::Count() {
    return _count;
}

void Emitter::Pos(float *x, float *y, float *z) {
    *x = _pos[0];
    *y = _pos[1];
    *z = _pos[2];
}

void Emitter::Pos(float x, float y, float z) {
    _pos[0] = x;
    _pos[1] = y;
    _pos[2] = z;
}

void Emitter::Orientation(float *x, float *y, float *z) {
    *x = _mangle[0];
    *y = _mangle[1];
    *z = _mangle[2];
}

void Emitter::Orientation(float x, float y, float z) {
    _mangle[0] = x;
    _mangle[1] = y;
    _mangle[2] = z;
}

unsigned int Emitter::Flags() {
    return _flags;
}

void Emitter::Flags(unsigned int flag, bool op) {
    _flags |= flag;

    if (!op)
        _flags ^= flag;
}

void Emitter::ParticleArray(int n) {
    if (n) {
        if (_particle) {
            _count = 0;
            delete [] _particle;
        }

        _count = n;
        _particle = new Particle[_count];
    }
}

void Emitter::Name(const char *name) {
    if (name && name[0]) {
        if (_name)
            delete [] _name;

        int l = strlen(name);
        _name = new char[l+1];

        // Mongoose 2002.01.09, Mongoose says 'Only you can prevent overflows'
        strncpy(_name, name, l);
        _name[l] = 0;
    }
}

void Emitter::SetTextureId(int id) {
    for (unsigned int i = 0; i < _count; i++)
        _particle[i].TextureId(id);
}

void Emitter::TextureId(unsigned int particle_start, unsigned int particle_end, int id) {
    if ((particle_start < _count) && (particle_end > 0 && particle_end <= _count) && (particle_start < particle_end)) {
        for (unsigned int i = particle_start; i < particle_end; i++)
            _particle[i].TextureId(id);
    }
}

void Emitter::Color(unsigned int particle_start, unsigned int particle_end, float r, float g, float b) {
    if ((particle_start < _count) && (particle_end > 0 && particle_end <= _count) && (particle_start < particle_end)) {
        for (unsigned int i = particle_start; i < particle_end; i++)
            _particle[i].Color(r, g, b);
    }
}

void Emitter::Speed(unsigned int particle_start, unsigned int particle_end, float x, float y, float z) {
    if ((particle_start < _count) && (particle_end > 0 && particle_end <= _count) && (particle_start < particle_end)) {
        for (unsigned int i = particle_start; i < particle_end; i++)
            _particle[i].Speed(x, y, z);
    }
}

void Emitter::Force(unsigned int particle_start, unsigned int particle_end, float x, float y, float z) {
    if ((particle_start < _count) && (particle_end > 0 && particle_end <= _count) && (particle_start < particle_end)) {
        for (unsigned int i = particle_start; i < particle_end; i++)
            _particle[i].Force(x, y, z);
    }
}

void Emitter::Draw() {
    unsigned int i, p;
    float x, y, z;
    float r, g, b;
    float life;

    if (!_count || !_particle) {
        printf("Emitter::Draw> No particles!\n");
        return;
    }

    glPushMatrix();

    //glRotatef(_mangle[0], 1.0, 0.0, 0.0);
    //glRotatef(_mangle[1], 0.0, 1.0, 0.0);
    //glRotatef(_mangle[2], 0.0, 0.0, 1.0);
    //glTranslatef(_pos[0], _pos[1], _pos[2]);

    if (_flags & fUseDepthSorting)
        qsort(_particle, _count, sizeof(Particle), compareParticleDist);

    for (i = 0; i < _count; i++) {
        if (_particle[i].isActive()) {
            _particle[i].Pos(&x, &y, &z);

            if (_flags & fUseFrustumCulling) {
                for (p = 0; p < 6; ++p) {
                    if (((mFrustum[p][0] * x) + (mFrustum[p][1] * y) + (mFrustum[p][2] * z) + mFrustum[p][3]) < 0) {
                        _particle[i].setActive(false);
                        break;
                    }
                }

                if (!_particle[i].isActive())
                    continue;
            }

            _particle[i].Color(&r, &g, &b);
            life = _particle[i].Life();

            // Mongoose 2002.01.01, Removed Texture agent dep
            glBindTexture(GL_TEXTURE_2D, _particle[i].Texture());

            // Alpha is determined by life, which is affected by blend amount
            glColor4f(r, g, b, life);

            // Render tristrip quad
            glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2d(1.0, 1.0);
            glVertex3f(x + 0.5f, y + 0.5f, z);

            glTexCoord2d(0.0, 1.0);
            glVertex3f(x - 0.5f, y + 0.5f, z);

            glTexCoord2d(1.0, 0.0);
            glVertex3f(x + 0.5f, y - 0.5f, z);

            glTexCoord2d(0.0, 0.0);
            glVertex3f(x - 0.5f, y - 0.5f, z);
            glEnd();

            // Update particle's attributes for it's life cycle
            _particle[i].Update();
        }
    }

    glPopMatrix();
}
