/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : SkeletalModel
 * License : No use w/o permission (C) 2003 Mongoose
 * Comments: This is the factored out skeletal model class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2003.05.19:
 * Mongoose - Created
 =================================================================*/

#include "SkeletalModel.h"


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

SkeletalModel::SkeletalModel()
{
	model = 0x0;
	flags = 0;
	mBoneFrame = 0;
	mAnimationFrame = 0;
	mIdleAnimation = 0;
}


SkeletalModel::~SkeletalModel()
{
	if (model)
	{
		for (model->animation.start();
			  model->animation.forward();
			  model->animation.next())
		{
			animation_frame_t *af = model->animation.current();

			if (!af)
				continue;

			for (af->frame.start(); af->frame.forward(); af->frame.next())
			{
				bone_frame_t *bf = af->frame.current();

				if (!bf)
					continue;

				bf->tag.erase();

				//for (bf->tag.start(); bf->tag.forward(); bf->tag.next())
				//{
				//	if (bf->tag.current())
				//		delete bf->tag.current();
				//}

				delete bf;
			}

			af->frame.clear();

			delete af;
		}

		model->animation.clear();

		delete model;
	}
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

int SkeletalModel::getAnimation()
{
	return mAnimationFrame;
}


int SkeletalModel::getFrame()
{
	return mBoneFrame;
}


int SkeletalModel::getIdleAnimation()
{
	return mIdleAnimation;
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void SkeletalModel::setModel(skeletal_model_t *mdl)
{
	if (mdl)
		model = mdl;
}


void SkeletalModel::setAnimation(int index)
{
	if (!model) // index > (int)model->animation.size())
		return;

	animation_frame_t *a = model->animation[index];

	if (a)
	{
		mAnimationFrame = index;
		mBoneFrame = 0;
		rate = a->rate;
	}
}


void SkeletalModel::setFrame(int index)
{
	if (!model)
		return;

	animation_frame_t *a = model->animation[mAnimationFrame];

	if (a) // index > (int)a->frame.size())
	{
		bone_frame_t *b = a->frame[index];

		if (b)
		{
			mBoneFrame = index;
		}
	}
}


void SkeletalModel::setIdleAnimation(int index)
{
	if (!model)
		return;

	animation_frame_t *a = model->animation[index];

	if (a)
	{
		mIdleAnimation = index;
	}
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

