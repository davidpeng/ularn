/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: ularn.c
 *
 * DESCRIPTION:
 * This is the main module for ularn.
 * It contains the setup and main processing loop.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * None.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#include "patchlevel.h"

#include "ularn_game.h"
#include "ularn_win.h"
#include "ularn_ask.h"
#include "getopt.h"
#include "savegame.h"
#include "scores.h"
#include "header.h"
#include "dungeon_obj.h"
#include "dungeon.h"
#include "player.h"
#include "monster.h"
#include "action.h"
#include "object.h"
#include "potion.h"
#include "scroll.h"
#include "spell.h"
#include "show.h"
#include "help.h"
#include "diag.h"
#include "itm.h"

/* =============================================================================
 * Local variables
 */

#define BUFSZ   256 /* for getlin buffers */
#define MAX_CMDLINE_PARAM 255

static char copyright[]=
  "\nUlarn created by Phil Cordier -- based on Larn by Noah Morgan\n"
  "  Updated by Josh Brandt and David Richerby\n"
  "  Rewrite and Windows32/X11/Amiga graphics conversion by Julian Olds";

static char cmdhelp[] = "\
Cmd line format: Ularn [-sicnh] [-o <optsfile>] [-d #] [-r]\n\
  -s   show the scoreboard\n\
  -i   show scoreboard with inventories\n\
  -c   create new scoreboard (wizard only)\n\
  -n   suppress welcome message on starting game\n\
  -h   print this help text\n\
  -o <optsfile> specify .Ularnopts file to be used instead of \"~/.Ularnopts\"\n\
  -d # specify level of difficulty (example: Ularn -d 5)\n\
  -r   restore checkpoint (.ckp) file\n";

static char *optstring = "sicnhro:d:";

static short viewflag;

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: ULarnSetup
 *
 * DESCRIPTION:
 * Perform once off initialisation and parse the command parameters.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
//extern int scoreboardExists();

int difficulty = 0;

void set_difficulty(int diff)
{
    difficulty = diff;
}

int get_difficulty()
{
    return c[HARDGAME];
}

void ULarnSetup(char character_class)
{
  int restore_ckp;

  /* clear the loginname and logname */
  loginname[0] = 0;
  logname[0] = 0;

  nap(100);

  /* initialize dungeon storage */

  init_cells();

  /* set the initial clock and initialise the random number generator*/
  newgame();
  restore_ckp = 0;

  /* now make scoreboard if it is not there (don't clear) */
  if (/*!scoreboardExists()*/1)
  {
    /* score file not there */
    if (makeboard() == -1)
    {
      Printf("I can't create the scoreboard.");
      Printf("Check permissions on %s\n", libdir);
      endgame();
    }
  }

  /* the pre-made cave level data file */
  sprintf(larnlevels, "%s/%s", libdir, LEVELSNAME);

  /* the fortune data file name */
  sprintf(fortfile, "%s/%s", libdir, FORTSNAME);

  /* create new game */
  if (restorflag == 0 && character_class != 0)
  {
    /* make the character that will play */
    char_picked = character_class;
    wizard = 0;
    makeplayer();
    /* make the dungeon */
    newcavelevel(0);
    
    if (nowelcome == 0)
    {
      /* welcome the player to the game */
      welcome();
    }
  }

  /* set up the desired difficulty  */
  sethard(difficulty);

  showplayer();

  yrepcount = 0;
  hit2flag = 0;

  EM_ASM(
    showGame();
  );
}

/* =============================================================================
 * FUNCTION: parse
 *
 * DESCRIPTION:
 * Execute a command entered by the player.
 *
 * PARAMETERS:
 *
 *   Action : The action command requested.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void parse (ActionType Action)
{
  int i, j;
  int flag;

  switch (Action)
  {
    case ACTION_DIAG:
      if (wizard)
      {
        diag();
      }
      yrepcount = 0;
      return;

    case ACTION_MOVE_WEST:
      moveplayer(4);
      return;

    case ACTION_RUN_WEST:
      run(4);
      return;

    case ACTION_MOVE_EAST:
     moveplayer(2);
     return;

    case ACTION_RUN_EAST:
      run(2);
      return;

    case ACTION_MOVE_SOUTH:
      moveplayer(1);
      return;

    case ACTION_RUN_SOUTH:
      run(1);
      return;

    case ACTION_MOVE_NORTH:
      moveplayer(3);
      return;

    case ACTION_RUN_NORTH:
      run(3);
      return;

    case ACTION_MOVE_NORTHEAST:
      moveplayer(5);
      return;

    case ACTION_RUN_NORTHEAST:
      run(5);
      return;

    case ACTION_MOVE_NORTHWEST:
      moveplayer(6);
      return;

    case ACTION_RUN_NORTHWEST:
      run(6);
      return;

    case ACTION_MOVE_SOUTHEAST:
      moveplayer(7);
      return;

    case ACTION_RUN_SOUTHEAST:
      run(7);
      return;

    case ACTION_MOVE_SOUTHWEST:
      moveplayer(8);
      return;

    case ACTION_RUN_SOUTHWEST:
      run(8);
      return;

    case ACTION_WAIT:
      if (yrepcount) viewflag=1;
      return;

    case ACTION_NONE:
      yrepcount = 0;
      nomove = 1;
      return;

    case ACTION_CAST_SPELL:
      yrepcount=0;
  	  j = 0;
	  for (i = 0; i < SPELL_COUNT; i++)
	  {
		if (spelknow[i])
		{
			add_option(i, spelname[i], "");
			j++;
		}
	  }
	  if (j > 0)
      {
        add_option(-1, "Back", "");
	  	set_callback("cast");
      }
	  else
		Print("You don't know any spells.");
      return;

    case ACTION_OPEN_CHEST:
      yrepcount=0;
      openchest();
      return;

    case ACTION_INVENTORY:
      yrepcount=0;
      nomove=1;
      showstr();
      return;

    case ACTION_LIST_SPELLS:
      yrepcount=0;
      seemagic(0);
      nomove=1;
      return;

    case ACTION_HELP:
      yrepcount=0;
      help();
      nomove=1;
      return;

    case ACTION_SAVE:
      Print("Saving . . .");

      if (savegame() == NULL)
      {
        Print("Save game failed.");
      }
      wizard=1;   /* so not show scores */
      died(DIED_SUSPENDED, 0);
      return;

    case ACTION_TELEPORT:
      yrepcount=0;
      if (wizard)
      {
		set_input_type("number");
		set_callback("action_teleport");
        return;
      }
      if (c[LEVEL] >= INNATE_TELEPORT_LEVEL)
      {
        oteleport(1);
        return;
      }

      Print("You don't know how to teleport yet.");
      return;

    case ACTION_IDENTIFY_TRAPS:
      flag=0;
      yrepcount=0;

      for (j=playery-1; j<playery+2; j++)
      {
        if (j < 0) j=0;
        if (j >= MAXY) break;
        for (i=playerx-1; i<playerx+2; i++)
        {
          if (i < 0) i=0;
          if (i >= MAXX) break;
          switch (item[i][j])
          {
            case OTRAPDOOR:
            case ODARTRAP:
            case OTRAPARROW:
            case OTELEPORTER:
            case OELEVATORUP:
            case OELEVATORDOWN:
              Print("It's ");
              Print(objectname[(int) item[i][j]]);
              flag++;
          }
        }
      }
      if (flag==0)
        Print("No traps are visible.");
      return;

    case ACTION_BECOME_CREATOR:
      yrepcount=0;
      nomove=1;

      raiseexperience(370 * 1000000);

      recalc();
      UpdateStatus();
      drawscreen();
      return;

    case ACTION_TOGGLE_WIZARD:
      yrepcount=0;
      nomove=1;
      if (wizard)
      {
        Print("You are no longer a wizard.");
        wizard = 0;
        return;
      }
      Print("You are now a wizard.");
      wizard = 1;
      return;

    case ACTION_DEBUG_MODE:
      yrepcount=0;
      nomove=1;
      if (!wizard)
      {
        wizard=1;
      }

      for (i=0; i<6; i++)
      {
        c[i]=70;
      }
      iven[0] = ONOTHING;
      iven[1] = ONOTHING;
      take(OPROTRING,50);
      take(OLANCE,25);
      for (i=0; i<IVENSIZE; i++)
      {
        if (iven[i]==OLANCE && ivenarg[i]==25)
        {
          c[WIELD]=i;
          break;
        }
      }
      c[LANCEDEATH]=1;
      c[WEAR] = c[SHIELD] = -1;
      raiseexperience(370*1000000);
      c[AWARENESS] += 25000;

      /* learn all spells, scrolls and potions */
      for (i = 0; i < SPELL_COUNT ; i++) spelknow[i]=1;
      for (i = 0; i < MAXSCROLL ; i++) potionknown[i]=1;
      for (i = 0; i < MAXPOTION ; i++) potionknown[i]=1;

      for (i = 0; i < MAXSCROLL; i++)
      {
        if (strlen(scrollname[i]) > 2)
        {
          item[i][0] = OSCROLL;
          iarg[i][0] = (short) i;
        }
      }

      for (i = 0 ; i < MAXPOTION ; i++)
      {
        /* no null items */
        if (strlen(potionname[i]) > 2)
        {
          item[(MAXX-1)-i][0] = OPOTION;
          iarg[(MAXX-1)-i][0] = (short) (i);
        }
      }

      j = OALTAR;

      for (i = 1; i < MAXY; i++)
      {
        item[0][i] = (char) j;
        iarg[0][i] = (short) 0;
        j++;
      }

      for (i = 1; i < MAXX ; i++)
      {
        item[i][MAXY-1] = (char) j;
        iarg[i][MAXY-1] = (short) 0;
        j++;
      }

      for (i = 1 ; i < MAXY - 1 ; i++)
      {
        item[MAXX-1][i] = (char) j;
        iarg[MAXX-1][i] = 0;
        j++;
      }

      for (i=0; i<MAXY; i++)
      {
        for (j=0; j<MAXX; j++)
        {
          know[j][i] = item[j][i];
        }
      }

      c[GOLD] += 250000;

      recalc();
      UpdateStatus();
      drawscreen();
      return;

    case ACTION_REMOVE_ARMOUR:
      yrepcount=0;

      if (c[SHIELD] != -1)
      {
        c[SHIELD] = -1;
        Print("Your shield is off.");
        recalc();
        UpdateStatus();
      }
      else
      {
        if (c[WEAR] != -1)
        {
          c[WEAR] = -1;
          Print("Your armor is off.");
          recalc();
          UpdateStatus();
        }
        else
        {
          Print("You aren't wearing anything.");
        }
      }
      return;

    case ACTION_PACK_WEIGHT:
      Printf("The stuff you are carrying presently weighs %d pound%s.",
              (long) packweight(),
              plural(packweight()));
      nomove=1;
      yrepcount=0;
      return;

    case ACTION_VERSION:
      yrepcount=0;
      Printf("The Addiction of Ularn -- Version %s.%s (%s)\nDifficulty level %d",
              LARN_VERSION,
              LARN_PATCHLEVEL,
              LARN_DATE,
              (long)c[HARDGAME]);
      if (wizard) Print(" (WIZARD)");
      nomove=1;
      if (cheat) Print(" (Cheater)");
      Print(copyright);
      return;

    case ACTION_REDRAW_SCREEN:
      yrepcount=0;
      drawscreen();
      nomove=1;
      return;

    case ACTION_SHOW_TAX:
      if (outstanding_taxes>0)
      {
        Printf("You presently owe %d gp in taxes.", (long)outstanding_taxes);
      }
      else
      {
        Print("You do not owe any taxes.");
      }
      return;

    default:
      Print("HELP! unknown command");
      break;
  }
}

void action_teleport(int t)
{
    if (t > VBOTTOM || t < 0)
    {
        Print(" sorry!");
        return;
    }

    playerx = (char) rnd(MAXX-2);
    playery = (char) rnd(MAXY-2);
    newcavelevel(t);
    positionplayer();
    draws(0, MAXX, 0, MAXY);
    UpdateStatusAndEffects();
}

void move_world()
{
  if (nomove || get_callback()[0] != 0)
    return;
    
  /* regenerate hp and spells */
  regen();

  if (c[TIMESTOP]==0)
  {
    rmst--;
    if (rmst <= 0)
    {
      rmst = (char) (120-(level<<2));
      fillmonst(makemonst(level));
    }
  }

  if (dropflag==0)
  {
    lookforobject(); /* see if there is an object here*/
  }
  else
  {
    dropflag=0;      /* don't show it just dropped an item */
  }

  if (hitflag == 0)
  {
    if (c[HASTEMONST]) movemonst();

     movemonst();
  }

  if (viewflag==0)
  {
    showcell(playerx, playery);
  }
  else
  {
    viewflag = 0; /* show stuff around player */
  }

  hitflag = 0;
  hit3flag = 0;
}

int act(ActionType Action)
{
  nomove = 0;
  parse(Action);
  move_world();
  return !nomove;
}
