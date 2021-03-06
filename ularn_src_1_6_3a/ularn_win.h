/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: ularn_win.h
 *
 * DESCRIPTION:
 * This module contains all operating system dependant code for input and
 * display update.
 * Each version of ularn should provide a different implementation of this
 * module.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * nonap         : Set to true if no time delays are to be used.
 * nosignal      : Set if ctrl-C is to be trapped to prevent exit.
 * enable_scroll : Probably superfluous
 * yrepcount     : Repeat count for input commands.
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
 * UpdateStatus           : Update the status display
 * UpdateEffects          : Update the effects display
 * UpdateStatusAndEffects : Update both status and effects display
 * ClearText              : Clear the text output area
 * UlarnBeep              : Make a beep
 * MoveCursor             : Set the cursor location
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

#ifndef __ULARN_WIN_H
#define __ULARN_WIN_H

#include "config.h"

/*
 * Text formats
 */
typedef enum FormatType
{
  FORMAT_NORMAL,
  FORMAT_STANDOUT,
  FORMAT_STANDOUT2,
  FORMAT_STANDOUT3,
  FORMAT_BOLD,
  FORMAT_INVERSE,
} FormatType;

/*
 * Flag indicating if time delays are active.
 *   0 = Use delays
 *   1 = No delays
 */
extern int nonap;

/*
 * Flag to indicate if signals are to be trapped.
 */
extern int nosignal;

/* constant for enabled/disabled scrolling regn */
extern char enable_scroll;

/* Repeat count for the current command */
extern int yrepcount;

/*
 * Display modes for the main window
 */
typedef enum DisplayModeType
{
  DISPLAY_MAP,     /* Map display mode for moving around dungeons */
  DISPLAY_TEXT     /* Text display mode for scores, shops etc.    */
} DisplayModeType;

//
// Effects types for map_effect
//
typedef enum DirEffectsType
{
  EFFECT_MLE,
  EFFECT_SSP,
  EFFECT_BAL,
  EFFECT_LIT,
  EFFECT_CLD,
  EFFECT_COUNT
} DirEffectsType;

//
// Effects for show_nagic_effect.
//
typedef enum MagicEffectsType
{
  MAGIC_SPARKLE,
  MAGIC_SLEEP,
  MAGIC_WEB,
  MAGIC_PHANTASMAL,
  MAGIC_CLOUD,
  MAGIC_VAPORIZE,
  MAGIC_DEHYDRATE,
  MAGIC_DRAIN,
  MAGIC_FLOOD,
  MAGIC_FINGER,
  MAGIC_TELEPORT,
  MAGIC_FIRE,
  MAGIC_WALL,
  MAGIC_DEMON,
  MAGIC_ANNIHILATE,
  MAGIC_COUNT
} MagicEffectsType;

//
// Actions that can be selected in the game.
//

typedef enum ActionType
{
  ACTION_NULL,
  ACTION_DIAG,
  ACTION_MOVE_WEST,
  ACTION_RUN_WEST,
  ACTION_MOVE_EAST,
  ACTION_RUN_EAST,
  ACTION_MOVE_SOUTH,
  ACTION_RUN_SOUTH,
  ACTION_MOVE_NORTH,
  ACTION_RUN_NORTH,
  ACTION_MOVE_NORTHEAST,
  ACTION_RUN_NORTHEAST,
  ACTION_MOVE_NORTHWEST,
  ACTION_RUN_NORTHWEST,
  ACTION_MOVE_SOUTHEAST,
  ACTION_RUN_SOUTHEAST,
  ACTION_MOVE_SOUTHWEST,
  ACTION_RUN_SOUTHWEST,
  ACTION_WAIT,
  ACTION_NONE,
  ACTION_WIELD,
  ACTION_WEAR,
  ACTION_READ,
  ACTION_QUAFF,
  ACTION_DROP,
  ACTION_CAST_SPELL,
  ACTION_OPEN_DOOR,
  ACTION_CLOSE_DOOR,
  ACTION_OPEN_CHEST,
  ACTION_INVENTORY,
  ACTION_EAT_COOKIE,
  ACTION_LIST_SPELLS,
  ACTION_HELP,
  ACTION_SAVE,
  ACTION_TELEPORT,
  ACTION_IDENTIFY_TRAPS,
  ACTION_BECOME_CREATOR,
  ACTION_CREATE_ITEM,
  ACTION_TOGGLE_WIZARD,
  ACTION_DEBUG_MODE,
  ACTION_REMOVE_ARMOUR,
  ACTION_PACK_WEIGHT,
  ACTION_VERSION,
  ACTION_QUIT,
  ACTION_REDRAW_SCREEN,
  ACTION_SHOW_TAX,
  ACTION_COUNT
} ActionType;

/* =============================================================================
 * FUNCTION: close_app
 *
 * DESCRIPTION:
 * Close the application, freeing all resources.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void close_app(void);

/* =============================================================================
 * Status and effects display update functions
 */

/* =============================================================================
 * FUNCTION: UpdateStatus
 *
 * DESCRIPTION:
 * Update the status lines showing the current player's attributes, HP, Exp
 * etc.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void UpdateStatus(void);

/* =============================================================================
 * FUNCTION: UpdateEffects
 *
 * DESCRIPTION:
 * Updates the display of effects curently in play.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void UpdateEffects(void);

/* =============================================================================
 * FUNCTION: UpdateStatusAndEffects
 *
 * DESCRIPTION:
 * Update status and effects display.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void UpdateStatusAndEffects(void);

/* =============================================================================
 * Text routines (operates on message or text window)
 */

/* =============================================================================
 * FUNCTION: beep
 *
 * DESCRIPTION:
 * Make a beep.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void UlarnBeep(void);

/* =============================================================================
 * FUNCTION: Printc
 *
 * DESCRIPTION:
 * Print a single character to the current text output.
 *
 * PARAMETERS:
 *
 *   c : The character to print.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void Printc(char c);

/* =============================================================================
 * FUNCTION: Print
 *
 * DESCRIPTION:
 * Print a string to the current text output.
 *
 * PARAMETERS:
 *
 *   string : The string to print
 *
 * RETURN VALUE:
 *
 *   None.
 */
void Print(char *string);

/* =============================================================================
 * FUNCTION: Printf
 *
 * DESCRIPTION:
 * Formatted print to the current text output. Takes the same input as the
 * standard printf.
 *
 * PARAMETERS:
 *
 *   fmt : Theformat string
 *
 *   ... : Values to be printed as specified by the format string
 *
 * RETURN VALUE:
 *
 *   None.
 */
void Printf(char *fmt, ...);

/* =============================================================================
 * Map display functions
 */

/* =============================================================================
 * FUNCTION: show1cell
 *
 * DESCRIPTION:
 * Display a single cell of the map, making that cell known if the player
 * isn't blind.
 *
 * PARAMETERS:
 *
 *   x : The x coordiante of the cell to show.
 *
 *   y : The y coordinate of the cell to show.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void show1cell(int x, int y);

/* =============================================================================
 * FUNCTION: showplayer
 *
 * DESCRIPTION:
 * Show the player on the map, scrolling the map if necessary.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void showplayer(void);

/* =============================================================================
 * FUNCTION: showcell
 *
 * DESCRIPTION:
 * Display a cell location around the player on the screen.
 * The player gets to know about the cells shown (sets the know array).
 *
 * PARAMETERS:
 *
 *   x : The x position to display
 *
 *   y : The y position to display
 *
 * RETURN VALUE:
 *
 *   None.
 */
void showcell(int x, int y);

/* =============================================================================
 * FUNCTION: drawscreen
 *
 * DESCRIPTION:
 * Redraw the entire screen.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void drawscreen(void);

/* =============================================================================
 * FUNCTION: draws
 *
 * DESCRIPTION:
 * Redraw a sub-section of the screen.
 *
 * PARAMETERS:
 *
 *   minx : The min x coordiante of the area to redraw.
 *
 *   miny : The min y coordiante of the area to redraw.
 *
 *   maxx : The max x coordiante of the area to redraw.
 *
 *   maxy : The max y coordiante of the area to redraw.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void draws(int minx, int miny, int maxx, int maxy);

/* =============================================================================
 * FUNCTION: mapeffect
 *
 * DESCRIPTION:
 * Draw a directional effect on the map.
 * This is used for drawing the range based spell effects.
 *
 * PARAMETERS:
 *
 *   x      : The x location to draw the effect
 *
 *   y      : The y location to draw the effect
 *
 *   effect : The effect type to show.
 *
 *   dir    : The direction the effect is going.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void mapeffect(int x, int y, DirEffectsType effect, int dir);

/* =============================================================================
 * FUNCTION: magic_effect_frames
 *
 * DESCRIPTION:
 * The number of frames in a magic effect animation.
 *
 * PARAMETERS:
 *
 *   fx : The effect type.
 *
 * RETURN VALUE:
 *
 *   The number of frames in the effect animation
 */
int magic_effect_frames(MagicEffectsType fx);

/* =============================================================================
 * FUNCTION: magic_effect
 *
 * DESCRIPTION:
 * Draw 1 frame of the animatin for a magic effect.
 *
 * PARAMETERS:
 *
 *   x     : The x location for the effect
 *
 *   y     : The y location for the effect
 *
 *   fx    : The effect type.
 *
 *   frame : The frame number to display
 *
 * RETURN VALUE:
 *
 *   None.
 */
void magic_effect(int x, int y, MagicEffectsType fx, int frame);

/* =============================================================================
 * Timing functions
 */

/* =============================================================================
 * FUNCTION: nap
 *
 * DESCRIPTION:
 * Delay for a number of milliseconds.
 *
 * PARAMETERS:
 *
 *   delay : The number of milliseconds to delay
 *
 * RETURN VALUE:
 *
 *   None.
 */
void nap(int delay);

/* =============================================================================
 * User name functions
 */

/* =============================================================================
 * FUNCTION: GetUser
 *
 * DESCRIPTION:
 * This function gets the username and user id.
 * This should use operating system calls if available.
 * If not, ask the player for their name (I guess we'll just have to trust
 * them).
 *
 * PARAMETERS:
 *
 *   username : On input this should be set to the name specified in the
 *              ularn options file. If the OS doesn't provide a username
 *              then this should be used for the username.
 *              On exit this is set to the username to use for the game.
 *
 *   uid      : This is set to the user id of the user.
 *              Systems that do not support user ids must provide a mechanism
 *              for associating a number with each username.
 *              I suggest using a player id file to store the mappings
 *              between users and numbers, incrementing the uid for each new
 *              player.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void GetUser(char *username, int *uid);

void set_input_type(char *type);
void set_callback(char *function);
char *get_callback();
void add_option(int code, char *text, char *subtext);

void print_header(char *string);
void printf_header(char *format, ...);

#endif
