/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: savegame.c
 *
 * DESCRIPTION:
 * This module contains functions for saving and loading the game.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * savegame    : Function to save the game
 * restoregame : Function to load the game
 *
 * =============================================================================
 */

#include <stdio.h>
#include <fcntl.h>

#include "header.h"
#include "savegame.h"
#include "saveutils.h"
#include "ularn_game.h"
#include "ularn_win.h"
#include "monster.h"
#include "player.h"
#include "spell.h"
#include "dungeon.h"
#include "sphere.h"
#include "store.h"
#include "scores.h"
#include "itm.h"

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: greedy
 *
 * DESCRIPTION:
 * Subroutine to not allow greedy cheaters.
 * Displays a cheater message and quits the game.
 *
 * PARAMETERS:
 *
 *   None
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void greedy(void)
{
  if (wizard)
    return;

  Print("I am so sorry but your character is a little TOO good!  Since this");
  Print("cannot normally happen from an honest game, I must assume that you cheated.");
  Print("Since you are GREEDY as well as a CHEATER, I cannot allow this game");
  Print("to continue.");
  nap(5000);
  cheat = 1;
  c[GOLD] = c[BANKACCOUNT] = 0;
  died(DIED_GREEDY_CHEATER, 0);
  return;
}

/* =============================================================================
 * FUNCTION: fsorry
 *
 * DESCRIPTION:
 * Subroutine to not allow altered save files and terminate the attempted
 * restart.
 * Sets the cheat flag to disallow scoring for this game.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void fsorry(void)
{
  if (cheat)
    return;

  Print("Sorry but your savefile has been altered.");
  Print("However, since I am a good sport, I will let you play.");
  Print("Be advised, though, that you won't be placed on the scoreboard.");
  cheat = 1;
  nap(4000);
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: savegame
 */
MemoryFile *savegame()
{
  MemoryFile *fp;
  /* Save the current level to storage */
  savelevel();

  /* make sure the interest on bank deposits is up to date */
  ointerest();

  /*
   * try and create the save file
   */
  fp = MemoryFile_create();

  FileSum = 0;

  write_player(fp);
  write_levels(fp);
  write_store(fp);
  write_monster_data(fp);
  write_spheres(fp);

  /* file sum */
  bwrite(fp, (char *) &FileSum, sizeof(FileSum));
  return fp;
}

/* =============================================================================
 * FUNCTION: restoregame
 */
void restoregame(char *data)
{
  int i;
  unsigned int thesum;
  unsigned int asum;
  int TotalAttr;
  MemoryFile *fp;

  fp = MemoryFile_open(data);

  FileSum = 0;

  read_player(fp);
  read_levels(fp);
  read_store(fp);
  read_monster_data(fp);
  read_spheres(fp);

  /* sum of everything so far */
  thesum = FileSum;

  bread(fp, (char *)&asum, sizeof(asum));
  if (asum != thesum)
  {
    fsorry();
  }

  MemoryFile_dispose(fp);

  lastpx = 0;
  lastpy = 0;

  /*  for the greedy cheater checker  */
  TotalAttr = 0;
  for (i = ABILITY_FIRST ; i < ABILITY_LAST ; i++)
  {
    TotalAttr += c[i];
    if (c[i] > 300)
    {
      greedy();
    }
  }
  
  if (TotalAttr > 600)
  {
    greedy();
  }

  if ((c[HPMAX] > 999) || (c[SPELLMAX] > 125))
  {
    greedy();
  }

  /* XP has been boosted in the save file, so fix character level */
  if ((c[LEVEL] == 25) && (c[EXPERIENCE] > skill[24]))
  {
    long tmp_xp = c[EXPERIENCE]-skill[24]; /* amount to go up */
    c[EXPERIENCE] = skill[24];
    raiseexperience((long) tmp_xp);
  }

  /* Get the current dungeon level from storage */

  getlevel();

}

