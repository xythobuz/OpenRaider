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
 *-- History ------------------------------------------------ 
 *
 * 2001.08.13:
 * Mongoose - Created
 ================================================================*/


#ifndef __FREYJA_MONGOOSE_PARTICLE_H_
#define __FREYJA_MONGOOSE_PARTICLE_H_

#include <hel/math.h>

class Particle
{
 public:

  Particle();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Constructs an object of Particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  ~Particle();
  /*------------------------------------------------------
   * Pre  : Particle object is allocated
   * Post : Deconstructs an object of Particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

	void setActive(bool active);
  /*------------------------------------------------------
   * Pre  : 
   * Post : Toggles active state of particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Force(float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : 
   * Post : Sets gravity/force acting on particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Reset();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Resets particle to defaults
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Speed(float x, float y, float z);
  /*------------------------------------------------------
   * Pre  : note speed inits at 2000, lower is faster
   * Post : Sets particle speed
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Color(float r, float g, float b);
  /*------------------------------------------------------
   * Pre  : r,g,b are colors 0.0 to 1.0
   * Post : Sets new particle coloring
	*
	* NOTE : White {1.0, 1.0, 1.0} is the init color
   *
   *-- History ------------------------------------------
   *
   * 2001.08.14: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Pos(float *x, float *y, float *z);
  /*------------------------------------------------------
   * Pre  : x,y,z exist
   * Post : Returns position of particle in 3 space
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Color(float *r, float *g, float *b);
  /*------------------------------------------------------
   * Pre  : r,g,b exist
   * Post : Returns current color of particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  float Life();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Returns current life ( blend ) of particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void Update();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Ajusts for particle life cycle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  int Texture();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Returens texture binding for this particle
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  bool isActive();
  /*------------------------------------------------------
   * Pre  : 
   * Post : Returns active value
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/

  void TextureId(int t);
  /*------------------------------------------------------
   * Pre  : 
   * Post : 
   *
   *-- History ------------------------------------------
   *
   * 2001.08.13: 
   * Mongoose - Created
   ------------------------------------------------------*/


 private:

	bool _active;           /* Is this particle in use? */
	
	float _life;            /* Life of particle  */
	
	float _blend;           /* Blend amount or fade */

	int _texture;           /* Texture polygon to use */

	vec3_t _pos;            /* Current position in 3 space */

	vec3_t _color;          /* Current color */

	vec3_t _dir;            /* Current direction */

	vec3_t _force;          /* Current force or 'gravity' */

	vec3_t _speed;          /* Speed of particle movement */
};

#endif
