/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: ularn_ask.h
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

#ifndef __ULARN_ASK_H
#define __ULARN_ASK_H

/* =============================================================================
 * FUNCTION: clearpager
 *
 * DESCRIPTION:
 * Function to initialise text pagination.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void clearpager(void);

/* =============================================================================
 * FUNCTION: Pager
 *
 * DESCRIPTION:
 * Pagination function to be called for each line of text added to the
 * display.
 * When a full page of text has been displayed the more() function is called
 * to halt further output until the player has pressed space.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void pager(void);

/* =============================================================================
 * FUNCTION: dirsub
 *
 * DESCRIPTION:
 * Function to ask for a direction and modify an x,y for that direction
 * Enter with the origination coordinates in (x,y). Returns index into diroffx[]
 * (0-8).
 * NOTE: x,y may be outside the map after calling this function.
 *
 * PARAMETERS:
 *
 *   x : This is the X coord to be modified by the selected direction.
 *
 *   y : This is the Y coord to be modified by the selected direction.
 *
 * RETURN VALUE:
 *
 *   The index into diroff of the direction selected.
 */
int dirsub(int *x, int *y, ActionType ans);

#endif
