/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: object.c
 *
 * DESCRIPTION:
 * This module contains function for handling what a player finds when moving
 * onto a new square in the dungeon.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * lookforobject : Performs processing for the item found at the player's
 *                 current location.
 *
 * =============================================================================
 */

#include "ularn_game.h"
#include "ularn_win.h"
#include "header.h"
#include "object.h"
#include "player.h"
#include "monster.h"
#include "itm.h"
#include "potion.h"
#include "scroll.h"
#include "spell.h"
#include "dungeon.h"
#include "dungeon_obj.h"
#include "store.h"
#include "fortune.h"
#include "scores.h"

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: opotion
 *
 * DESCRIPTION:
 * Function to handle finding a potion.
 *
 * PARAMETERS:
 *
 *   pot : The potion type found.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void opotion(int pot, int ans)
{
  switch(ans)
  {
    case ESC:
    case 'i':
      break;

    case 'd':
      forget(); /*  destroy potion  */
      quaffpotion(pot);
      break;

    case 't':
      if (take(OPOTION,pot)==0)  forget();
      break;

    default:
      break;
  }
}

/* =============================================================================
 * FUNCTION: oscroll
 *
 * DESCRIPTION:
 * Function to process finding a mogic scroll.
 *
 * PARAMETERS:
 *
 *   typ : The type of scroll found.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void oscroll(int typ, int ans)
{
  switch(ans)
  {
    case ESC:
    case 'i':
      break;

    case 'r':
      if (c[BLINDCOUNT]) break;
      /* remove the scroll */
      forget();
      /* read the scroll */
      read_scroll(typ);
      break;

    case 't':
      if (take(OSCROLL,typ)==0)
        forget(); /*  destroy it  */
      break;

    default:
      break;
  }
}

/* =============================================================================
 * FUNCTION: obook
 *
 * DESCRIPTION:
 * Function to process finding a book.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void obook(int ans)
{
  switch (ans)
  {
    case ESC:
    case 'i':
      return;

    case 'r':
      if (c[BLINDCOUNT]) break;
      readbook(iarg[playerx][playery]);
      /* no more book */
      forget();
      return;

    case 't':
      if (take(OBOOK,iarg[playerx][playery])==0)
        forget(); /* no more book */
      return;

    default:
      break;
  }
}

/* =============================================================================
 * FUNCTION: ocookie
 *
 * DESCRIPTION:
 * Function to process finding a cookie.
 *
 * PARAMETERS:
 *
 *   None
 *
 * RETURN VALUE:
 *
 *   None
 */
void ocookie(int ans)
{
  char *p;

  switch(ans)
  {
    case ESC:
    case 'i':
      return;

    case 'e':
      Print("The cookie tasted good.");
      forget(); /* no more cookie */
      if (c[BLINDCOUNT]) return;

      p = fortune(fortfile);
      if (p == (char *)NULL) return;

      Print("A message inside the cookie reads:");
      Print(p);
      return;

    case 't':
      if (take(OCOOKIE,0)==0) forget();
      return;

    default:
      break;
  }
}

/* =============================================================================
 * FUNCTION: ogold
 *
 * DESCRIPTION:
 * Function to pick up some gold.
 *
 * PARAMETERS:
 *
 *   arg : The size of the gold pile
 *         if arg ==
 *           ODGOLD   then the pile is worth 10x  the argument
 *           OMAXGOLD then the pile is worth 100x the argument
 *           OKGOLD   then the pile is worth 1000x the argument
 *
 * RETURN VALUE:
 *
 *   None.
 */
void ogold(int arg)
{
  long i;

  i = iarg[playerx][playery];

  if (arg == ODGOLD)
  {
    i *= 10;
  }
  else if (arg == OMAXGOLD)
  {
    i *= 100;
  }
  else if (arg == OKGOLD)
  {
    i *= 1000;
  }

  Printf("You find %d gold piece%s.", i, plural(i));
  c[GOLD] += i;
  /* destroy gold */
  item[playerx][playery] = ONOTHING;
}

/* =============================================================================
 * FUNCTION: ochest
 *
 * DESCRIPTION:
 * Function to handle finding a chest.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void ochest(int ans)
{
  switch (ans)
  {
    case 'o':
      oopenchest();
      break;

    case 't':
      if (take(OCHEST,iarg[playerx][playery])==0)
      {
        item[playerx][playery] = ONOTHING;
      }
      break;

    case 'i':
    case ESC:
      break;

    default:
      break;
  }

}

/* =============================================================================
 * FUNCTION: finditem
 *
 * DESCRIPTION:
 * Function to say what object we found and ask if player wants to take it
 *
 * PARAMETERS:
 *
 *   itm : The item found
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void finditem(int itm)
{
  int tmp;

  Printf("You find %s", objectname[itm]);

  tmp = iarg[playerx][playery];
  switch (itm)
  {
    case ODIAMOND:
    case ORUBY:
    case OEMERALD:
    case OSAPPHIRE:
    case OSPIRITSCARAB:
    case OORBOFDRAGON:
    case OORB:
    case OHANDofFEAR:
    case OWWAND:
    case OCUBEofUNDEAD:
    case ONOTHEFT:
      Print(".");
      break;

    default:
      if (tmp>0)
      {
        Printf(" + %d",(long)tmp);
      }
      else if (tmp<0)
      {
        Printf(" %d",(long)tmp);
      }
  }

  add_option('t', "Take", "");
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: oopenchest
 */
void oopenchest(void)
{
  int i;
  int k;

  if (item[playerx][playery] != OCHEST)
  {
    return;
  }

  k = rnd(101);
  if (k < 40)
  {
    Print("The chest explodes as you open it.");
    UlarnBeep();
    i = rnd(10);
    if (i > c[HP]) i = c[HP];

    Printf("You suffer %d hit point%s damage!", (long)i, plural(i));
    losehp(DIED_EXPLODING_CHEST, i);
    switch (rnd(10))
    {
      case 1:
        c[ITCHING]+= rnd(1000)+100;
        Print("You feel an irritation spread over your skin!");
        UlarnBeep();
        break;

      case 2:
        c[CLUMSINESS]+= rnd(1600)+200;
        Print("You begin to lose hand-eye co-ordination!");
        UlarnBeep();
        break;

      case 3:
        c[HALFDAM]+= rnd(1600)+200;
        Print("You suddenly feel sick and BARF all over your shoes!");
        UlarnBeep();
        break;
    }

    /* Remove the chest */
    item[playerx][playery] = ONOTHING;

    /* create the items in the chest */
    if (rnd(100)<69)
    {
      /* gems from the chest */
      creategem();
    }

    dropgold(rnd(110 * iarg[playerx][playery] + 200));

    for (i=0; i<rnd(4); i++)
    {
      something(playerx, playery, iarg[playerx][playery]+2);
    }
  }
  else
  {
    Print("Nothing happens.");
  }
}

/* =============================================================================
 * FUNCTION: lookforobject
 */
void lookforobject(void)
{
  int i;
  int dx, dy;

  /* can't find objects is time is stopped */
  if (c[TIMESTOP]) return;

  i=item[playerx][playery];
  if (i == ONOTHING) return;

  showcell(playerx,playery);
  switch (i)
  {
    case OGOLDPILE:
    case OMAXGOLD:
    case OKGOLD:
    case ODGOLD:
      ogold(i);
      break;

    case OPOTION:
      Print("You find a magic potion");
      i = iarg[playerx][playery];
      if (potionknown[i]) Printf(" of %s",&potionname[i][1]);
      Print(".");
	  add_option('d', "Drink", "");
	  add_option('t', "Take", "");
      break;

    case OSCROLL:
      Print("You find a magic scroll");
      i = iarg[playerx][playery];
      if (scrollknown[i]) Printf(" of %s",&scrollname[i][1]);
      Print(".");
	  if (c[BLINDCOUNT] == 0)
	    add_option('r', "Read", "");
	  add_option('t', "Take", "");
      break;

    case OALTAR:
      if (nearbymonst()) return;
      Print("There is a holy altar here.");
	  set_callback("act_on_object");
	  add_option('p', "Pray", "");
	  add_option('d', "Desecrate", "");
	  add_option('i', "Ignore", "");
      break;

    case OBOOK:
      Print("You find a book.");
	  if (c[BLINDCOUNT] == 0)
	    add_option('r', "Read", "");
	  add_option('t', "Take", "");
      break;

    case OCOOKIE:
      Print("You find a fortune cookie.");
	  add_option('e', "Eat", "");
	  add_option('t', "Take", "");
      break;

    case OTHRONE:
    case OTHRONE2:
      if (nearbymonst()) return;
      Printf("There is %s here.", objectname[i]);
	  add_option('p', "Pry Jewels", "");
	  add_option('s', "Sit", "");
      break;

    case ODEADTHRONE:
      Printf("There is %s here.", objectname[i]);
	  add_option('s', "Sit", "");
      break;

    case OORB:
      if (nearbymonst()) return;
      finditem(i);
      break;

    case OBRASSLAMP:
      Print("You find a brass lamp.");
	  add_option('r', "Rub", "");
	  add_option('t', "Take", "");
      return;

    case OWWAND:
      if (nearbymonst()) return;
      finditem(i);
      break;

    case OHANDofFEAR:
      if (nearbymonst()) return;
      finditem(i);
      break;

    case OPIT:
      Print("You're standing at the top of a pit.");
      opit();
      break;

    case OSTAIRSUP:
      Print("There is a circular staircase here.");
	  add_option('u', "Up", "");
      break;

    case OELEVATORUP:
      Print("You have found an express elevator going up.");
      oelevator(1);  /*  up  */
      break;

    case OELEVATORDOWN:
      Print("You have found an express elevator going down.");
      oelevator(-1);  /*  down  */
      break;

    case OFOUNTAIN:
      if (nearbymonst()) return;
      Print("There is a fountain here.");
	  add_option('d', "Drink", "");
	  add_option('w', "Wash Yourself", "");
      break;

    case OSTATUE:
      if (nearbymonst()) return;
      Print("You stand before a statue.");
      ostatue();
      break;

    case OCHEST:
      Print("There is a chest here.");
	  add_option('t', "Take", "");
	  add_option('o', "Open", "");
      break;

    case OIVTELETRAP:
      if (rnd(11)<6) return;
      item[playerx][playery] = OTELEPORTER;

    case OTELEPORTER:
      /*
       * The player is being teleported, so obviously the player gets
       * to know that a teleport trap is here.
       * oteleport forces a screen redraw, so don't bother display anything
       * here.
       */
      know[playerx][playery] = item[playerx][playery];
      Print("Zaaaappp!  You've been teleported!");
      UlarnBeep();
      nap(3000);
      oteleport(0);
      break;

    case OSCHOOL:
      if (nearbymonst()) return;
      Print("You have found the College of Ularn.");
	  add_option('g', "Go in", "");
      break;

    case OMIRROR:
      if (nearbymonst()) return;
      Print("There is a mirror here.");
      omirror();
      break;

    case OBANK2:
    case OBANK:
      if (nearbymonst()) return;
      if (i==OBANK)
      {
        Print("You have found the bank of Ularn.");
      }
      else
      {
        Print("You have found a branch office of the bank of Ularn.");
      }
	  add_option('g', "Go in", "");
      break;

    case ODEADFOUNTAIN:
      if (nearbymonst()) return;
      Print("There is a dead fountain here.");
      break;

    case ODNDSTORE:
      if (nearbymonst()) return;
      Print("There is a DND store here.");
	  add_option('g', "Go in", "");
      break;

    case OSTAIRSDOWN:
      Print("There is a circular staircase here.");
	  add_option('d', "Down", "");
      break;

    case OOPENDOOR:
      Print("There is an open door here.");
      break;

    case OCLOSEDDOOR:
      
      /* can't move objects if time is stopped */
      if (c[TIMESTOP]) return;

      dx = playerx;
      dy = playery;

      if (dropflag)
        return;
      Printf("You find %s",objectname[i]);
	  set_callback("act_on_object");
	  add_option('o', "Open", "");
	  add_option('i', "Ignore", "");
      break;

    case OENTRANCE:
      Print("You have found ");
      Print(objectname[OENTRANCE]);
	  add_option('g', "Go in", "");
      break;

    case OVOLDOWN:
      Print("You have found ");
      Print(objectname[OVOLDOWN]);
	  add_option('c', "Climb down", "");
      return;

    case OVOLUP:
      Print("You have found ");
      Print(objectname[OVOLUP]);
	  add_option('c', "Climb up", "");
      return;

    case OTRAPARROWIV:
      if (rnd(17)<13) return; /* for an arrow trap */
      item[playerx][playery] = OTRAPARROW;
    case OTRAPARROW:
      Print("You are hit by an arrow!");
      UlarnBeep(); /* for an arrow trap */

      losehp(DIED_SHOT_BY_ARROW, rnd(10)+level);
      return;

    case OIVDARTRAP:
      if (rnd(17)<13) return;   /* for a dart trap */
      item[playerx][playery] = ODARTRAP;
    case ODARTRAP:
      Print("You are hit by a dart!");
      UlarnBeep(); /* for a dart trap */
      
      losehp(DIED_HIT_BY_DART, rnd(5));
      if ((--c[STRENGTH]) < 3) c[STRENGTH] = 3;
      return;

    case OIVTRAPDOOR:
      if (rnd(17)<13) return;   /* for a trap door */
      item[playerx][playery] = OTRAPDOOR;
    case OTRAPDOOR:
      for (i=0;i<IVENSIZE;i++)
      {
        if (iven[i]==OWWAND)
        {
          Print("You escape a trap door.");
          return;
        }
      }
      if ((level==DBOTTOM)||(level==VBOTTOM))
      {
        Print("You fall through a trap door leading straight to HELL!");
        UlarnBeep();
        nap(3000);
        died(DIED_FELL_THROUGH_BOTTOMLESS_TRAPDOOR, 0);
      }
      Print("You fall through a trap door!");
      UlarnBeep();
      losehp(DIED_FELL_THROUGH_TRAPDOOR, rnd(5+level));
      nap(2000);
      newcavelevel(level+1);
      return;

    case OTRADEPOST:
      if (nearbymonst()) return;
      Print("You have found the Ularn trading Post.");
	  add_option('g', "Go in", "");
      return;

    case OHOME:
      if (nearbymonst()) return;
      Print("You have found your way home.");
	  add_option('g', "Go in", "");
      return;

    case OPAD:
      if (nearbymonst()) return;
      Print("You have found Dealer McDope's Hideout!");
	  add_option('g', "Go in", "");
      return;

    case OSPEED:
      Print("You find some speed.");
	  add_option('s', "Snort", "");
	  add_option('t', "Take", "");
      break;

    case OSHROOMS:
      Print("You find some magic mushrooms.");
	  add_option('e', "Eat", "");
	  add_option('t', "Take", "");
      break;

    case OACID:
      Print("You find some LSD.");
	  add_option('e', "Eat", "");
	  add_option('t', "Take", "");
      break;

    case OHASH:
      Print("You find some hashish.");
	  add_option('s', "Smoke", "");
	  add_option('t', "Take", "");
      break;

    case OCOKE:
      Print("You find some cocaine.");
	  add_option('s', "Snort", "");
	  add_option('t', "Take", "");
      break;

    case OWALL:
      break;

    case OANNIHILATION:
      if (player_has_item(OSPHTALISMAN))
      {
        Print("The Talisman of the Sphere protects you from annihilation!");
        return;
      }
      
      /* annihilated by sphere of annihilation */
      died(DIED_SPHERE_ANNIHILATION, 0);
      return;

    case OLRS:
      if (nearbymonst()) return;
      Print("There is an LRS office here.");
	  add_option('g', "Go in", "");
      break;

    default:
	  finditem(i);
      break;
  }
}

/* =============================================================================
 * FUNCTION: act_on_object
 */
void take_item(int ans)
{
	if (ans != 't')
		return;

	if (take(item[playerx][playery], iarg[playerx][playery]) == 0)
		forget();
}

extern void move_world(int check_for_object);

void act_on_object(int ans)
{
  int i, j;
  int dx, dy;

  /* can't find objects is time is stopped */
  if (c[TIMESTOP]) return;

  i=item[playerx][playery];
  if (i == ONOTHING) return;

  showcell(playerx,playery);
  switch (i)
  {
    case OPOTION:
      i = iarg[playerx][playery];
      opotion(i, ans);
      break;

    case OSCROLL:
      i = iarg[playerx][playery];
      oscroll(i, ans);
      break;

    case OALTAR:
      oaltar(ans);
      move_world(0);
      break;

    case OBOOK:
      obook(ans);
      break;

    case OCOOKIE:
      ocookie(ans);
      break;

    case OTHRONE:
      othrone(0, ans);
      break;

    case OTHRONE2:
      othrone(1, ans);
      break;

    case ODEADTHRONE:
      odeadthrone(ans);
      break;

    case OORB:
      take_item(ans);
      break;

    case OBRASSLAMP:
      if (ans=='r')
      {
        i=rnd(100);
        if (i>90)
        {
          Print("The magic genie was very upset at being disturbed!");
      
          losehp(DIED_ANNOYED_GENIE, (int)c[HP]/2+1);
          UlarnBeep();
        }

        /* higher level, better chance of spell */
        else if ( (rnd(100)+c[LEVEL]/2) > 80)
        {
		  j = 0;
		  for (i = 0; i < SPELL_COUNT; i++)
		  {
			if (!spelknow[i])
			{
			  add_option(i, spelname[i], "");
			  j++;
			}
		  }
		  if (j > 0)
		  {
			  print_header("A magic genie appears!  What spell would you like?");
			  set_callback("obrasslamp");
			  break;
		  }
		  else
			  Print("A magic genie appears!  The genie has no new spells to teach you.");
        }
        else
        {
          Print("Nothing happened.");
        }

        if (rnd(100) < 15)
        {
          Print("The genie prefers not to be disturbed again!");
          forget();
          c[LAMP]=0;  /* chance of finding lamp again */
        }
      }
      else if (ans=='t')
      {
        if (take(OBRASSLAMP,0)==0)
          forget();
      }
      return;

    case OWWAND:
      take_item(ans);
      break;

    case OHANDofFEAR:
      take_item(ans);
      break;

    case OSTAIRSUP:
      ostairs(1, ans);  /* up */
      break;

    case OFOUNTAIN:
      ofountain(ans);
      break;

    case OCHEST:
      ochest(ans);
      break;

    case OSCHOOL:
      if (ans == 'g')
      {
		  oschool();
      }
      break;

    case OBANK2:
    case OBANK:
      if (ans == 'g')
      {
        if (i==OBANK)
			obank();
        else
			obank2();
      }
      break;

    case ODNDSTORE:
      if (ans == 'g')
		  dndstore();
      break;

    case OSTAIRSDOWN:
      ostairs(-1, ans); /* down */
      break;

    case OCLOSEDDOOR:
      
      /* can't move objects if time is stopped */
      if (c[TIMESTOP]) return;

      dx = playerx;
      dy = playery;

      if (dropflag)
        return;

      if ((ans==ESC) || (ans=='i'))
      {
        playerx = lastpx;
        playery = lastpy;
        lastpx = (char) dx;
        lastpy = (char) dy;
        break;
      }
      else
      {
        /* Try and open the door that is here */
        oopendoor(playerx, playery);

        if (item[playerx][playery] == OCLOSEDDOOR)
        {
          /*
           * Door didn't open.
           * Move the player back where they came from.
           */

          playerx = lastpx;
          playery = lastpy;
          lastpx = (char) dx;
          lastpy = (char) dy;
        }
        
        move_world(0);
      }
      break;

    case OENTRANCE:
      if (ans == 'g')
      {
        newcavelevel(1);
        playerx = 33;
        playery = MAXY - 2;

        /* Make sure the entrance to the dungeon is clear */
        item[33][MAXY-1] = ONOTHING;
        mitem[33][MAXY-1].mon = MONST_NONE;
        return;
      }
      break;

    case OVOLDOWN:
      if ((ans==ESC) || (ans=='i'))
      {
        break;
      }
      if (level!=0)
      {
        Print("The shaft only extends 5 feet downward!");
        return;
      }
      if (packweight() > 45+3*(c[STRENGTH]+c[STREXTRA]))
      {
        Print("You slip and fall down the shaft.");
        UlarnBeep();
        
        losehp(DIED_SLIPPED_VOLCANO_SHAFT, 30+rnd(20));
      }
      nap(3000);
      newcavelevel(DBOTTOM+1); /* down to V1 */
      playerx = (char) rnd(MAXX-2);
      playery = (char) rnd(MAXY-2);
      positionplayer();
      return;

    case OVOLUP:
      if ((ans==ESC) || (ans=='i'))
      {
        break;
      }
      if (packweight() > 40+5*(c[DEXTERITY]+c[STRENGTH]+c[STREXTRA]))
      {
        Print("You slip and fall down the shaft.");
        UlarnBeep();
        
        losehp(DIED_SLIPPED_VOLCANO_SHAFT, 15+rnd(20));
        return;
      }

      nap(3000);
      newcavelevel(0);
      for (i=0; i<MAXY; i++)  for (j=0; j<MAXX; j++)
      {
        /* put player near volcano shaft */
        if (item[j][i]==OVOLDOWN)
        {
          playerx = (char) j;
          playery = (char) i;
          j=MAXX;
          i=MAXY;
          positionplayer();
        }
      }
      return;

    case OTRADEPOST:
      if (ans == 'g')
      {
		  otradepost();
      }
      return;

    case OHOME:
      if (ans == 'g')
      {
        ohome();
      }
      return;

    case OPAD:
      if (ans == 'c')
      {
		  opad();
      }
      return;

    case OSPEED:
      if (ans=='s')
      {
        Print("Ohwowmanlikethingstotallyseemtoslowdown!");
        c[HASTESELF] += 200 + c[LEVEL];
        c[HALFDAM] += 300 + rnd(200);
        adjust_ability(INTELLIGENCE, -2);
        adjust_ability(WISDOM, -2);
        adjust_ability(CONSTITUTION, -2);
        adjust_ability(DEXTERITY, -2);
        adjust_ability(STRENGTH, -2);
        forget();
      }
      else if (ans=='t')
      {
        if (take(OSPEED,0)==0) forget();
      }
      break;

    case OSHROOMS:
      if (ans=='e')
      {
        Print("Things start to get real spacey...");
        c[HASTEMONST] += rnd(75) + 25;
        c[CONFUSE] += 30+rnd(10);
        adjust_ability(WISDOM, 2);
        adjust_ability(CHARISMA, 2);
        forget();
      }
      else if (ans=='t')
      {
        if (take(OSHROOMS,0)==0) forget();
      }
      break;

    case OACID:
      if (ans=='e')
      {
        Print("You are now frying your ass off!");
        c[CONFUSE] += 30 + rnd(10);
        adjust_ability(WISDOM, 2);
        adjust_ability(INTELLIGENCE, 2);
        c[AWARENESS] += 1500;
        c[AGGRAVATE] += 1500;
        {
          int j, k; /* heal monsters */
          for(j = 0 ; j < MAXY ; j++)
          {
            for(k = 0 ; k < MAXX ; k++)
            {
              if (mitem[k][j].mon)
              {
                hitp[k][j] = monster[(int) mitem[k][j].mon].hitpoints;
              }
            }
          }
        }
        forget();
      }
      else if (ans=='t')
      {
        if (take(OACID,0)==0) forget();
      }
      break;

    case OHASH:
      if (ans=='s')
      {
        Print("WOW! You feel stooooooned...");
        c[HASTEMONST] += rnd(75)+25;
        adjust_ability(INTELLIGENCE, 2);
        adjust_ability(WISDOM, 2);
        adjust_ability(CONSTITUTION, -2);
        adjust_ability(DEXTERITY, -2);
        c[HALFDAM] += 300+rnd(200);
        c[CLUMSINESS] += rnd(1800)+200;
        forget();
      }
      else if (ans=='t')
      {
        if (take(OHASH,0)==0) forget();
      }
      break;

    case OCOKE:
      if (ans=='s')
      {
        Print("Your nose begins to bleed!");
        adjust_ability(DEXTERITY, -2);
        adjust_ability(CONSTITUTION, -2);
        adjust_ability(CHARISMA, 3);

        for(i = ABILITY_FIRST ; i <= ABILITY_LAST ; i++)
        {
          adjust_ability(i, 33);
        }
        c[COKED] += 10;
        forget();
      }
      else if (ans=='t')
      {
        if (take(OCOKE, 0) == 0) forget();
      }
      break;

    case OWALL:
      break;

    case OANNIHILATION:
      if (player_has_item(OSPHTALISMAN))
      {
        Print("The Talisman of the Sphere protects you from annihilation!");
        return;
      }
      
      /* annihilated by sphere of annihilation */
      died(DIED_SPHERE_ANNIHILATION, 0);
      return;

    case OLRS:
      if (nearbymonst()) return;

      if (ans == 'g')
      {
		  olrs();
      }
      break;

    default:
	  if (ans == 't')
	  {
		  if (take(i, iarg[playerx][playery]) == 0)
			  forget();
	  }
      break;
  }
}

void obrasslamp(int i)
{
	spelknow[i]++;
	Printf("Spell:  %s -- %s<br/>", spelname[i], speldescript[i]);
	Print("The genie prefers not to be disturbed again.");
	forget();

	recalc();
}

