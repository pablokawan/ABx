TIME_FOREVER = 4294967295    -- Largets possible int 32 value

-- Actor properties
SEX_UNKNOWN = 0
SEX_FEMALE = 1
SEX_MALE = 2

-- Creature states -------------------------------------------------------------
CREATURESTATE_UNKNOWN = 0
CREATURESTATE_IDLE = 1
CREATURESTATE_MOVING = 2
CREATURESTATE_USINGSKILL = 3
CREATURESTATE_ATTACKING = 4
CREATURESTATE_KNOCKDOWN = 5
CREATURESTATE_EMOTE = 6
CREATURESTATE_EMOTE_SIT = 7

CREATURESTATE_EMOTE_START = 8
CREATURESTATE_EMOTE_CRY = 9
CREATURESTATE_EMOTE_TAUNT = 10
CREATURESTATE_EMOTE_PONDER = 11
CREATURESTATE_EMOTE_WAVE = 12
CREATURESTATE_EMOTE_LAUGH = 13
CREATURESTATE_EMOTE_END = 14

CREATURESTATE_CHEST_CLOSED = 15
CREATURESTATE_CHEST_OPEN = 16
CREATURESTATE_TRIGGERED = 17

CREATURESTATE_DEAD = 255
-- /Creature states ------------------------------------------------------------

SHAPETYPE_BOUNDINGBOX = 0
SHAPETYPE_SPHERE = 1
SHAPETYPE_CONVEXHULL = 2
SHAPETYPE_HEIGHTMAP = 3
SHAPETYPE_NONE = 99

RANGE_AGGRO = 0
RANGE_COMPASS = 1
RANGE_SPIRIT = 2
RANGE_EARSHOT = 3
RANGE_CASTING = 4
RANGE_PROJECTILE = 5
RANGE_HALFCOMPASS = 6
RANGE_TOUCH = 7
RANGE_ADJECENT = 8
RANGE_VISIBLE = 9
RANGE_MAP = 10

GAMETYPE_UNKNOWN = 0
GAMETYPE_OUTPOST = 1
GAMETYPE_PVPCOMBAT = 2
GAMETYPE_EXPLOREABLE = 3
GAMETYPE_MISSION = 4

-- SetResource types
SETVALUE_TYPE_ABSOLUTE = 0
SETVALUE_TYPE_INCREASE = 1
SETVALUE_TYPE_INCREASE_PERCENT = 2
SETVALUE_TYPE_DECREASE = 3
SETVALUE_TYPE_DECREASE_PERCENT = 4

-- GetResource/SetResource types
RESOURCE_TYPE_NONE = 0
RESOURCE_TYPE_ENERGY = 1
RESOURCE_TYPE_HEALTH = 2
RESOURCE_TYPE_ADRENALINE = 3
RESOURCE_TYPE_OVERCAST = 4
RESOURCE_TYPE_HEALTHREGEN = 5
RESOURCE_TYPE_ENERGYREGEN = 6
RESOURCE_TYPE_MAXHEALTH = 7
RESOURCE_TYPE_MAXENERGY = 8

-- Group mask
GROUPMASK_NONE = 0
GROUPMASK_1 = 1
GROUPMASK_2 = 1 << 1
GROUPMASK_3 = 1 << 2
GROUPMASK_4 = 1 << 3
GROUPMASK_5 = 1 << 4
GROUPMASK_6 = 1 << 5
GROUPMASK_7 = 1 << 7
GROUPMASK_8 = 1 << 8
