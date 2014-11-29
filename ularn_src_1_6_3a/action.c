/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: action.c
 *
 * DESCRIPTION:
 * This module contains functions to process the player's actions.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * run       : Move in a direction until something interesting happens
 * wield     : Wield an item
 * wear      : Wear armour or shield
 * dropobj   : Drop an object
 * readsrc   : Read a scroll ot a book for the inventory
 * eatcookie : Eat a cookie in inventory, and display fortune if possible.
 * quaff     : Drink a potion in inventory
 * closedoor : Close an open door
 * quit      : Asks if really want to quit.
 * do_create : Create an item (wizard only)
 *
 * =============================================================================
 */

#include "ularn_game.h"
#include "ularn_win.h"
#include "ularn_ask.h"
#include "header.h"
#include "savegame.h"
#include "itm.h"
#include "player.h"
#include "monster.h"
#include "dungeon.h"
#include "dungeon_obj.h"
#include "object.h"
#include "potion.h"
#include "scroll.h"
#include "show.h"
#include "fortune.h"

/* =============================================================================
 * Local variables
 */

/*
 * Types of uses of an item
 */
typedef enum UseType
{
  USE_WIELD,
  USE_QUAFF,
  USE_READ,
  USE_WEAR,
  USE_EAT,
  USE_DROP,
  USE_COUNT
} UseType;


/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: ydhi
 *
 * DESCRIPTION:
 * Function to print the 'you don't have item' message.
 *
 * PARAMETERS:
 *
 *   x : The character index (a .. z) of the inventory slot.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void ydhi (int x)
{
  Printf("You don't have item %c!", x);
}

/* =============================================================================
 * FUNCTION: ycwi
 *
 * DESCRIPTION:
 * Function to print the 'you can't wield item' message.
 *
 * PARAMETERS:
 *
 *   x : The character index (a .. z) of the inventory slot.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void ycwi (int x)
{
  Printf("You can't wield item %c!", x);
}



/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: wield
 */
void wield (int i)
{
  int it;

  while (1)
  {
    if (i == ESC)
    {
      return;
    }
    else if (i == '*')
    {
      showwield();
    }
    else if (i == '-')
    {
      c[WIELD] = -1;
      Print("You unwield your weapon.");
      recalc();
      return;
    }
    else if ((i >= 'a') && (i <= 'z'))
    {
      it = i - 'a';
      if (iven[it] == ONOTHING)
      {
        ydhi(i);
        return;
      }
      else if (iven[it]==OPOTION)
      {
        ycwi(i);
        return;
      }
      else if (iven[it]==OSCROLL)
      {
        ycwi(i);
        return;
      }
      else if ((c[SHIELD] != -1) && (iven[it] == O2SWORD))
      {
        Print("But one arm is busy with your shield!");
        return;
      }
      else if ((c[WEAR] == it) || (c[SHIELD] == it))
      {
        Printf("You can't wield your %s while you're wearing it!",
            (c[WEAR] == it) ? "armor" : "shield");
        return;
      }
      else
      {
        c[WIELD] = it;
        Printf("You wield %s", objectname[(int) iven[it]]);
        show_plusses(ivenarg[it]);
        Printc('.');

        recalc();
        return;
      }
    }

  }
}

/* =============================================================================
 * FUNCTION: wear
 */
void wear (int i)
{
  int it;

  while (1)
  {
    if (i == ESC)
    {
      return;
    }
    else if (i == '*')
    {
      showwear();
    }
    else if ((i >= 'a') && (i <= 'z'))
    {
      it = i - 'a';

      switch (iven[it])
      {
        case ONOTHING:
          ydhi(i);
          return;
        case OLEATHER:
        case OCHAIN:
        case OPLATE:
        case OSTUDLEATHER:
        case ORING:
        case OSPLINT:
        case OPLATEARMOR:
        case OELVENCHAIN:
        case OSSPLATE:
          c[WEAR] = it;

          if (c[WIELD] == it) c[WIELD] = -1;

          Printf("You put on your %s", objectname[(int) iven[it]]);
          show_plusses(ivenarg[it]);
          Printc('.');

          recalc();
          return;

        case OSHIELD:
          if (iven[c[WIELD]] == O2SWORD)
          {
            Print("Your hands are busy with the two handed sword!");
            return;
          }
          c[SHIELD] = it;
          if (c[WIELD] == it) c[WIELD] = -1;

          Print("You put on your shield");

          show_plusses(ivenarg[it]);
          Printc('.');

          recalc();
          return;

        default:
          Print("You can't wear that!");
      }
    }
  }

}

/* =============================================================================
 * FUNCTION: dropobj
 */
void dropobj (int i)
{
  int pitflag=0;
  char *p;

  p = &item[playerx][playery];

    if (i=='*')
    {
      showstr();
    }
    else
    {
      if (c[TIMESTOP])
      {
        Printf("Nothing can be moved while time is stopped!");
        return;
      }
      /* drop some gold */
      if (i == '.')
      {
        if (*p == OPIT) pitflag=1;
        if ((*p != ONOTHING) && !pitflag)
        {
          Print("There's something here already!");
          return;
        }

        print_header("How much gold do you drop? ");
		set_input_type("number");
		set_callback("drop_gold");
      }
      else
      {
        drop_object(i-'a');
      }
    }
}

void drop_gold(long amt)
{
	int i;
	int pitflag=0;
	char *p;

	p = &item[playerx][playery];

    if (amt <= 0) return;

    if (amt > c[GOLD])
    {
        Print("You don't have that much!");
        return;
    }

    if (amt <= 32767)
    {
        *p=OGOLDPILE;
        i = (int) amt;
    }
    else if (amt <= 327670L)
    {
        *p = ODGOLD;
        i = (int) (amt/10);
        amt = 10L*i;
    }
    else if (amt<=3276700L)
    {
        *p=OMAXGOLD;
        i = (int) (amt/100);
        amt = 100L*i;
    }
    else if (amt<=32767000L)
    {
        *p = OKGOLD;
        i = (int) (amt/1000);
        amt = 1000L*i;
    }
    else
    {
        *p = OKGOLD;
        i = (int) (32767);
        amt = 32767000L;
    }
    c[GOLD] -= amt;
    Printf(" You drop %d gold piece%s.", (long) amt, plural(amt));

    if (pitflag)
    {
        *p = OPIT;
        Print("The gold disappears down the pit.");
    }
    else
    {
        iarg[playerx][playery] = (short) i;
    }
    dropflag = 1;
}

/* =============================================================================
 * FUNCTION: readscr
 */
void readscr (int i)
{
  while (1)
  {
    if (i==ESC)  return;
    if (i != '.')
    {
      if (i=='*')
      {
        showread();
      }
      else
      {
        if (c[BLINDCOUNT])
        {
          Print("You can't read anything when you're blind!");
          return;
        }
        if (c[TIMESTOP])
        {
          Printf("Nothing can be moved while time is stopped!");
          return;
        }
        if (iven[i-'a']==OSCROLL)
        {
          read_scroll(ivenarg[i-'a']);
          iven[i-'a'] = ONOTHING;
          return;
        }
        if (iven[i-'a']==OBOOK)
        {
          readbook(ivenarg[i-'a']);
          iven[i-'a'] = ONOTHING;
          return;
        }
        if (iven[i-'a'] == ONOTHING)
        {
          ydhi(i);
          return;
        }
        Print("There's nothing on it to read.");  return;
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: eatcookie
 */
void eatcookie (int i)
{
  char *p;

  while (1)
  {
    if (i==ESC)
    {
      return;
    }

    if (i != '.')
    {
      if (i=='*')
      {
        showeat();
      }
      else if (iven[i-'a']==OCOOKIE)
      {
        if (c[TIMESTOP])
        {
          Printf("Nothing can be moved while time is stopped!");
          return;
        }
        Print("The cookie was delicious.");
        iven[i-'a'] = ONOTHING;
        if (!c[BLINDCOUNT])
        {
          p = fortune(fortfile);

          if (p != NULL)
          {
            Print("Inside you find a scrap of paper that says:");
            Print(p);
          }
        }
        return;
      }
      else if (iven[i-'a'] == ONOTHING)
      {
        ydhi(i);
        return;
      }
      else
      {
        Print("You can't eat that!");
        return;
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: quaff
 */
void quaff (int i)
{
  while (1)
  {
    if (i == ESC)
    {
      return;
    }

    if (i != '.')
    {
      if (i=='*')
      {
        showquaff();
      }
      else if (iven[i-'a']==OPOTION)
      {
        if (c[TIMESTOP])
        {
          Printf("Nothing can be moved while time is stopped!");
          return;
        }
        quaffpotion(ivenarg[i-'a']);
        iven[i-'a'] = ONOTHING;
        return;
      }
      else if (iven[i-'a'] == ONOTHING)
      {
        ydhi(i);
        return;
      }
      else
      {
        Print("You wouldn't want to quaff that, would you? ");
        return;
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: opendoor
 */
void opendoor(ActionType direction)
{
  int dx, dy;

  if (!enhance_interface)
  {
    return;
  }

  /* can't move objects is time is stopped */
  if (c[TIMESTOP])
  {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }

  dx = playerx;
  dy = playery;

  dirsub(&dx, &dy, direction);

  /* don't ask about items at player's location after trying to open a door */
  dropflag = 1;

  if (!checkxy(dx, dy))
  {
  	if (c[BLINDCOUNT] == 0)
    {
  	  Print("You see no door there.");
    }
    else
    {
      Print("You cannot feel any door there.");
    }
  }
  else
  {
    if (item[dx][dy] == OOPENDOOR)
    {
    	Print("That door is already open.");
    }
    else if (item[dx][dy] != OCLOSEDDOOR)
    {
    	if (c[BLINDCOUNT] == 0)
      {
  	    Print("You see no door there.");
      }
      else
      {
        Print("You cannot feel any door there.");
      }
    }
    else
    {
      oopendoor(dx, dy);
      if (item[dx][dy] == OOPENDOOR)
      {
      	Print("The door opens.");
      }
      else
      {
        Print("The door resists.");
      }
    }
  }

}

/* =============================================================================
 * FUNCTION: closedoor
 */
void closedoor(ActionType direction)
{
  int i;
  int dx, dy;

  /* can't move objects is time is stopped */
  if (c[TIMESTOP])
  {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }

  showcell(playerx, playery);

  if (enhance_interface)
  {
    dx = playerx;
    dy = playery;

    dirsub(&dx, &dy, direction);

    /* don't ask about items at player's location after trying to close a door */
    dropflag = 1;

    if (!checkxy(dx, dy))
    {
      if (c[BLINDCOUNT] == 0)
      {
  	  Print("You see no door there.");
      }
      else
      {
        Print("You cannot feel any door there.");
      }
    }
    else
    {
      if (item[dx][dy] == OCLOSEDDOOR)
      {
    	  Print("That door is already closed.");
      }
      else if (item[dx][dy] != OOPENDOOR)
      {
    	  if (c[BLINDCOUNT] == 0)
        {
          Print("You see no door there.");
        }
        else
        {
          Print("You cannot feel any door there.");
        }
      }
      else
      {
        item[dx][dy] = OCLOSEDDOOR;
        iarg[dx][dy] = 0;            /* Clear traps on door */
        Print("The door closes.");
      }
    }
    
  }
  else
  {
    i = item[playerx][playery];
    if (i != OOPENDOOR)
    {
      Print("There is no open door here.");
      return;
    }

    Print("The door closes.");
    forget();
    item[playerx][playery]=OCLOSEDDOOR;
    iarg[playerx][playery]=0;

    dropflag=1; /* So we won't be asked to open it */
  }
}

/* =============================================================================
 * FUNCTION: openchest
 */
void openchest(void)
{
  if (item[playerx][playery] != OCHEST)
  {
    Print("There isn't a chest to open here.");
  }
  else
  {
    oopenchest();
    dropflag = 1; /* so we don't get asked to open it again */
  }
}

/* =============================================================================
 * FUNCTION: quit
 */
void quit (int i)
{
  strcpy(lastmonst, "");
  switch (i)
  {
    case 'y':
      died(DIED_QUICK_QUIT, 0);
      break;

    case 'n':
    case ESC:
      Print(" no.");
      break;

    case 's':
      Print(" save.");
      Print("Saving...");
      savegame();
      wizard=1;
      died(DIED_SUSPENDED, 0);
      break;

    default:
      break;
  }
}

/* =============================================================================
 * FUNCTION: do_create
 */
void do_create(int t, int a, int b)
{
  int Retry;

  do
  {
    Retry = 0;

    switch(t)
    {
      case ESC:
        break;

      case 's':
      case 'S':
        Printf("createitem(OSCROLL, %d)", a);
        createitem(playerx,playery,OSCROLL, a);
        dropflag=1;
        break;

      case 'p':
      case 'P':
        Printf("createitem(OPOTION, %d)", a);
        createitem(playerx,playery,OPOTION, a);
        dropflag=1;
        break;

      case 'o':
      case 'O':
        Printf("createitem(%d, %d)", a, b);
        createitem(playerx,playery,a,b);
        dropflag=1;
        break;

      case 'm':
      case 'M':
        Printf("createmonster(%d)", a);
        createmonster(a);
        dropflag=1;
        break;

      default:
        Retry = 1;
        break;
    }
  } while (Retry);
}

void takeoff(int i)
{
	if (c[SHIELD] == i - 'a')
	{
		c[SHIELD] = -1;
		Print("Your shield is off.");
		recalc();
	}
	else if (c[WEAR] == i - 'a')
	{
		c[WEAR] = -1;
		Print("Your armor is off.");
		recalc();
	}
}

void unwield()
{
	c[WIELD] = -1;
	Print("You unwield your weapon.");
	recalc();
}
