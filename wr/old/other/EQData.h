namespace EQData
{

// ***************************************************************************
// Defines
// ***************************************************************************
#define SPAWN_PLAYER                    0
#define SPAWN_NPC                       1
#define SPAWN_CORPSE                    2
#define SPAWN_ANY                       3
#define SPAWN_PET                       4

#define ITEM_NORMAL1                    0x0031
#define ITEM_NORMAL2                    0x0036
#define ITEM_NORMAL3                    0x315f
#define ITEM_NORMAL4                    0x3336
#define ITEM_NORMAL5                    0x0032
#define ITEM_NORMAL6                    0x0033
#define ITEM_NORMAL7                    0x0034
#define ITEM_NORMAL8                    0x0039
#define ITEM_CONTAINER                  0x7900
#define ITEM_CONTAINER_PLAIN            0x7953
#define ITEM_BOOK                       0x7379

#define COLOR_DEFAULT                   0x00 +   0
#define COLOR_DARKGREY                  0x00 +   1
#define COLOR_DARKGREEN                 0x00 +   2
#define COLOR_DARKBLUE                  0x00 +   3
#define COLOR_PURPLE                    0x00 +   5
#define COLOR_LIGHTGREY                 0x00 +   6

#define CONCOLOR_GREEN                  0x00 +  14
#define CONCOLOR_LIGHTBLUE              0x00 +  18
#define CONCOLOR_BLUE                   0x00 +  16
#define CONCOLOR_BLACK                  0x00 +  20
#define CONCOLOR_YELLOW                 0x00 +  15
#define CONCOLOR_RED                    0x00 +  13

#define USERCOLOR_SAY                   0xFF +   1 //  1  - Say
#define USERCOLOR_TELL                  0xFF +   2 //  2  - Tell
#define USERCOLOR_GROUP                 0xFF +   3 //  3  - Group
#define USERCOLOR_GUILD                 0xFF +   4 //  4  - Guild
#define USERCOLOR_OOC                   0xFF +   5 //  5  - OOC
#define USERCOLOR_AUCTION               0xFF +   6 //  6  - Auction
#define USERCOLOR_SHOUT                 0xFF +   7 //  7  - Shout
#define USERCOLOR_EMOTE                 0xFF +   8 //  8  - Emote
#define USERCOLOR_SPELLS                0xFF +   9 //  9  - Spells (meming, scribing, casting, etc.)
#define USERCOLOR_YOU_HIT_OTHER         0xFF +  10 //  10 - You hit other
#define USERCOLOR_OTHER_HIT_YOU         0xFF +  11 //  11 - Other hits you
#define USERCOLOR_YOU_MISS_OTHER        0xFF +  12 //  12 - You miss other
#define USERCOLOR_OTHER_MISS_YOU        0xFF +  13 //  13 - Other misses you
#define USERCOLOR_DUELS                 0xFF +  14 //  14 - Some broadcasts (duels)
#define USERCOLOR_SKILLS                0xFF +  15 //  15 - Skills (ups, non-combat use, etc.)
#define USERCOLOR_DISCIPLINES           0xFF +  16 //  16 - Disciplines or special abilities
#define USERCOLOR_UNUSED001             0xFF +  17 //  17 - Unused at this time
#define USERCOLOR_DEFAULT               0xFF +  18 //  18 - Default text and stuff you type
#define USERCOLOR_UNUSED002             0xFF +  19 //  19 - Unused at this time
#define USERCOLOR_MERCHANT_OFFER        0xFF +  20 //  20 - Merchant Offer Price
#define USERCOLOR_MERCHANT_EXCHANGE     0xFF +  21 //  21 - Merchant Buy/Sell
#define USERCOLOR_YOUR_DEATH            0xFF +  22 //  22 - Your death message
#define USERCOLOR_OTHER_DEATH           0xFF +  23 //  23 - Others death message
#define USERCOLOR_OTHER_HIT_OTHER       0xFF +  24 //  24 - Other damage other
#define USERCOLOR_OTHER_MISS_OTHER      0xFF +  25 //  25 - Other miss other
#define USERCOLOR_WHO                   0xFF +  26 //  26 - /who command
#define USERCOLOR_YELL                  0xFF +  27 //  27 - yell for help
#define USERCOLOR_NON_MELEE             0xFF +  28 //  28 - Hit for non-melee
#define USERCOLOR_SPELL_WORN_OFF        0xFF +  29 //  29 - Spell worn off
#define USERCOLOR_MONEY_SPLIT           0xFF +  30 //  30 - Money splits
#define USERCOLOR_LOOT                  0xFF +  31 //  31 - Loot message
#define USERCOLOR_RANDOM                0xFF +  32 //  32 - Dice Roll (/random)
#define USERCOLOR_OTHERS_SPELLS         0xFF +  33 //  33 - Others spells
#define USERCOLOR_SPELL_FAILURE         0xFF +  34 //  34 - Spell Failures (resists, fizzles, missing component, bad target, etc.)
#define USERCOLOR_CHAT_CHANNEL          0xFF +  35 //  35 - Chat Channel Messages
#define USERCOLOR_CHAT_1                0xFF +  36 //  36 - Chat Channel 1
#define USERCOLOR_CHAT_2                0xFF +  37 //  37 - Chat Channel 2
#define USERCOLOR_CHAT_3                0xFF +  38 //  38 - Chat Channel 3
#define USERCOLOR_CHAT_4                0xFF +  39 //  39 - Chat Channel 4
#define USERCOLOR_CHAT_5                0xFF +  40 //  40 - Chat Channel 5
#define USERCOLOR_CHAT_6                0xFF +  41 //  41 - Chat Channel 6
#define USERCOLOR_CHAT_7                0xFF +  42 //  42 - Chat Channel 7
#define USERCOLOR_CHAT_8                0xFF +  43 //  43 - Chat Channel 8
#define USERCOLOR_CHAT_9                0xFF +  44 //  44 - Chat Channel 9
#define USERCOLOR_CHAT_10               0xFF +  45 //  45 - Chat Channel 10
#define USERCOLOR_MELEE_CRIT            0xFF +  46 //  46 - Melee Crits
#define USERCOLOR_SPELL_CRIT            0xFF +  47 //  47 - Spell Crits
#define USERCOLOR_TOO_FAR_AWAY          0xFF +  48 //  48 - Too far away (melee)
#define USERCOLOR_NPC_RAMAGE            0xFF +  49 //  49 - NPC Rampage
#define USERCOLOR_NPC_FURRY             0xFF +  50 //  50 - NPC Furry
#define USERCOLOR_NPC_ENRAGE            0xFF +  51 //  51 - NPC Enrage
#define USERCOLOR_ECHO_SAY              0xFF +  52 //  52 - say echo
#define USERCOLOR_ECHO_TELL             0xFF +  53 //  53 - tell echo
#define USERCOLOR_ECHO_GROUP            0xFF +  54 //  54 - group echo
#define USERCOLOR_ECHO_GUILD            0xFF +  55 //  55 - guild echo
#define USERCOLOR_ECHO_OOC              0xFF +  56 //  56 - group echo
#define USERCOLOR_ECHO_AUCTION          0xFF +  57 //  57 - auction echo
#define USERCOLOR_ECHO_SHOUT            0xFF +  58 //  58 - shout echo
#define USERCOLOR_ECHO_EMOTE            0xFF +  59 //  59 - emote echo
#define USERCOLOR_ECHO_CHAT_1           0xFF +  60 //  60 - chat 1 echo
#define USERCOLOR_ECHO_CHAT_2           0xFF +  61 //  61 - chat 2 echo
#define USERCOLOR_ECHO_CHAT_3           0xFF +  62 //  62 - chat 3 echo
#define USERCOLOR_ECHO_CHAT_4           0xFF +  63 //  63 - chat 4 echo
#define USERCOLOR_ECHO_CHAT_5           0xFF +  64 //  64 - chat 5 echo
#define USERCOLOR_ECHO_CHAT_6           0xFF +  65 //  65 - chat 6 echo
#define USERCOLOR_ECHO_CHAT_7           0xFF +  66 //  66 - chat 7 echo
#define USERCOLOR_ECHO_CHAT_8           0xFF +  67 //  67 - chat 8 echo
#define USERCOLOR_ECHO_CHAT_9           0xFF +  68 //  68 - chat 9 echo
#define USERCOLOR_ECHO_CHAT_10          0xFF +  69 //  69 - chat 10 echo

#define DEITY_Bertoxxulous          201
#define DEITY_BrellSerilis          202
#define DEITY_CazicThule            203
#define DEITY_ErollisiMarr          204
#define DEITY_Bristlebane           205
#define DEITY_Innoruuk              206
#define DEITY_Karana                207
#define DEITY_MithanielMarr         208
#define DEITY_Prexus                209
#define DEITY_Quellious             210
#define DEITY_RallosZek             211
#define DEITY_RodcetNife            212
#define DEITY_SolusekRo             213
#define DEITY_TheTribunal           214
#define DEITY_Tunare                215
#define DEITY_Veeshan               216

#define ITEMTYPE_NORMAL     0
#define ITEMTYPE_PACK       1
#define ITEMTYPE_BOOK       2

#define COMP_NONE       0
#define COMP_EQ         1
#define COMP_NE         2
#define COMP_GT         3
#define COMP_LT         4
#define COMP_GE         5
#define COMP_LE         6
#define COMP_CONT       7
#define COMP_NOTCONT    8
#define COMP_BITAND     9
#define COMP_BITOR      10

#define COMP_TYPE_STRING    0
#define COMP_TYPE_NUMERIC   1
#define COMP_TYPE_BIT       2

#define CMD_MQ 1
#define CMD_EQ 2

#define LASTFIND_NOTFOUND   96
#define LASTFIND_PRIMARY    97

#define EVENT_CHAT          0
#define EVENT_TIMER         1
#define EVENT_CUSTOM        2
#define EVENT_EVAL          3
#define EVENT_EXEC          4
#define EVENT_PULSE         5
#define EVENT_SHUTDOWN      6
#define EVENT_BREAK         7
#define NUM_EVENTS          8

#define CHAT_SAY            1
#define CHAT_TELL           2
#define CHAT_OOC            4
#define CHAT_SHOUT          8
#define CHAT_AUC            16
#define CHAT_GUILD          32
#define CHAT_GROUP          64
#define CHAT_CHAT           128
#define CHATEVENT(x)    (gEventChat & x)

#define FILTERSKILL_ALL      0
#define FILTERSKILL_INCREASE 1
#define FILTERSKILL_NONE     2

#define FILTERMACRO_ALL      0
#define FILTERMACRO_ENHANCED 1
#define FILTERMACRO_NONE     2


#define MAX_STRING            2048
#define MAX_VARNAME           64

// Defines for CXWnd WindowStyle
#define CWS_VSCROLL     0x1
#define CWS_HSCROLL     0x2
#define CWS_TITLE       0x4
#define CWS_CLOSE       0x8
//#define CWS_UNKNOWN     0x10
#define CWS_MINIMIZE    0x20
#define CWS_BORDER      0x40
//#define CWS_UNKNOWN     0x80
//#define CWS_UNKNOWN     0x100
#define CWS_RESIZEALL   0x200
#define CWS_TRANSPARENT 0x400
//#define CWS_UNKNOWN 0x800
#define CWS_NOMOVE      0x8000
#define ToggleBit(field,bit) field^=bit;
#define BitOn(field,bit) field|=bit;
#define BitOff(field,bit) field&=~bit;
// End CXWnd WindowStyle Defines

enum MOUSE_DATA_TYPES {
   MD_Unknown = -1,
   MD_Button0Click=0,
   MD_Button1Click,
   MD_Button0,
   MD_Button1
};

// ***************************************************************************
// Structures
// ***************************************************************************

typedef struct _MOUSESPOOF {
   MOUSE_DATA_TYPES   mdType;
   DWORD            dwData;
   struct _MOUSESPOOF   *pNext;
} MOUSESPOOF, *PMOUSESPOOF;

typedef struct _MOUSECLICK {
   BYTE ConfirmLeftClick;// DO NOT WRITE TO THIS BYTE
   BYTE ConfirmRightClick;// DO NOT WRITE TO THIS BYTE
   BYTE RightClick;
   BYTE LeftClick;
} MOUSECLICK, *PMOUSECLICK;

typedef struct _UILOCATION {
   DWORD x;
   DWORD y;
   DWORD w;
   DWORD h;
   CHAR error[MAX_STRING];
} UILOCATION, *PUILOCATION;

#define ITEM_NAME_LEN         64
#define LORE_NAME_LEN         80
// Actual Size 0x204 10-9-2003
typedef struct _ITEMINFO {
/*0x00*/   CHAR			 Name[ITEM_NAME_LEN];
/*0x40*/   CHAR			LoreName[LORE_NAME_LEN];
/*0x90*/   CHAR			IDFile[0x20];
/*0xb0*/   DWORD		ItemNumber;
/*0xb4*/   DWORD		EquipSlots;
/*0xb8*/   DWORD		Cost;
/*0xbc*/   DWORD		IconNumber;
/*0xc0*/   BYTE			Unknown0xc0[0x10];
/*0xd0*/   BYTE			UnknownData0xd0;
/*0xd1*/   BYTE			Weight;
/*0xd2*/   BYTE			NoRent;
/*0xd3*/   BYTE			NoDrop;
/*0xd4*/   BYTE			Size;
/*0xd5*/   BYTE			Type;
/*0xd6*/   BYTE			TradeSkills; 
/*0x0d7*/  BYTE			Lore; 
/*0x0d8*/  BYTE			PendingLore; 
/*0x0d9*/  BYTE			Artifact; 
/*0x0da*/  BYTE			Summoned; 
/*0x0db*/  BYTE  		SvCold;
/*0x0dc*/  BYTE  		SvFire;
/*0x0dd*/  BYTE  		SvMagic;
/*0x0de*/  BYTE  		SvDisease;
/*0x0df*/  BYTE  		SvPoison;
/*0x0e0*/  BYTE  		STR;
/*0x0e1*/  BYTE  		STA;
/*0x0e2*/  BYTE  		AGI;
/*0x0e3*/  BYTE  		DEX;
/*0x0e4*/  BYTE  		CHA;
/*0x0e5*/  BYTE  		INT;
/*0x0e6*/  BYTE  		WIS;
/*0x0e7*/  BYTE  		Unknown0xe7;
/*0x0e8*/  DWORD 		HP;
/*0x0ec*/  DWORD 		Mana;
/*0x0f0*/  DWORD 		AC;
/*0x0f4*/  BYTE  		SkillModType;
/*0x0f5*/  BYTE  		Unknown0xf5[0x7];
/*0x0fc*/  DWORD 		BaneDMGType;
/*0x100*/  DWORD 		RequiredLevel;
/*0x104*/  DWORD 		InstrumentType;
/*0x108*/  DWORD 		InstrumentMod;
/*0x10c*/  DWORD 		Classes;
/*0x110*/  DWORD 		Races;
/*0x114*/  DWORD 		Diety;
/*0x118*/  BYTE  		Unknown0x118[0x4];
/*0x11c*/  DWORD 		SpellId;
/*0x120*/  DWORD 		Color;
/*0x124*/  BYTE  		SkillModValue;
/*0x125*/  BYTE  		BaneDMG;
/*0x126*/  BYTE  		Unknown0x126;
/*0x127*/  BYTE  		Magic;
/*0x128*/  BYTE  		Level;
/*0x129*/  BYTE  		Light;
/*0x12a*/  BYTE  		Delay;
/*0x12b*/  BYTE  		RecommendedLevel;
/*0x12c*/  BYTE			RecommendedSkill; 
/*0x12d*/  BYTE  		DmgBonusType;
/*0x12e*/  BYTE  		DmgBonusVal;
/*0x12f*/  BYTE  		Range;
/*0x130*/  BYTE  		Damage;
/*0x131*/  BYTE  		ItemType;
/*0x132*/  WORD 		Material;
/*0x134*/  DWORD 		AugSlot1;
/*0x138*/  DWORD 		AugSlot2;
/*0x13c*/  DWORD 		AugSlot3;
/*0x140*/  DWORD 		AugSlot4;
/*0x144*/  DWORD 		AugSlot5;
/*0x148*/  DWORD 		AugType;
/*0x14c*/  DWORD        AugRestrictions; 
/*0x150*/  DWORD 		LDTheme;
/*0x154*/  DWORD 		LDCost;
/*0x158*/  BYTE  		Unknown0x158[0x24];
/*0x17c*/  BYTE  		CharmFile[0x20];
/*0x19c*/  FLOAT 		QuestValue; //everything over 1.00000 is a good thing... ring of the ancients is 777.500
/*0x1a0*/  BYTE  		Unknown0x1a0[0x8];
/*0x1a8*/  DWORD		ProcRate;
/*0x1ac*/  DWORD 		FocusId;
/*0x1b0*/  DWORD		CombatEffects; 
/*0x1b4*/  DWORD		Shielding; 
/*0x1b8*/  DWORD		StunResist; 
/*0x1bc*/  DWORD		StrikeThrough; 
/*0x1c0*/  DWORD		Unknown0x1c0[0x2]; 
/*0x1c8*/  DWORD		SpellShield; 
/*0x1cc*/  DWORD		Avoidance; 
/*0x1d0*/  BYTE			Unknown0x1d0[0x8]; 
/*0x1d8*/  DWORD		CastTime;
   union {
/*0x1dc*/  BYTE			MaxCharges;
/*0x1dc*/  BYTE			Stackable;
   };
/*0x1dd*/  WORD 		EffectType;
/*0x1df*/  CHAR  		BookFile[0x1e];
/*0x1fd*/  BYTE  		Skill;
/*0x1fe*/  BYTE  		Combine;
/*0x1ff*/  BYTE  		Slots;
/*0x200*/  BYTE  		SizeCapacity;
/*0x201*/  BYTE  		WeightReduction;
/*0x202*/  BYTE  		Unknown0x202[0xa];
/*0x20c*/
} ITEMINFO, *PITEMINFO; 

typedef struct _CONTENTS {
/*0x00*/ struct _ITEMINFO *Item;
	union {
/*0x04*/ struct _CONTENTS *Contents[0x0a]; //addresses to whats inside the bag if its a bag
/*0x04*/ struct _ITEMINFO *Augments[0x0a]; //Only 1-5 are actually used (for now)
	};
	union {
/*0x2c*/  DWORD   StackCount;
/*0x2c*/  DWORD   Charges;
    };
    union {
/*0x30*/  DWORD   ItemSlot;// slotid for Player Items
/*0x30*/  DWORD   Unknown1; // why is this here?
    };
/*0x34*/  BYTE    Unknown2[0xc];
/*0x40*/  DWORD	  ItemSlot2;// slotid for Merchant Items
/*0x44*/  DWORD   Unknown3;
/*0x48*/  DWORD   Price; //price a player vendor set the item at 
/*0x4c*/  DWORD   Open;
/*0x50*/
} CONTENTS, *PCONTENTS;

// 6-11-2003 Amadeus
// Size 0x10
typedef struct _SPELLBUFF {
/*0x00*/    BYTE      Unknown0x00;
/*0x01*/    BYTE      Level;
/*0x02*/    CHAR      Modifier; // bard song modifier, divide by 10 to get 2.8 etc
/*0x03*/    CHAR      DamageShield;  // maybe.. I've noticed this is -1 on a lot of ds's.
/*0x04*/    DWORD     SpellID;
/*0x08*/    DWORD     Duration;
/*0x0c*/    DWORD     DamageAbsorbRemaining;  // Melee or Spellshield type
/*0x10*/
} SPELLBUFF, *PSPELLBUFF;

// 12-23-2003   TheColonel
typedef struct _INVENTORY { 
/*0x00*/  struct	_CONTENTS* Charm; 
/*0x04*/  struct	_CONTENTS* LeftEar; 
/*0x08*/  struct	_CONTENTS* Head; 
/*0x0c*/  struct	_CONTENTS* Face; 
/*0x10*/  struct	_CONTENTS* RightEar; 
/*0x14*/  struct	_CONTENTS* Neck; 
/*0x18*/  struct	_CONTENTS* Shoulders; 
/*0x1c*/  struct	_CONTENTS* Arms; 
/*0x20*/  struct	_CONTENTS* Back; 
/*0x24*/  struct	_CONTENTS* LeftWrist; 
/*0x28*/  struct	_CONTENTS* RightWrist; 
/*0x2c*/  struct	_CONTENTS* Range; 
/*0x30*/  struct	_CONTENTS* Hands; 
/*0x34*/  struct	_CONTENTS* Primary; 
/*0x38*/  struct	_CONTENTS* Secondary; 
/*0x3c*/  struct	_CONTENTS* LeftFinger; 
/*0x40*/  struct	_CONTENTS* RightFinger; 
/*0x44*/  struct	_CONTENTS* Chest; 
/*0x48*/  struct	_CONTENTS* Legs; 
/*0x4c*/  struct	_CONTENTS* Feet; 
/*0x50*/  struct	_CONTENTS* Waist; 
/*0x54*/  struct	_CONTENTS* Ammo; 
/*0x58*/  struct	_CONTENTS* Pack[0x8]; 
} INVENTORY, *PINVENTORY; 

#define      NUM_BANK_SLOTS         0x12
#define      NUM_BOOK_SLOTS         0x200
typedef struct _CHARINFO { 
/*0x0000*/	BYTE		Unknown00000; 
/*0x0001*/	BYTE		Unknown00001; 
/*0x0002*/	CHAR		Name[0x40]; 
/*0x0042*/	CHAR		Lastname[0x20]; 
/*0x0062*/	CHAR		Unknown0x0062[0x20]; 
/*0x0082*/	BYTE		Unused0x0082[0x2]; 
/*0x0084*/	DWORD		Unknown0x0084; 
/*0x0088*/	BYTE		Gender; 
/*0x0089*/	BYTE		Unknown0x0089[3]; 
/*0x008c*/	BYTE		Race; 
/*0x008d*/	BYTE		Unknown0x008d[3]; 
/*0x0090*/	BYTE		Class; 
/*0x0091*/	BYTE		Unknown0x0091[3]; 
/*0x0094*/	DWORD		Unknown0x0094; 
/*0x0098*/	DWORD		Level; 
/*0x009c*/	DWORD		Exp; 
/*0x00a0*/	DWORD		PracticePoints; 
/*0x00a4*/	DWORD		Mana; 
/*0x00a8*/	DWORD		Unknown0x00a8;   // Possibly Endurance?
/*0x00ac*/	DWORD		BaseHP; 
/*0x00b0*/	BYTE		Stunned; 
/*0x00b1*/	BYTE		Unknown0x00b1[3]; 
/*0x00b4*/	DWORD		BaseSTR; 
/*0x00b8*/	DWORD		BaseSTA; 
/*0x00bc*/	DWORD		BaseCHA; 
/*0x00c0*/	DWORD		BaseDEX; 
/*0x00c4*/	DWORD		BaseINT; 
/*0x00c8*/	DWORD		BaseAGI; 
/*0x00cc*/	DWORD		BaseWIS; 
/*0x00d0*/  BYTE		Face;
/*0x00d1*/  BYTE		field_D1[0x9];
/*0x00da*/  BYTE		Unknown0xda[0x2];
/*0x00dc*/  DWORD		field_DC;
/*0x00e0*/  BYTE		Unknown0xe0[0x20];
/*0x0100*/	BYTE		languages[0x20]; 
/*0x0120*/	struct  	_SPELLBUFF Buff[0x0f]; 
/*0x0210*/	DWORD		SpellBook[NUM_BOOK_SLOTS]; 
/*0x0a10*/  DWORD		MemorizedSpells[0x10];
/*0x0a50*/  BYTE		field_A50;
/*0x0a51*/  BYTE		Unknown0xa51[0x3];
/*0x0a54*/	FLOAT		y; 
/*0x0a58*/	FLOAT		x; 
/*0x0a5c*/	FLOAT		z; 
/*0x0a60*/	FLOAT		heading; 
/*0x0a64*/  BYTE		standstate;
/*0x0a65*/  BYTE		Unknown0xa65[0x3];
/*0x0a68*/	DWORD		Plat; 
/*0x0a6c*/	DWORD		Gold; 
/*0x0a70*/	DWORD		Silver; 
/*0x0a74*/	DWORD		Copper; 
/*0x0a78*/	DWORD		BankPlat; 
/*0x0a7c*/	DWORD		BankGold; 
/*0x0a80*/	DWORD		BankSilver; 
/*0x0a84*/	DWORD		BankCopper; 
/*0x0a88*/  DWORD		field_A88;
/*0x0a8c*/  DWORD		field_A8C;
/*0x0a90*/  DWORD		field_A90;
/*0x0a94*/  DWORD		field_A94;
/*0x0a98*/	DWORD		BankSharedPlat; 
/*0x0a9c*/  DWORD		field_A9C;
/*0x0aa0*/  DWORD		field_AA0;
/*0x0aa4*/  DWORD		field_AA4;
/*0x0aa8*/  DWORD		field_AA8;
/*0x0aac*/  DWORD		field_AAC;
/*0x0ab0*/  DWORD		field_AB0;
/*0x0ab4*/  DWORD		field_AB4;
/*0x0ab8*/  DWORD		Skill[0x64];
/*0x0c48*/  BYTE		field_C48[0x64];
/*0x0cac*/  BYTE		field_CAC;
/*0x0cad*/  BYTE		field_CAD;
/*0x0cae*/  BYTE		Unknown0xcae[0x2];
/*0x0cb0*/  DWORD		field_CB0;
/*0x0cb4*/  DWORD		field_CB4;
/*0x0cb8*/  DWORD		field_CB8;
/*0x0cbc*/  DWORD		field_CBC;
/*0x0cc0*/  BYTE		field_CC0[0x40];
/*0x0d00*/  DWORD		field_D00;
/*0x0d04*/  DWORD		field_D04;
/*0x0d08*/  BYTE		Unknown0xd08[0x58];
/*0x0d60*/  DWORD		hungerlevel;
/*0x0d64*/  DWORD		thirstlevel;
/*0x0d68*/  BYTE		Unknown0xd68[0x14];
/*0x0d7c*/	WORD		zoneId; 
/*0x0d7e*/	WORD		Instance; 
/*0x0d80*/	struct		_SPAWNINFO* pSpawn;  
union{ 
     /*0x0d84*/ struct	_INVENTORY	Inventory; 
     /*0x0d84*/ struct	_CONTENTS*	InventoryArray[0x1e]; 
}; 
/*0x0dfc*/	struct		_CONTENTS* Cursor; 
/*0x0e00*/  BYTE		Unknown0xe00[0x14];
/*0x0e14*/  BYTE		field_E14;
/*0x0e15*/  BYTE		field_E15;
/*0x0e16*/  BYTE		Unknown0xe16[0x2];
/*0x0e18*/	DWORD		STR; 
/*0x0e1c*/	DWORD		STA; 
/*0x0e20*/	DWORD		CHA; 
/*0x0e24*/	DWORD		DEX; 
/*0x0e28*/	DWORD		INT; 
/*0x0e2c*/	DWORD		AGI; 
/*0x0e30*/	DWORD		WIS; 
/*0x0e34*/	DWORD		field_E34; 
/*0x0e38*/	DWORD		SaveMagic; 
/*0x0e3c*/	DWORD		SaveFire; 
/*0x0e40*/	DWORD		SaveCold; 
/*0x0e44*/	DWORD		SavePoison; 
/*0x0e48*/	DWORD		SaveDisease; 
/*0x0e4c*/	DWORD		Unknown0x0e4c; 
/*0x0e50*/	DWORD		CurrWeight; 
/*0x0e54*/	DWORD		Unknown0x0e54; 
/*0x0e58*/	DWORD		HPBonus;       //From +HP Gear & Probably Spells 
/*0x0e5c*/	DWORD		ManaBonus;       //From +MANA Gear & Probably Spells 
/*0x0e60*/  BYTE		Unknown0xe60[0x48];
/*0x0ea8*/  BYTE		field_EA8;
/*0x0ea9*/  BYTE		Unknown0xea9[0x3];
/*0x0eac*/	SPELLBUFF	ShortBuff[0x6]; 
/*0x0f0c*/	BYTE		Unknown0x0f0c[0x108]; 
/*0x1014*/	DWORD		ZoneBoundId; 
/*0x1018*/	BYTE		Unknown0x1018[0x10]; 
/*0x1028*/	FLOAT		ZoneBoundY; 
/*0x102c*/	BYTE		Unknown0x102c[0x10]; 
/*0x103c*/	FLOAT		ZoneBoundX; 
/*0x1040*/	BYTE		Unknown0x1040[0x10]; 
/*0x1050*/	FLOAT		ZoneBoundZ; 
/*0x1054*/	FLOAT		Unknown0x1054; 
/*0x1058*/	FLOAT		Unknown0x1058; 
/*0x105c*/	FLOAT		Unknown0x105c; 
/*0x1060*/	FLOAT		Unknown0x1060; 
/*0x1064*/  BYTE		field_1064[0x14];
/*0x1078*/  DWORD		field_1078;
/*0x107c*/	DWORD		Diety; 
/*0x1080*/	DWORD		GuildID; 
/*0x1084*/  DWORD		field_1084;
/*0x1088*/  DWORD		field_1088;
/*0x108c*/  DWORD		field_108C;
/*0x1090*/  BYTE		field_1090;
/*0x1091*/  BYTE		field_1091;
/*0x1092*/  BYTE		field_1092;
/*0x1093*/  BYTE		field_1093;
/*0x1094*/	BYTE		GuildStatus; 
/*0x1095*/  BYTE		Unknown0x1095[0x3];
/*0x1098*/  DWORD		Drunkedness;
/*0x109c*/  DWORD		field_109C;
/*0x10a0*/  BYTE		Unknown0x10a0[0x23];
/*0x10c3*/	BYTE		Grouped;  // WRONG!
/*0x10c4*/	DWORD		AAExp;   // AAExp/330 = %
/*0x10c8*/	BYTE		field_10C8; 
/*0x10c9*/	BYTE		PercentEXPtoAA; 
/*0x10ca*/  BYTE		field_10CA;
/*0x10cb*/  BYTE		 field_10CB;
/*0x10cc*/  BYTE		field_10CC;
/*0x10cd*/  BYTE		field_10CD;
/*0x10ce*/  BYTE		field_10CE;
/*0x10cf*/  BYTE		field_10CF;
/*0x10d0*/  BYTE		field_10D0;
/*0x10d1*/  BYTE		Unknown0x10d1[0x3];
/*0x10d4*/  BYTE		field_10D4[0x58];
/*0x112c*/  BYTE		field_112C[0x24];
/*0x1150*/  BYTE		field_1150[0xf0];
/*0x1240*/  DWORD		field_1240;
/*0x1244*/  DWORD		field_1244;
/*0x1248*/  DWORD		field_1248;
/*0x124c*/  DWORD		field_124C;
/*0x1250*/  DWORD		field_1250;
/*0x1254*/  DWORD		field_1254;
/*0x1258*/  DWORD		field_1258;
/*0x125c*/  DWORD		field_125C;
/*0x1260*/  DWORD		field_1260;
/*0x1264*/  DWORD		field_1264;
/*0x1268*/  DWORD		field_1268;
/*0x126c*/  DWORD		field_126C;
/*0x1270*/  DWORD		field_1270;
/*0x1274*/	DWORD		AAPoints;    
/*0x1278*/  DWORD		field_1278;
/*0x127c*/  DWORD		field_127C;
/*0x1280*/  BYTE		field_1280[0x320];
/*0x15a0*/  DWORD		field_15A0;
/*0x15a4*/  DWORD		field_15A4;
/*0x15a8*/  DWORD		field_15A8;
/*0x15ac*/  DWORD		field_15AC;
/*0x15b0*/  BYTE		field_15B0[0x680];
/*0x1c30*/	CHAR		Server[0x20]; 
/*0x1c50*/  DWORD		field_1C50;
/*0x1c54*/  BYTE		field_1C54[0x14];
/*0x1c68*/  BYTE		field_1C68[0x14];
/*0x1c7c*/  DWORD		field_1C7C;
/*0x1c80*/  DWORD		field_1C80;
/*0x1c84*/  DWORD		field_1C84;
/*0x1c88*/  DWORD		field_1C88;
/*0x1c8c*/  DWORD		field_1C8C;
/*0x1c90*/  DWORD		field_1C90;
/*0x1c94*/  DWORD		field_1C94;
/*0x1c98*/  DWORD		field_1C98;
/*0x1c9c*/  DWORD		field_1C9C;
/*0x1ca0*/  BYTE		field_1CA0[0x30];
/*0x1cd0*/  BYTE		field_1CD0[0x30];
/*0x1d00*/  BYTE		field_1D00[0x30];
/*0x1d30*/  BYTE		field_1D30[0x30];
/*0x1d60*/  DWORD		field_1D60;
/*0x1d64*/  DWORD		field_1D64;
/*0x1d68*/  BYTE		Unknown0x1d68[0x4];
/*0x1d6c*/	DWORD		GukEarned; 
/*0x1d70*/	DWORD		MirEarned; 
/*0x1d74*/	DWORD		MMEarned; 
/*0x1d78*/	DWORD		RujEarned;  
/*0x1d7c*/	DWORD		TakEarned; 
/*0x1d80*/  BYTE  		Unknown0x1d80[0x18];
/*0x1d98*/	DWORD		LDoNPoints;   
/*0x1d9c*/  DWORD 		field_1D9C;
/*0x1da0*/  DWORD 		field_1DA0;
/*0x1da4*/  DWORD 		field_1DA4;
/*0x1da8*/  DWORD 		field_1DA8;
/*0x1dac*/  DWORD 		field_1DAC;
/*0x1db0*/  DWORD 		field_1DB0;
/*0x1db4*/  DWORD 		field_1DB4;
/*0x1db8*/  DWORD 		field_1DB8;
/*0x1dbc*/  DWORD 		field_1DBC;
/*0x1dc0*/  DWORD 		field_1DC0;
/*0x1dc4*/  DWORD 		field_1DC4;
/*0x1dc8*/  DWORD 		field_1DC8;
/*0x1dcc*/  DWORD 		field_1DCC;
/*0x1dd0*/  DWORD 		field_1DD0;
/*0x1dd4*/  DWORD 		field_1DD4;
/*0x1dd8*/  DWORD 		field_1DD8;
/*0x1ddc*/  DWORD 		field_1DDC;
/*0x1de0*/  DWORD 		field_1DE0;
/*0x1de4*/  DWORD 		field_1DE4;
/*0x1de8*/  DWORD 		field_1DE8;
/*0x1dec*/  DWORD 		field_1DEC;
/*0x1df0*/  DWORD 		field_1DF0;
/*0x1df4*/  DWORD 		field_1DF4;
/*0x1df8*/  DWORD 		field_1DF8;
/*0x1dfc*/  DWORD 		field_1DFC;
/*0x1e00*/  DWORD 		field_1E00;
/*0x1e04*/  BYTE  		field_1E04[0x2000];
/*0x3e04*/  BYTE  		field_3E04[0x4e20];
/*0x8c24*/  BYTE  		field_8C24[0xbb8];
/*0x97dc*/  BYTE  		field_97DC[0x28];
/*0x9804*/  DWORD 		field_9804;
/*0x9808*/  DWORD 		field_9808;
/*0x980c*/  DWORD 		field_980C;
/*0x9810*/  DWORD 		field_9810;
/*0x9814*/  DWORD 		field_9814;
/*0x9818*/  BYTE  		field_9818[0xc8];
/*0x98e0*/  BYTE  		field_98E0[0x50];
/*0x9930*/  DWORD 		field_9930;
/*0x9934*/  DWORD 		field_9934;
/*0x9938*/  DWORD 		field_9938;
/*0x993c*/  DWORD 		field_993C;
/*0x9940*/  BYTE  		field_9940[0x50];
/*0x9990*/  BYTE  		field_9990;
/*0x9991*/  BYTE  		Unknown0x9991[0xf];
/*0x99a0*/  DWORD 		field_99A0;
/*0x99a4*/  DWORD 		field_99A4;
/*0x99a8*/  BYTE  		field_99A8;
/*0x99a9*/  BYTE  		Unknown0x99a9[0x3];
/*0x99ac*/  DWORD 		field_99AC;
/*0x99b0*/  DWORD 		field_99B0;
/*0x99b4*/  DWORD 		field_99B4;
/*0x99b8*/  BYTE  		Unknown0x99b8[0x8];
/*0x99c0*/	struct		_CONTENTS* Bank[NUM_BANK_SLOTS]; 
/*0x9a08*/  DWORD 		field_9A08[0x24];
/*0x9a98*/  DWORD 		field_9A98;
/*0x9a9c*/  BYTE  		Unknown0x9a9c[0x10];
/*0x9aac*/  DWORD 		field_9AAC;
/*0x9ab0*/  BYTE  		field_9AB0[0x180];
/*0x9c30*/  BYTE  		field_9C30[0x18];
/*0x9c48*/  BYTE  		field_9C48[0x40];
/*0x9c88*/  BYTE  		field_9C88[0x50];
/*0x9cd8*/  BYTE  		Unknown0x9cd8[0x8];
/*0x9ce0*/
} CHARINFO, *PCHARINFO; 

typedef struct _MODELINFONAME {
/*0x00*/    DWORD Unknown0000;
/*0x04*/    DWORD Unknown0004;
/*0x08*/    PCHAR Name;
/*0x0c*/
} MODELINFONAME, *PMODELINFONAME;

typedef struct _MODELINFO_GENERIC {
/*0x00*/    DWORD Type;
/*0x04*/    DWORD Unknown0x04;
/*0x08*/    DWORD Unknown0x08;
/*0x0c*/    DWORD Unknown0x0c;
/*0x10*/    DWORD Unknown0x10;
/*0x14*/
} MODELINFO_GENERIC, *PMODELINFO_GENERIC;

typedef struct _MODELINFO_48 {
/*0x00*/    struct _MODELINFO_GENERIC Header;
/*0x14*/    FLOAT Float1;
/*0x18*/    FLOAT Float2;
/*0x1c*/    FLOAT Float3;
/*0x20*/    struct _MODELINFONAME *pModelName;
/*0x24*/
} MODELINFO_48, *PMODELINFO_48;

typedef struct _MODELINFO_51 {
/*0x00*/    struct _MODELINFO_GENERIC Header;
/*0x14*/    struct _MODELINFONAME *pFontName;
/*0x18*/    PCHAR LabelText;
/*0x1c*/
} MODELINFO_51, *PMODELINFO_51;

// 10/09/2003 build      plazmic
// 12/24/2003 verified   Amadeus
typedef struct _MODELINFO {
/*0x00*/    DWORD   Unknown;
/*0x04*/    PCHAR NameDAG;
/*0x08*/    struct _MODELINFO_GENERIC *pModelInfo;
/*0x0c*/    struct _MODELINFO *pNextInChain;
/*0x10*/    PVOID pUnknown;
/*0x14*/    struct _MODELINFONAME *pTrack1;
/*0x18*/    struct _MODELINFONAME *pTrack2;
   //moredata
} MODELINFO, *PMODELINFO;

// 4/30/2003 build      eqmule
typedef struct _CAMERAINFO {
/*0x00*/   DWORD Unknown0x00;
/*0x04*/   DWORD Unknown0x04;
/*0x08*/   BYTE Unknown0x08[0x8];
/*0x10*/   FLOAT Y;
/*0x14*/   FLOAT X;
/*0x18*/   FLOAT Z;
/*0x1c*/   FLOAT LightRadius;
/*0x20*/
} CAMERAINFO, *PCAMERAINFO;

// 4-30-2003  build      eqmule
// 10-28-2003 build      Amadeus
typedef struct _ACTORINFO {
/*0x0000*/   struct		_CAMERAINFO *pCamera;
/*0x0004*/   DWORD		Unknown0000;
/*0x0008*/   CHAR		ActorDef[0x40];
/*0x0048*/   FLOAT		Z;  // Z coordinates for the floor where standing
/*0x004c*/   BYTE       Unknown0x04c[0x4]; 
/*0x0050*/   DWORD      TimeStamp;     // Some kind of timestamp in microseconds. Updates as fast as my display can refresh. 
/*0x0054*/   DWORD      Unknown0x054;  // Being set to TimeStamp about once per second 
/*0x0058*/   DWORD      LastTick;      // Being set to TimeStamp on every tick? 
/*0x005c*/   DWORD      Unknown0x05c;  // Being set to TimeStamp at unknown intervals 
/*0x0060*/   BYTE       Unknown0x060[0x14]; 
/*0x0074*/   DWORD      Unknown0x074;  // Being set to TimeStamp at unknown intervals 
/*0x0078*/   DWORD      Unknown0x078;  // Being set to TimeStamp at unknown intervals 
/*0x007c*/   BYTE       Unknown0x07c[0x28]; 
/*0x00a4*/   BYTE       UnderWater;    // 5 = Underwater; otherwise zero 
/*0x00a5*/   BYTE       Swimming;      // 5 = Swimming (under or on top of water); otherwise zero 
/*0x00a6*/   BYTE       FeetWet;        // 5 = Feet are in the water; otherwise zero 
/*0x00a7*/   BYTE       LeftWater;     // 5 = Just got out of water, but still very close to shore 
/*0x00a8*/   BYTE       Unknown0x0a8[0x28]; 
/*0x00d0*/   DWORD      SpellETA;      // Calculated TimeStamp when current spell being casted lands. 0 while not casting. 
/*0x00d4*/   BYTE       Unknown0x0d4[0xb0];
/*0x0184*/   VOID		*FaceRelatedActorStruct;  //NEW
/*0x0188*/   DWORD		Unknown0x0188;
/*0x018c*/   DWORD		Animation; 
/*0x0190*/   DWORD		Unknown0x190;
/*0x0194*/   BYTE		Unknown0x194[0x1c];	
/*0x01b0*/   struct		_SPAWNINFO   *Mount;   // NULL if no mount present
/*0x01b4*/   BYTE		Unknown0x01b4[0x2434];
/*0x25e8*/	 DWORD      PetID;
/*0x25ec*/	 BYTE       Unknown0x25ec[0x8];
/*0x25f4*/   CHAR		NameOfLastSuccessfulTargetHit[0x40];
/*0x2634*/   BYTE		Unknown0x2634[0x006c];
/*0x26a0*/   DWORD		InvitedToGroup; // 1 = currently invited to group
/*0c26a4*/   DWORD		UnknownGroupRelatedFlag;
/*0x26a8*/   DWORD      CastingSpellID; // -1 = not casting a spell
/*0x26ac*/   DWORD      Unknown0x26ac[0x02];
/*0x26b4*/	 DWORD		UnknownModelRelatedPointer;  //research this later
/*0x26b8*/   struct		_MODELINFO *Model[0x14]; 
/*0x270c*/   BYTE		Unknown0x270c[0x28];
/*0x2730*/   struct		_SPAWNINFO   *WhoFollowing;  // NULL if autofollow off
/*0x2734*/   BYTE		Unknown0x2734[0x190];
/*0x28c4*/   DWORD		Trader;                      //0=normal 1=trader
/*0x28c8*/   BYTE		Unknown0x28c8[0x50];
} ACTORINFO, *PACTORINFO; 

#define MODEL_LABEL         0 
#define MODEL_LABELINFO     1
#define MODEL_NULL2         2
#define MODEL_HELD_R        3
#define MODEL_HELD_L        4
#define MODEL_SHIELD        5
#define MODEL_NULL6         6

/*
//Work in progress...
#define MODEL_HEAD			0x00
#define MODEL_HEAD_POINT	0x01
#define MODEL_NULL_1		0x02
#define MODEL_HELD_R		0x03
#define MODEL_HELD_L		0x04
#define MODEL_SHIELD		0x05
#define MODEL_NULL_2		0x06
#define MODEL_TUNIC			0x07
#define MODEL_HAIR			0x08
#define MODEL_BEARD			0x09
#define MODEL_CHEST			0x0a
#define MODEL_GLOVES		0x0b
#define MODEL_GLOVES2		0b0c
*/

typedef struct _ARGBCOLOR {
    union {
        struct {
            BYTE B;
            BYTE G;
            BYTE R;
            BYTE A;
        };
        DWORD ARGB;
    };
} ARGBCOLOR, *PARGBCOLOR;

// 10-22-2003 Lax
typedef struct _EQUIPMENT {
   union
   {
      struct// EQUIPARRAY
      {
         DWORD Item[9];
      };// Array;
      struct //EQUIPUNIQUE
      {
         DWORD Head;
         DWORD Chest;
         DWORD Arms;
         DWORD Wrists;
         DWORD Hands;
         DWORD Legs;
         DWORD Feet;
         DWORD Primary;
         DWORD Offhand;
      };// Unique;
   };
} EQUIPMENT, *PEQUIPMENT;

// actual size 0x1C0  12-19-2003
typedef struct _SPAWNINFO {
/*0x000*/   BYTE    Unknown0x000;
/*0x001*/   CHAR    Name[0x40];
/*0x041*/   CHAR	Lastname[0x20];
/*0x061*/   BYTE    Unknown0x061[0x7];
/*0x068*/   FLOAT   Y;
/*0x06c*/   FLOAT   X;
/*0x070*/   FLOAT   Z;
/*0x074*/   FLOAT   SpeedY;
/*0x078*/   FLOAT   SpeedX;
/*0x07C*/   FLOAT   SpeedZ;
/*0x080*/   FLOAT   SpeedRun;
/*0x084*/   FLOAT   Heading;
/*0x088*/   FLOAT   Unknown0x088;
/*0x08c*/   BYTE    Unknown0x08c[0x4];
/*0x090*/   FLOAT   SpeedHeading;
/*0x094*/   FLOAT   CameraAngle;
/*0x098*/   BYTE    Unknown0x098[0x44];
/*0x0dc*/   struct  _ACTORINFO   *pActorInfo;
/*0x0e0*/   BYTE    Unknown0x0e0[0x5];
/*0x0e5*/   BYTE    Sneak;  // returns 01 on both Sneak and Shroud of Stealth
/*0x0e6*/   BYTE    Linkdead;  // Uncharmable flag when used on mobs?
/*0x0e7*/   BYTE	Unknown0x0e7;
/*0x0e8*/   BYTE    LFG;
/*0x0e9*/   BYTE    Unknown0x0e9[0x3];
/*0x0ec*/   ARGBCOLOR ArmorColor[0x7];  // Armor Dye if custom, otherwise Item Tint
/*0x108*/   BYTE	Unknown0x108[0x8];
/*0x110*/   struct  _EQUIPMENT Equipment;
/*0x134*/   WORD    Zone;
/*0x136*/   WORD    Instance;
/*0x138*/   DWORD   Unknown0x138; // some pointer
/*0x13c*/   struct  _SPAWNINFO *pNext;
/*0x140*/   struct  _CHARINFO  *pCharInfo;
/*0x144*/   BYTE    Unknown0x144[0x4];
/*0x148*/   struct  _SPAWNINFO *pPrev;
/*0x14c*/   DWORD   Unknown0x14c;
/*0x150*/   BYTE    Unknown0x150[0x8];
/*0x158*/   FLOAT   Unknownf0x158;
/*0x15c*/   FLOAT   Unknownf0x15c;
/*0x160*/   FLOAT   Unknownf0x160; // commonly used by actor positioning
/*0x164*/   FLOAT   AvatarHeight;  // height of avatar from ground when standing
/*0x168*/   FLOAT   Unknownf0x168;
/*0x16c*/   BYTE    Type;
/*0x16d*/   BYTE    Face;      // Needs Testing
/*0x16e*/   BYTE	BeardColor;
/*0x16f*/   BYTE	Unknown0x16f;
/*0x170*/   BYTE    Eyes;
/*0x171*/   BYTE	Hair;      // Hair with no headgear (?)
/*0x172*/   BYTE	BeardType;
/*0x173*/   BYTE	Holding;   // 0=holding/wielding nothing
/*0x174*/   BYTE    Level;
/*0x175*/   BYTE    FaceHair;  // Face/Hair combination with headgear
/*0x176*/   BYTE    Gender;
/*0x177*/   BYTE    PvPFlag;
/*0x178*/   BYTE    HideMode;
/*0x179*/   BYTE    StandState;
/*0x17a*/   BYTE    Class;
/*0x17b*/   BYTE    Light;
/*0x17c*/   BYTE    InNonPCRaceIllusion;  // This is buggy ...not sure exact usage
/*0x17d*/   BYTE    Unknown0x17d;  // form related, unsure how
/*0x17e*/   BYTE    GM;
/*0x17f*/   BYTE    Unknown0x17f;
/*0x180*/   DWORD   SpawnID;
/*0x184*/   DWORD   MasterID;
/*0x188*/   DWORD   Race;
/*0x18c*/   DWORD   Anon;
/*0x190*/   DWORD   Unkonwn0x190;
/*0x194*/   DWORD   AFK;
/*0x198*/   DWORD   BodyType;
/*0x19c*/   DWORD   HPCurrent;
/*0x1a0*/   BYTE    AARank; //0=none 1=first 2=second 3=third 
/*0x1a1*/   BYTE    Unknown0x1a1[0x3]; 
/*0x1a4*/   DWORD   GuildStatus;
/*0x1a8*/   DWORD   Deity;
/*0x1ac*/   DWORD   HPMax;
/*0x1b0*/   DWORD   GuildID;
/*0x1b4*/   BYTE    Levitate;   //0-normal state  2=levitating 3=mob (not levitating)
/*0x1b5*/   BYTE    Unknown0x1b5[0xb]; 
/*0x1c0*/
} SPAWNINFO, *PSPAWNINFO;

#define STANDSTATE_STAND            0x64
#define STANDSTATE_BIND             0x69
#define STANDSTATE_SIT              0x6E
#define STANDSTATE_DUCK             0x6F
#define STANDSTATE_FEIGN            0x73
#define STANDSTATE_DEAD             0x78

#define MONITOR_SPAWN_X             1
#define MONITOR_SPAWN_Y             2
#define MONITOR_SPAWN_Z             4
#define MONITOR_SPAWN_HEADING       8
#define MONITOR_SPAWN_SPEEDRUN      16
#define MONITOR_SPAWN_HPCURRENT     32

typedef struct _SPAWNMONITORINFO {
    WORD SpawnID;
    FLOAT Y;
    FLOAT X;
    FLOAT Z;
    FLOAT Heading;
    FLOAT SpeedRun;
    DWORD HPCurrent;
    DWORD MonitorMask;
} SPAWNMONITORINFO, *PSPAWNMONITORINFO;

// 7-21-2003    Stargazer
#define   DOORLIST_STARTOFFSET      0x04
typedef struct _DOOR {
/*0x00*/   BYTE Unknown0x00;
/*0x01*/   BYTE ID;
/*0x02*/   CHAR Name[0x0b];
/*0x0d*/   BYTE Unknown0x0d[0x7];
/*0x14*/   FLOAT DefaultY;
/*0x18*/   FLOAT DefaultX;
/*0x1c*/   FLOAT DefaultZ;
/*0x20*/   FLOAT DefaultHeading;
/*0x24*/   FLOAT DoorAngle;
/*0x28*/   FLOAT Y;
/*0x2c*/   FLOAT X;
/*0x30*/   FLOAT Z;
/*0x34*/   FLOAT Heading;
/*0x38*/
} DOOR, *PDOOR;

// 7-21-2003    Stargazer
typedef struct _DOORTABLE {
/*0x000*/   DWORD NumEntries;
/*0x004*/   PDOOR pDoor[0x0FF];
/*0x400*/
} DOORTABLE, *PDOORTABLE;


// 8-26-2003 mule
typedef struct _GROUNDITEM {
/*0x00*/ struct _GROUNDITEM *pPrev;
/*0x04*/ struct _GROUNDITEM *pNext;
/*0x08*/ DWORD ID;
/*0x0C*/ DWORD DropID;
/*0x10*/ DWORD Unknown0x10;
/*0x14*/ DWORD DxID; // (class EQSwitch *)
/*0x18*/ DWORD Unknown0x18;
/*0x1C*/ FLOAT Heading;
/*0x20*/ FLOAT Z;
/*0x24*/ FLOAT X;
/*0x28*/ FLOAT Y;
/*0x2C*/ CHAR Name[0x18];
/*0x44*/
} GROUNDITEM, *PGROUNDITEM;

// 5-15-2003    Amadeus
#define   ZONELIST_STARTOFFSET      0x1c
#define   MAX_ZONES                                    0x129   
extern    PCHAR szZoneExpansionName[];     //defined in LibEQ_Utilities.cpp
typedef struct _ZONELIST {
/*0x000*/   DWORD   Header;
/*0x004*/   DWORD   Expansion;          // szZoneExpansionName[]
/*0x008*/   WORD    Id;
/*0x00a*/   WORD    Instance;
/*0x00c*/   CHAR    ShortName[0x81];
/*0x08d*/   CHAR    LongName[0x103];
/*0x190*/   DWORD   Flags;             
/*0x194*/   DWORD   Id2;                // This is Id+2242
/*0x198*/   DWORD   PoPValue;           // This has something to do with PoP zones.
/*0x19c*/   DWORD   MinLevel;           // Minimum level to access
/*0x1a0*/   WORD    Unknown0x1a0;         
/*0x1a2*/   BYTE    Unknown0x1a2[0x6];   
/*0x1a8     Next Zone in list */
} ZONELIST, *PZONELIST; 

// 5-15-2003    Amadeus (discovered by Lax)
// Actual Size 0x240, old
typedef struct _ZONEINFO {
/*0x000*/   CHAR    CharacterName[0x40];
/*0x040*/   CHAR    ShortName[0x20];
/*0x060*/   CHAR    LongName[0x80];
/*0x0E0*/   CHAR    Unknown0x0e0[0x96];  // <-- this isnt part of zone name, see CEverQuest__CEverQuest
/*0x176*/   BYTE    ZoneType; // (usually FF)
/*0x177*/	ARGBCOLOR FogRed;
/*0x17b*/	ARGBCOLOR FogGreen;
/*0x17f*/	ARGBCOLOR FogBlue;
/*0x183*/   BYTE    Unknown0x183;
/*0x184*/   BYTE	Unknown0x184[0x10];
/*0x194*/   BYTE    Unknown0x194[0x10];
/*0x1a4*/   FLOAT   ZoneGravity;
/*0x1a8*/   BYTE    Unknown0x1a8[0x4];
/*0x1ac*/   BYTE    Unknown0x1ac[0x2e];
/*0x1da*/	BYTE	SkyType;	
/*0x1db*/	BYTE	Unknown0x1db[0xd];
/*0x1e8*/   FLOAT   ZoneExpModifier;
/*0x1ec*/   FLOAT   SafeYLoc;
/*0x1f0*/   FLOAT   SafeXLoc;
/*0x1f4*/   FLOAT   SafeZLoc;
/*0x1f8*/   FLOAT   Ceiling;
/*0x1fc*/   FLOAT   Floor; 
/*0x200*/   FLOAT   MinClip; 
/*0x204*/   FLOAT   MaxClip; 
/*0x208*/   BYTE    Unknown0x208[0x18];
/*0x220*/   BYTE    Unknown0x220[0x20];
/*0x240*/
} ZONEINFO, *PZONEINFO;

#define   TOTAL_SPELL_COUNT         0x11E0       // # of spells in game
// note: fuck using TOTAL_SPELL_COUNT in the struct, but keep it for users to use
// because they cant figure out how to use pSpellMgr->InvalidSpell
typedef struct _SPELLMGR {
            BYTE                unknown[0x24];
            struct  _SPELL*     Spells[0x1388];
            struct  _SPELL*     Spells2[0x1388];
			struct _SPELL*		InvalidSpell;
} SPELLMGR, *PSPELLMGR;


// 12-19-2003  Amadeus
typedef struct _SPELL { 
/*0x000*/   DWORD   ID; 
/*0x004*/   FLOAT   Range; 
/*0x008*/   FLOAT   AERange; 
/*0x00c*/	FLOAT	PushBack;
/*0x010*/   BYTE    Unknown0x00c[0x04];  
/*0x014*/   DWORD   CastTime; 
/*0x018*/   DWORD   FizzleTime; 
/*0x01c*/   DWORD   RecastTime; 
/*0x020*/   DWORD   DurationType;       //DurationFormula on Lucy 
/*0x024*/   DWORD   DurationValue1; 
/*0x028*/   DWORD   Unknown0x028; 
/*0x02c*/   DWORD   Mana; 
/*0x030*/   LONG    Base[0x0c];         //Base1-Base12 
/*0x060*/   LONG    Base2[0x0c];     //Mostly unused, setting name to Base2 from Lucy for now 
/*0x090*/   LONG    Max[0x0c];          //Max1-Max12 
/*0x0c0*/   DWORD   BookIcon; 
/*0x0c4*/   DWORD   GemIcon; 
/*0x0c8*/   DWORD   ReagentId[0x4];     //ReagentId1-ReagentId4 
/*0x0d8*/   DWORD   ReagentCount[0x4];  //ReagentCount1-ReagentCount4 
/*0x0e8*/   BYTE	Unknown0x0e8[0x10]; 
/*0x0f8*/   BYTE	Calc[0x0c];         //Calc1-Calc12 
/*0x104*/   BYTE	LightType; 
/*0x105*/   BYTE	SpellType;          //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
/*0x106*/   BYTE	Unknown0x106; 
/*0x107*/   BYTE    Resist;             //0=un 1=mr 2=fr 3=cr 4=pr 5=dr 6=chromatic
/*0x108*/   BYTE	Attrib[0x0c];       //Attrib1-Attrib12 
/*0x114*/   BYTE    TargetType;         // 03=Group v1, 04=PB AE, 05=Single, 06=Self, 08=Targeted AE, 0e=Pet, 28=AE PC v2, 29=Group v2
/*0x115*/   BYTE	FizzleAdj; 
/*0x116*/   BYTE	Skill; 
/*0x117*/   BYTE    Location;            // 01=Outdoors, 02=dungeons, ff=Any 
/*0x118*/   BYTE	Environment;
/*0x119*/   BYTE	TimeOfDay;		     // 0=any, 1=day only, 2=night only
/*0x11a*/	BYTE	Unknown0x11a;
/*0x11b*/   BYTE    Level[0x10];         // per class. 
/*0x12b*/   BYTE    Unknown0x12b[0x11]; 
/*0x13c*/   DWORD   CastingAnim; 
/*0x140*/   DWORD   descnum; 
/*0x144*/   BYTE    Uninterruptable;  // 00=Interruptable, 01=Uninterruptable 
/*0x145*/	BYTE	Unknown0x145;
/*0x146*/	BYTE	Deletable;
/*0x147*/   BYTE    Unknown0x147;	
/*0x148*/   DWORD   Autocast;  // SpellID of spell to instacast on caster when current spell lands on target 
/*0x14c*/	BYTE	Unknown0x14c[0x06];
/*0x152*/   BYTE    DurationWindow; // 0=Long, 1=Short
/*0x153*/   BYTE    Unknown0x153;
/*0x154*/   DWORD   Unknown144;  // Unknown144 from lucy 
/*0x158*/   DWORD   Unknown145;  // Unknown145 from lucy 
/*0x15c*/   DWORD   Unknown0x15c; 
/*0x160*/   BYTE    Unknown0x160;
/*0x161*/	BYTE	Unknown0x161;  //data here on detrimental spells
/*0x162*/	BYTE	Unknown0x162;  //data here on detrimental spells 
/*0x163*/	BYTE	Unknown0x163[0x2d];
/*0x190*/   CHAR	*Name; 
/*0x194*/   CHAR	*Target; 
/*0x198*/   CHAR	*Extra;			// This is 'Extra' from Lucy (portal shortnames etc)
/*0x19c*/   CHAR	*Unknown0x19c; 
/*0x1a0*/   CHAR	*Unknown0x1a0;      
/*0x1a4*/   CHAR	*CastOnYou; 
/*0x1a8*/   CHAR	*CastOnAnother; 
/*0x1ac*/   CHAR	*WearOff; 
/*0x1b0*/   CHAR	*Unknown0x1b0;      
/*0x1b4*/   DWORD   spaindex; 
/*0x1b8*/   CHAR	*Unknown0x1b8;      
/*0x1bc*/   DWORD   SpellAnim; 
/*0x1c0*/   DWORD   Unknown0x1c0; 
/*0x1c4*/   DWORD   Unknown130;    // This is Unknown130 from Lucy 
/*0x1c8*/   DWORD   Unknown0x1c8; 
/*0x1cc*/   DWORD   SpellIcon; 
/*0x1d0*/	DWORD	ResistAdj;
/*0x1d4*/   BYTE	Unknown0x1d0[0x0c]; 
/*0x1e0*/ 
} SPELL, *PSPELL;

//works for 23 dec 2003 patch
#define EQ_INTERACTGROUNDITEM 0x00FB
typedef struct _INTERACTGROUNDITEM {
	DWORD DropID;
	DWORD SpawnID;
} INTERACTGROUNDITEM, *PINTERACTGROUNDITEM;

typedef struct _SPELLFAVORITE {
/*0x000*/   DWORD   SpellId[8];
/*0x020*/   CHAR    Name[25];
/*0x039*/   BYTE    Byte_39;
/*0x03A*/   BYTE    Byte_3A;
/*0x03B*/   BYTE    Byte_3B;
/*0x03C*/
} SPELLFAVORITE, *PSPELLFAVORITE;

typedef struct _CMDLIST {
    DWORD LocalizedStringID;
    PCHAR szName;
    PCHAR szLocalized;
    VOID  (__cdecl *fAddress)(PSPAWNINFO, PCHAR);
    DWORD Restriction;
    DWORD Category;
} CMDLIST, *PCMDLIST;

};
using namespace EQData;
