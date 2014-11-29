/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: ularn_ask.c
 *
 * DESCRIPTION:
 * This module provides functions for getting answers to common questions
 * in the game from teh player.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * more        - Asks press space to continue
 * clearpager  - resets pagination calculations for paged text output
 * pager       - Call to perform pagination ofter each line of text is output
 * getyn       - ASks for a yess no answer
 * getpassword - Get the password for wizard
 * dirsub      - Asks for a direction
 *
 * =============================================================================
 */

#include "ularn_game.h"
#include "ularn_win.h"
#include "ularn_ask.h"
#include "header.h"
#include "player.h"
#include "dungeon.h"

/* =============================================================================
 * Local variables
 */

/* the current line number for paginating text */
static int srcount = 0;

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: clearpager
 */
void clearpager(void)
{
  srcount = 0;
}

/* =============================================================================
 * FUNCTION: pager
 */
void pager(void)
{
  if (++srcount >= 22)
  {
    srcount = 0;
  }
}

/* =============================================================================
 * FUNCTION: dirsub
 */
int dirsub(int *x, int *y, ActionType ans)
{
  int d;
  ActionType dir_order[5];

  /*
   * Direction keys. Order must match diroff
   */

  dir_order[0] = ACTION_MOVE_SOUTH;
  dir_order[1] = ACTION_MOVE_EAST;
  dir_order[2] = ACTION_MOVE_NORTH;
  dir_order[3] = ACTION_MOVE_WEST;
  dir_order[4] = 0;

  d = 0;
  while (dir_order[d] != ans) d++;

  d++;

  *x = playerx + diroffx[d];
  *y = playery + diroffy[d];

  return (d);
}
