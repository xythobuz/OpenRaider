/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 * 
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Particle
 * License : No use w/o permission (C)2001Mongoose
 * Comments: Partcle system's atomic base
 *
 *
 *           This file was generated using Mongoose's C++ 
 *           template generator script.  <stu7440@westga.edu>
 * 
 *-- History ------------------------------------------------- 
 *
 * 2001.08.13:
 * Mongoose - Created
 =================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include "Particle.h"

#ifdef DEBUG_MEMEORY
#   include "memeory_test.h"
#endif



Particle::Particle()
{
	setActive(true);
	TextureId(0);
	Speed(2000.0, 2000.0, 2000.0);
	Color(1.0, 1.0, 1.0);
	Force(0.0, 0.8, 0.0);

	Reset();
}


Particle::~Particle()
{
}


void Particle::TextureId(int id)
{
	_texture = id;
}


void Particle::setActive(bool active)
{
	_active = active;
}


void Particle::Speed(float x, float y, float z)
{
	_speed[0] = x;
	_speed[1] = y;
	_speed[2] = z;
}


void Particle::Color(float r, float g, float b)
{
	_color[0] = r;
	_color[1] = g;
	_color[2] = b;
}


void Particle::Force(float x, float y, float z)
{
	_force[0] = x;
	_force[1] = y;
	_force[2] = z;
}


void Particle::Reset()
{
	// Mongoose 2002.01.01, Ah, how old is that code?
#ifdef OBSOLOETE
	_active = true;
	_life = 1.0;
	_blend = (float)(rand() % 100) / 1000.0 + 0.003;

	_pos[0] = _pos[1] = _pos[2] = 0.0;
	
	_dir[0] = (float)((rand() % 50) - 26.0) * 10.0;
	_dir[1] = (float)((rand() % 50) - 25.0) * 10.0;
	_dir[2] = _dir[1];
	
	_force[0] = 0.0;
	_force[1] = -0.8;
	_force[2] = 0.0;
#else
	// FIXME: _blend prob should have nonstatic range
	_blend = (float)(0.003 + (0.1 * rand() / (RAND_MAX + 1.0))); // high order
	//_blend = (float)(rand() % 100) / 1000.0 + 0.003; 

	// FIXME: Reset these using some nonstatic functions and values later
	_life = 1.0;

	_pos[0] = _pos[1] = _pos[2] = 0.0;

	_dir[0] = (float)(-26.0 + (50.0 * rand() / (RAND_MAX + 1.0))); // high order
	_dir[0] *= 10.0;
	//_dir[0] = (float)((rand() % 50) - 26.0) * 10.0;
	_dir[1] = (float)(-25.0 + (50.0 * rand() / (RAND_MAX + 1.0))); // high order
	_dir[1] *= 10.0;
	//_dir[1] = (float)((rand() % 50) - 25.0) * 10.0;
	_dir[2] = _dir[1];
#endif
}


void Particle::Pos(float *x, float *y, float *z)
{
	*x = _pos[0];
	*y = _pos[1];
	*z = _pos[2];
}


void Particle::Color(float *r, float *g, float *b)
{
	*r = _color[0];
	*g = _color[1];
	*b = _color[2];
}


float Particle::Life()
{
	return _life;
}


void Particle::Update()
{
	// Adjust particle position
	_pos[0] += _dir[0] / _speed[0];
	_pos[1] += _dir[1] / _speed[1];
	_pos[2] += _dir[2] / _speed[2];
	
	// Adjust particle direction
	_dir[0] += _force[0];
	_dir[1] += _force[1];
	_dir[2] += _force[2];

	// Adjust particle blending/life
	_life -= _blend;
	
	// Reset 'dead' OR fully blended particles
	if (_life < 0.0)
	{
		Reset();
	}
}


int Particle::Texture()
{
	return _texture;
}


bool Particle::isActive()
{ 
	return _active;
}
