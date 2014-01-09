/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Emitter
 * License : No use w/o permission (C)2001Mongoose
 * Comments: Particle emitter for freyja
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2001.08.13:
 * Mongoose - Now using new Particle class!
 *
 * 2001.06.30:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_EMITTER_H_
#define __FREYJA_MONGOOSE_EMITTER_H_

#include "Particle.h"


class Emitter
{
 public:

	typedef enum
	{
		fUseFrustumCulling = 1,
		fUseDepthSorting = 2
	} EmitterFlags;


  Emitter(const char *name, int n);
  /*------------------------------------------------------
   * Pre  : name is a valid C string
	*        n is a number greater than 0
	*
   * Post : Constructs an object of Emitter
   *
   *-- History ------------------------------------------
   *
   * 2001.06.30:
   * Mongoose - Created
   ------------------------------------------------------*/

  ~Emitter();
  /*------------------------------------------------------
   * Pre  : Emitter object is allocated
   * Post : Deconstructs an object of Emitter
   *
   *-- History ------------------------------------------
   *
   * 2001.06.30:
   * Mongoose - Created
   ------------------------------------------------------*/

   Particle *Particles();
  /*------------------------------------------------------
   * Pre  :
   * Post : Returns particle array
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  int Count();
  /*------------------------------------------------------
   * Pre  :
   * Post : Returns number of particles emitted
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Pos(float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : x,y,z exist
   * Post : Sets position of emitter in 3 space
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Pos(float *x, float *y, float *z);
  /*------------------------------------------------------
   * Pre  : x,y,z exist
   * Post : Returns position of emitter in 3 space
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

	void Orientation(float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : x,y,z exist
   * Post : Sets orientation of emitter in 3 space
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

	void Orientation(float *x, float *y, float *z);
  /*------------------------------------------------------
   * Pre  : x,y,z exist
   * Post : Returns orientation of emitter in 3 space
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

  unsigned int Flags();
  /*------------------------------------------------------
   * Pre  :
   * Post : Returns emitter flags
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/


  void Flags(unsigned int flag, bool op);
  /*------------------------------------------------------
   * Pre  :
   * Post : Set and Unset flag with op
	*
	*          true - set
	*          flase - unset
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  void ParticleArray(int n);
  /*------------------------------------------------------
   * Pre  : n is a number greater than 0
   * Post : Allocates the particle array and sets the count.
	*        If the array has been allocated previously, then
	*        the array is deallocated and a new one made.
	*
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Draw();
  /*------------------------------------------------------
   * Pre  :
   * Post : Renders particles
   *
   *-- History ------------------------------------------
   *
   * 2001.06.30:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Name(const char *name);
  /*------------------------------------------------------
   * Pre  : name is a valid C string
   * Post : Sets the emitters name
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

  void SetTextureId(int id);
  /*------------------------------------------------------
   * Pre  :
   * Post : Resets all particle texture ids
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13:
   * Mongoose - Created
   ------------------------------------------------------*/

  void TextureId(unsigned int particle_start, unsigned int particle_end,
					  int id);
  /*------------------------------------------------------
   * Pre  : particle_start and particle_end are a valid
	*        range of particles in the array
	*
	*        id is a valid texture id
	*
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Color(unsigned int particle_start, unsigned int particle_end,
				 float r, float g, float b);
  /*------------------------------------------------------
   * Pre  : particle_start and particle_end are a valid
	*        range of particles in the array
	*
	*        r,g,b are colors 0.0 to 1.0
	*
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Speed(unsigned int particle_start, unsigned int particle_end,
				 float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : particle_start and particle_end are a valid
	*        range of particles in the array
	*
	*        take note speed inits at 2000, lower is faster
	*
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/

  void Force(unsigned int particle_start, unsigned int particle_end,
				 float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : particle_start and particle_end are a valid
	*        range of particles in the array
	*
	*        Force/Gravity acting on particles
	*
   * Post :
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14:
   * Mongoose - Created
   ------------------------------------------------------*/


	static vec_t mFrustum[6][4]; /* View Volume copy */


 private:

	char *_name;              /* Emitter name */

	unsigned int _flags;      /* Emitter flags */

	vec3_t _pos;              /* Position in 3 space */

	vec3_t _mangle;           /* Oreintation in 3 space */

	Particle *_particle;      /* Array of particles */

	unsigned int _count;      /* Particle count */
};

#endif
