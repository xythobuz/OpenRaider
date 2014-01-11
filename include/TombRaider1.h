/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : OpenRaider
 * Author  : Mongoose
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : OpenRaider
 * License : GPL, See file COPYING for details
 * Comments: Loads TR 1 states and etc
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * Mongoose - Created, based on:
 *                TR Rosetta Stone ( TombRaider pak format specs )
 ================================================================*/

#ifndef _TOMBRAIDER1_H_
#define _TOMBRAIDER1_H_


class TombRaider1
{
public:

	TombRaider1();

	~TombRaider1();

	enum Items
   {
		Lara = 0,
		PistolAnimation  = 1,
		ShotgunAnimation = 2,
		MagnumAnimation = 3,
		LaraAlternate = 4, // Lara's home appearance, wounded, or turned to gold
		UziAnimation = 5,
		LaraMutant = 6,
		Wolf = 7,
		Bear = 8,
		Bat = 9
	};

	enum WolfStates
	{
		WolfState_Walking   = 1,
		WolfState_Running   = 2,
		WolfState_Jumping   = 3,
		WolfState_Stalking  = 5,
		WolfState_JumpingAttack   = 6,
		WolfState_Attacking = 7,
		WolfState_Lying     = 8,  // down
		WolfState_Getting   = 9,  // ready to strike
		WolfState_RunningJump   = 10,
		WolfState_Dying     = 11,
		WolfState_Biting    = 12
	};
};

#ifdef FIXME_DONT_COMPILE
//  Bear = 8,
//         Walking = 0, on all fours
//         Getting = 1, back to all fours
//         Walking = 2, on hind legs
//         Running = 3, on all fours
//         Rearing = 4, up on hind legs
//         Growling = 5,?
//         Running = 6, and attacking
//         Standing = 7, on hind legs
//         Biting = 8,
//         Dying = 9,
//  Bat = 9,
//         Starting = 1, to fly
//         Flying = 2, straight
//         Biting = 3,
//         Circling = 4,
//         Dying = 5,
// Crocodile = 10, (on land)
//         Stationary = 1,
//         Walking = 2,
//         Walking = 3,
//         Turning = 4,
//         Biting = 5,
//         Dying = 7,
// Crocodile = 11, (in water)
//         Swimming = 1,
//         Biting = 2,
//         Dying = 3,
// Lion = 12, (male)
//         Standing = 1,
//         Walking = 2,
//         Leaping = 3,
//         Leaping = 4, and biting
//         Dying = 5,
//         Biting = 6,
//         Biting = 7,
// Lion = 13, (female)
//         Standing = 1,
//         Walking = 2,
//         Leaping = 3,
//         Leaping = 4, and biting
//         Dying = 5,
//         Biting = 6,
//         Biting = 7,
// Panther = 14,
//         Standing = 1,
//         Walking = 2,
//         Leaping = 3,
//         Leaping = 4, and biting
//         Dying = 5,
//         Biting = 6,
//         Biting = 7,
// Gorilla = 15,
//         Standing = 1, on all fours
//         Running = 3, on all fours
//        Walking = 4, on legs (attacking?)
//         Dying = 5,
//         Thumping = 6, chest
//         Waving = 7, arms
//         Turning = 8, leftward?
//         Turning = 9, rightward?
//        Jumping = 10, up and waving arms
//        Climbing = 11,
// Giant = 16, Rat (on land)
//         Standing = 1,
//         Jumping = 2, and Biting
//         Running = 3,
//         Biting = 4,
//         Dying = 5,
//         Rearing = 6, up
// Giant = 17, Rat (in water)
//         Swimming = 1,
//         Biting = 2,
//         Dying = 3,
// Tyrannosaur = 18,
//        Standing = 1,
//         Walking = 2,
//         Running = 3,
//         Dying = 5,
//         Bellowing = 6,
//         Biting = 7,
//         Shaking = 8, Head and Spitting Out
// Raptor = 19,
//         Dying = 0,
//         Standing = 1,
//         Walking = 2,
//         Running = 3,
//         Jumping = 4, and Biting?
//         Bellowing = 6,
//         Running = 7, and bellowing
//         Biting = 8,
// Winged = 20, mummy (unused) / Winged mutant
//         Crouching = 1,
//         Walking = 2,
//         Running = 3,
//         Biting = 4,?
//         Looking = 6,
//         Jumping = 7,?
//         Clawing = 8,?
//         Aiming = 9, right-hand gun
//        Aiming = 10, and firing left-hand gun
//        Firing = 11, right-hand gun
//        Standing = 12,
//        Flying = 13,
// Lara = 21,'s hips
//         [Only one state]
// Lara = 22,'s hips
//         [Only one state]
// Centaur = 23, mutant
//         Standing = 1,
//         Firing = 2,
//         Galloping = 3,
//         Aiming = 4,
//         Dying = 5,
//         Rearing = 6, up
// Mummy = 24,
//         Standing = 1,
//         Falling = 2, forward
// Larson = 27,
//         Walking = 0,
//         Standing = 1, holding gun
//         Walking = 2,
//         Running = 3,
//         Aiming = 4, gun
//         Injured = 5, by gunshot / Dying
//         Standing = 6,
//         Firing = 7, gun
// Pierre = 28,
//         Standing = 1,
//         Walking = 2,
//         Running = 3,
//         Aiming = 4, guns
//         Dying = 5,
//         Putting = 6, guns away
//         Firing = 7, guns
// Skateboard = 29,
//         Being = 0, turned around
//         Stationary = 1,
//         Stationary = 2,
//         Stationary = 3,
//         Stationary = 4,
// Skateboard = 30, kid
//         Turning = 0, and Aiming?
//         Firing = 1,
//         Skating = 2,
//         Aiming = 3,
//         Firing = 4,
//         Dying = 5,
// Cowboy = 31,
//         Aiming = 1,
//         Walking = 2,
//         Running = 3,
//         Aiming = 4,
//         Dying = 5,
//         Firing = 6,
//  32: "Mr. T"
//         Dying = 0,
//         Standing = 1,
//         Walking = 2,
//         Running = 3,
//         Aiming = 4,
//         Firing = 6,
// Winged = 33, Natla (actually, Natla with a winged mutant)
//         Standing = 1,
//         Flying = 2,
//         Running = 3,
//         Aiming = 4, and firing
//          5: "Dying" the first time
//         Spinning = 7, around in air
//         Standing = 8,
//         Dying = 9, for real
// Giant = 34, mutant
//         Dying = 0,
//         Sitting = 1, on floor
//         Pulling = 2, self forward
//         Slapping = 4, with right hand
//         Slapping = 5, with both hands
//         Making = 6, big wave with right hand
//         Dropping = 8, to floor after hatching
//         Raising = 9, arms
//         Shaking = 11, victim with right hand
// Collapsible = 35, floor
//         Stationary = 0,
//         Shaking = 1,
//         Falling = 2,
//         Settling = 3, down
// Swinging = 36, blade
//         Stationary = 0,
//         Swinging = 2,
// Spikes = 37,
//         [Only one state]
// Boulder = 38,
//         Stationary = 0,
//         Rolling = 1,
// Dart = 39,
//         [Only one state]
// Wall = 40,-mounted dartgun
//         Idle = 0,?
//         Firing = 1,?
// Door = 41, (opens upward)
//         Upward = 0,
//         On = 1, side
// Slamming = 42, doors
//         Open = 0,
//         Closed = 1,
// Sword = 43, of Damocles
//         [Only one state]
// Thor = 44,'s hammer's handle
//         Stationary = 0, in "up" position
//         Moving = 1, down a little and returning
//         Moving = 2, down all the way
//         Stopped = 3, at "down" position
// Thor = 45,'s hammer's block
//         Stationary = 0,
//         Moving = 1, down a little and returning
//         Moving = 2, down all the way
// Hanging = 46, ball? / Some kind of box?
//         [Only one state]
// Metal = 47, rod? / Powered mining cart
//         Stationary = 0,
//         Moving = 1,
// Movable = 48, cubical block (pushable)
//         Stationary = 0,
//         Being = 1, pulled?
//         Being = 2, pushed?
// Movable = 49, cubical block (pushable)
//         Stationary = 0,
//         Being = 1, pulled?
//         Being = 2, pushed?
// Movable = 50, cubical block (pushable)
//         Stationary = 0,
//         Being = 1, pulled?
//         Being = 2, pushed?
// Movable = 51, cubical block (pushable)
//         Stationary = 0,
//         Being = 1, pulled?
//         Being = 2, pushed?
// Movable = 52, tall block
//         Stationary = 0,
//         Moving = 1, forward?
//         Moving = 2, backward?
// Pieces = 53, of something?
//         Stationary = 0,
//         Falling = 1,
//         Settling = 2, down
// Sword = 54, of Damocles
//         [Only one state]
// Above = 55,-water switch
//         Off = 0,
//         On = 1,
//          [states may be reversed]
// Underwater = 56, switch
//         Off = 0,
//         On = 1,
//          [states may be reversed]
// Door = 57,
//         Closed = 0,
//         Open = 1,
// Door = 58,
//         Closed = 0,
//         Open = 1,
// Door = 59,
//         Closed = 0,
//         Open = 1,
// Door = 60,
//         Closed = 0,
//         Open = 1,
// Door = 61,
//         Closed = 0,
//         Open = 1,
// Door = 62,
//         Closed = 0,
//         Open = 1,
// Door = 63,
//         Closed = 0,
//         Open = 1,
// Door = 64,
//         Closed = 0,
//         Open = 1,
// Trapdoor = 65, (opens downward)
//         Closed = 0,
//         Open = 1,
// Trapdoor = 66, (opens downward)
//         Closed = 0,
//         Open = 1,
// Bridge = 68, (flat)
//         [Only one state]
// Bridge = 69, (slope = 1)
//         [Only one state]
// Bridge = 70, (slope = 2)
//         [Only one state]
// Passport = 71, (opening up)
//         [Only one state]
// Compass = 72,
//         [Only one state]
// Cogs = 74, (animated)
//         Stationary = 0,
//         Turning = 1,
// Cogs = 75, (animated)
//         Stationary = 0,
//         Turning = 1,
// Cogs = 76, (animated)
//         Stationary = 0,
//         Turning = 1,
// Lara = 77, in CS / Scion holder in CS
// Larson = 78, in CS / Natla in CS / Scion holder in CS
// Larson = 79,'s gun in CS / Scion in CS / Natla in CS
// Scion = 80, in CS
// Passport = 81, (closed)
//         [Only one state]
// N = 82,-thingy (Playstation memory card?)
//         [Only one state]
// Save = 83, crystal
//         [Only one state]
//  84: * Pistols
//  85: * Shotgun
//  86: * Magnums
//  87: * Uzis
//  88: * Pistol ammo(?)
//  89: * Shotgun ammo
//  90: * Magnum ammo
//  91: * Uzi ammo
//  93: * Small medipack
//  94: * Large medipack
// Sunglasses = 95,
//         [Only one state]
// Cassette = 96, player and headphones
//         [Only one state]
// Direction = 97, keys
//         [Only one state]
// Pistol = 99,
//         [Only one state]
// 100: Shotgun
//         [Only one state]
// 101: Magnum
//         [Only one state]
// 102: Uzi
//         [Only one state]
// 103: Pistol ammo(?)
//         [Only one state]
// 104: Shotgun ammo
//         [Only one state]
// 105: Magnum ammo
//         [Only one state]
// 106: Uzi ammo
//         [Only one state]
// 108: Small medipack
//         [Only one state]
// 109: Large medipack
//         [Only one state]
// 110: * Puzzle 1
// 111: * Puzzle 2
// 112: * Puzzle 3
// 113: * Puzzle 4
// 114: Puzzle 1
//         [Only one state]
// 115: Puzzle 2
//         [Only one state]
// 116: Puzzle 3
//         [Only one state]
// 117: Puzzle 4
//         [Only one state]
// 118: Slot 1 empty
//         [Only one state]
// 119: Slot 2 empty
//         [Only one state]
// 120: Slot 3 empty
//         [Only one state]
// 121: Slot 4 empty
//         [Only one state]
// 122: Slot 1 full
//         [Only one state]
// 123: Slot 2 full
//         [Only one state]
// 124: Slot 3 full
//         [Only one state]
// 125: Slot 4 full
//         [Only one state]
// 126: * Pickup 1
// 127: Pickup 1
//         [Only one state]
// 128: Lara's hips
//         [Only one state]
// 129: * Key 1
// 130: * Key 2
// 131: * Key 3
// 132: * Key 4
// 133: Key 1
//         [Only one state]
// 134: Key 2
//         [Only one state]
// 135: Key 3
//         [Only one state]
// 136: Key 4
//         [Only one state]
// 137: Lock 1
//         [Only one state]
// 138: Lock 2
//         [Only one state]
// 139: Lock 3
//         [Only one state]
// 140: Lock 4
//         [Only one state]
// 143: * Scion Piece
// 146: Complete Scion
//         [Only one state]
// 147: Scion Holder
//         [Only one state]
// 150: Scion Piece
//         [Only one state]
// 151: * Flare(?) / Explosion
// 153: * Splash
// 155: * Bubbles
// 156: * Bubbles
// 158: * Blood splatter
// 160: * Flying disk
// 161: Centaur statue
//         [Only one state]
// 162: Shack suspended from wire rope
//         Starting = 0, position
//         Dropping = 1, after first fuse
//         Dropping = 2, after second fuse
//         Dropping = 3, after third fuse (to ground)
//         On = 4, the ground
// 163: Mutant egg and holder (normal size)
//         Starting = 0, to hatch
//         Hatching = 1, (is the fragmenting hardcoded?)
// 164: * Bullet hit
// 165: * Sparkle
// 166: Gunflare
//         [Only one state]
// 169: Lara's hips
//         [Only one state]
// 170: Lara's hips
//         [Only one state]
// 172: Mutant bullet
//         [Only one state]
// 173: Mutant grenade
//         [Only one state]
// 176: * Splatter
// 177: Lara's hips
//         [Only one state]
// 178: * Fire
// 179: Lara's hips
//         [Only one state]
// 180: Flowing Atlantean lava
//         [Only one state]
// 181: Mutant egg and holder (big)
//         Starting = 0, to hatch
//         Hatching = 1, (is the fragmenting hardcoded?)
// 182: Motorboat
//         Stationary = 1, (initial)
//         Moving = 2,
//         Stationary = 3, (final)
// 183: Lara's hips
//         [Only one state]
// 189: Shrinking wedge?
//         [Only one state]
// 190: * Standard symbols
// 191: * Plant 1
// 192: * Plant 2
// 193: * Plant 3
// 194: * Plant 4
// 195: * Plant 5
// 200: * Bag 1
// 204: * Bag 2
// 207: Gunflare
//         [Only one state]
// 212: * Rock 1
// 213: * Rock 2
// 214: * Rock 3
// 215: * Bag 3
// 216: * Pottery 1
// 217: * Pottery 2
// 231: * Painted pot
// 233: * Inca mummy
// 236: * Pottery 3
// 237: * Pottery 4
// 238: * Pottery 5
// 239: * Pottery 6

#endif


#endif
