/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Freyja
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : Md3AnimModel
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Md3 animation model class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.06.19:
 * Mongoose - Created
 =================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef USING_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#include "hel/math.h"
#include "hel/Quaternion.h"
#include "Md3AnimModel.h"


// Update this with renderer's frame interval
extern unsigned int getTicks();


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

Md3AnimModel::Md3AnimModel()
{
	mFlags = fAnimate;

	initMd3(m_lower, 1);
	initMd3(m_upper, 2);
	initMd3(m_head, 3);
	initMd3(m_weapon, 4);

	memset(m_anim, 0, sizeof(md3_animation_t)*32);

	m_animCount = 0;
	m_sex = MD3_SEX_FEMALE;

	texNumTest = 0;

	setDebug(1);
}


Md3AnimModel::~Md3AnimModel()
{
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

unsigned int Md3AnimModel::getFlags()
{
	return mFlags;
}


md3_animation_id_t Md3AnimModel::getAnimUpper()
{
	return m_upperAnim;
}


md3_animation_id_t Md3AnimModel::getAnimLower()
{
	return m_lowerAnim;
}


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void Md3AnimModel::setFlag(Md3AnimModelFlags flag)
{
	mFlags |= flag;
}


void Md3AnimModel::toggleFlag(Md3AnimModelFlags flag)
{
	mFlags ^= flag;
}


// Mongoose 2002.06.29, FIXME: Add handler here to avoid
//   using animations not loaded
void Md3AnimModel::setAnimUpper(md3_animation_id_t anim)
{
	switch (anim)
	{
	case BOTH_DEATH1:
		m_upper.currentAnimation = 0;
		break;
	case BOTH_DEAD1:
		m_upper.currentAnimation = 1;
		break;
	case BOTH_DEATH2:
		m_upper.currentAnimation = 2;
		break;
	case BOTH_DEAD2:
		m_upper.currentAnimation = 3;
		break;
	case BOTH_DEATH3:
		m_upper.currentAnimation = 4;
		break;
	case BOTH_DEAD3:
		m_upper.currentAnimation = 5;
		break;
	case TORSO_GESTURE:
		m_upper.currentAnimation = 6;
		break;
	case TORSO_ATTACK:
		m_upper.currentAnimation = 7;
		break;
	case TORSO_ATTACK2:
		m_upper.currentAnimation = 8;
		break;
	case TORSO_DROP:
		m_upper.currentAnimation = 9;
		break;
	case TORSO_RAISE:
		m_upper.currentAnimation = 10;
		break;
	case TORSO_STAND:
		m_upper.currentAnimation = 11;
		break;
	case TORSO_STAND2:
		m_upper.currentAnimation = 12;
		break;
	default:
		m_upper.currentAnimation = 11;
	}

	m_upper.currentFrame = m_anim[m_upper.currentAnimation].first_frame;

	//m_upper.currentAnimation = anim;
	m_upperAnim = anim;

	if (anim != (int)m_upper.currentAnimation)
	{
		printf("ERROR upperAnim %i != %i\n", anim, m_upper.currentAnimation);
	}
}


// Mongoose 2002.06.29, FIXME: Add handler here to avoid
//   using animations not loaded
void Md3AnimModel::setAnimLower(md3_animation_id_t anim)
{
	switch (anim)
	{
	case BOTH_DEATH1:
		setAnimUpper(anim);
		m_lower.currentAnimation = 0;
		break;
	case BOTH_DEAD1:
		setAnimUpper(anim);
		m_lower.currentAnimation = 1;
		break;
	case BOTH_DEATH2:
		setAnimUpper(anim);
		m_lower.currentAnimation = 2;
		break;
	case BOTH_DEAD2:
		setAnimUpper(anim);
		m_lower.currentAnimation = 3;
		break;
	case BOTH_DEATH3:
		setAnimUpper(anim);
		m_lower.currentAnimation = 4;
		break;
	case BOTH_DEAD3:
		setAnimUpper(anim);
		m_lower.currentAnimation = 5;
		break;
	case LEGS_WALKCR:
		m_lower.currentAnimation = 13;
		break;
	case LEGS_WALK:
		m_lower.currentAnimation = 14;
		break;
	case LEGS_RUN:
		m_lower.currentAnimation = 15;
		break;
	case LEGS_BACK:
		m_lower.currentAnimation = 16;
		break;
	case LEGS_SWIM:
		m_lower.currentAnimation = 17;
		break;
	case LEGS_JUMP:
		m_lower.currentAnimation = 18;
		break;
	case LEGS_LAND:
		m_lower.currentAnimation = 19;
		break;
	case LEGS_JUMPB:
		m_lower.currentAnimation = 20;
		break;
	case LEGS_LANDB:
		m_lower.currentAnimation = 21;
		break;
	case LEGS_IDLE:
		m_lower.currentAnimation = 22;
		break;
	case LEGS_IDLECR:
		m_lower.currentAnimation = 23;
		break;
	case LEGS_TURN:
		m_lower.currentAnimation = 24;
		break;
	default:
		m_lower.currentAnimation = 14;
	}

	m_lower.currentFrame = m_anim[m_lower.currentAnimation].first_frame;

	//m_lower.currentAnimation = anim;
	m_lowerAnim = anim;

	if (anim != (int)m_lower.currentAnimation)
	{
		printf("ERROR lowerAnim %i != %i\n", anim, m_lower.currentAnimation);
	}
}


void Md3AnimModel::setDebug(unsigned char level)
{
	m_debug = level;

	// Mongoose 2002.06.29, Added to allow new warning level
	--level;

	m_lower.setDebug(level);
	m_upper.setDebug(level);
	m_head.setDebug(level);
	m_weapon.setDebug(level);
}


int Md3AnimModel::load(char *modelPath, char *skin, md3_lod_t modelLoD)
{
	int err = 0;


	if (!modelPath || !modelPath[0] || !skin || !skin[0] || modelLoD > 2)
	{
		return -1;
	}

	err += loadMd3(m_head,  "head",  modelPath, skin, modelLoD);

	if (err)
	{
		modelLoD = MD3_LOD_MED;
		err = 0;
		err += loadMd3(m_head,  "head",  modelPath, skin, modelLoD);
	}

	if (err)
	{
		modelLoD = MD3_LOD_LOW;
		err = 0;
		err += loadMd3(m_head,  "head",  modelPath, skin, modelLoD);
	}

	err += loadMd3(m_upper, "upper", modelPath, skin, modelLoD);
	err += loadMd3(m_lower, "lower", modelPath, skin, modelLoD);

	// Connect upper to lower with torso tag
	connectModels("tag_torso", m_upper, m_lower);

	// Connect head to upper
	connectModels("tag_head", m_head, m_upper);

	loadAnimations(modelPath);

	setAnimUpper(TORSO_STAND);
	setAnimLower(LEGS_WALK);

	if (m_animCount < 24)
	{
		printf("ERROR: Not all animations loaded %i/24\n", m_animCount);
		return -2;
	}

	return err;
}


int Md3AnimModel::loadWeapon(char *modelPath, const char *name)
{
	int err = 0;


	if (!modelPath || !modelPath[0] || !name || !name[0])
	{
		return -1;
	}

	err = loadMd3(m_weapon,  name,  modelPath, "", MD3_LOD_HIGH);

	// Connect weapon to upper
	connectModels("tag_weapon", m_weapon, m_upper);

	// If they loaded a weapon they prob want to see it
	setFlag(fDrawWeapon);

	return err;
}


int Md3AnimModel::loadAnimations(char *modelPath)
{
	unsigned int firstFrame, numFrames, loopingFrames, framesPerSecond;
	FILE *f;
	int eof;
	md3_animation_id_t id;
	unsigned int i = 0,j,  state, state2, len;
	char buffer[256];
	char c, lc;


	if (!modelPath || !modelPath[0])
	{
		return -1;
	}

	len = strlen(modelPath);
	snprintf(buffer, 250, "%s%s", modelPath,
				(modelPath[len-1] == '/') ? "animation.cfg" : "/animation.cfg");
	buffer[250] = 0;

	f = fopen(buffer, "r");

	if (!f)
	{
		perror("Md3AnimModel::loadAnimations> ");
		return -1;
	}

	eof = 0;
	i = 0;
	c = 0;
	buffer[0] = 0;
	state = 4;
	state2 = 0;

	printf("\n");

	while (eof != EOF)
	{
		lc = c;
		eof = fscanf(f, "%c", &c);

		// Treat EOF like end of line
		if (eof == EOF)
		{
			//if (i && isdigit(buffer[i-1])) c = ' '; else
			c = '\n';
		}

		switch (c)
		{
		case '/': // Comment?
			if (lc == '/')
			{
				if (state == 4)
				{
					state = 0;
					state2 = 0;
				}
				else
				{
					state2 = 1;
				}
			}
			break;
		case '\t': // Seperator triggers
		case ' ':
			if (isdigit(buffer[0]))
			{
				switch (state)
				{
				case 0:
					firstFrame = atoi(buffer);
					++state;
					i = 0;
					break;
				case 1:
					numFrames = atoi(buffer);
					++state;
					i = 0;
					break;
				case 2:
					loopingFrames = atoi(buffer);
					++state;
					i = 0;
					break;
				case 3:
					framesPerSecond = atoi(buffer);
					state = 4; // override comment buffer block
					i = 0;
					break;
				}
			}
			break;
		case '\n': // Seperator EOL trigger
		case '\r': // Seperator whitespace
		case '\0':
			state2 = 0;

			if (strncmp("BOTH_", buffer, 5) == 0 ||
				 strncmp("TORSO_", buffer, 6) == 0 ||
				 strncmp("LEGS_", buffer, 5) == 0)
			{
				for (j = 0; j < i; ++j)
				{
					if (!isdigit(buffer[j]) && !isupper(buffer[j]) &&
						 buffer[j] != '_')
					{
						printf("WARNING: Stripping '%s'.\n", buffer);
						buffer[j] = 0;
						j = i;
					}
				}

				i = 0;
			}
			else
			{
				i = 0;
			}

			if (strncmp("BOTH_", buffer, 5) == 0)
			{
				if (strncmp("BOTH_DEATH1\0", buffer, 12) == 0)
				{
					id = BOTH_DEATH1;
				}
				else if (strncmp("BOTH_DEATH2\0", buffer, 12) == 0)
				{
					id = BOTH_DEATH2;
				}
				else if (strncmp("BOTH_DEATH3\0", buffer, 12) == 0)
				{
					id = BOTH_DEATH3;
				}
				else if (strncmp("BOTH_DEAD1\0", buffer, 11) == 0)
				{
					id = BOTH_DEAD1;
				}
				else if (strncmp("BOTH_DEAD2\0", buffer, 11) == 0)
				{
					id = BOTH_DEAD2;
				}
				else if (strncmp("BOTH_DEAD3\0", buffer, 11) == 0)
				{
					id = BOTH_DEAD3;
				}
				else
				{
					id = UNSUPPORTED;
				}

				if (m_debug > 0)
				{
					printf("<%s> %3i %3i %3i %3i \t'%s' \t(%i)\n",
							 (id == UNSUPPORTED) ? "Unsupported" : "",
							 firstFrame, numFrames,
							 loopingFrames, framesPerSecond, buffer, id);
				}

				addAnim(buffer, id, firstFrame, numFrames,
								loopingFrames, framesPerSecond);

				i = 0;
			}
			else if (strncmp("TORSO_", buffer, 6) == 0)
			{
				if (strncmp("TORSO_GESTURE\0", buffer, 14) == 0)
				{
					id = TORSO_GESTURE;
				}
				else if (strncmp("TORSO_ATTACK2\0", buffer, 14) == 0)
				{
					id = TORSO_ATTACK2;
				}
				else if (strncmp("TORSO_ATTACK\0", buffer, 13) == 0)
				{
					id = TORSO_ATTACK;
				}
				else if (strncmp("TORSO_DROP\0", buffer, 11) == 0)
				{
					id = TORSO_DROP;
				}
				else if (strncmp("TORSO_RAISE\0", buffer, 12) == 0)
				{
					id = TORSO_RAISE;
				}
				else if (strncmp("TORSO_STAND2\0", buffer, 13) == 0)
				{
					id = TORSO_STAND2;
				}
				else if (strncmp("TORSO_STAND\0", buffer, 12) == 0)
				{
					id = TORSO_STAND;
				}
				else
				{
					id = UNSUPPORTED;
				}

				if (m_debug > 0)
				{
					printf("<%s> %3i %3i %3i %3i \t'%s' (%i)\n",
							 (id == UNSUPPORTED) ? "Unsupported" : "",
							 firstFrame, numFrames,
							 loopingFrames, framesPerSecond, buffer, id);
				}

				if (id != UNSUPPORTED)
				{
					addAnim(buffer, id, firstFrame, numFrames,
							  loopingFrames, framesPerSecond);
				}

				i = 0;
			}
			else if (strncmp("LEGS_", buffer, 5) == 0)
			{
				if (strncmp("LEGS_WALKCR\0", buffer, 12) == 0)
				{
					id = LEGS_WALKCR;
				}
				else if (strncmp("LEGS_WALK\0", buffer, 10) == 0)
				{
					id = LEGS_WALK;
				}
				else if (strncmp("LEGS_RUN\0", buffer, 9) == 0)
				{
					id = LEGS_RUN;
				}
				else if (strncmp("LEGS_BACK\0", buffer, 10) == 0)
				{
					id = LEGS_BACK;
				}
				else if (strncmp("LEGS_SWIM\0", buffer, 10) == 0)
				{
					id = LEGS_SWIM;
				}
				else if (strncmp("LEGS_JUMPB\0", buffer, 11) == 0)
				{
					id = LEGS_JUMPB;
				}
				else if (strncmp("LEGS_LANDB\0", buffer, 11) == 0)
				{
					id = LEGS_LANDB;
				}
				else if (strncmp("LEGS_IDLECR\0", buffer, 12) == 0)
				{
					id = LEGS_IDLECR;
				}
				else if (strncmp("LEGS_JUMP\0", buffer, 10) == 0)
				{
					id = LEGS_JUMP;
				}
				else if (strncmp("LEGS_LAND\0", buffer, 10) == 0)
				{
					id = LEGS_LAND;
				}
				else if (strncmp("LEGS_IDLE\0", buffer, 10) == 0)
				{
					id = LEGS_IDLE;
				}
				else if (strncmp("LEGS_TURN\0", buffer, 10) == 0)
				{
					id = LEGS_TURN;
				}
				else
				{
					id = UNSUPPORTED;
				}

				if (m_debug > 0)
				{
					printf("<%s> %3i %3i %3i %3i \t'%s' (%i)\n",
							 (id == UNSUPPORTED) ? "Unsupported" : "",
							 firstFrame, numFrames,
							 loopingFrames, framesPerSecond, buffer, id);
				}

				if (id != UNSUPPORTED)
				{
					addAnim(buffer, id, firstFrame, numFrames,
							  loopingFrames, framesPerSecond);
				}

				i = 0;
			}
			else if (strncmp("sexf", buffer, 4) == 0)
			{
				m_sex = MD3_SEX_FEMALE;

				if (m_debug > 0)
				{
					printf("<> sex = female\n");
				}

				i = 0;
			}
			else if (strncmp("sexm", buffer, 4) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> sex = male\n");
				}

				m_sex = MD3_SEX_MALE;
				i = 0;
			}
			else if (strncmp("sexn", buffer, 4) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> sex = neuter\n");
				}

				m_sex = MD3_SEX_NEUTER;
				i = 0;
			}
			else if (strncmp("sex", buffer, 3) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> sex = ?\n");
				}

				m_sex = MD3_SEX_FEMALE;
				i = 0;
			}
			else if (strncmp("footstepsenergy", buffer, 15) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> footsteps = energy\n");
				}

				i = 0;
				state = 0;
			}
			else if (strncmp("footstepsboot", buffer, 13) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> footsteps = boot\n");
				}

				i = 0;
				state = 0;
			}
			else if (strncmp("footsteps", buffer, 9) == 0)
			{
				if (m_debug > 0)
				{
					printf("<> footsteps = ?\n");
				}

				//i = 0;
				state = 0;
			}

			//printf("\"%s\"\n", buffer);
			break;
		default:
			if (!state2)
			{
				//printf("'%c' %i\n", c, state);
				buffer[i++] = c;
				buffer[i] = 0;
			}
		}

		if (!i)
		{
			buffer[i] = 0;
		}
	}

	fclose(f);

	int torsoOffset = 0;

	for (i = 0; i < m_animCount; ++i)
	{
		if (!torsoOffset && m_anim[i].id > 12)
		{
			torsoOffset = m_anim[LEGS_WALKCR].first_frame - m_anim[TORSO_GESTURE].first_frame;

			m_anim[i].first_frame -= torsoOffset;
			m_anim[i].end_frame -= torsoOffset;
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

void Md3AnimModel::setCurrentTime(Md3 &model)
{
	int animationSpeed;
	float elapsedTime = 0.0f;
	float time, t;


	// No animations in this model
	if (model.numAnimations == 0)
	{
		return;
	}

	// Get time in milliseconds
	time = getTicks();

	// Find the time that has elapsed since the last time that was stored
	elapsedTime = time - model.lastTime;

	// Store the animation speed for this animation in a local variable
	animationSpeed = m_anim[model.currentAnimation].frames_per_second;

	// t value is a value between 0.0 and 1.0
	// Tells out far from the current key frame to the next key frame.
	t = (elapsedTime / (1000.0f / animationSpeed));

	//printf("t = %f, et = %f, lt = %f, fps = %i\n",
	//		 t, elapsedTime, model.lastTime, animationSpeed);

	// If our elapsed time goes over the desired time segment, start over
	// and go to the next key frame.
	if (elapsedTime >= (1000.0f / animationSpeed))
	{
		// Set our current frame to the next key frame
		// (which could be the start of the anim)
		model.currentFrame = model.nextFrame;

		// Set our last time for the model to the current time
		model.lastTime = time;
	}

	model.time = t;
}


void Md3AnimModel::updateModel(Md3 &model)
{
	int startFrame = 0;
	int endFrame = 1;
	unsigned int anim;


	anim = model.currentAnimation;

	if (model.numAnimations)
	{
		startFrame = m_anim[anim].first_frame;
		endFrame   = m_anim[anim].end_frame;
	}

	// Compute next frame
	model.nextFrame = (model.currentFrame + 1) % endFrame;

	// Wrap to first frame if past last
	if (model.nextFrame == 0)
	{
		model.nextFrame = startFrame;
	}

	// Set interpolating time 0.0 - 1.0 ( start to end of animation)
	setCurrentTime(model);
}


void Md3AnimModel::initMd3(Md3 &model, unsigned int id)
{
	model.idTest = id;
	model.numAnimations = 0;
	model.currentAnimation = 0;
	model.currentFrame = 0;
	model.nextFrame = 0;
	model.time = 0.0f;
	model.lastTime = 0.0f;
}


int Md3AnimModel::addAnim(char *modelPath, md3_animation_id_t id,
								  unsigned int firstFrame,
								  unsigned int numFrames,
								  unsigned int loopingFrames,
								  unsigned int framesPerSecond)
{
	if (m_animCount == 30)
	{
		return -1;
	}

	m_anim[id].id = id;
	m_anim[id].first_frame = firstFrame;
	m_anim[id].end_frame = firstFrame + numFrames;
	m_anim[id].num_frames = numFrames;
	m_anim[id].looping_frames = loopingFrames;
	m_anim[id].frames_per_second = framesPerSecond;

	if (id > 12 || id < 6)
	{
		m_lower.numAnimations++;
	}

	if (id < 13)
	{
		m_upper.numAnimations++;
	}

	++m_animCount;

	return 0;
}


int Md3AnimModel::loadMd3(Md3 &model, const char *base, char *modelPath, char *skin,
								  md3_lod_t modelLoD)
{
	char filename[256];
	unsigned int len = 0, l = 250;


	if (!modelPath || !modelPath[0] || !base || !base[0] || modelLoD > 2)
	{
		printf("Md3AnimModel::loadMd3> ERROR: Invalid arguments\n");
		return -1;
	}

	len = strlen(modelPath);

	// Load model with requested LoD
	snprintf(filename, l, "%s%s%s%s", modelPath,
				(modelPath[len-1] == '/') ? "" : "/",
				base,
				(modelLoD == MD3_LOD_HIGH) ? ".md3" :
				(modelLoD == MD3_LOD_MED) ? "_1.md3" : "_2.md3");

	model.reset();

	if (model.load(filename))
	{
		printf("Md3AnimModel::loadMd3> ERROR: Loading Model %s\n", filename);
		return -2;
	}

	// Load skin if requested
	if (skin && skin[0])
	{
		snprintf(filename, l, "%s%s%s_%s.skin", modelPath,
					(modelPath[len-1] == '/') ? "" : "/", base, skin);

		if (loadSkin(model, filename))
		{
			printf("Md3AnimModel::loadMd3> ERROR: Loading Skin %s\n", filename);
			return -3;
		}
	}
	else // Try to load internal skins
	{
		unsigned int i, j, m;
		md3_mesh_t *meshes;


		m = model.getNumMeshes();
		meshes = model.getMeshes();

		for (i = 0; i < m; ++i)
		{
			strncpy(filename, meshes[i].skin[0].name, 250);
			filename[250] = 0;

			for (j = 0; filename[j] != 0; ++j)
			{
				// Replace MSDOS path with unistd ( Should work on NT too iirc )
				if (filename[j] == '\\')
				{
					filename[j] = '/';
				}

				// Lowercase alphabet MSDOS is case insensitive
				// data created with it's skin names are bad
				if (filename[j] > 64 && filename[j] < 91)
				{
					filename[j] += 32;
				}
			}

			model.texTest[i] = cacheTexture(filename);
		}
	}

	return 0;
}


int Md3AnimModel::cacheTexture(char *texture)
{
	unsigned int len, i, id;
	bool cached = false;


	if (!texture || !texture[0])
	{
		return -1;
	}

	len = strlen(texture);

	for (i = 0; i < texNumTest; ++i)
	{
		if (strncmp(texture, texTest[i].name, texTest[i].name_len) == 0)
		{
			cached = true;
			id = i;
			break;
		}
	}

	if (!cached)
	{
		texTest[texNumTest].name = new char [len+1];
		texTest[texNumTest].name_len = len+1;
		strncpy(texTest[texNumTest].name, texture, len+1);
		texTest[texNumTest].name[len] = 0;

		id = texNumTest;

		texNumTest++;
	}

	return id;
}


int Md3AnimModel::loadSkin(Md3 &model, char *filename)
{
	FILE *f;
	md3_mesh_t *meshes;
	int i, j, m, eof, id = 0;
	unsigned int b = 0;
	bool ready = false;
	char buffer[128];
	char mesh[68];
	char c;


	f = fopen(filename, "r");

	if (!f)
	{
		perror("Md3AnimModel::loadSkin> ");
		return -1;
	}

	eof = 0;

	while (eof != EOF)
	{
		eof = fscanf(f, "%c", &c);

		// Treat EOF like end of line
		if (eof == EOF)
		{
			c = '\n';
		}

		switch (c)
		{
		case '\t':
		case '\r':
		case '\0':
			break;
		case ',':
			strncpy(mesh, buffer, b);
			mesh[b] = 0;
			b = 0;
			ready = true;
			break;
		case '\n':
			if (!ready || !b || !buffer[1])
			{
				b = 0;
				ready = false;
				continue;
			}

			id = cacheTexture(buffer);

			for (i = 0; i < 4; ++i)
			{
				switch (i)
				{
				case 1:
					m = m_lower.getNumMeshes();
					meshes = m_lower.getMeshes();
					break;
				case 2:
					m = m_upper.getNumMeshes();
					meshes = m_upper.getMeshes();
					break;
				case 3:
					m = m_head.getNumMeshes();
					meshes = m_head.getMeshes();
					break;
				default:
					continue;
				}

				for (j = 0; j < m; ++j)
				{
					if (strncmp(mesh, meshes[j].name, 68) == 0)
					{
						switch (i)
						{
						case 1:
							m_lower.texTest[j] = id;
							break;
						case 2:
							m_upper.texTest[j] = id;
							break;
						case 3:
							m_head.texTest[j] = id;
							break;
						default:
							continue;
						}

						if (m_debug > 0)
						{
							printf("Mapping '%s' -> '%s' (%i)\n", mesh, buffer, id);
						}
					}
				}
			}

			ready = false;
			b = 0;
			break;
		default:
			buffer[b++] = c;
			buffer[b] = 0;
		}
	}

	fclose(f);

	return 0;
}


// FIXME: should actually parse shaders  =)
int Md3AnimModel::loadShader(Md3 &model, char *filename)
{
	unsigned int i;


	if (model.idTest == 4)
	{
		m_weapon.texTest[0] = cacheTexture("models/weapons/machinegun/skin2.tga");

		for (i = m_weapon.getNumMeshes() - 1; i > 0; --i)
		{
			m_weapon.texTest[i] = m_weapon.texTest[0];
		}

		//m_weapon.texTest[0] = cacheTexture("models/weapons/railgun.tga");
		//m_weapon.texTest[1] = cacheTexture("models/weapons/railgun2.tga");
		//m_weapon.texTest[2] = cacheTexture("models/weapons/railgun3.tga");
		//m_weapon.texTest[3] = cacheTexture("models/weapons/railgun4.tga");
	}

	return 0;
}


void Md3AnimModel::connectModels(char *tagName, Md3 &modelA, Md3 &modelB)
{
	unsigned int i, t;
	md3_tag_t *tags;


	t = modelB.getNumTags();
	tags = modelB.getTags();

	for (i = 0; i < t; ++i)
	{
		if (strncmp(tags[i].name, tagName, 63) == 0)
		{
			modelB.slaveTest[i] = modelA.idTest;
			break;
		}
	}
}


void Md3AnimModel::render()
{
#ifdef USING_OPENGL
	if (m_animCount < 1)
	{
		static unsigned int errors;


		if (errors < 8)
		{
			printf("ERROR: %i. Not all animations loaded %i/24\n",
					 errors+1, m_animCount);
			++errors;
		}
		else if (errors == 8)
		{
			printf("Md3AnimModel::render> Error reporting off after %i errors\n",
					 errors);
			++errors;
		}
		return;
	}

	//glRotatef(-90, 1, 0, 0);
	//glScalef(0.01, 0.01, 0.01);

	if (mFlags & fAnimate)
	{
		updateModel(m_upper);
		updateModel(m_lower);
	}

	glPushMatrix();
	renderTag(1);
	glPopMatrix();
#endif
}


void renderBone(vec3_t min, vec3_t max, vec3_t center, vec_t scale);

void Md3AnimModel::renderModel(Md3 &model)
{
#ifdef USING_OPENGL
	unsigned int i, j, k, numBones, numMeshes;
	unsigned int index, currentIndex, nextIndex, maxIndex;
	md3_mesh_t *meshes, *mesh;
	md3_bone_t *bones, *bone;


	numMeshes = model.getNumMeshes();
	meshes = model.getMeshes();

	if (mFlags & fRenderBones)
	{
		numBones = model.getNumBones();
		bones = model.getBones();

		for (i = 0; i < numBones; ++i)
		{
			bone = bones + i;

			glColor4f(1.0, 1.0, 1.0, 1.0);
			renderBone(bone->mins, bone->maxs, bone->center, bone->scale);
		}
	}

	glEnable(GL_TEXTURE_2D);

	for (i = 0; i < numMeshes; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texTest[model.texTest[i]].gl_texture_id);

		mesh = meshes + i;

		if (mesh->num_triangles <= 0)
		{
			continue;
		}

		currentIndex = mesh->num_vertices * model.currentFrame;
		nextIndex = mesh->num_vertices * model.nextFrame;
		maxIndex = mesh->num_frames * mesh->num_vertices;

		// Try to avoid bad models/loading taking down the subsystem
		// and also handle bad data as well as we can
		if ((int)model.currentFrame >= mesh->num_frames ||
			 (int)model.nextFrame >= mesh->num_frames)
		{
			if (mFlags & fRenderingWarnings)
			{
				fprintf(stderr, "renderModel> WARNING: Frame index would be out of bounds\n model[%i].mesh[%i], %i out of %i frames\n",
						  model.idTest, i, model.currentFrame, mesh->num_frames);
			}

			model.currentFrame = mesh->num_frames-1;
			model.nextFrame = mesh->num_frames-1;

			currentIndex = (mesh->num_frames-1) * mesh->num_vertices;
			nextIndex = (mesh->num_frames-1) * mesh->num_vertices;
		}

		glBegin(GL_TRIANGLES);

		for (j = 0; (int)j < mesh->num_triangles; ++j)
		{
			for (k = 0; k < 3; ++k)
			{
				index = mesh->tris[j].triangle[k];

				if (index + nextIndex > maxIndex ||
					 index + currentIndex > maxIndex)
				{
					currentIndex = 0;
					nextIndex = 0;
					fprintf(stderr, "renderModel> ERROR: Bad frame index\n");
				}

				if (mFlags & fUseNormals)
				{
					glNormal3fv(mesh->vertex[index+currentIndex].norm);
				}

				if (mesh->texel)
				{
					glTexCoord2fv(mesh->texel[index].st);
				}

				if (mFlags & fDisableMeshInterpolate)
				{
					glVertex3f(mesh->vertex[index + currentIndex].pos[0],
								  mesh->vertex[index + currentIndex].pos[1],
								  mesh->vertex[index + currentIndex].pos[2]);
				}
				else
				{
					glVertex3f(mesh->vertex[index+currentIndex].pos[0] +model.time *
								  (mesh->vertex[index+nextIndex].pos[0] -
									mesh->vertex[index+currentIndex].pos[0]),
								  mesh->vertex[index+currentIndex].pos[1] +model.time *
								  (mesh->vertex[index+nextIndex].pos[1] -
									mesh->vertex[index+currentIndex].pos[1]),
								  mesh->vertex[index+currentIndex].pos[2] +model.time *
								  (mesh->vertex[index+nextIndex].pos[2] -
									mesh->vertex[index+currentIndex].pos[2]));
				}
			}
		}

		glEnd();
	}
#endif
}


#ifdef USING_OPENGL
void renderBone(vec3_t min, vec3_t max, vec3_t center, vec_t scale)
{
	glDisable(GL_TEXTURE_2D);

	//scale *= 0.2f;

	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	//glRotatef(-90.0f, 1, 0, 0);
	glScalef(scale, scale, scale);

	glBegin(GL_LINES);
	// max, top quad
	glVertex3f(max[0], max[1], max[2]);
	glVertex3f(max[0], min[1], max[2]);

	glVertex3f(max[0], max[1], max[2]);
	glVertex3f(min[0], max[1], max[2]);

	glVertex3f(max[0], max[1], max[2]);
	glVertex3f(max[0], max[1], min[2]);

	// max-min, vertical quads
	glVertex3f(min[0], max[1], max[2]);
	glVertex3f(min[0], max[1], min[2]);

	glVertex3f(max[0], min[1], max[2]);
	glVertex3f(max[0], min[1], min[2]);

	glVertex3f(max[0], min[1], max[2]);
	glVertex3f(min[0], min[1], max[2]);

	// min-max, vertical quads
	glVertex3f(max[0], max[1], min[2]);
	glVertex3f(max[0], min[1], min[2]);

	glVertex3f(max[0], max[1], min[2]);
	glVertex3f(min[0], max[1], min[2]);

	glVertex3f(min[0], max[1], max[2]);
	glVertex3f(min[0], min[1], max[2]);


	// min, bottom quad
	glVertex3f(min[0], min[1], min[2]);
	glVertex3f(min[0], max[1], min[2]);

	glVertex3f(min[0], min[1], min[2]);
	glVertex3f(max[0], min[1], min[2]);

	glVertex3f(min[0], min[1], min[2]);
	glVertex3f(min[0], min[1], max[2]);
	glEnd();

	glPopMatrix();
}


void renderLaserSight()
{
	const float lenght = 56.0;
	const float start = 8.0;
	float d = 0.01 + (0.15*rand()/(RAND_MAX+1.0)); // for flicker fx


	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glScalef(100, 100, 100);

	// Draw two long quads that skrink and fade the they go further out
	glBegin(GL_QUADS);
	glColor4f(0.9-d, 0.1, 0.1, 0.5);
	glVertex3f(start,  0.0, 0.0);
	glVertex3f(start, 0.2, 0.0);
	glColor4f(0.3-d, 0.1, 0.1, 0.5);
	glVertex3f(lenght, 0.1, 0.0);
	glVertex3f(lenght,  0.0, 0.0);

	glColor4f(0.9-d, 0.1, 0.1, 0.5);
	glVertex3f(start, 0.0, 0.0);
	glVertex3f(start, 0.0, 0.2);
	glColor4f(0.3-d, 0.1, 0.1, 0.5);
	glVertex3f(lenght, 0.0, 0.1);
	glVertex3f(lenght, 0.0, 0.0);
	glEnd();

	glPopMatrix();

	glColor4f(1.0, 1.0, 1.0, 1.0);
}
#endif


void convertQuake3ToHelMatrix(mat3_t q3, matrix_t hel)
{
	hel[0] = q3[0][0];   hel[1] = q3[0][1];   hel[ 2] = q3[0][2];
	hel[4] = q3[1][0];   hel[5] = q3[1][1];   hel[ 6] = q3[1][2];
	hel[8] = q3[2][0];   hel[9] = q3[2][1];   hel[10] = q3[2][2];

	// yeah, wolf
	hel[3] = hel[7] = hel[11] = hel[12] = hel[13] = hel[14] = 0.0f;
	hel[15] = 1.0f;
}


void Md3AnimModel::renderTag(unsigned int id)
{
#ifdef USING_OPENGL
	const float scaleHack = 64.0f;  // Ah Md3.cpp doesn't scale translate?
	unsigned int i, t, f, nf;
	unsigned int *slaves;
	md3_tag_t *tags;
	float time;
	matrix_t finalMatrix, matrix, nextMatrix;
	vec3_t pos, nextPos;
	Quaternion q, qNext, qInterpolate;


	nf = f = 0;

	switch (id)
	{
	case 1:
		renderModel(m_lower);

		f = m_lower.currentFrame;
		nf = m_lower.nextFrame;
		time = m_lower.time;

		t = m_lower.getNumTags();
		tags = m_lower.getTags();
		slaves = m_lower.slaveTest;
		break;
	case 2:
		renderModel(m_upper);

		f = m_upper.currentFrame;
		nf = m_upper.nextFrame;
		time = m_upper.time;

		t = m_upper.getNumTags();
		tags = m_upper.getTags();
		slaves = m_upper.slaveTest;
		break;
	case 3:
		renderModel(m_head);

		f = m_head.currentFrame;
		nf = m_head.nextFrame;
		time = m_head.time;

		t = m_head.getNumTags();
		tags = m_head.getTags();
		slaves = m_head.slaveTest;
		break;
	case 4:
		if (mFlags & fDrawWeapon)
		{
			renderLaserSight();
			renderModel(m_weapon);

			f = m_weapon.currentFrame;
			nf = m_weapon.nextFrame;
			time = m_weapon.time;

			t = m_weapon.getNumTags();
			tags = m_weapon.getTags();
			slaves = m_weapon.slaveTest;
		}
		return;
		break;
	default:
		return;
	}

	for (i = 0; i < t; ++i)
	{
		if (slaves[i] > 0)
		{
			// Using digiben's tag anim notes
			// To find the current translation position for
			// this frame of animation, we multiply
			// the currentFrame by the number of tags,
			// then add i.  This is similar to how
			// the vertex key frames are interpolated.
			pos[0] = tags[f*t+i].center[0];
			pos[1] = tags[f*t+i].center[1];
			pos[2] = tags[f*t+i].center[2];

			// Grab the next key frame translation position
			nextPos[0] = tags[nf*t+i].center[0];
			nextPos[1] = tags[nf*t+i].center[1];
			nextPos[2] = tags[nf*t+i].center[2];

			// By using the equation: p(t) = p0 + t(p1 - p0), with a time t,
			// we create a new translation position that
			// is closer to the next key frame.
			pos[0] = pos[0] + time * (nextPos[0] - pos[0]);
			pos[1] = pos[1] + time * (nextPos[1] - pos[1]);
			pos[2] = pos[2] + time * (nextPos[2] - pos[2]);

			// Now comes the more complex interpolation.  Just like
			// the translation, we want to store the current and
			// next key frame rotation matrix, then interpolate
			// between the 2.

			// Get a pointer to the start of the 3x3 rotation matrix
			// for the current frame
			//matrix = &tags[f*t+i].rotation[0][0];
			convertQuake3ToHelMatrix(tags[f*t+i].rotation, matrix);

			// Get a pointer to the start of the 3x3 rotation matrix
			// for the next frame
			//nextMatrix = &tags[nf*t+i].rotation[0][0];
			convertQuake3ToHelMatrix(tags[nf*t+i].rotation, nextMatrix);

			// Convert the current and next key frame matrix into
			// a quaternion
			q.setByMatrix(matrix);
			qNext.setByMatrix(nextMatrix);

			// Using spherical linear interpolation, find the
			// interpolated quaternion
			qInterpolate = Quaternion::slerp(q, qNext, time);

			// Here we convert the interpolated quaternion into a matrix
			qInterpolate.getMatrix(finalMatrix);

			glPushMatrix();

			if (mFlags & fDisableTagInterpolate)
			{
				// Translate w/ scale hack
				//glTranslatef(pos[0]*64, pos[1]*64, pos[2]*64);
				matrix[12] = pos[0]*scaleHack;
				matrix[13] = pos[1]*scaleHack;
				matrix[14] = pos[2]*scaleHack;
				glMultMatrixf(matrix);
			}
			else
			{
				// Translate w/ scale hack
				//glTranslatef(pos[0]*64, pos[1]*64, pos[2]*64);
				finalMatrix[12] = pos[0]*scaleHack;
				finalMatrix[13] = pos[1]*scaleHack;
				finalMatrix[14] = pos[2]*scaleHack;
				glMultMatrixf(finalMatrix);
			}

			renderTag(slaves[i]);

			glPopMatrix();
		}
	}
#endif
}


//////////////////////////////////////////////////////////////////
// Unit Testing
//////////////////////////////////////////////////////////////////

#ifdef UNIT_TEST_MD3ANIMMODEL_SDL

#include <GL/glu.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include "Texture.h"

void updateTitle();
void updateWindowTitle(char *title);

Md3AnimModel gMd3;
Texture gTexture;
char gTitle[128];
char gAnimUpper[32];
char gAnimLower[32];
float gYaw = 0.0f;
bool gYawOn = true;
float gPitch = -20.0f;
char gMessage[512];
unsigned int gWidth = 640;
unsigned int gHeight = 480;


void loadMd3(char *model, char *skin, char *weapon)
{
	char pathname[256];
	unsigned int i;


	snprintf(gTitle, 255, "Md3AnimModel.test: %s ", model);
	updateWindowTitle(gTitle);

	snprintf(pathname, 255, "data/models/players/%s", model);
	pathname[255] = 0;

	if (gMd3.load(pathname, skin, MD3_LOD_HIGH) < 0)
	{
		printf("ERROR: MD3 '%s' not loaded\n", pathname);
	}
	else
	{
		snprintf(pathname, 255, "data/models/weapons2/%s", weapon);
		pathname[255] = 0;

		if (gMd3.loadWeapon(pathname, weapon))
		{
			printf("Couldn't load weapon model '%s'.\n", pathname);
		}

		gMd3.setAnimUpper(TORSO_STAND);
		gMd3.setAnimLower(LEGS_IDLE);

		// Setup textures
		printf("\nLoading textures: ");
		for (i = 0; i < gMd3.texNumTest; ++i)
		{
			snprintf(pathname, 255, "data/%s", gMd3.texTest[i].name);
			pathname[255] = 0;

			gMd3.texTest[i].gl_texture_id = gTexture.loadTGA(pathname);

			if (gMd3.texTest[i].gl_texture_id < 0)
			{
				printf("ERROR: Md3 texture '%s' not loaded\n", pathname);
			}
		}
	}
}


void renderScene()
{
	static float lastTime = 0.0f;
	const float size = 500.0f, step = 50.0f;
	float x, y, time;


	gluLookAt(0.0, 0.0, -256.0,
				 0.0, 8.0, 0.0,
				 0.0, 1.0, 0.0);


	glDisable(GL_TEXTURE_2D);


	time = getTicks() * 0.1f;

	if (time - lastTime > 5.0f)
	{
		lastTime = time;

		if (gYawOn)
			++gYaw;
	}

#ifdef DRAW_ACTUAL_ORIGIN_AXIS_WITH_ROTATION
	glPushMatrix();
	glRotatef(yaw, 0, 1, 0);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();
#endif

	glTranslatef(0.0f, -128.0f, 512.0f);
	glRotatef(gPitch, 1, 0, 0);
	glRotatef(gYaw,   0, 1, 0);

	// Draw transformed origin axis
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-30.0f, 0.0f, 0.0f);
	glVertex3f(30.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -30.0f, 0.0f);
	glVertex3f(0.0f, 30.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -30.0f);
	glVertex3f(0.0f, 0.0f, 30.0f);
	glEnd();

	// Draw grid
	glPushMatrix();
	glScalef(2.0f, 2.0f, 2.0f);
	glColor3f(0.4f, 0.4f, 0.6f);

	for (x = -size; x < size; x += step)
	{
		glBegin(GL_LINE_LOOP);
		for (y = -size; y < size; y += step)
		{
			glVertex3f(x + step, 0.0f, y);
			glVertex3f(x, 0.0f, y);
			glVertex3f(x, 0.0f, y + step);
			glVertex3f(x + step, 0.0f, y + step);
		}
		glEnd();
	}
	glPopMatrix();

	// Draw model
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glTranslatef(0.0f, 148.0f, 0.0f);
	glRotatef(-90.0f, 1, 0, 0);
	glScalef(0.1f, 0.1f, 0.1f);
	gMd3.render();
	glPopMatrix();

#ifdef HAVE_SDL_TTF
	/* Render 2d text */
	glEnterMode2d(gWidth, gHeight);
	glColor3f(1.0, 1.0, 1.0);
	glPrint2d(8, 8, 0.90, gMessage);
	glExitMode2d();
#endif
}


void initScene(int argc, char *argv[])
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);    // Q3A uses front face culling
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);


	if (argc > 3)
	{
		loadMd3(argv[1], argv[2], argv[3]);
	}
	else if (argc > 2)
	{
		loadMd3(argv[1], argv[2], "machinegun");
	}
	else if (argc > 1)
	{
		loadMd3(argv[1], "default", "machinegun");
	}
	else
	{
		printf("Usage:\n");
		printf("%s ModelDir [Skin] [WeaponDir]\n", argv[0]);
		exit(0);
	}

#ifdef HAVE_SDL_TTF
	gTexture.loadFontTTF("data/test.ttf", 32, 126 - 32);
	bufferedPrintf(gMessage, 256, "Md3AnimModel Unit Test");
#endif

	printf("\n");
	printf("----------------------------------\n");
	printf("ESC        - Exit\n");
	printf("F1         - Take screenshot\n");
	printf("ALT+ENTER  - Toogle fullscreen\n");
	printf("UP/DOWN    - Adjust scene pitch\n");
	printf("RIGHT/LEFT - Adjust scene yaw\n");
	printf("----------------------------------\n");
	printf("1 - Toggle tag interpolating\n");
	printf("2 - Toggle animation\n");
	printf("3 - Toggle bone rendering\n");
	printf("4 - Toggle weapon rendering\n");
	printf("5 - Toggle mesh interpolating\n");
	printf("6 - Toggle scene rotation\n");
	printf("7 - Toggle rendering warning reporting\n");
	printf("8 - Toggle alpha blending\n");
	printf("w - Toggle wireframe rendering\n");
	printf("[ - Loop through lower animations\n");
	printf("] - Loop through upper animations\n");
	printf("\\ - Loop through both animations\n");
	printf("----------------------------------\n");
}


void updateTitle()
{
	char title[256];


	snprintf(title, 255, "%s %s:%s", gTitle, gAnimUpper, gAnimLower);
	updateWindowTitle(title);
}


void handleKey(int key)
{
	static bool wireframe = false;
	static bool alphaBlend = false;
	static bool lighting = false;

	switch (key)
	{
	case SDLK_RIGHT:
		++gYaw;
		break;
	case SDLK_LEFT:
		--gYaw;
		break;
	case SDLK_UP:
		++gPitch;
		break;
	case SDLK_DOWN:
		--gPitch;
		break;
	case '1':
		gMd3.toggleFlag(Md3AnimModel::fDisableTagInterpolate);
		printf("%sabled tag interpolating\n",
				 (gMd3.getFlags() & Md3AnimModel::fDisableTagInterpolate) ? "Dis" : "En");
		break;
	case '2':
		gMd3.toggleFlag(Md3AnimModel::fAnimate);
		printf("%sabled animation\n",
				 (gMd3.getFlags() & Md3AnimModel::fAnimate) ? "En" : "Dis");
		break;
	case '3':
		gMd3.toggleFlag(Md3AnimModel::fRenderBones);
		printf("%sabled bone rendering\n",
				 (gMd3.getFlags() & Md3AnimModel::fRenderBones) ? "En" : "Dis");
		break;
	case '4':
		gMd3.toggleFlag(Md3AnimModel::fDrawWeapon);
		printf("%sabled weapon rendering\n",
				 (gMd3.getFlags() & Md3AnimModel::fDrawWeapon) ? "En" : "Dis");
		break;
	case '5':
		gMd3.toggleFlag(Md3AnimModel::fDisableMeshInterpolate);
		printf("%sabled mesh interpolating\n",
				 (gMd3.getFlags() & Md3AnimModel::fDisableMeshInterpolate) ? "Dis" : "En");
		break;
	case '6':
		gYawOn = !gYawOn;
		break;
	case '7':
		gMd3.toggleFlag(Md3AnimModel::fRenderingWarnings);
		printf("%sabled rendering warning reporting\n",
				 (gMd3.getFlags() & Md3AnimModel::fRenderingWarnings) ? "En" : "Dis");
		break;
	case '8':
		alphaBlend = !alphaBlend;

		if (alphaBlend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glAlphaFunc(GL_GREATER, 0);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		printf("%sabled alpha blending\n", alphaBlend  ? "En" : "Dis");
		break;
	case '9':
		lighting = !lighting;

		if (lighting)
		{
			static bool firsttime = true;


			glEnable(GL_LIGHTING);

			if (firsttime)
			{
				float specular[] = { 0.7, 0.7, 0.7, 1.0 };
				float ambient[] = { 0.64, 0.64, 0.96, 1.0 };
				float shine[] = { 50.0 };
				float pos[] = { 0.0, 0.0, 64.0, 0.0 };


				// Caustic fx possible with mod amb?
				glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
				glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
				//glMaterialfv(GL_FRONT, GL_DIFFUSE,   ambient);
				glMaterialfv(GL_FRONT, GL_SHININESS, shine);
				glLightfv(GL_LIGHT0,   GL_POSITION,  pos);
				glEnable(GL_LIGHT0);

				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

				//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, WHITE);
				//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, WHITE);
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, DIM_WHITE);
				//glLightf(GL_LIGHT0,  GL_SPOT_CUTOFF,    cutoff);
				//glLightfv(GL_LIGHT0, GL_POSITION,       pos);
				//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
				//glLightfv(GL_LIGHT0, GL_DIFFUSE,        color);

				firsttime = false;
			}
		}
		else
		{
			glDisable(GL_LIGHTING);
		}
		break;
	case '0':
		gMd3.toggleFlag(Md3AnimModel::fUseNormals);
		break;
	case 'w':
		wireframe = !wireframe;

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		printf("%sabled wireframe rendering\n", wireframe ? "En" : "Dis");
		break;
	case '\\':
		switch(gMd3.getAnimLower())
		{
		case BOTH_DEATH1:
			snprintf(gAnimUpper, 32, "BOTH_DEATH1");
			snprintf(gAnimLower, 32, gAnimUpper);
			updateTitle();
			gMd3.setAnimLower(BOTH_DEAD1);
			break;
		case BOTH_DEAD1:
			snprintf(gAnimUpper, 32, "BOTH_DEAD2");
			snprintf(gAnimLower, 32, gAnimUpper);
			updateTitle();
			gMd3.setAnimLower(BOTH_DEATH2);
			break;
		case BOTH_DEATH2:
			snprintf(gAnimUpper, 32, "BOTH_DEATH2");
			snprintf(gAnimLower, 32, gAnimUpper);
			updateTitle();
			gMd3.setAnimLower(BOTH_DEAD2);
			break;
		case BOTH_DEAD2:
			snprintf(gAnimUpper, 32, "BOTH_DEAD2");
			snprintf(gAnimLower, 32, gAnimUpper);
			updateTitle();
			gMd3.setAnimLower(BOTH_DEATH3);
			break;
		case BOTH_DEATH3:
			snprintf(gAnimUpper, 32, "BOTH_DEATH3");
			snprintf(gAnimLower, 32, gAnimUpper);
			gMd3.setAnimLower(BOTH_DEAD3);
			break;
		case BOTH_DEAD3:
			snprintf(gAnimUpper, 32, "BOTH_DEAD3");
			snprintf(gAnimLower, 32, gAnimUpper);
			gMd3.setAnimLower(BOTH_DEATH1);
			break;
		default:
			snprintf(gAnimUpper, 32, "BOTH_DEATH1");
			snprintf(gAnimLower, 32, gAnimUpper);
			gMd3.setAnimLower(BOTH_DEATH1);
		}
		break;
	case ']':
		switch(gMd3.getAnimUpper())
		{
		case TORSO_GESTURE:
			gMd3.setAnimUpper(TORSO_ATTACK);
			break;
		case TORSO_ATTACK:
			gMd3.setAnimUpper(TORSO_ATTACK2);
			break;
		case TORSO_ATTACK2:
			gMd3.setAnimUpper(TORSO_DROP);
			break;
		case TORSO_DROP:
			gMd3.setAnimUpper(TORSO_RAISE);
			break;
		case TORSO_RAISE:
			gMd3.setAnimUpper(TORSO_STAND);
			break;
		case TORSO_STAND:
			gMd3.setAnimUpper(TORSO_STAND2);
			break;
		case TORSO_STAND2:
			gMd3.setAnimUpper(TORSO_GESTURE);
			break;
		default:
			gMd3.setAnimUpper(TORSO_STAND);
		}
		break;
	case '[':
		switch(gMd3.getAnimLower())
		{
		case LEGS_WALKCR:
			gMd3.setAnimLower(LEGS_WALK);
			break;
		case LEGS_WALK:
			gMd3.setAnimLower(LEGS_RUN);
			break;
		case LEGS_RUN:
			gMd3.setAnimLower(LEGS_BACK);
			break;
		case LEGS_BACK:
			gMd3.setAnimLower(LEGS_SWIM);
			break;
		case LEGS_SWIM:
			gMd3.setAnimLower(LEGS_JUMP);
			break;
		case LEGS_JUMP:
			gMd3.setAnimLower(LEGS_LAND);
			break;
		case LEGS_LAND:
			gMd3.setAnimLower(LEGS_JUMPB);
			break;
		case LEGS_JUMPB:
			gMd3.setAnimLower(LEGS_LANDB);
			break;
		case LEGS_LANDB:
			gMd3.setAnimLower(LEGS_IDLE);
			break;
		case LEGS_IDLE:
			gMd3.setAnimLower(LEGS_IDLECR);
			break;
		case LEGS_IDLECR:
			gMd3.setAnimLower(LEGS_TURN);
			break;
		case LEGS_TURN:
			gMd3.setAnimLower(LEGS_WALKCR);
			break;
		default:
			gMd3.setAnimLower(LEGS_IDLE);
		}
		break;
	}
}


///////////////////////////////////////////////////

SDL_Surface *gSDLWindow = NULL;


void updateWindowTitle(char *title)
{
  SDL_WM_SetCaption(title,"Md3AnimModel Test");
}


unsigned int getTicks()
{
	return SDL_GetTicks();
}


void swap_buffers()
{
	SDL_GL_SwapBuffers();
}


void event_resize(int width, int height)
{
	GLfloat aspect;

	aspect = (GLfloat)width/(GLfloat)height;

	gWidth = width;
	gHeight = height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 8.0f, 4600.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void event_display(int width, int height)
{
	glClearColor(0.3, 0.3, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	renderScene();

	glFlush();
	swap_buffers();
}


void shutdown_gl()
{
	SDL_Quit();
}


void init_gl(unsigned int width, unsigned int height)
{
	// Print driver support information
	printf("\n\n\t## GL Driver Info 4 ##\n");
	printf("\tVendor     : %s\n", glGetString(GL_VENDOR));
	printf("\tRenderer   : %s\n", glGetString(GL_RENDERER));
	printf("\tVersion    : %s\n", glGetString(GL_VERSION));
	printf("\tExtensions : %s\n\n\n", (char*)glGetString(GL_EXTENSIONS));

	// Setup GL
	glClearColor(0.3, 0.3, 0.5, 1.0);
	event_resize(width, height);

	// Texture setup
	gTexture.reset();
	gTexture.setFlag(Texture::fUseMipmaps);
	gTexture.setMaxTextureCount(64);

}


int main_gl(int argc, char *argv[])
{
  SDL_Event event;
  unsigned int mkeys, mod, key, flags;
  unsigned int width = 640;
  unsigned int height = 460;
  bool fullscreen = false;
  char *driver = 0x0;


  // Setup clean up on exit
  atexit(shutdown_gl);

  // NOTE: Removed fullscreen/driver option parser args

  // Create GL context
  SDL_Init(SDL_INIT_VIDEO);
  printf("\n@Created OpenGL Context...\n");

  if (!driver || !driver[0] || SDL_GL_LoadLibrary(driver) < 0)
  {
	  SDL_ClearError();

	  // Fallback 1
	  if (SDL_GL_LoadLibrary("libGL.so") < 0)
	  {
		  SDL_ClearError();

		  // Fallback 2
		  if (SDL_GL_LoadLibrary("libGL.so.1") < 0)
		  {
			  fprintf(stderr, "main_gl> SDL_GL_LoadLibrary failed!\n");
			  fprintf(stderr, "main_gl> Error is [%s].\n", SDL_GetError());
			  exit(1);
		  }
	  }
  }

  flags = SDL_OPENGL;

  if (fullscreen)
  {
	  flags |= SDL_FULLSCREEN;
	  SDL_ShowCursor(SDL_DISABLE);
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  gSDLWindow = SDL_SetVideoMode(width, height, 16, flags);
  SDL_WM_SetCaption("Md3AnimModel Test", "Md3AnimModel Test");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  // Init rendering
  init_gl(width, height);
  initScene(argc, argv);

  printf("\n@Starting event loop...\n");

  for (;;)
  {
	  while (SDL_PollEvent(&event))
	  {
		  switch (event.type)
		  {
		  case SDL_QUIT:
			  exit(0);
			  break;
		  case SDL_MOUSEMOTION:
			  break;
		  case SDL_MOUSEBUTTONDOWN:
		  case SDL_MOUSEBUTTONUP:
			  break;
		  case SDL_KEYDOWN:
			  mkeys = (unsigned int)SDL_GetModState();
			  mod = 0;

			  if (mkeys & KMOD_LSHIFT)
				  mod |= KMOD_LSHIFT;

			  if (mkeys & KMOD_RSHIFT)
				  mod |= KMOD_RSHIFT;

			  if (mkeys & KMOD_LCTRL)
				  mod |= KMOD_LCTRL;

			  if (mkeys & KMOD_RCTRL)
				  mod |= KMOD_RCTRL;

			  if (mkeys & KMOD_LALT)
				  mod |= KMOD_LALT;

			  if (mkeys & KMOD_RALT)
				  mod |= KMOD_RALT;

			  key = event.key.keysym.sym;

			  switch (key)
			  {
			  case SDLK_F1:
				  gTexture.glScreenShot("Md3AnimModel.Test", width, height);
				  break;
			  case SDLK_ESCAPE: // 0x1B, 27d, ESC
				  exit(0);
				  break;
			  case SDLK_RETURN:
				  if (mod & KMOD_LALT)
				  {
					  SDL_ShowCursor(SDL_DISABLE);
					  SDL_WM_ToggleFullScreen(gSDLWindow);
				  }
			  }

			  handleKey(key);
			  break;
		  case SDL_KEYUP:
			  break;
		  case SDL_VIDEORESIZE:
			  event_resize(event.resize.w, event.resize.h);

			  width = event.resize.w;
			  height = event.resize.h;
			  event_display(width, height);
			  break;
		  }
	  }

	  event_display(width, height);
  }

  return 0;
}


int main(int argc, char *argv[])
{
	main_gl(argc, argv);
	return 0;
}
#endif

#ifdef UNIT_TEST_MD3ANIMMODEL
int main(int argc, char *argv[])
{
	Md3AnimModel md3;
	unsigned int i;


	printf("[MD3AnimModel class test]\n");

	if (argc > 3)
	{
		if (!md3.load(argv[2], argv[3], (md3_lod_t)atoi(argv[1])))
		{
			for (i = 0; i < md3.texNumTest; ++i)
			{
				printf("Depends: Texture '%s'\n", md3.texTest[i].name);
			}

			printf("main: load() reports success.\n");
		}

		if (argc > 5)
		{
			if (!md3.loadWeapon(argv[4], argv[6]))
			{
				printf("main: weaponLoad() reports success.\n");
			}
		}
	}
	else
	{
		printf("\n\n%s LoD path skin_name\neg %s 0 /usr/local/games/quake3/qbase3/players/slash/ default [weapon_path weapon_name]\n",
				 argv[0], argv[0]);
	}
}
#endif
