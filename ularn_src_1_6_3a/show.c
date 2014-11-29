/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: show.c
 *
 * DESCRIPTION:
 * This module contains functions to print item names and show lists of items.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * show_plusses - Prints the number of plusses as indicated by the input arg
 * qshowstr     - Show the player's inventory without setting up text mode
 * showstr      - Show the player's invertory, setting/exiting text mode
 * showwear     - Show wearable items
 * showwield    - Show wieldable items
 * showread     - Show readable items
 * showeat      - Show edible items
 * showquaff    - Show quaffable items
 * seemagic     - Show magic spells/scrolls/potions discovered
 * show1        - Show an item name
 * show3        - Show an item name with the numebr of plusses
 *
 * =============================================================================
 */

#include "header.h"
#include "ularn_game.h"
#include "ularn_win.h"
#include "ularn_ask.h"
#include "dungeon.h"
#include "player.h"
#include "potion.h"
#include "scroll.h"
#include "spell.h"
#include "itm.h"
#include "show.h"

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: show_plusses
 */
void show_plusses(int plus)
{
  if ((plus > 0) || wizard)
  {
    Printf(" +%d", plus);
  }
  else if (plus < 0)
  {
    Printf(" %d", plus);
  }
}

/* =============================================================================
 * FUNCTION: qshowstr
 */
void qshowstr(void)
{
  int k;
  int mobuls_used;
  int mobuls_left;

  clearpager();
  if (c[GOLD])
  {
    Printf(".)   %d gold piece%s.",(long)c[GOLD], plural(c[GOLD]));
    pager();
  }

  for (k = 0 ; k < IVENSIZE ; k++)
  {
    if (iven[k] != ONOTHING)
    {
      show3(k);
      pager();
    }
  }

  mobuls_used = (gtime/100)+1;
  mobuls_left = (TIMELIMIT/100) - mobuls_used;
  Printf("Elapsed time is %d.  You have %d mobul%s left.",
    mobuls_used, mobuls_left, plural(mobuls_left));
}


/* =============================================================================
 * FUNCTION: showstr
 */
void showstr(void)
{
  int i, number;

  number = 3;

  /* count items in inventory */
  for (i=0; i<IVENSIZE; i++)
  {
    if (iven[i] != ONOTHING) number++;
  }

  qshowstr();
}


/* =============================================================================
 * FUNCTION: showwear
 */
void showwear(void)
{
  int count;
  int i;
  int j;
  clearpager();

  /* count number of items we will display */
  count = 2;
  for (j = 0; j < IVENSIZE ; j++)
  {
    i = iven[j];

    if (i != ONOTHING)
    {
      switch (i)
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
          count++;

        default:
          break;
      }
    }
  }

  /* Display the wearable items in item index order */
  for (i = 22 ; i < 93 ; i++)
  {
    for (j = 0 ; j < IVENSIZE ; j++)
    {
      if (iven[j] == i)
      {
        switch (i)
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
            show3(j);
            pager();
            break;

          default:
            break;
        }
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: showwield
 */
void showwield(void)
{
  int i;
  int j;
  int count;
  clearpager();

  /* count how many items */
  count = 2;
  for (j = 0; j < IVENSIZE ; j++)
  {
    i = iven[j];
    if (i != ONOTHING)
    {
      switch (i)
      {
        case ODIAMOND:
        case ORUBY:
        case OEMERALD:
        case OSAPPHIRE:
        case OBOOK:
        case OCHEST:
        case OLARNEYE:
        case ONOTHEFT:
        case OSPIRITSCARAB:
        case OCUBEofUNDEAD:
        case OPOTION:
        case OORB:
        case OHANDofFEAR:
        case OBRASSLAMP:
        case OURN:
        case OWWAND:
        case OSPHTALISMAN:
        case OSCROLL:
          break;

        default:
          count++;
          break;
      }
    }
  }

  /* display the wieldable items in item order */
  for (i = 22 ; i < 93 ; i++)
  {
    for (j = 0; j < IVENSIZE ; j++)
    {
      if (iven[j] == i)
      {
        switch (i)
        {
          case ODIAMOND:
          case ORUBY:
          case OEMERALD:
          case OSAPPHIRE:
          case OBOOK:
          case OCHEST:
          case OLARNEYE:
          case ONOTHEFT:
          case OSPIRITSCARAB:
          case OCUBEofUNDEAD:
          case OPOTION:
          case OHANDofFEAR:
          case OBRASSLAMP:
          case OURN:
          case OSPHTALISMAN:
          case OWWAND:
          case OORB:
          case OSCROLL:
            break;

          default:
            show3(j);
            pager();
            break;
        }
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: showread
 */
void showread (void)
{
  int i;
  int j;
  int count;
  clearpager();

  count = 2;
  for (j=0; j<IVENSIZE; j++)
  {
    switch(iven[j])
    {
      case OBOOK:
      case OSCROLL:
        count++;
        break;

      default:
        break;
    }
  }



  /* display the readable items in item order */
  for (i = 22; i < 84; i++)
  {
    for (j = 0; j < IVENSIZE ; j++)
    {

      if (iven[j] == i)
      {
        switch(i)
        {
          case OBOOK:
          case OSCROLL:
            show3(j);
            pager();
            break;

          default:
            break;
        }
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: showeat
 */
void showeat (void)
{
  int i;
  int j;
  int count;
  clearpager();

  count = 2;
  for (j = 0; j < IVENSIZE ; j++)
  {
    switch (iven[j])
    {
      case OCOOKIE:
        count++;
        break;

      default:
        break;

    }
  }


  /* show the edible items in item order */
  for (i = 22 ; i < 84 ; i++)
  {
    for (j = 0 ; j < IVENSIZE ; j++)
    {

      if (iven[j] == i)
      {
        switch (i)
        {
          case OCOOKIE:
            show3(j);
            pager();
            break;

          default:
            break;
        }
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: showquaff
 */
void showquaff (void)
{
  int i,j,count;
  clearpager();

  count = 2;
  for (j = 0 ; j < IVENSIZE ; j++)
  {
    switch (iven[j])
    {
      case OPOTION:
        count++;
        break;

      default:
        break;
    }
  }

  /* show the quaffable items in item order */
  for (i = 22 ; i < 84 ; i++)
  {
    for (j = 0 ; j < IVENSIZE ; j++)
    {
      if (iven[j] == i)
      {
        switch(i)
        {
          case OPOTION:
            show3(j);
            pager();
            break;

          default:
            break;
        }
      }
    }
  }
}

/* =============================================================================
 * FUNCTION: seemagic
 */
void seemagic(int arg)
{
  int i;
  int first;

  if (arg == 99)
  {
    print_header("Available spells are:");
    first = 1;
    for (i = 0 ; i < SPELL_COUNT ; i++)
    {
      if (first)
        first = 0;
      else
        print_header(", ");
      print_header(spelname[i]);
    }
    return;
  }

  print_header("The magic spells you have discovered to date are:");
  first = 1;
  for (i = 0 ; i < SPELL_COUNT ; i++)
  {
    if (spelknow[i])
    {
      if (first)
        first = 0;
      else
        print_header(", ");
      print_header(spelname[i]);
    }
  }
  if (first)
    print_header("none");

  if (arg == -1)
  {
    return;
  }

  print_header("<br/>The magic scrolls you have found to date are:");
  first = 1;
  for (i=0; i<MAXSCROLL; i++)
  {
    if (scrollknown[i] && scrollname[i][1] != ' ')
    {
      if (first)
        first = 0;
      else
        print_header(", ");
      print_header(&scrollname[i][1]);
    }
  }
  if (first)
    print_header("none");

  print_header("<br/>The magic potions you have found to date are:");
  first = 1;
  for (i=0; i<MAXPOTION; i++)
  {
    if (potionknown[i] && potionname[i][1] != ' ')
    {
      if (first)
        first = 0;
      else
        print_header(", ");
      print_header(&potionname[i][1]);
    }
  }
  if (first)
    print_header("none");
  
  set_callback("*");
  add_option(-1, "Back", "");
}

/* =============================================================================
 * FUNCTION: show1
 */
void show1 (int idx, char *str2[], int known[])
{
  /* standard */
  if (known == 0)
  {
    if (str2 == 0)
    {
      Printf(" %s", objectname[(int) iven[idx]]);
    }
    else if (*str2[ivenarg[idx]]==0)
    {
      Printf(" %s", objectname[(int) iven[idx]]);
    }
    else
    {
    Printf(" %s of%s", objectname[(int) iven[idx]], str2[ivenarg[idx]]);
    }
  }
  else
  {
    /* scroll or potion or something with a known array */
    if (str2 == 0)
    {
      Printf(" %s",objectname[(int) iven[idx]]);
    }
    else if (*str2[ivenarg[idx]]==0)
    {
      Printf(" %s",objectname[(int) iven[idx]]);
    }
    else if (known[ivenarg[idx]]==0)
    {
      Printf(" %s",objectname[(int) iven[idx]]);
    }
    else
    {
      Printf(" %s of%s", objectname[(int) iven[idx]], str2[ivenarg[idx]]);
    }
  }

  if (wizard)
  {
    Printf(" [ %d ]", ivenarg[idx]);
  }
}

/* =============================================================================
 * FUNCTION: show3
 */
void show3 (int index)
{
  switch (iven[index])
  {
    case OPOTION:
      show1(index, potionname, potionknown);
      break;

    case OSCROLL:
      show1(index, scrollname, scrollknown);
      break;

    case OLARNEYE:
    case OBOOK:
    case OSPIRITSCARAB:
    case ODIAMOND:
    case ORUBY:
    case OCUBEofUNDEAD:
    case OEMERALD:
    case OCHEST:
    case OCOOKIE:
    case OSAPPHIRE:
    case OORB:
    case OHANDofFEAR:
    case OBRASSLAMP:
    case OURN:
    case OWWAND:
    case OSPHTALISMAN:
    case ONOTHEFT:
      show1(index, (char **)0, (int *)0);
      break;

    default:
      Printf(" %s", objectname[(int) iven[index]]);
      show_plusses(ivenarg[index]);
      break;
  }

  if (c[WIELD]==index)
  {
    Print(" (weapon in hand)");
  }

  if ((c[WEAR]==index) || (c[SHIELD]==index))
  {
    Print(" (being worn)");
  }

}

char *get_inventory_name1(int idx, char *str2[], int known[])
{
	static char name[80];
	/* standard */
	if (known == 0)
	{
		if (str2 == 0)
		{
			strcpy(name, objectname[(int) iven[idx]]);
		}
		else if (*str2[ivenarg[idx]]==0)
		{
			strcpy(name, objectname[(int) iven[idx]]);
		}
		else
		{
			sprintf(name, "%s of%s", objectname[(int) iven[idx]], str2[ivenarg[idx]]);
		}
	}
	else
	{
		/* scroll or potion or something with a known array */
		if (str2 == 0)
		{
			strcpy(name, objectname[(int) iven[idx]]);
		}
		else if (*str2[ivenarg[idx]]==0)
		{
			strcpy(name, objectname[(int) iven[idx]]);
		}
		else if (known[ivenarg[idx]]==0)
		{
			strcpy(name, objectname[(int) iven[idx]]);
		}
		else
		{
			sprintf(name, "%s of%s", objectname[(int) iven[idx]], str2[ivenarg[idx]]);
		}
	}

	if (wizard)
	{
		sprintf(name + strlen(name), " [ %d ]", ivenarg[idx]);
	}

	return name;
}

char *get_plusses(int plus)
{
	static char plusses[10];
	if (plus > 0 || wizard)
		sprintf(plusses, " +%d", plus);
	else if (plus < 0)
		sprintf(plusses, " %d", plus);
	else
		plusses[0] = 0;
	return plusses;
}

char *get_inventory_name3(int index)
{
	static char name[80];
	switch (iven[index])
	{
		case OPOTION:
			strcpy(name, get_inventory_name1(index, potionname, potionknown));
			break;

		case OSCROLL:
			strcpy(name, get_inventory_name1(index, scrollname, scrollknown));
			break;

		case OLARNEYE:
		case OBOOK:
		case OSPIRITSCARAB:
		case ODIAMOND:
		case ORUBY:
		case OCUBEofUNDEAD:
		case OEMERALD:
		case OCHEST:
		case OCOOKIE:
		case OSAPPHIRE:
		case OORB:
		case OHANDofFEAR:
		case OBRASSLAMP:
		case OURN:
		case OWWAND:
		case OSPHTALISMAN:
		case ONOTHEFT:
			strcpy(name, get_inventory_name1(index, (char **)0, (int *)0));
			break;

		default:
			strcpy(name, objectname[(int) iven[index]]);
			strcat(name, get_plusses(ivenarg[index]));
			break;
	}

	if (c[WIELD]==index)
	{
		strcat(name, " (weapon in hand)");
	}

	if ((c[WEAR]==index) || (c[SHIELD]==index))
	{
		strcat(name, " (being worn)");
	}

	return name;
}

