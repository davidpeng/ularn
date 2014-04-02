/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: help.c
 *
 * DESCRIPTION:
 * This module contains functions to display the help file and the welcome
 * screen.
 *
 * format of the Ularn help file:
 *
 * The 1st character of file is the # of pages of help available (ascii digit).
 * The first page (23 lines) of the file is for the introductory message
 * (not counted in above).
 * The pages of help text are 23 lines per page.
 * The help file allows escape sequences for specifying formatting of the
 * text.
 * The currently supported sequences are:
 *   ^[[7m - Set text format to standout  (red)
 *   ^[[8m - Set text format to standout2 (green)
 *   ^[[9m - Set text format to standout3 (blue)
 *   ^[[m  - Set text format to normal.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * help    - Display the help file.
 * welcome - Display the welcome message
 *
 * =============================================================================
 */

#include <stdio.h>

#include "header.h"
#include "ularn_game.h"
#include "ularn_win.h"

#include "player.h"

/* =============================================================================
 * Local variables
 */

/*
 * Help file escape sequence states.
 */
typedef enum
{
  HELP_NORMAL,
  HELP_ESC,
  HELP_COUNT
} HelpStateType;

/*
 * A pointer to the help file handle.
 */
static FILE *help_fp;

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: openhelp
 *
 * DESCRIPTION:
 * Function to open the help file and return the number of pages in the help
 * file.
 *
 * PARAMETERS:
 *
 *   None
 *
 * RETURN VALUE:
 *
 *   The number of pages in the help file as specified by the first character.
 */
static int openhelp (void)
{
  help_fp = fopen(helpfile, "r");

  if (help_fp == NULL)
  {
    Printf("Can't open help file \"%s\" ", helpfile);

    nap(4000);
    return -1;
  }

  return (fgetc(help_fp) - '0');
}

/* =============================================================================
 * FUNCTION: show_help_page
 *
 * DESCRIPTION:
 * This function prints the next page of help in the help file.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void show_help_page(void)
{
  HelpStateType state;
  int line;
  int line_len;
  int line_pos;
  char tmbuf[128];

  for (line = 0; line < 23 ; line++)
  {
    fgets(tmbuf, 128, help_fp);

    line_len = strlen(tmbuf);

    state = HELP_NORMAL;

    line_pos = 0;
    while (line_pos < line_len)
    {
      switch (state)
      {
        case HELP_NORMAL:
          if ((tmbuf[line_pos] == '^') &&
              ((line_pos + 3) < line_len))
          {
            if ((tmbuf[line_pos + 1] == '[') &&
                (tmbuf[line_pos + 2] == '['))
            {
              state = HELP_ESC;
              line_pos += 3;
            }
          }

          if (state == HELP_NORMAL)
          {
            Printc(tmbuf[line_pos]);
            line_pos++;
          }
          break;

        case HELP_ESC:
          if (tmbuf[line_pos] == '7')
          {
            line_pos++;
          }
          else if (tmbuf[line_pos] == '8')
          {
            line_pos++;
          }
          else if (tmbuf[line_pos] == '9')
          {
            line_pos++;
          }

          line_pos++;
          state = HELP_NORMAL;
          break;

        default:
          break;
      }

    }
  }

}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: help
 */
void help (void)
{
  int num_pages;
  int page;
  int i;
  char tmbuf[128];

  /* open the help file and get # pages */
  num_pages = openhelp();

  if (num_pages < 0)
  {
    return;
  }

  /* skip over intro message */
  for (i = 0; i < 23 ; i++)
  {
    fgets(tmbuf, 128, help_fp);
  }

  for (page = num_pages ; page > 0 ; page--)
  {
    show_help_page();
  }

  fclose(help_fp);

}

/* =============================================================================
 * FUNCTION: welcome
 */
void welcome (void)
{
  print_header("<p>Welcome to the game of Ularn.  At this moment, you face a great problem.  ");
  print_header("Your daughter has contracted a strange disease and none of your home remedies ");
  print_header("seems to have any effect.  You sense that she is in mortal danger and you must ");
  print_header("try to save her.  Time ago you heard of a land of great danger and opportunity.  ");
  print_header("Perhaps here is the solution you need.</p>");
  print_header("<p>It has been said that there once was a great magician who called himself ");
  print_header("Polinneaus.  Many years ago, after having many miraculous successes, Polinneaus ");
  print_header("retired to the caverns of Ularn, where he devoted most of his time to the ");
  print_header("creation of magic.   Rumors have it that one day Polinneaus set out to dispel ");
  print_header("an attacking army in a forest some distance to the north.  It is believed that ");
  print_header("there he met his demise.</p>");
  print_header("<p>The caverns of Ularn, it is thought, must be magnificent in design ");
  print_header("and contain much magic and treasure.  One option you have is to undertake a ");
  print_header("journey into these caverns.</p>");
  set_callback("*");
  add_option(-1, "Good Luck!  (You're going to need it.)", "");
}


