/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : World
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: The game world ( model )
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History -------------------------------------------------
 *
 * 2002.12.16:
 * Mongoose - Created
 =================================================================*/

#ifdef DEBUG_MEMORY
#include <memory_test.h>
#endif

#include <math.h>

#include <World.h>


////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////

World::World()
{
	mClearLock = false;
	mFlags = 0;
	mEntities.setError(0x0);
	mRooms.setError(0x0);
	mMeshes.setError(0x0);
	mSprites.setError(0x0);
	mModels.setError(0x0);
}


World::~World()
{
	destroy();
}


////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////

int World::getRoomByLocation(int index, float x, float y, float z)
{
	room_mesh_t *room = mRooms[index];

	if (room)
	{
		if (x > room->bbox_min[0] && x < room->bbox_max[0] &&
			 z > room->bbox_min[2] && z < room->bbox_max[2])
		{
			if (y > room->bbox_min[1] && y < room->bbox_max[1])
				return index;
		}
	}

	return getRoomByLocation(x, y, z);
}


int World::getRoomByLocation(float x, float y, float z)
{
	room_mesh_t *room;
	int hop = -1;


	for (mRooms.start(); mRooms.forward(); mRooms.next())
	{
		room = mRooms.current();

		if (!room)
			continue;

		if (x > room->bbox_min[0] && x < room->bbox_max[0] &&
			 z > room->bbox_min[2] && z < room->bbox_max[2])
		{
			// This room contains current position
			if (y > room->bbox_min[1] && y < room->bbox_max[1])
				return mRooms.getCurrentIndex();

			// This room is above or below current position
			hop = mRooms.getCurrentIndex();
		}
  }

	// Room is -1?  Must be in void, try to hop to room with same X,Z
	if (mFlags & fEnableHopping)
		return hop;

	return -1;
}


int World::getAdjoiningRoom(int index,
									 float x, float y, float z,
									 float x2, float y2, float z2)
{
	room_mesh_t *room = mRooms[index];
	portal_t * portal;
	vec3_t intersect, p1, p2;


	p1[0] = x;	p1[1] = y;	p1[2] = z;
	p2[0] = x2;	p2[1] = y2;	p2[2] = z2;

	if (room)
	{
		for (room->portals.start(); room->portals.forward();
			  room->portals.next())
		{
			portal = room->portals.current();

			if (!portal)
				continue;

			if (helIntersectionLineAndPolygon(intersect, p1, p2, // 4,
														 portal->vertices))
			{
				return portal->adjoining_room;
			}
		}
	}

	return -1;
}


int World::getSector(int room, float x, float z, float *floor, float *ceiling)
{
	room_mesh_t *r;
	sector_t * s;
	int sector;


	r = mRooms[room];

	if (!r)
		return -1;

	sector = (((((int)x - (int)r->pos[0]) / 1024) * r->numZSectors) +
				 (((int)z - (int)r->pos[2]) / 1024));

	if (sector > -1)
	{
		s = r->sectors[sector];

		if (!s)
			return -1;

		*floor = s->floor;
		*ceiling = s->ceiling;
	}

	return sector;
}


int World::getSector(int room, float x, float z)
{
	int sector;
	room_mesh_t *r;


	r = mRooms[room];

	if (!r)
	{
		return -1;
	}

	sector = (((((int)x - (int)r->pos[0]) / 1024) * r->numZSectors) +
				 (((int)z - (int)r->pos[2]) / 1024));

	if (sector < 0)
	{
		return -1;
	}

	return sector;
}


unsigned int World::getRoomInfo(int room)
{
	room_mesh_t *r;


	r = mRooms[room];

	if (!r)
	{
		return 0;
	}

	return r->flags;
}


bool World::isWall(int room, int sector)
{
	room_mesh_t *r;
	sector_t *sect;


	r = mRooms[room];

	if (!r)
	{
		return true;
	}

	sect = r->sectors[sector];

	if (!sect)
	{
		return true;
	}

	return (sector > 0 && sect->wall);
}


bool World::getHeightAtPosition(int index, float x, float *y, float z)
{
	room_mesh_t *room = mRooms[index];

#ifdef OBSOLETE_USING_BOXES
	unsigned int i;
	float zmax, xmax, zmin, xmin;


	if (!room)
	{
		return false;
	}

	// Mongoose 2002.08.14, It's 0302 - give me a fucking break --
	//   this works albeit poorly  =)
	for (i = 0; (int)i < room->num_boxes; ++i)
	{
		xmax = room->boxes[i].c.pos[0];
		xmin = room->boxes[i].a.pos[0];
		zmax = room->boxes[i].c.pos[2];
		zmin = room->boxes[i].a.pos[2];

		if (x < xmax && x > xmin && z < zmax && z > zmin)
		{
			//printf("%f %f %f %f\n", xmax, xmin, zmax, zmin);

			*y =  room->boxes[i].a.pos[1]; // hhmm...room->pos[1] +
			return true;
		}
	}

	return false;
#else
	int sector;
	sector_t *sect;


	if (!room)
	{
		return false;
	}

	// Mongoose 2002.08.14, Remember sector_z is width of sector array
	sector = getSector(index, x, z);

	sect = room->sectors[sector];

	if (!sect)
	{
		return true;
	}

	*y = sect->floor;

	return true;
#endif
}


// Temp methods for rendering use until more refactoring is done
#ifdef BAD_BLOOD
model_mesh_t *World::getMesh(int index)
{
	return mMeshes[index];
}

skeletal_model_t *World::getModel(int index)
{
	return mModels[index];
}

room_mesh_t *World::getRoom(int index)
{
	return mRooms[index];
}

Vector<entity_t *> *World::getEntities()
{
	return &mEntities;
}

Vector<sprite_seq_t *> *World::getSprites()
{
	return &mSprites;
}

Vector<room_mesh_t *> *World::getRooms()
{
	return &mRooms;
}
#endif


////////////////////////////////////////////////////////////
// Public Mutators
////////////////////////////////////////////////////////////

void World::setFlag(WorldFlag flag)
{
	mFlags |= flag;
}



void World::clearFlag(WorldFlag flag)
{
	mFlags |= flag;
	mFlags ^= flag;
}


void World::destroy()
{
	// Add some locking to check use state first
	if (!mClearLock)
	{
		clear();
	}
}


void World::addRoom(room_mesh_t *room)
{
	mClearLock = false;
	mRooms.pushBack(room);
}


void World::addMesh(model_mesh_t *mesh)
{
	if (mesh)
	{
		mClearLock = false;
		mMeshes.pushBack(mesh);
	}
}


void World::addEntity(entity_t *e)
{
	if (e)
	{
		mClearLock = false;
		e->master = 0x0;
		e->moveType = worldMoveType_walk; // Walk
		e->room = getRoomByLocation(e->pos[0], e->pos[1], e->pos[2]);
		mEntities.pushBack(e);
	}
}


int World::addModel(skeletal_model_t *model)
{
	if (model)
	{
		mClearLock = false;
		mModels.pushBack(model);
		return mModels.end();
	}

	return -1;
}


void World::addSprite(sprite_seq_t *sprite)
{
	if (sprite)
	{
		mClearLock = false;
		mSprites.pushBack(sprite);
	}
}


void World::moveEntity(entity_t *e, char movement)
{
	const float moved = 180.0f;
	const float testd = 220.0f;
	const float camHeight = 8.0f;
	float x, y, z, pitch, h, floor, ceiling;
	int room, sector;
	bool wall;
	unsigned int roomFlags;


	if (!e)
	{
		return;
	}

	switch (e->moveType)
	{
	case worldMoveType_walkNoSwim:
	case worldMoveType_walk:
		pitch = 0.0f;  // in the future pitch could control jump up blocks here
		break;

    case worldMoveType_noClipping:
    case worldMoveType_fly:
    case worldMoveType_swim:
	default:
		pitch = e->angles[2];
        break;
	}

	switch (movement)
	{
	case 'f':
		x = e->pos[0] + (testd * sin(e->angles[1]));
		y = e->pos[1] + (testd * sin(pitch));
		z = e->pos[2] + (testd * cos(e->angles[1]));
		break;
	case 'b':
		x = e->pos[0] - (testd * sin(e->angles[1]));
		y = e->pos[1] - (testd * sin(pitch));
		z = e->pos[2] - (testd * cos(e->angles[1]));
		break;
	case 'l':
		x = e->pos[0] - (testd * sin(e->angles[1] + 90.0));
		y = e->pos[1];
		z = e->pos[2] - (testd * cos(e->angles[1] + 90.0));
		break;
	case 'r':
		x = e->pos[0] + (testd * sin(e->angles[1] + 90.0));
		y = e->pos[1];
		z = e->pos[2] + (testd * cos(e->angles[1] + 90.0));
		break;
	default:
		return;
	}

	//room = getRoomByLocation(x, y, z);
	room = getRoomByLocation(e->room, x, y, z);

	if (room == -1) // Will we hit a portal?
	{
#define ADJ_ROOM_CHECK
#ifdef ADJ_ROOM_CHECK
		room = getAdjoiningRoom(e->room,
										e->pos[0],  e->pos[1], e->pos[2],
										x, y, z);
#else
		if (!mFlags & fEnableHopping)
		{
			mFlags |= fEnableHopping;
			room = getRoomByLocation(e->room, x, y, z);
			printf("Hopped\n");
			mFlags ^= fEnableHopping;
		}

		//room = getRoomByLocation(x, y, z);
#endif

		if (room > -1)
		{
			printf("Crossing from room %i to %i\n", e->room, room);
		}
	}

	roomFlags = getRoomInfo(room);
	sector = getSector(room, x, z, &floor, &ceiling);
	wall = isWall(room, sector);

	// If you're underwater you may want to swim  =)
	// ...if you're worldMoveType_walkNoSwim, you better hope it's shallow
	if (roomFlags & roomFlag_underWater && e->moveType == worldMoveType_walk)
	{
		e->moveType = worldMoveType_swim;
	}

	// Don't swim on land
	if (!(roomFlags & roomFlag_underWater) && e->moveType == worldMoveType_swim)
	{
		e->moveType = worldMoveType_walk;
	}

	// Mongoose 2002.09.02, Add check for room -> room transition
	//   ( Only allow by movement between rooms by using portals )
	if (((e->moveType == worldMoveType_noClipping) ||
		  (e->moveType == worldMoveType_fly) ||
		  (e->moveType == worldMoveType_swim)) ||
		 ((room > -1) && (!wall)))
	{
		e->room = room;

		switch (movement)
		{
		case 'f':
			x = e->pos[0] + (moved * sin(e->angles[1]));
			y = e->pos[1] + (moved * sin(pitch));
			z = e->pos[2] + (moved * cos(e->angles[1]));
			break;
		case 'b':
			x = e->pos[0] - (moved * sin(e->angles[1]));
			y = e->pos[1] - (moved * sin(pitch));
			z = e->pos[2] - (moved * cos(e->angles[1]));
			break;
		case 'l':
			x = e->pos[0] - (moved * sin(e->angles[1] + 90.0));
			z = e->pos[2] - (moved * cos(e->angles[1] + 90.0));
			break;
		case 'r':
			x = e->pos[0] + (moved * sin(e->angles[1] + 90.0));
			z = e->pos[2] + (moved * cos(e->angles[1] + 90.0));
			break;
		}

		/*! \fixme Test for vector (move vector) / plane (portal) collision here
		 * to see if we need to switch rooms... man...
         */

		h = y;
		getHeightAtPosition(room, x, &h, z);

		switch (e->moveType)
		{
		case worldMoveType_fly:
		case worldMoveType_swim:
#ifdef DIVE_GAP
			// Clips to top of water, waiting for DIVE event
			if (h < floor)
				e->pos[1] = floor;
			else if (h > ceiling)
				e->pos[1] = ceiling;
			else
				e->pos[1] = y;
#endif

			// Don't fall out of world, avoid a movement that does
			if (h > y - camHeight)
			{
				e->pos[0] = x;
				e->pos[1] = y;
				e->pos[2] = z;
			}
			break;
		case worldMoveType_walk:
		case worldMoveType_walkNoSwim:
			y = e->pos[1];  // Override vector movement walking ( er, not pretty )

			// Now fake gravity
			// Mongoose 2002.08.14, Remember TR is upside down ( you fall 'up' )

			//ddist = h - e->pos[1];

			// This is to force false gravity, by making camera stay on ground
			e->pos[1] = h; //roomFloor->bbox_min[1];

			// Check for camera below terrian and correct
			if (e->pos[1] < h - camHeight)
			{
				e->pos[1] = h - camHeight;
			}

			e->pos[0] = x;
			e->pos[2] = z;
			break;
		case worldMoveType_noClipping:
			e->pos[0] = x;
			e->pos[1] = y;
			e->pos[2] = z;
		}

#ifdef OBSOLETE
		m_text->SetString(1,"Room %2i  Sector %2i %sPos %.0f %.0f %.0f Yaw %.0f",
								room, sector,
								wall ? " Wall " : " ",
								e->pos[0], e->pos[1], e->pos[2], e->angles[1]);
#endif
	}
	else
	{
		e->moving = false;
		return;
	}

	e->room = room;
	e->moving = true;
}


////////////////////////////////////////////////////////////
// Private Accessors
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Private Mutators
////////////////////////////////////////////////////////////

void World::clear()
{
	room_mesh_t *room;
	model_mesh_t *mesh;
	sprite_seq_t *sprite;
	skeletal_model_t *model;
	bone_frame_t *boneframe;
	bone_tag_t *tag;
	animation_frame_t *animation;
	List<skeletal_model_t *> cache;


	mClearLock = true;

	mEntities.erase();

	for (mRooms.start(); mRooms.forward(); mRooms.next())
	{
		room = mRooms.current();

		if (room)
		{
			room->portals.erase();
			room->models.erase();
			room->sprites.erase();
			room->sectors.erase();
			room->boxes.erase();
		}
	}

	mRooms.erase();

	for (mMeshes.start(); mMeshes.forward(); mMeshes.next())
	{
		mesh = mMeshes.current();

		if (!mesh)
			continue;

		for (mesh->texturedTriangles.start();
			  mesh->texturedTriangles.forward();
			  mesh->texturedTriangles.next())
		{
			if (mesh->texturedTriangles.current())
				delete mesh->texturedTriangles.current();
		}

		for (mesh->coloredTriangles.start(); mesh->coloredTriangles.forward();
			  mesh->coloredTriangles.next())
		{
			if (mesh->coloredTriangles.current())
				delete mesh->coloredTriangles.current();
		}

		for (mesh->texturedRectangles.start();
			  mesh->texturedRectangles.forward();
			  mesh->texturedRectangles.next())
		{
			if (mesh->texturedRectangles.current())
				delete mesh->texturedRectangles.current();
		}

		for (mesh->coloredRectangles.start(); mesh->coloredRectangles.forward();
			  mesh->coloredRectangles.next())
		{
			if (mesh->coloredRectangles.current())
				delete mesh->coloredRectangles.current();
		}

		if (mesh->vertices)
			delete [] mesh->vertices;

		if (mesh->colors)
			delete [] mesh->colors;

		if (mesh->normals)
			delete [] mesh->normals;

		delete mesh;
	}

	mMeshes.clear();

	for (mSprites.start(); mSprites.forward(); mSprites.next())
	{
		sprite = mSprites.current();

		if (!sprite)
			continue;

		if (sprite->sprite)
			delete [] sprite->sprite;

		delete sprite;
	}

	mSprites.clear();

	for (mModels.start(); mModels.forward(); mModels.next())
	{
		model = mModels.current();

		if (!model)
			continue;

		// No smart pointers, so skip if deleted once  =)
		if (!cache.SearchKey(model))
		{
			cache.Add(model);
		}
		else
		{
			continue;
		}

		for (model->animation.start(); model->animation.forward();
			  model->animation.next())
		{
			animation  = model->animation.current();

			if (!animation)
				continue;

			for (animation->frame.start(); animation->frame.forward();
				  animation->frame.next())
			{
				boneframe = animation->frame.current();

				if (!boneframe)
					continue;

				for (boneframe->tag.start(); boneframe->tag.forward();
					  boneframe->tag.next())
				{
					tag = boneframe->tag.current();

					if (!tag)
						continue;

					delete tag;
				}

				delete boneframe;
			}

			delete animation;
		}

		delete model;
	}

	mModels.clear();
}

