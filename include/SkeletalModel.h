/*!
 * \file SkeletalModel.h
 * \brief This is the factored out skeletal model class
 *
 * Defining UNIT_TEST_SKELETALMODEL builds SkeletalModel class as a console unit test
 *
 * \author Mongoose
 *
 * \todo Start cutting off old hacks by simple force use of method interface.
 * Also move the publicly exposed attributes out  =)
 * Better animation system in general - this is memory wasteful
 */

#ifndef GUARD__OPENRAIDER_MONGOOSE_SKELETALMODEL_H_
#define GUARD__OPENRAIDER_MONGOOSE_SKELETALMODEL_H_

#include "mstl/Vector.h"
#include "hel/math.h"


typedef struct bone_tag_s
{
	int mesh;
	vec3_t off;
	vec3_t rot;
	char flag;

} bone_tag_t;


typedef struct bone_frame_s
{
	Vector<bone_tag_t *> tag;
	vec3_t pos;
	float yaw;

} bone_frame_t;


typedef struct animation_frame_s
{
	int id;
	char rate;
	Vector<bone_frame_t *> frame;

} animation_frame_t;


typedef struct skeletal_model_s
{
	int id;
	bool tr4Overlay;
	bool pigtails;
	int ponytailId;
	vec3_t ponytail;
	int ponytailMeshId;
	unsigned int ponytailNumMeshes;
	float ponytailAngle;
	float ponyOff;
	float ponyOff2;
	Vector<animation_frame_t *> animation;

} skeletal_model_t;


class SkeletalModel
{
 public:

	////////////////////////////////////////////////////////////
	// Constructors
	////////////////////////////////////////////////////////////

	SkeletalModel();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of SkeletalModel
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~SkeletalModel();
	/*------------------------------------------------------
	 * Pre  : SkeletalModel object is allocated
	 * Post : Deconstructs an object of SkeletalModel
	 *
	 *-- History ------------------------------------------
	 *
	 * 2003.05.19:
	 * Mongoose - Created
	 ------------------------------------------------------*/


	////////////////////////////////////////////////////////////
	// Public Accessors
	////////////////////////////////////////////////////////////

	int getAnimation();

	int getFrame() ;

	int getIdleAnimation();


	////////////////////////////////////////////////////////////
	// Public Mutators
	////////////////////////////////////////////////////////////

	void setModel(skeletal_model_t *mdl);

	void setAnimation(int index);

	void setFrame(int index);

	void setIdleAnimation(int index);


	enum SkeletalModelFlag { fReserved = 1 };

	unsigned int flags;

	skeletal_model_t *model;   /* World render model */

	float time;                /* Interpolation use */
	float lastTime;

	float rate; // temp cache this here for old animation system use

private:

	////////////////////////////////////////////////////////////
	// Private Accessors
	////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////
	// Private Mutators
	////////////////////////////////////////////////////////////


	int mBoneFrame;             /* Bone frame */
	int mAnimationFrame;        /* Animation frame */
	int mIdleAnimation;         /* Idle animation */
};

#endif
