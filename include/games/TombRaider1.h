/*!
 * \file include/games/TombRaider1.h
 * \brief Tomb Raider 1 items and states.
 *
 * Based on TR Rosetta Stone
 *
 * \author Mongoose
 * \author xythobuz
 */
#ifndef _TOMBRAIDER1_H_
#define _TOMBRAIDER1_H_

/*!
 * \brief Tomb Raider 1 items and states
 */
class TombRaider1 {
public:

    /*!
     * \brief States of a Wolf (Item 7)
     */
    enum WolfStates {
        WolfState_Walking       = 1,
        WolfState_Running       = 2,
        WolfState_Jumping       = 3,
        WolfState_Stalking      = 5,
        WolfState_JumpingAttack = 6,
        WolfState_Attacking     = 7,
        WolfState_Lying         = 8,  //!< Lying down
        WolfState_Getting       = 9,  //!< Getting ready to strike
        WolfState_RunningJump   = 10,
        WolfState_Dying         = 11,
        WolfState_Biting        = 12
    };

    /*!
     * \brief States of a Bear (Item 8)
     */
    enum BearStates {
        BearState_Walking       = 0, //!< Walking on all fours
        BearState_Getting       = 1, //!< Getting back to all fours
        BearState_WalkingHind   = 2, //!< Walking on hind legs
        BearState_Running       = 3, //!< Running on all fours
        BearState_Rearing       = 4, //!< Rearing up on hind legs
        BearState_Growling      = 5, //!< ?
        BearState_RunningAttack = 6, //!< Running and attacking
        BearState_Standing      = 7, //!< Standing on hind legs
        BearState_Biting        = 8,
        BearState_Dying         = 9
    };

    /*!
     * \brief States of a Bat (Item 9)
     */
    enum BatStates {
        BatState_Starting = 1, //!< Starting to fly
        BatState_Flying   = 2, //!< Flying straight
        BatState_Biting   = 3,
        BatState_Circling = 4,
        BatState_Dying    = 5
    };

    /*!
     * \brief States of a Crocodile on land (Item 10)
     */
    enum CrocodileLandStates {
        CrocodileLandState_Stationary = 1,
        CrocodileLandState_Walking1   = 2,
        CrocodileLandState_Walking2   = 3,
        CrocodileLandState_Turning    = 4,
        CrocodileLandState_Biting     = 5,
        CrocodileLandState_Dying      = 7
    };

    /*!
     * \brief States of a Crocodile in water (Item 11)
     */
    enum CrocodileWaterStates {
        CrocodileWaterState_Swimming = 1,
        CrocodileWaterState_Biting   = 2,
        CrocodileWaterState_Dying    = 3
    };

    /*!
     * \brief States of a Lion (Male & Female) and a Panther (Items 12, 13, 14)
     */
    enum LionStates {
        LionState_Standing = 1,
        LionState_Walking  = 2,
        LionState_Leaping  = 3,
        LionState_LeapBite = 4,
        LionState_Dying    = 5,
        LionState_Biting1  = 6,
        LionState_Biting2  = 7
    };

    /*!
     * \brief States of a Gorilla (Item 15)
     */
    enum GorillaStates {
        GorillaState_Standing = 1,  //!< Standing on all fours
        GorillaState_Running  = 3,  //!< Running on all fours
        GorillaState_Walking  = 4,  //!< Walking on legs (attacking?)
        GorillaState_Dying    = 5,
        GorillaState_Thumping = 6,  //!< Thumping on chest
        GorillaState_Waving   = 7,  //!< Waving arms
        GorillaState_TurningL = 8,  //!< Turning leftward?
        GorillaState_TurningR = 9,  //!< Turning rightward?
        GorillaState_Jumping  = 10, //!< Jumping up and waving arms
        GorillaState_Climbing = 11
    };

    /*!
     * \brief States of a Giant Rat on land (Item 16)
     */
    enum GiantRatLandStates {
        GiantRatLandState_Standing  = 1,
        GiantRatLandState_JumpBite  = 2,
        GiantRatLandState_Running   = 3,
        GiantRatLandState_Biting    = 4,
        GiantRatLandState_Dying     = 5,
        GiantRatLandState_RearingUp = 6
    };

    /*!
     * \brief States of a Giant Rat in the water (Item 17)
     */
    enum GiantRatWaterStates {
        GiantRatWaterState_Swimming = 1,
        GiantRatWaterState_Biting   = 2,
        GiantRatWaterState_Dying    = 3
    };

    /*!
     * \brief States of a Tyrannosaur (Item 18)
     */
    enum TyrannosaurStates {
        TyrannosaurState_Standing  = 1,
        TyrannosaurState_Walking   = 2,
        TyrannosaurState_Running   = 3,
        TyrannosaurState_Dying     = 5,
        TyrannosaurState_Bellowing = 6,
        TyrannosaurState_Biting    = 7,
        TyrannosaurState_Shaking   = 8  //!< Shaking Head and Spitting Out
    };

    /*!
     * \brief States of a Raptor (Item 19)
     */
    enum RaptorStates {
        RaptorState_Dying     = 0,
        RaptorState_Standing  = 1,
        RaptorState_Walking   = 2,
        RaptorState_Running   = 3,
        RaptorState_JumpBite  = 4,
        RaptorState_Bellowing = 6,
        RaptorState_RunBellow = 7,
        RaptorState_Biting    = 8
    };

    /*!
     * \brief States of a Winged Mutant (Item 20)
     */
    enum WingedMutantStates {
        WingedMutantState_Crouching = 1,
        WingedMutantState_Walking   = 2,
        WingedMutantState_Running   = 3,
        WingedMutantState_Biting    = 4,
        WingedMutantState_Looking   = 6,
        WingedMutantState_Jumping   = 7,
        WingedMutantState_Clawing   = 8,
        WingedMutantState_Aiming    = 9,  //!< Aiming Right-Hand Gun
        WingedMutantState_AimFire   = 10, //!< Aiming and Firing Left-Hand Gun
        WingedMutantState_Firing    = 11, //!< Firing Right-Hand Gun
        WingedMutantState_Standing  = 12,
        WingedMutantState_Flying    = 13
    };

    /*!
     * \brief States of a Centaur Mutant (Item 23)
     */
    enum CentaurMutantStates {
        CentaurMutantState_Standing  = 1,
        CentaurMutantState_Firing    = 2,
        CentaurMutantState_Galloping = 3,
        CentaurMutantState_Aiming    = 4,
        CentaurMutantState_Dying     = 5,
        CentaurMutantState_Rearing   = 6  //!<  Rearing up
    };

    /*!
     * \brief States of a Mummy (Item 24)
     */
    enum MummyStates {
        MummyState_Standing = 1,
        MummyState_Falling  = 2  //!< Falling forward
    };

    /*!
     * \brief States of Larson (Item 27)
     */
    enum LarsonStates {
        LarsonState_Walking1    = 0,
        LarsonState_StandingGun = 1,
        LarsonState_Walking2    = 2,
        LarsonState_Running     = 3,
        LarsonState_Aiming      = 4,
        LarsonState_Injured     = 5, //!< Injured by gunshot / Dying
        LarsonState_Standing    = 6,
        LarsonState_Firing      = 7
    };

    /*!
     * \brief States of Pierre (Item 28)
     */
    enum PierreStates {
        PierreState_Standing   = 1,
        PierreState_Walking    = 2,
        PierreState_Running    = 3,
        PierreState_Aiming     = 4,
        PierreState_Dying      = 5,
        PierreState_Holstering = 6,
        PierreState_Firing     = 7
    };

    /*!
     * \brief States of the Skateboard (Item 29)
     */
    enum SkateboardStates {
        SkateboardState_BeingTurned = 0,
        SkateboardState_Stationary1 = 1,
        SkateboardState_Stationary2 = 2,
        SkateboardState_Stationary3 = 3,
        SkateboardState_Stationary4 = 4
    };

    /*!
     * \brief States of the Skateboard Kid (Item 30)
     */
    enum SkateboardKidStates {
        SkateboardKidState_Turning = 0, //!< Turning and Aiming?
        SkateboardKidState_Firing1 = 1,
        SkateboardKidState_Skating = 2,
        SkateboardKidState_Aiming  = 3,
        SkateboardKidState_Firing2 = 4,
        SkateboardKidState_Dying   = 5
    };

    /*!
     * \brief States of the Cowboy (Item 31)
     */
    enum CowboyStates {
        CowboyState_Aiming1 = 1,
        CowboyState_Walking = 2,
        CowboyState_Running = 3,
        CowboyState_Aiming2 = 4,
        CowboyState_Dying   = 5,
        CowboyState_Firing  = 6
    };

    /*!
     * \brief States of Mr. T (Item 32)
     */
    enum MrTStates {
        MrTState_Dying    = 0,
        MrTState_Standing = 1,
        MrTState_Walking  = 2,
        MrTState_Running  = 3,
        MrTState_Aiming   = 4,
        MrTState_Firing   = 6
    };

    /*!
     * \brief States of Winged Natla (Item 33)
     */
    enum WingedNatlaStates {
        WingedNatlaState_Standing1 = 1,
        WingedNatlaState_Flying    = 2,
        WingedNatlaState_Running   = 3,
        WingedNatlaState_Aiming    = 4, //!< Aiming and Firing
        WingedNatlaState_Dying1    = 5, //!< "Dying" the first time
        WingedNatlaState_Spinning  = 7, //!< Spinning around in air
        WingedNatlaState_Standing2 = 8,
        WingedNatlaState_Dying2    = 9  //!< Dying for real
    };

    /*!
     * \brief States of the Giant Mutant (Item 34)
     */
    enum GiantMutantStates {
        GiantMutantState_Dying         = 0,
        GiantMutantState_Sitting       = 1, //!< Sitting on floor
        GiantMutantState_Pulling       = 2, //!< Pulling self forward
        GiantMutantState_SlappingRight = 4, //!< Slapping with right hand
        GiantMutantState_SlappingBoth  = 5, //!< Slapping with both hands
        GiantMutantState_MakingWave    = 6, //!< Making big wave with right hand
        GiantMutantState_Dropping      = 8, //!< Dropping to floor after hatching
        GiantMutantState_RaisingArms   = 9,
        GiantMutantState_Shaking       = 11 //!< Shaking victim with right hand
    };

    /*!
     * \brief States of a piece of Collapsible Floor (Item 35)
     */
    enum CollapsibleFloorStates {
        CollapsibleFloorState_Stationary = 0,
        CollapsibleFloorState_Shaking    = 1,
        CollapsibleFloorState_Falling    = 2,
        CollapsibleFloorState_Settling   = 3  //!< Settling down
    };

    /*!
     * \brief States of a Swinging Blade (Item 36)
     */
    enum SwingingBladeStates {
        SwingingBladeState_Stationary = 0,
        SwingingBladeState_Swinging   = 2
    };

    /*!
     * \brief States of a Boulder (Item 38)
     */
    enum BoulderStates {
        BoulderState_Stationary = 0,
        BoulderState_Rolling    = 1
    };

    /*!
     * \brief States of a Dart Gun (Item 40)
     */
    enum DartGunStates {
        DartGunState_Idle   = 0, //!< ?
        DartGunState_Firing = 1  //!< ?
    };

    /*!
     * \brief States of a Door opening upwards (Item 41)
     */
    enum DoorUpwardStates {
        DoorUpwardState_Upward = 0,
        DoorUpwardState_OnSide = 1
    };

    /*!
     * \brief States of a Slamming Door (Item 42)
     */
    enum SlammingDoorStates {
        SlammingDoorState_Open   = 0,
        SlammingDoorState_Closed = 1
    };

    /*!
     * \brief States of the Handle of Thors Hammer (Item 44)
     */
    enum ThorHammerHandleStates {
        ThorHammerHandleState_Stationary = 0, //!< Stationary in up position
        ThorHammerHandleState_Moving1    = 1, //!< Moving down a little and returning
        ThorHammerHandleState_Moving2    = 2, //!< Moving down all the way
        ThorHammerHandleState_Stopped    = 3  //!< Stopped at down position
    };

    /*!
     * \brief States of the Block of Thors Hammer (Item 45)
     */
    enum ThorHammerBlockStates {
        ThorHammerBlockState_Stationary = 0,
        ThorHammerBlockState_Moving1    = 1, //!< Moving down a little and returning
        ThorHammerBlockState_Moving2    = 2  //!< Moving down all the way
    };

    /*!
     * \brief States of a Metal Rod (Item 47)
     */
    enum MetalRodStates {
        MetalRodState_Stationary = 0,
        MetalRodState_Moving     = 1
    };

    /*!
     * \brief States of a Pushable Cubical (Item 48 - 51)
     */
    enum PushableStates {
        PushableState_Stationary = 0,
        PushableState_Pulled     = 1, //!< ?
        PushableState_Pushed     = 2  //!< ?
    };

    /*!
     * \brief States of a Movable Tall Block (Item 52)
     */
    enum MovableTallBlockStates {
        MovableTallBlockState_Stationary     = 0,
        MovableTallBlockState_MovingForward  = 1, //!< ?
        MovableTallBlockState_MovingBackward = 2  //!< ?
    };

    /*!
     * \brief States of Falling Pieces (Item 53)
     */
    enum FallingPiecesStates {
        FallingPiecesState_Stationary = 0,
        FallingPiecesState_Falling    = 1,
        FallingPiecesState_Settling   = 2  //!< Settling down
    };

    /*!
     * \brief States of a Switch (Item 55, 56)
     */
    enum SwitchStates {
        SwitchState_Off = 0,
        SwitchState_On  = 1  //!< States may be reversed
    };

    /*!
     * \brief States of a Door (Item 57 - 66)
     */
    enum DoorStates {
        DoorState_Closed = 0,
        DoorState_Open   = 1
    };

    /*!
     * \brief States of a Cog (Item 74 - 76)
     */
    enum CogStates {
        CogState_Stationary = 0,
        CogState_Turning    = 1
    };

    /*!
     * \brief States of a Shack (Item 162)
     */
    enum ShackStates {
        ShackState_StartingPosition = 0,
        ShackState_DroppingFirst    = 1, //!< Dropping after first fuse
        ShackState_DroppingSecond   = 2, //!< Dropping after second fuse
        ShackState_DroppingThird    = 3, //!< Dropping to ground after third fuse
        ShackState_OnGround         = 4
    };

    /*!
     * \brief States of a Mutant Egg & Holder (Item 163 & 181)
     */
    enum MutantEggStates {
        MutantEggState_Starting = 0,
        MutantEggState_Hatching = 1  //!< Is the fragmenting hardcoded?
    };

    /*!
     * \brief States of a Motorboat (Item 182)
     */
    enum MotorboatStates {
        MotorboatState_StationaryInitial = 1,
        MotorboatState_Moving            = 2,
        MotorboatState_StationaryFinal   = 3
    };

    /*!
     * \brief Items & IDs in Tomb Raider 1
     */
    enum Items {
        Lara             = 0,
        PistolAnimation  = 1,
        ShotgunAnimation = 2,
        MagnumAnimation  = 3,
        LaraAlternate    = 4,  //!< Lara's home appearance, wounded, or turned to gold
        UziAnimation     = 5,
        LaraMutant       = 6,
        Wolf             = 7,
        Bear             = 8,
        Bat              = 9,
        CrocodileLand    = 10,
        CrocodileWater   = 11,
        LionMale         = 12,
        LionFemale       = 13, //!< Same states as Male Lion
        Panther          = 14, //!< Same states as Male and Female Lion
        Gorilla          = 15,
        GiantRatLand     = 16,
        GiantRatWater    = 17,
        Tyrannosaur      = 18,
        Raptor           = 19,
        WingedMutant     = 20, //!< Also winged mummy (unused)
        LaraHips1        = 21,
        LaraHips2        = 22,
        CentaurMutant    = 23,
        Mummy            = 24,

        Larson           = 27,
        Pierre           = 28,
        Skateboard       = 29,
        SkateboardKid    = 30,
        Cowboy           = 31,
        MrT              = 32,
        WingedNatla      = 33, //!< Actually Natla with a winged mutant
        GiantMutant      = 34,
        CollapsibleFloor = 35,
        SwingingBlade    = 36,
        Spikes           = 37,
        Boulder          = 38,
        Dart             = 39,
        DartGun          = 40,
        DoorUpward       = 41,
        SlammingDoor     = 42,
        SwordOfDamocles1 = 43,
        ThorHammerHandle = 44,
        ThorHammerBlock  = 45,
        HangingBall      = 46, //!< Hanging ball? Some kind of box?
        MetalRod         = 47, //!< Metal rod? / Powered mining cart
        PushableCubical1 = 48,
        PushableCubical2 = 49,
        PushableCubical3 = 50,
        PushableCubical4 = 51,
        MovableTallBlock = 52,
        FallingPieces    = 53,
        SwordOfDamocles2 = 54,
        AboveWaterSwitch = 55,
        UnderWaterSwitch = 56,
        Door1            = 57,
        Door2            = 58,
        Door3            = 59,
        Door4            = 60,
        Door5            = 61,
        Door6            = 62,
        Door7            = 63,
        Door8            = 64,
        Trapdoor1        = 65, //!< Uses DoorStates
        Trapdoor2        = 66, //!< Uses DoorStates

        BridgeFlat       = 68,
        BridgeSlope1     = 69,
        BridgeSlope2     = 70,
        PassportOpen     = 71, //!< Passport opening up
        Compass          = 72,

        Cogs1            = 74, //!< animated
        Cogs2            = 75, //!< animated
        Cogs3            = 76, //!< animated
        CS_Lara          = 77, //!< Lara / Scion holder in Cut Scene
        CS_Larson        = 78, //!< Larson / Natla / Scion holder in Cut Scene
        CS_LarsonGun     = 79, //!< Larsons gun / Scion / Natla in Cut Scene
        CS_Scion         = 80, //!< Scion in Cut Scene
        PassportClosed   = 81, //!< Passport closed
        N                = 82, //!< N-thingy, Playstation memory card?
        SaveCrystal      = 83,
        _Pistols         = 84,
        _Shotgun         = 85,
        _Magnums         = 86,
        _Uzis            = 87,
        _PistolAmmo      = 88, //!< ?
        _ShotgunAmmo     = 89,
        _MagnumAmmo      = 90,
        _UziAmmo         = 91,

        _SmallMedipack   = 93,
        _LargeMedipack   = 94,
        Sunglasses       = 95,
        Cassette         = 96, // Cassette player and headphones
        DirectionKeys    = 97,

        Pistol           = 99,
        Shotgun          = 100,
        Magnum           = 101,
        Uzi              = 102,
        PistolAmmo       = 103, //!< ?
        ShotgunAmmo      = 104,
        MagnumAmmo       = 105,
        UziAmmo          = 106,

        SmallMedipack    = 108,
        LargeMedipack    = 109,
        _Puzzle1         = 110,
        _Puzzle2         = 111,
        _Puzzle3         = 112,
        _Puzzle4         = 113,
        Puzzle1          = 114,
        Puzzle2          = 115,
        Puzzle3          = 116,
        Puzzle4          = 117,
        Slot1Empty       = 118,
        Slot2Empty       = 119,
        Slot3Empty       = 120,
        Slot4Empty       = 121,
        Slot1Full        = 122,
        Slot2Full        = 123,
        Slot3Full        = 124,
        Slot4Full        = 125,
        _Pickup1         = 126,
        Pickup1          = 127,
        LaraHips3        = 128,
        _Key1            = 129,
        _Key2            = 130,
        _Key3            = 131,
        _Key4            = 132,
        Key1             = 133,
        Key2             = 134,
        Key3             = 135,
        Key4             = 136,
        Lock1            = 137,
        Lock2            = 138,
        Lock3            = 139,
        Lock4            = 140,

        _ScionPiece      = 143,

        CompleteScion    = 146,
        ScionHolder      = 147,

        ScionPiece       = 150,
        _Flare           = 151, //!< Flare(?) / Explosion

        _Splash          = 153,

        _Bubbles1        = 155,
        _Bubbles2        = 156,

        _BloodSplatter   = 158,

        _FlyingDisk      = 160,
        CentaurStatue    = 161,
        Shack            = 162, //!< Suspended from wire rope
        MutantEggNormal  = 163, //!< Mutant Egg and Holder (Normal)
        _BulletHit       = 164,
        _Sparkle         = 165,
        Gunflare1        = 166,

        LaraHips4        = 169,
        LaraHips5        = 170,

        MutantBullet     = 172,
        MutantGrenade    = 173,

        _Splatter        = 176,
        LaraHips6        = 177,
        _Fire            = 178,
        LaraHips7        = 179,
        FlowingLava      = 180, //!< Flowing Atlantean Lava
        MutantEggBig     = 181, //!< Mutant Egg and Holder (Big)
        Motorboat        = 182,
        LaraHips8        = 183,

        ShrinkingWedge   = 189, //!< ?
        _StandardSymbols = 190,
        _Plant1          = 191,
        _Plant2          = 192,
        _Plant3          = 193,
        _Plant4          = 194,
        _Plant5          = 195,

        _Bag1            = 200,

        _Bag2            = 204,

        Gunflare2        = 207,

        _Rock1           = 212,
        _Rock2           = 213,
        _Rock3           = 214,
        _Bag3            = 215,
        _Pottery1        = 216,
        _Pottery2        = 217,

        _PaintedPot      = 231,

        _IncaMummy       = 233,

        _Pottery3        = 236,
        _Pottery4        = 237,
        _Pottery5        = 238,
        _Pottery6        = 239
    };
};

#endif
