/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: ularn_win.c
 *
 * DESCRIPTION:
 * This module contains all operating system dependant code for input and
 * display update.
 * Each version of ularn should provide a different implementation of this
 * module.
 *
 * This is the Windows 32 window display and input module.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * init_app               : Initialise the app
 * close_app              : Close the app and free resources
 * get_normal_input       : Get the next command input
 * get_prompt_input       : Get input in response to a question
 * get_password_input     : Get a password
 * get_num_input          : Geta number
 * get_dir_input          : Get a direction
 * set_display            : Set the display mode
 * ClearText              : Clear the text output area
 * UlarnBeep              : Make a beep
 * Cursor                 : Set the cursor location
 * Printc                 : Print a single character
 * Print                  : Print a string
 * Printf                 : Print a formatted string
 * Standout               : Print a string is standout format
 * SetFormat              : Set the output text format
 * ClearEOL               : Clear to end of line
 * ClearEOPage            : Clear to end of page
 * show1cell              : Show 1 cell on the map
 * showplayer             : Show the player on the map
 * showcell               : Show the area around the player
 * drawscreen             : Redraw the screen
 * draws                  : Redraw a section of the screen
 * mapeffect              : Draw a directional effect
 * magic_effect_frames    : Get the number of animation frames in a magic fx
 * magic_effect           : Draw a frame in a magic fx
 * nap                    : Delay for a specified number of milliseconds
 * GetUser                : Get the username and user id.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdarg.h>

#include "header.h"
#include "ularn_game.h"

#include "dungeon.h"
#include "player.h"
#include "ularn_win.h"
#include "monster.h"
#include "itm.h"
#include "show.h"
#include "savegame.h"

/* =============================================================================
 * Local variables
 */

static int beep;
static char json_buffer[8000];
static char map_effect_json[8000] = "";


/* Tiles for different character classes, (female, male) */
static int PlayerTiles[8][2] =
{
  { 165, 181 }, /* Ogre */
  { 166, 182 }, /* Wizard */
  { 167, 183 }, /* Klingon */
  { 168, 184 }, /* Elf */
  { 169, 185 }, /* Rogue */
  { 170, 186 }, /* Adventurer */
  { 171, 187 }, /* Dwarf */
  { 172, 188 } /* Rambo */
};

#define TILE_CURSOR1 174
#define TILE_CURSOR2 190
#define WALL_TILES   352

/* Tiles for directional effects */
static int EffectTile[EFFECT_COUNT][9] =
{
  { 191, 198, 196, 194, 192, 195, 193, 197, 199 },
  { 191, 206, 204, 202, 200, 203, 201, 205, 207 },
  { 191, 214, 212, 210, 208, 211, 209, 213, 215 },
  { 191, 222, 220, 218, 216, 219, 217, 221, 223 },
  { 191, 230, 228, 226, 224, 227, 225, 229, 231 }
};

#define MAX_MAGICFX_FRAME 8

struct MagicEffectDataType
{
  int Overlay;                  /* 0 = no overlay, 1 = overlay     */
  int Frames;                   /* Number of frames in the effect  */
  int Tile1[MAX_MAGICFX_FRAME]; /* The primary tile for this frame */
  int Tile2[MAX_MAGICFX_FRAME]; /* Only used for overlay effects   */
};

static struct MagicEffectDataType magicfx_tile[MAGIC_COUNT] =
{
  /* Sparkle */
  { 1, /* Overlay this on current tile */
    8,
    { 240, 241, 242, 243, 244, 245, 246, 247 },
    { 248, 249, 250, 251, 252, 253, 254, 255 }
  },

  /* Sleep */
  {
    0,
    6,
    { 256, 272, 288, 304, 320, 336, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Web */
  {
    0,
    6,
    { 257, 273, 289, 305, 321, 337, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Phantasmal forces */
  {
    0,
    6,
    { 258, 274, 290, 306, 322, 338, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Cloud kill */
  {
    0,
    6,
    { 259, 275, 291, 307, 323, 339, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Vaporize rock */
  {
    0,
    6,
    { 260, 276, 292, 308, 324, 340, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Dehydrate */
  {
    0,
    6,
    { 261, 277, 293, 309, 325, 341, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Drain life */
  {
    0,
    6,
    { 262, 278, 294, 310, 326, 342, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Flood */
  {
    0,
    6,
    { 263, 279, 295, 311, 327, 343, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Finger of death */
  {
    0,
    6,
    { 264, 280, 296, 312, 328, 344, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Teleport away */
  {
    0,
    6,
    { 265, 281, 297, 313, 329, 345, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Magic fire */
  {
    0,
    6,
    { 266, 282, 298, 314, 330, 346, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Make wall */
  {
    0,
    6,
    { 267, 283, 299, 315, 331, 347, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Summon demon */
  {
    0,
    6,
    { 268, 284, 300, 316, 332, 348, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  },

  /* Annihilate (scroll) */
  {
    0,
    6,
    { 269, 285, 301, 317, 333, 349, 0, 0 },
    { 0, 0, 0, 0, 0, 0 ,0 ,0 }
  }
};


//
// Messages
//
static char Text[8000] = "";

//
// The monster to use for showing mimics. Changes every 10 turns.
//
static MonsterIdType mimicmonst = MIMIC;

/* =============================================================================
 * Local functions
 */

/*
 * Repaint flag to force redraw of everything, not just deltas
 */

char *get_status_json()
{
	if (level == 0 || wizard)
		c[TELEFLAG] = 0;

	sprintf(json_buffer, "{\"spells\":%ld,\"spellMax\":%ld,\"ac\":%ld,\"wClass\":%ld,\"level\":%ld,\"experience\":%ld,\"class\":\"%s\",\"hp\":%ld,\"hpMax\":%ld,\"strength\":%ld,\"intelligence\":%ld,\"wisdom\":%ld,\"constitution\":%ld,\"dexterity\":%ld,\"charisma\":%ld,\"levelName\":\"%s\",\"gold\":%ld,\"packWeight\":%d,\"outstandingTaxes\":%ld}",
		c[SPELLS], c[SPELLMAX], c[AC], c[WCLASS], c[LEVEL], c[EXPERIENCE], class[c[LEVEL] - 1], c[HP] >= 0 ? c[HP] : 0, c[HPMAX], c[STRENGTH] + c[STREXTRA], c[INTELLIGENCE], c[WISDOM], c[CONSTITUTION], c[DEXTERITY], c[CHARISMA], c[TELEFLAG] ? "?" : levelname[level], c[GOLD], packweight(), outstanding_taxes);

	//
	// Mark all character values as displayed.
	//
	c[TMP] = c[STRENGTH] + c[STREXTRA];
	for (int i = 0; i < 100; i++)
		cbak[i] = c[i];

	return json_buffer;
}

long get_stat_hp()
{
    return c[HP] >= 0 ? c[HP] : 0;
}

long get_stat_hp_max()
{
    return c[HPMAX];
}

long get_stat_spells()
{
    return c[SPELLS];
}

long get_stat_spell_max()
{
    return c[SPELLMAX];
}

long get_stat_gold()
{
    return c[GOLD];
}

char *get_stat_level_name()
{
    return c[TELEFLAG] ? "?" : levelname[level];
}

/* Effects strings */
static struct bot_side_def
{
  int typ;
  char *string;
} bot_data[] =
{
  { STEALTH,        "Stealth" },
  { UNDEADPRO,      "Undead Protection" },
  { SPIRITPRO,      "Spirit Protection" },
  { CHARMCOUNT,     "Charm" },
  { TIMESTOP,       "Time Stop" },
  { HOLDMONST,      "Hold Monster" },
  { GIANTSTR,       "Giant Strength" },
  { FIRERESISTANCE, "Fire Resistance" },
  { DEXCOUNT,       "Dexterity" },
  { STRCOUNT,       "Strength" },
  { SCAREMONST,     "Scare Monster" },
  { HASTESELF,      "Haste Self" },
  { CANCELLATION,   "Cancellation" },
  { INVISIBILITY,   "Invisibility" },
  { ALTPRO,         "Protection By Altar" },
  { PROTECTIONTIME, "Protection" },
  { WTW,            "Walk Through Walls" }
};

char *get_effects_json()
{
	strcpy(json_buffer, "[");

	for (int i = 0; i < 17; i++)
	{
        if (!c[bot_data[i].typ])
            continue;
		if (json_buffer[1] != 0)
			strcat(json_buffer, ",");
		strcat(json_buffer, "\"");
		strcat(json_buffer, bot_data[i].string);
		strcat(json_buffer, "\"");
	}

	strcat(json_buffer, "]");
	return json_buffer;
}

/* =============================================================================
 * FUNCTION: GetTile
 *
 * DESCRIPTION:
 * Get the tile to be displayed for a location on the map.
 *
 * PARAMETERS:
 *
 *   x      : The x coordinate for the tile
 *
 *   y      : The y coordiante for the tile
 *
 *   TileId : This is set to the tile to be displayed for (x, y).
 *
 * RETURN VALUE:
 *
 *   None.
 */
void GetTile(int x, int y, int *TileId)
{
  MonsterIdType k;

  if ((x == playerx) && (y == playery) && (c[BLINDCOUNT] == 0))
  {
    //
    // This is the square containing the player and the players isn't
    // blind, so return the player tile.
    //
    *TileId = PlayerTiles[class_num][(int) sex];
    return;
  }

  //
  // Work out what is here
  //
  if (know[x][y] == OUNKNOWN)
  {
    //
    // The player doesn't know what is at this position.
    //
    *TileId = objtilelist[OUNKNOWN];
  }
  else
  {
    k = mitem[x][y].mon;
    if (k != 0)
    {
      if ((c[BLINDCOUNT] == 0) &&
          (((stealth[x][y] & STEALTH_SEEN) != 0) ||
           ((stealth[x][y] & STEALTH_AWAKE) != 0)))
      {
        //
        // There is a monster here and the player is not blind and the
        // monster is seen or awake.
        //
        if (k == MIMIC)
        {
          if ((gtime % 10) == 0)
          {
            while ((mimicmonst = rnd(MAXMONST))==INVISIBLESTALKER);
          }

          *TileId = monsttilelist[mimicmonst];
        }
        else if ((k==INVISIBLESTALKER) && (c[SEEINVISIBLE]==0))
        {
          *TileId = objtilelist[(int) know[x][y]];
        }
        else if ((k>=DEMONLORD) && (k<=LUCIFER) && (c[EYEOFLARN]==0))
        {
          /* demons are invisible if not have the eye */
          *TileId = objtilelist[(int) know[x][y]];
        }
        else
        {
          *TileId = monsttilelist[k];
        }

      } /* can see monster */
      else
      {
        /*
         * The monster at this location is not known to the player, so show
         * the tile for the item at this location
         */
        *TileId = objtilelist[(int) know[x][y]];
      }
    } /* monster here */
    else
    {
      k = know[x][y];
      *TileId = objtilelist[k];
    }
  }
  
  /* Handle walls */
  if (*TileId == objtilelist[OWALL])
  {
    *TileId = WALL_TILES + iarg[x][y];
  }
}

int get_map_width()
{
    return MAXX;
}

int get_map_height()
{
    return MAXY;
}

int get_map_tile(int x, int y)
{
    int tileId;
    GetTile(x, y, &tileId);
    return tileId;
}

char *get_text_html()
{
	strcpy(json_buffer, Text);
    Text[0] = 0;
    return json_buffer;
}

/* =============================================================================
 * Exported functions
 */


/* =============================================================================
 * FUNCTION: UlarnBeep
 */
void UlarnBeep(void)
{
	beep = 1;
}

/* =============================================================================
 * FUNCTION: Printc
 */
void Printc(char c)
{
  if (player_score_json[0] != 0)
    return;
  int length = strlen(Text);
  Text[length] = c;
  Text[length + 1] = 0;
}

/* =============================================================================
 * FUNCTION: Print
 */
static int ends_with_punctuation(char *string)
{
  switch (string[strlen(string) - 1])
  {
    case '.':
    case '!':
    case ':':
    case ',':
    case '?':
    case ')':
    case ']':
      return 1;
  }
  return 0;
}

void Print(char *string)
{
  if (player_score_json[0] != 0)
    return;
  if (string == NULL) return;
  strcat(Text, string);
  if (ends_with_punctuation(string))
    strcat(Text, "  ");
}

/* =============================================================================
 * FUNCTION: Printf
 */
void Printf(char *fmt, ...)
{
  if (player_score_json[0] != 0)
    return;
    
  va_list argptr;

  va_start(argptr, fmt);
  vsprintf(Text + strlen(Text), fmt, argptr);
  va_end(argptr);
  
  if (ends_with_punctuation(Text))
    strcat(Text, "  ");
}

/* =============================================================================
 * FUNCTION: show1cell
 */
void show1cell(int x, int y)
{
  /* see nothing if blind   */
  if (c[BLINDCOUNT]) return;

  /* we end up knowing about it */
  know[x][y] = item[x][y];
  if (mitem[x][y].mon != MONST_NONE)
  {
    stealth[x][y] |= STEALTH_SEEN;
  }
}

/* =============================================================================
 * FUNCTION: showcell
 */
void showcell(int x, int y)
{
  int minx, maxx;
  int miny, maxy;
  int mx, my;

  /*
   * Decide how much the player knows about around him/her.
   */
  if (c[AWARENESS])
  {
    minx = x-3;
    maxx = x+3;
    miny = y-3;
    maxy = y+3;
  }
  else
  {
    minx = x-1;
    maxx = x+1;
    miny = y-1;
    maxy = y+1;
  }

  if (c[BLINDCOUNT])
  {
    minx = x;
    maxx = x;
    miny = y;
    maxy = y;
  }

  /*
   * Limit the area to the map extents
   */
  if (minx < 0) minx = 0;
  if (maxx > MAXX-1) maxx = MAXX-1;
  if (miny < 0) miny=0;
  if (maxy > MAXY-1) maxy = MAXY-1;

  for (my = miny; my <= maxy; my++)
  {
    for (mx = minx; mx <= maxx; mx++)
    {
      if ((mx == playerx) && (my == playery))
      {
        know[mx][my] = item[mx][my];
      }
      else if ((know[mx][my] != item[mx][my]) ||       /* item changed    */
               ((mx == lastpx) && (my == lastpy)) ||   /* last player pos */
               ((mitem[mx][my].mon != MONST_NONE) &&   /* unseen monster  */
                ((stealth[mx][my] & STEALTH_SEEN) == 0)))
      {
        //
        // Only draw areas not already known (and hence displayed)
        //
        know[mx][my] = item[mx][my];
        if (mitem[mx][my].mon != MONST_NONE)
        {
          stealth[mx][my] |= STEALTH_SEEN;
        }

      } // if not known

    }
  }
}

/* =============================================================================
 * FUNCTION: mapeffect
 */
void mapeffect(int x, int y, DirEffectsType effect, int dir)
{
  int TileId;

  /* see nothing if blind   */
  if (c[BLINDCOUNT]) return;

  TileId = EffectTile[effect][dir];
  if (map_effect_json[0] != 0 && map_effect_json[strlen(map_effect_json) - 1] != '[')
	  strcat(map_effect_json, ",");
  sprintf(map_effect_json + strlen(map_effect_json),
	  "{\"x\":%d,\"y\":%d,\"tileIds\":[%d]}", x, y, TileId);
}

/* =============================================================================
 * FUNCTION: magic_effect_frames
 */
int magic_effect_frames(MagicEffectsType fx)
{
  return magicfx_tile[fx].Frames;
}

/* =============================================================================
 * FUNCTION: magic_effect
 */
void magic_effect(int x, int y, MagicEffectsType fx, int frame)
{
  int TileId;

  if (frame > magicfx_tile[fx].Frames)
  {
    return;
  }

  /*
   * draw the tile that is at this location
   */

  /* see nothing if blind   */
  if (c[BLINDCOUNT]) return;

  if (map_effect_json[0] != 0 && map_effect_json[strlen(map_effect_json) - 1] != '[')
	  strcat(map_effect_json, ",");

  if (magicfx_tile[fx].Overlay)
  {
    GetTile(x, y, &TileId);
	sprintf(map_effect_json + strlen(map_effect_json),
		"{\"x\":%d,\"y\":%d,\"tileIds\":[%d,%d,%d]}",
		x, y, TileId, magicfx_tile[fx].Tile1[frame], magicfx_tile[fx].Tile2[frame]);
  }
  else
  {
	sprintf(map_effect_json + strlen(map_effect_json),
		"{\"x\":%d,\"y\":%d,\"tileIds\":[%d]}",
		x, y, magicfx_tile[fx].Tile1[frame]);
  }

}

char *get_map_effect_json()
{
    if (map_effect_json[0] != 0)
    {
        sprintf(json_buffer, "[[%s]]", map_effect_json);
        map_effect_json[0] = 0;
    }
    else
        strcpy(json_buffer, "[]");
    return json_buffer;
}

/* =============================================================================
 * FUNCTION: nap
 */
void nap()
{
  if (map_effect_json[0] != 0 && map_effect_json[strlen(map_effect_json) - 1] != '[')
    strcat(map_effect_json, "],[");
}

//
//
//

static char input_type[20] = "";
static char callback[20] = "";
static char options[8000] = "";

void set_input_type(char *type)
{
	strcpy(input_type, type);
}

void set_callback(char *function)
{
	strcpy(callback, function);
}

char *get_callback()
{
    return callback;
}

void add_option(int code, char *text, char *subtext)
{
	if (options[0] != 0)
		strcat(options, ",");
	sprintf(options + strlen(options), "{\"code\":%d,\"text\":\"%s\",\"subtext\":\"%s\"}", code, text, subtext);
}

char *get_option_json()
{
	sprintf(json_buffer, "{\"inputType\":\"%s\",\"callback\":\"%s\",\"options\":[%s]}", input_type, callback, options);
	input_type[0] = 0;
	callback[0] = 0;
	options[0] = 0;
	return json_buffer;
}

int get_player_x()
{
	return playerx;
}

int get_player_y()
{
	return playery;
}

char *get_inventory_json()
{
	int can_drop = item[playerx][playery] == ONOTHING;
	strcpy(json_buffer, "[");

	if (c[GOLD])
	{
		sprintf(json_buffer + strlen(json_buffer), "{\"code\":%d,\"name\":\"%ld gold piece%s\",\"options\":[", '.', c[GOLD], plural(c[GOLD]));
		if (can_drop)
			strcat(json_buffer, "{\"text\":\"Drop\",\"action\":\"dropobj\"}");
		strcat(json_buffer, "]}");
	}

	for (int i = 0; i < IVENSIZE; i++)
	{
		if (iven[i] == ONOTHING)
			continue;

		if (json_buffer[1] != 0)
			strcat(json_buffer, ",");
		sprintf(json_buffer + strlen(json_buffer), "{\"code\":%d,\"name\":\"%s\",\"options\":[", i + 'a', get_inventory_name3(i));
		int first_option = 1;

		if (can_drop)
		{
			strcat(json_buffer, "{\"text\":\"Drop\",\"action\":\"dropobj\"}");
			first_option = 0;
		}

		switch (iven[i])
		{
			case OLEATHER:
			case OPLATE:
			case OCHAIN:
			case ORING:
			case OSTUDLEATHER:
			case OSPLINT:
			case OPLATEARMOR:
			case OSSPLATE:
			case OSHIELD:
			case OELVENCHAIN:
				if (!first_option)
					strcat(json_buffer, ",");
				if (c[WEAR] != i && c[SHIELD] != i)
					strcat(json_buffer, "{\"text\":\"Wear\",\"action\":\"wear\"}");
				else
					strcat(json_buffer, "{\"text\":\"Take off\",\"action\":\"takeoff\"}");
				first_option = 0;
				break;
		}

		switch (iven[i])
		{
			case OPLATE:
			case OCHAIN:
			case OLEATHER:
			case ORING:
			case OSTUDLEATHER:
			case OSPLINT:
			case OPLATEARMOR:
			case OSSPLATE:
			case OSHIELD:
			case OELVENCHAIN:
			case OSWORDofSLASHING:
			case OHAMMER:
			case OSWORD:
			case O2SWORD:
			case OSPEAR:
			case ODAGGER:
			case OBATTLEAXE:
			case OLONGSWORD:
			case OFLAIL:
			case OLANCE:
			case OVORPAL:
			case OSLAYER:
			case ORINGOFEXTRA:
			case OREGENRING:
			case OPROTRING:
			case OENERGYRING:
			case ODEXRING:
			case OSTRRING:
			case OCLEVERRING:
			case ODAMRING:
			case OBELT:
			case OAMULET:
			case OORBOFDRAGON:
			case OPSTAFF:
			case OLIFEPRESERVER:
			case OANNIHILATION:
			case OCOOKIE:
			case OSPEED:
			case OACID:
			case OHASH:
			case OSHROOMS:
			case OCOKE:
				if (!first_option)
					strcat(json_buffer, ",");
				if (c[WIELD] != i)
					strcat(json_buffer, "{\"text\":\"Wield\",\"action\":\"wield\"}");
				else
					strcat(json_buffer, "{\"text\":\"Unwield\",\"action\":\"unwield\"}");
				first_option = 1;
				break;
		}

		switch (iven[i])
		{
			case OBOOK:
			case OSCROLL:
				if (!first_option)
					strcat(json_buffer, ",");
				strcat(json_buffer, "{\"text\":\"Read\",\"action\":\"readscr\"}");
				first_option = 0;
				break;
		}

		switch (iven[i])
		{
			case OCOOKIE:
				if (!first_option)
					strcat(json_buffer, ",");
				strcat(json_buffer, "{\"text\":\"Eat\",\"action\":\"eat\"}");
				first_option = 0;
				break;
		}

		switch (iven[i])
		{
			case OPOTION:
				if (!first_option)
					strcat(json_buffer, ",");
				strcat(json_buffer, "{\"text\":\"Drink\",\"action\":\"quaff\"}");
				first_option = 0;
				break;
		}

		strcat(json_buffer, "]}");
	}

	strcat(json_buffer, "]");
	return json_buffer;
}


MemoryFile *savegame_file = NULL;

void free_savegame()
{
    if (savegame_file != NULL)
    {
        MemoryFile_dispose(savegame_file);
        savegame_file = NULL;
    }
}

char *get_savegame()
{
    free_savegame();
    savegame_file = savegame();
    return savegame_file->start;
}


char header[2000] = "";

void print_header(char *string)
{
    strcat(header, string);
    if (ends_with_punctuation(string))
        strcat(header, "  ");
}

void printf_header(char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vsprintf(header + strlen(header), format, argptr);
    va_end(argptr);
    if (ends_with_punctuation(header))
        strcat(header, "  ");
}

char *get_header_html()
{
    strcpy(json_buffer, header);
    header[0] = 0;
    return json_buffer;
}


