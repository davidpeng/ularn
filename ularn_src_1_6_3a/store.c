/* =============================================================================
 * PROGRAM:  ularn
 * FILENAME: store.c
 *
 * DESCRIPTION:
 * This module contaions functions to handle the locations found on the home
 * level and in the dungeon.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 * None
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * dndstore    : Handles entering the DND store
 * oschool     : Handles entering the college of larn
 * obank       : Handles entering the bank of larn
 * obank2      : Handles entering the 8th branch of the bank of larn
 * ointerest   : Calculates interest on bank accounts
 * item_value  : Calculates the value of an item as per the trading post
 * otradepost  : Handles entering the trading post.
 * olrs        : Handles entering the Larn Revenue Service
 * opad        : Handles enetering dealer McDope's pad.
 * ohome       : Handes the player returning home.
 * write_store : Writes all shop data to the save file
 * read_store  : Reads all shop data from the save file
 *
 * =============================================================================
 */

#include "ularn_game.h"
#include "ularn_win.h"
#include "ularn_ask.h"
#include "saveutils.h"
#include "store.h"
#include "header.h"
#include "player.h"
#include "potion.h"
#include "scroll.h"
#include "dungeon.h"
#include "scores.h"
#include "show.h"
#include "itm.h"

/* =============================================================================
 *
 * ######          ######           #####
 * #     #  #    # #     #         #     #   #####   ####   #####   ######
 * #     #  ##   # #     #         #           #    #    #  #    #  #
 * #     #  # #  # #     #          #####      #    #    #  #    #  #####
 * #     #  #  # # #     #               #     #    #    #  #####   #
 * #     #  #   ## #     #         #     #     #    #    #  #   #   #
 * ######   #    # ######           #####      #     ####   #    #  ######
 *
 */


/* =============================================================================
 * Local variables
 */

/* max # items in the dnd store */
#define DNDSIZE   93

static int dnditm = 0;

struct _itm
{
  long  price;
  int *mem;
  char  obj;
  char  arg;
  char  qty;
};

/*
 * Store inventory and prices
 */
static struct _itm itm[DNDSIZE] =  {

/*cost    memory   iven name  iven arg      how
   gp   pointer  iven[] ivenarg[]     many */
{ 2,  0,  OLEATHER, 0,    3 } ,
{ 10, 0,  OSTUDLEATHER, 0,    2 } ,
{ 40, 0,  ORING,    0,    2 } ,
{ 85, 0,  OCHAIN,   0,    2 } ,
{ 220,  0,  OSPLINT,  0,    1 } ,
{ 400,  0,  OPLATE,   0,    1 } ,
{ 900,  0,  OPLATEARMOR,  0,    1 } ,
{ 2600, 0,  OSSPLATE, 0,    1 } ,
{ 150,  0,  OSHIELD,  0,    1 } ,
{ 5000, 0,      OELVENCHAIN,    0,        0 } ,
{ 1000, 0,  OORB,   0,    0 } , /* 10 */
{ 10000,0,  OSLAYER,  0,    0 } ,

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */

{ 2,  0,  ODAGGER,    0,  3 } ,
{ 20, 0,  OSPEAR,     0,  3 } ,
{ 80, 0,  OFLAIL,     0,  2 } ,
{ 150,  0,  OBATTLEAXE,   0,  2 } ,
{ 450,  0,  OLONGSWORD,   0,  2 } ,
{ 1000, 0,  O2SWORD,    0,  2 } ,
{ 5000, 0,  OSWORD,     0,  1 } ,
{ 20000,0,  OLANCE,     0,  1 } ,
{ 2000, 0,  OSWORDofSLASHING, 0,  0 } , /* 20 */
{ 7500, 0,  OHAMMER,    0,  0 } ,

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */
{ 150,  0,  OPROTRING,  1,  1 } ,
{ 85, 0,  OSTRRING, 1,  1 } ,
{ 120,  0,  ODEXRING, 1,  1 } ,
{ 120,  0,  OCLEVERRING,  1,  1 } ,
{ 180,  0,  OENERGYRING,  0,  1 } ,
{ 125,  0,  ODAMRING, 0,  1 } ,
{ 220,  0,  OREGENRING, 0,  1 } ,
{ 1000, 0,  ORINGOFEXTRA, 0,  1 } ,

{ 280,  0,  OBELT,    0,  1 } ,   /* 30 */
{ 400,  0,  OAMULET,  5,  1 } ,

{ 500,  0,  OCUBEofUNDEAD,  0,  0 } ,
{ 600,  0,  ONOTHEFT, 0,  0 } ,

{ 590,  0,  OCHEST,   3,  1 } ,
{ 200,  0,  OBOOK,    2,  1 } ,
{ 10, 0,  OCOOKIE,  0,  3 } ,
{ 666,  0,  OHANDofFEAR,    0,  0 } ,

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */

{ 20, potionknown,  OPOTION,  PSLEEP,  6 } ,
{ 90, potionknown,  OPOTION,  PHEALING,  5 } ,
{ 520,  potionknown,  OPOTION,  PRAISELEVEL,  1 } ,   /* 40 */
{ 100,  potionknown,  OPOTION,  PINCABILITY,  2 } ,
{ 50, potionknown,  OPOTION,  PWISDOM,  2 } ,
{ 150,  potionknown,  OPOTION,  PSTRENGTH,  2 } ,
{ 70, potionknown,  OPOTION,  PCHARISMA,  1 } ,
{ 30, potionknown,  OPOTION,  PDIZZINESS,  7 } ,
{ 200,  potionknown,  OPOTION,  PLEARNING,  1 } ,
{ 50, potionknown,  OPOTION,  PGOLDDET,  1 } ,
{ 80, potionknown,  OPOTION,  PMONSTDET, 1 } ,

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */

{ 30, potionknown,  OPOTION,  PFORGETFUL, 3 } ,
{ 20, potionknown,  OPOTION,  PWATER, 5 } , /* 50 */
{ 40, potionknown,  OPOTION,  PBLINDNESS, 3 } ,
{ 35, potionknown,  OPOTION,  PCONFUSION, 2 } ,
{ 520,  potionknown,  OPOTION,  PHEROISM, 1 } ,
{ 90, potionknown,  OPOTION,  PSTURDINESS, 2 } ,
{ 200,  potionknown,  OPOTION,  PGIANTSTR, 2 } ,
{ 220,  potionknown,  OPOTION,  PFIRERESIST, 4 } ,
{ 80, potionknown,  OPOTION,  PTREASURE, 6 } ,
{ 370,  potionknown,  OPOTION,  PINSTHEAL, 3 } ,
{ 50, potionknown,  OPOTION,  PPOISON, 1 } ,
{ 150,  potionknown,  OPOTION,  PSEEINVIS, 3 } , /* 60 */

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */

{ 850,  0,    OORBOFDRAGON, 0,  0 } ,
{ 750,  0,    OSPIRITSCARAB,  0,  0 } ,
{ 8000, 0,    OVORPAL,  0,  0,  } ,

{ 100,  scrollknown,  OSCROLL,  SENCHANTARM,  2 } ,
{ 125,  scrollknown,  OSCROLL,  SENCHANTWEAP,  2 } ,
{ 60, scrollknown,  OSCROLL,  SENLIGHTEN,  4 } ,
{ 10, scrollknown,  OSCROLL,  SBLANK,  4 } ,
{ 100,  scrollknown,  OSCROLL,  SCREATEMONST,  3 } ,
{ 200,  scrollknown,  OSCROLL,  SCREATEITEM,  2 } ,
{ 110,  scrollknown,  OSCROLL,  SAGGMONST,  1 } , /* 70 */
{ 500,  scrollknown,  OSCROLL,  STIMEWARP,  2 } ,
{ 200,  scrollknown,  OSCROLL,  STELEPORT,  2 } ,
{ 250,  scrollknown,  OSCROLL,  SAWARENESS,  4 } ,
{ 20, scrollknown,  OSCROLL,  SHASTEMONST, 5 } ,
{ 30, scrollknown,  OSCROLL,  SMONSTHEAL, 3 } ,

/*cost    memory  iven name iven arg   how
  gp   pointer    iven[]  ivenarg[]  many */

{ 340,  scrollknown,  OSCROLL,  SSPIRITPROT, 1 } ,
{ 340,  scrollknown,  OSCROLL,  SUNDEADPROT, 1 } ,
{ 300,  scrollknown,  OSCROLL,  SSTEALTH, 2 } ,
{ 400,  scrollknown,  OSCROLL,  SMAGICMAP, 2 } ,
{ 500,  scrollknown,  OSCROLL,  SHOLDMONST, 2 } , /* 80 */
{ 1000, scrollknown,  OSCROLL,  SGEMPERFECT, 1 } ,
{ 500,  scrollknown,  OSCROLL,  SSPELLEXT, 1 } ,
{ 340,  scrollknown,  OSCROLL,  SIDENTIFY, 2 } ,
{ 220,  scrollknown,  OSCROLL,  SREMCURSE, 3 } ,
{ 3900, scrollknown,  OSCROLL,  SANNIHILATE, 0 } ,
{ 610,  scrollknown,  OSCROLL,  SPULVERIZE, 1 } ,
{ 3000, scrollknown,  OSCROLL,  SLIFEPROT, 0 } ,
{ 300,  0,    OSPHTALISMAN,   0,  0 } ,
{ 150,  0,    OWWAND,   0,  0 } ,
{ 50,   0,    OBRASSLAMP, 0,  0 } , /* 90 */
{ 9500, 0,    OPSTAFF,  0,  0 },
{ 10000, 0, OLIFEPRESERVER, 0, 0 }
};

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: write_dnd_store
 *
 * DESCRIPTION:
 * Writes the DnD store inventory data to the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being written.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void write_dnd_store(MemoryFile *fp)
{
  int i;

  /* write the number of each item remaining */
  for (i = 0 ; i < DNDSIZE ; i++)
  {
    bwrite(fp, &(itm[i].qty), sizeof(char));
  }
}

/* =============================================================================
 * FUNCTION: read_dnd_store
 *
 * DESCRIPTION:
 * Reads the DnD Store inventory from the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being read.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void read_dnd_store(MemoryFile *fp)
{
  int i;

  /* read the number of each item remaining */
  for (i = 0 ; i < DNDSIZE ; i++)
  {
    bread(fp, &(itm[i].qty), sizeof(char));
  }
}

/* =============================================================================
 * FUNCTION: handsfull
 *
 * DESCRIPTION:
 * Prints the hands full message.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void handsfull(void)
{
  Print("You can't carry anything more!");
  nap(2200);
}

/* =============================================================================
 * FUNCTION: outofstock
 *
 * DESCRIPTION:
 * Printd the out of stock message.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void outofstock(void)
{
  Print("Sorry, but we are out of that item.");
  nap(2200);
}

/* =============================================================================
 * FUNCTION: nogold
 *
 * DESCRIPTION:
 * Prints the insufficient funds message.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void nogold(void)
{
  Print("You don't have enough gold to pay for that!");
  nap(2200);
}

/* =============================================================================
 * FUNCTION: dnditem
 *
 * DESCRIPTION:
 * Function to print an item from the DnD store inventory at the appropriate
 * screen location for a 2 column output.
 * It is assumed that items will be displayed in pages of IVENSIZE items.
 *
 * PARAMETERS:
 *
 *   i : The index of the item in the DnD Store inventory.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void dnditem(int i)
{
  if (i >= DNDSIZE) return;

  if (itm[i].qty == 0)
  {
    Printf("%39s", "");
    return;
  }
  Printf("%c) ", (i%IVENSIZE)+'a');
  if (itm[i].obj == OPOTION)
  {
    Print("potion of ");
    Printf("%s", &potionname[(int) itm[i].arg][1]);
  }
  else if (itm[i].obj == OSCROLL)
  {
    Print("scroll of ");
    Printf("%s", &scrollname[(int) itm[i].arg][1]);
  }
  else
  {
    Printf("%s", objectname[(int) itm[i].obj]);
  }
  Printf("%6ld", ((long) (itm[i].price*10L)));
}

/* =============================================================================
 * FUNCTION: dnd_2hed
 *
 * DESCRIPTION:
 * Function to print the DnD Store heading text.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void dnd_2hed(void)
{
  print_header("Welcome to the Ularn Thrift Shoppe.  We stock many items explorers find useful ");
  print_header("in their adventures.  Feel free to browse to your heart's content. ");
  print_header("Also be advised that if you break 'em, you pay for 'em.");
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: dndstore
 */
void add_dndstore_options()
{
	char name[50], price[50];
	for (int i = 0; i < DNDSIZE; i++)
	{
		if (itm[i].qty == 0)
			continue;

		switch (itm[i].obj)
		{
		case OPOTION:
			sprintf(name, "potion of %s", &potionname[(int) itm[i].arg][1]);
			break;
		case OSCROLL:
			sprintf(name, "scroll of %s", &scrollname[(int) itm[i].arg][1]);
			break;
		default:
			strcpy(name, objectname[(int) itm[i].obj]);
			break;
		}
		sprintf(price, "%6ld gp", ((long) (itm[i].price*10L)));
		add_option(i, name, price);
	}
	add_option(-1, "Leave", "");
}

void dndstore()
{
  dnditm = 0;
  dnd_2hed();
  if (outstanding_taxes>0)
  {
    Print("The Ularn Revenue Service has ordered us to not do business with tax evaders.");
    UlarnBeep();
    Printf("They have also told us that you owe %d gp in back taxes and, as we must ",(long)outstanding_taxes);
    Print("comply with the law, we cannot serve you at this time.  So Sorry. ");
    return;
  }

  set_callback("buy_something");
  add_dndstore_options();
}

void buy_something(int i)
{
    if (i>=DNDSIZE)
		outofstock();
    else if (itm[i].qty <= 0)
		outofstock();
    else if (pocketfull())
		handsfull();
    else if (c[GOLD] < itm[i].price*10L)
		nogold();
    else 
    {
		/* Player learns this item */
		if (itm[i].mem != 0)
			itm[i].mem[(int) itm[i].arg] = 1 ;

		c[GOLD] -= itm[i].price*10L;
		itm[i].qty--;
		take(itm[i].obj,itm[i].arg);
		if (itm[i].qty==0)
			dnditem(i);
		nap(1001);
    }
    dndstore();
}



/* =============================================================================
 *
 *  #####
 * #     #   ####   #       #       ######   ####   ######
 * #        #    #  #       #       #       #    #  #
 * #        #    #  #       #       #####   #       #####
 * #        #    #  #       #       #       #  ###  #
 * #     #  #    #  #       #       #       #    #  #
 *  #####    ####   ######  ######  ######   ####   ######
 * 
 */


/* =============================================================================
 * Local variables
 */

/* the list of courses taken and the time required */
#define MAX_COURSES 8
char course[MAX_COURSES] = { 0, 0, 0, 0, 0, 0, 0, 0 };
char coursetime[MAX_COURSES] = { 10, 15, 10, 20, 10, 10, 10, 5 };

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: write_college
 *
 * DESCRIPTION:
 * Write the college data to the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being written.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void write_college(MemoryFile *fp)
{
  bwrite(fp, course, MAX_COURSES * sizeof(char));
}

/* =============================================================================
 * FUNCTION: read_college
 *
 * DESCRIPTION:
 * Function to read the college data from the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being read.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void read_college(MemoryFile *fp)
{
  bread(fp, course, MAX_COURSES * sizeof(char));
}

/* =============================================================================
 * FUNCTION: sch_hed
 *
 * DESCRIPTION:
 * Function to display the header info for the school.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void sch_hed(void)
{
  print_header("The College of Ularn offers the exciting opportunity of higher education to ");
  print_header("all inhabitants of the caves.  Here is the class schedule. ");

  print_header("All courses cost 250 gold pieces.");
}


/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: oschool
 */
void add_school_options()
{
	if (course[0] == 0)
		add_option('a', "Fighter's Training I", "10 mobuls");

	if (course[1] == 0)
		add_option('b', "Fighter's Training II", "15 mobuls");

	if (course[2] == 0)
		add_option('c', "Introduction to Wizardry", "10 mobuls");

	if (course[3] == 0)
		add_option('d', "Applied Wizardry", "20 mobuls");

	if (course[4] == 0)
		add_option('e', "Behavioral Psychology", "10 mobuls");

	if (course[5] == 0)
		add_option('f', "Faith for Today", "10 mobuls");

	if (course[6] == 0)
		add_option('g', "Contemporary Dance", "10 mobuls");

	if (course[7] == 0)
		add_option('h', "History of Ularn", "5 mobuls");

	add_option(-1, "Leave", "");
}

void oschool()
{
  sch_hed();

  set_callback("take_course");
  add_school_options();
}

void take_course(int i)
{
    long time_used;

    if (c[GOLD] < 250)
    {
      nogold();
    }
    else
    {
      if (course[i-'a'])
      {
        Print("Sorry but that class is filled.");
        nap(1000);
      }
      else if (i <= 'h')
      {
        c[GOLD] -= 250;
        time_used=0;
        switch(i)
        {
          case 'a':
            c[STRENGTH] += 2;
            c[CONSTITUTION]++;
            Print("You feel stronger!");
            break;

          case 'b':
            if (course[0]==0)
            {
              Print("Sorry but this class has a prerequisite of Fighter's Training I");
              c[GOLD]+=250;
              time_used= -10000;
            }
            else
            {
              Print("You feel much stronger!");
              c[STRENGTH] += 2;
              c[CONSTITUTION] += 2;
            }
            break;

          case 'c':
            c[INTELLIGENCE] += 2;
            Print("The task before you now seems more attainable!");
            break;

          case 'd':
            if (course[2]==0)
            {
              Print("Sorry but this class has a prerequisite of Introduction to Wizardry");
              c[GOLD]+=250;
              time_used= -10000;
            }
            else
            {
              Print("The task before you now seems very attainable!");
              c[INTELLIGENCE] += 2;
            }
            break;

          case 'e':
            c[CHARISMA] += 3;
            Print("You now feel like a born leader!");
            break;

          case 'f':
            c[WISDOM] += 2;
            Print("You now feel more confident that you can find the potion in time!");
            break;

          case 'g':
            c[DEXTERITY] += 3;
            Print("You feel like dancing!");
            break;

          case 'h':
            c[INTELLIGENCE]++;
            Print("Wow! e = mc^2!");
            break;
        }

        time_used += coursetime[i-'a']*100;

        if (time_used > 0)
        {
          gtime += time_used;

          /* remember that he has taken that course */
          course[i-'a']++;

          /* he regenerated */
          c[HP] = c[HPMAX];
          c[SPELLS] = c[SPELLMAX];

          /* cure blindness too!  */
          if (c[BLINDCOUNT])
            c[BLINDCOUNT] = 1;

          /*  end confusion */
          if (c[CONFUSE])
            c[CONFUSE] = 1;

          /* adjust parameters for time change */
          adjusttime((long) time_used);
        }
        nap(1000);
      }
    }
    oschool();
}

/* =============================================================================
 *
 * ######                                  #
 * #     #    ##    #    #  #    #         #          ##    #####   #    #
 * #     #   #  #   ##   #  #   #          #         #  #   #    #  ##   #
 * ######   #    #  # #  #  ####           #        #    #  #    #  # #  #
 * #     #  ######  #  # #  #  #           #        ######  #####   #  # #
 * #     #  #    #  #   ##  #   #          #        #    #  #   #   #   ##
 * ######   #    #  #    #  #    #    Of   #######  #    #  #    #  #    #
 *
 */

/* =============================================================================
 * Local variables
 */

/*
 * limit of 1 million gold pieces in bank
 */
#define BANKLIMIT 1000000

/* last time he was in bank */
static long lasttime = 0;

/* the reference to screen location for each gem */
static short gemorder[IVENSIZE]={0};
/* the appraisal of the gems */
static long gemvalue[IVENSIZE]={0};

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: write_bank
 *
 * DESCRIPTION:
 * Function to write the bank data to the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being written.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void write_bank(MemoryFile *fp)
{
  bwrite(fp, (char *) &lasttime, sizeof(long));
}

/* =============================================================================
 * FUNCTION: read_bank
 *
 * DESCRIPTION:
 * Function to read the bank data from the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being read.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void read_bank(MemoryFile *fp)
{
  bread(fp, (char *) &lasttime, sizeof(long));
}

/* =============================================================================
 * FUNCTION: appraise
 *
 * DESCRIPTION:
 * Function to appraise the eye of larn and offer to buy it.
 *
 * PARAMETERS:
 *
 *   eye   : The index of the inventory slot holding the eye of larn
 *
 *   order : The order of the eye oflarn in the gem appraisal display
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void obanksub2()
{
  printf_header("You have %8d gold piece%s in the bank.",
         (long)c[BANKACCOUNT],
         plural(c[BANKACCOUNT]));

  if (c[BANKACCOUNT]+c[GOLD] >= 500000L)
  {
    print_header("Note:  Only deposits under 1,000,000gp can earn interest.");
  }
  
  set_callback("obanksub3");
  add_option('d', "Deposit", "");
  add_option('w', "Withdraw", "");
  add_option('s', "Sell", "");
  add_option(-1, "Leave", "");
} 
 
static void appraise()
{
  printf_header("I see you have %s.", objectname[OLARNEYE]);
  print_header("I must commend you.  I didn't think you could get it.");
  print_header("Shall I appraise it for you? ");
  set_callback("appraise2");
  add_option('y', "Yes", "");
  add_option('n', "No", "");
}

void appraise2(int ans)
{
	if (ans == 'y')
	{
		print_header("Just one moment please...");
		nap(1000);
		long amt = (long)250000L-((gtime*7)/100)*100;

		if (amt<50000L) amt=50000L;

		printf_header("This is an excellent stone.");
		printf_header("It is worth %d gold pieces to us.",(long)amt);
		print_header("Would you like to sell it? ");
		set_callback("appraise3");
		add_option('y', "Yes", "");
		add_option('n', "No", "");
	}
    else
        obanksub2();
}

void appraise3(int ans)
{
	long amt;
	switch (ans)
	{
	case 'y':
		amt = (long)250000L-((gtime*7)/100)*100;
		if (amt<50000L) amt=50000L;
		c[GOLD]+=amt;

		for (int i = 0; i < IVENSIZE; i++)
		{
			if (iven[i] == OLARNEYE)
			{
				iven[i] = ONOTHING;
				break;
			}
		}

		c[EYEOFLARN] = 0;
		break;
	case 'n':
		Print("It is, of course, your privilege to keep the stone.");
		nap(500);
		break;
	}
    obanksub2();
}

/* =============================================================================
 * FUNCTION: obanksub
 *
 * DESCRIPTION:
 * Function to handle the main processing for the bank.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void obanksub()
{
  int i,k, eye=0;

  ointerest();  /* credit any needed interest */

  if (level==8) c[TELEFLAG] = 0;

  k = 0;
  for (i=0; i<IVENSIZE; i++)
  {
    switch(iven[i])
    {
      case OLARNEYE:
      case ODIAMOND:
      case OEMERALD:
      case ORUBY:
      case OSAPPHIRE:
        if (iven[i] == OLARNEYE)
        {
          eye=i;
          gemvalue[i] = (long)250000L-((gtime*7)/100)*100;
          if (gemvalue[i]<50000L)
            gemvalue[i]=50000L;
        }
        else
        {
          gemvalue[i] = ivenarg[i]*100;
        }
        gemorder[i] = (short) k;
        k++;
        break;

      default:
        gemvalue[i] = 0;
    }
  }
  
  if (eye)
  {
    appraise();
    return;
  }
  obanksub2();
}

void add_gem_options()
{
	char value[20];
	for (int i = 0; i < IVENSIZE; i++)
	{
		switch (iven[i])
		{
			case OLARNEYE:
			case ODIAMOND:
			case OEMERALD:
			case ORUBY:
			case OSAPPHIRE:
				sprintf(value, "%ld gp", gemvalue[i]);
				add_option(i + 'a', objectname[(int) iven[i]], value);
				break;
		}
	}
	add_option('*', "Sell All Gems", "");
	add_option(0, "Back", "");
}

void obanksub3(int ans)
{
	switch (ans)
	{
	case 'd':
		print_header("How much? ");
		set_input_type("number");
		set_callback("deposit_money");
		break;
	case 'w':
		print_header("How much? ");
		set_input_type("number");
		set_callback("withdraw_money");
		break;
	case 's':
		set_callback("sell_gem");
		add_gem_options();
		break;
	}
}

void deposit_money(long amt)
{
    if (amt < 0)
    {
        amt = 0;
    }
    else if (amt>c[GOLD])
    {
        Print("You don't have that much.");
        amt = 0;
        nap(2000);
    }

    /* Deposit the money */
    c[GOLD] -= amt;
    c[BANKACCOUNT] += amt;
    
    obanksub2();
}

void withdraw_money(long amt)
{
    if (amt < 0)
    {
        amt = 0;
    }
    else if (amt > c[BANKACCOUNT])
    {
        Print("You don't have that much in the bank!");
        amt = 0;
        nap(2000);
    }

    /* Withdraw the money */
    c[GOLD] += amt;
    c[BANKACCOUNT] -= amt;
    
    obanksub2();
}

void sell_gem(int arg)
{
	int i;
    if (arg == 0)
    {
        obanksub2();
        return;
    }
    
    if (arg=='*')
    {
        /* sell all gems */
        for (i=0; i<IVENSIZE; i++)
        {
			if (gemvalue[i])
			{
				if (iven[i]==OLARNEYE)
					c[EYEOFLARN] = 0;
				c[GOLD] += gemvalue[i];
				iven[i]=ONOTHING;
				gemvalue[i]=0;
			}
        }
        obanksub2();
    }
    else
    {
		i = arg;
        if (gemvalue[i=i-'a'] == 0)
        {
			Printf("Item %c is not a gemstone!",i+'a');
			nap(2000);
        }
        else
        {
			if (iven[i]==OLARNEYE)
				c[EYEOFLARN] = 0;
			c[GOLD]+=gemvalue[i];
			iven[i]=ONOTHING;
			gemvalue[i]=0;
        }
        set_callback("sell_gem");
        add_gem_options();
    }
}

/* =============================================================================
 * FUNCTION: banktitle
 *
 * DESCRIPTION:
 * Function to display the title for this branck of the bank and then
 * perform the bank processing.
 *
 * PARAMETERS:
 *
 *   str : The title string for the branch
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void banktitle(char *str)
{
  print_header(str);
  if (outstanding_taxes > 0)
  {
    Print("The Ularn Revenue Service has ordered that your account be frozen until all ");
    UlarnBeep();
    Printf("levied taxes have been paid.  They have also told us that you owe %d gp in ",(long)outstanding_taxes);
    Print("taxes and we must comply with them. We cannot serve you at this time.  Sorry. ");
    Print("We suggest you go to the LRS office and pay your taxes.");
    return;
  }

  obanksub();
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: obank
 */
void obank()
{
  banktitle("Welcome to the First National Bank of Ularn.");
}

/* =============================================================================
 * FUNCTION: obank2
 */
void obank2()
{
  banktitle("Welcome to the 8th-level branch office of the First National Bank of Ularn.");
}

/* =============================================================================
 * FUNCTION: ointerest
 */
void ointerest(void)
{
  int i;

  if (c[BANKACCOUNT] < 0)
  {
    c[BANKACCOUNT] = 0;
  }
  else if ((c[BANKACCOUNT] > 0) && (c[BANKACCOUNT] < BANKLIMIT))
  {
    i = (gtime-lasttime)/100; /*# mobuls elapsed since last here*/
    while ((i-- > 0) && (c[BANKACCOUNT]<BANKLIMIT))
        /*
        ** at 1 mobul ~=~ 1 hour, is 10 % a year
        */
      c[BANKACCOUNT] += (long) (c[BANKACCOUNT] / 877);
  }
  lasttime = (gtime/100)*100;
}


/* =============================================================================
 *
 * #######
 *    #     #####     ##    #####      #    #    #   ####
 *    #     #    #   #  #   #    #     #    ##   #  #    #
 *    #     #    #  #    #  #    #     #    # #  #  #
 *    #     #####   ######  #    #     #    #  # #  #  ###
 *    #     #   #   #    #  #    #     #    #   ##  #    #
 *    #     #    #  #    #  #####      #    #    #   ####
 */

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: otradhead
 *
 * DESCRIPTION:
 * Function to print the trading post heading text.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void otradhead(void)
{
  print_header("Welcome to the Ularn Trading Post.  We buy items that explorers no longer find ");
  print_header("useful.  Since the condition of the items you bring in is not certain, ");
  print_header("and we incur great expense in reconditioning the items, we usually pay ");
  print_header("only 20% of their value were they to be new.  If the items are badly ");
  print_header("damaged, we will pay only 10% of their new value. ");
}


/* =============================================================================
 * FUNCTION: cnsitm
 *
 * DESCRIPTION:
 * Prints the 'can't sell unidentified items' message.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void cnsitm(void)
{
  Print("Sorry, we can't accept unidentified objects.");
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: item_value
 */
int item_value(int it, int itarg)
{
  int arg;
  int value;
  int found_item;
  int j;

  value = -1;

  if ((it == ODIAMOND) ||
      (it == ORUBY) ||
      (it == OEMERALD) ||
      (it == OSAPPHIRE))
  {
    value = (long) 20 * itarg;
  }
  else
  {
    found_item = 0;
    j = 0;
    while ((j < DNDSIZE) && (!found_item))
    {
      if (itm[j].obj == it)
      {
        if ((itm[j].obj == OSCROLL) || (itm[j].obj == OPOTION))
        {
          value = (long) 2 * itm[j + itarg].price;
        }
        else
        {
          value = (long) itm[j].price;

          arg = itarg;

          if (arg >= 0) value *= 2;

          while ((arg != 0) && (value < 500000L))
          {
            if (arg > 0)
            {
              /* appreciate if a +n object */
              value = (14*(value + 67))/10;
              arg--;
            }
            else
            {
              /* depreciate -n object */
              value = (value * 10) / 14;
              arg++;
            }
          }
        }

        /* always offer at least 1 gp */
        if (value == 0) value = 1;

        found_item = 1;
      }

      j++;
    }
  }

  return value;
}


/* =============================================================================
 * FUNCTION: otradepost
 */
void add_tradepost_options()
{
	char subtext[20];
	for (int idx = 0; idx < IVENSIZE; idx++)
	{
		int it = iven[idx];
		int itarg = ivenarg[idx];

		if (it == OSCROLL)
		{
			if (!scrollknown[itarg])
			{
				continue;
			}
		}
		else if (it == OPOTION)
		{
			if (!potionknown[itarg])
			{
				continue;
			}
		}
		else if ((it == OLANCE) && ramboflag)
		{
			continue;
		}

		if (it == ONOTHING)
		{
			continue;
		}

		long value = item_value(it, itarg);

		if (value >= 0)
		{
			sprintf(subtext, "%ld gp", value);
			add_option(idx + 'a', get_inventory_name3(idx), subtext);
		}
	}
	add_option(-1, "Leave", "");
}

void otradepost()
{
  dnditm = 0;
  otradhead();

  set_callback("sell_item");
  add_tradepost_options();
}

void sell_item(char ans)
{
    int no_sell_flag = 0;

    int idx = ans - 'a';

    int it = iven[idx];
    int itarg = ivenarg[idx];

    if (it == OSCROLL)
    {
		if (!scrollknown[itarg])
		{
			/* can't sell unidentified item */
			no_sell_flag = 1;
			cnsitm();
		}
    }
    else if (it == OPOTION)
    {
		if (!potionknown[itarg])
		{
			/* can't sell unidentified item */
			no_sell_flag = 1;
			cnsitm();
		}
    }
    else if ((it == OLANCE) && ramboflag)
    {
		no_sell_flag = 1;
		Print("You don't *really* want to sell that, now do you?");
    }

    if (!no_sell_flag)
    {
		if (it == ONOTHING)
		{
			Printf("You don't have item %c!", ans);
		}
		else
		{

			long value = item_value(it, itarg);

			if (value >= 0)
			{
				c[GOLD] += value;
				if (c[WEAR] == idx) c[WEAR] = -1;
				if (c[WIELD] == idx) c[WIELD] = -1;
				if (c[SHIELD] == idx) c[SHIELD] = -1;
				adjustcvalues(it, itarg);
				iven[idx] = ONOTHING;
				recalc();
			}
			else
			{
				/*
				* negative value indicates that the item is not in the price list
				*/
				Print("So sorry but we are not authorized to accept that item.");
			}
		}
	} /* if not no_sell_flag */

    otradepost();
}

/* =============================================================================
 *
 * #       ######   #####
 * #       #     # #     #
 * #       #     # #
 * #       ######   #####
 * #       #   #         #
 * #       #    #  #     #
 * ####### #     #  #####
 *
 */

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: olrs
 */
static void add_lrs_options()
{
  add_option('p', "Pay Taxes", "");
  add_option(-1, "Leave", "");
}
 
void olrs()
{
  if (outstanding_taxes)
  {
    if (cheat)
    {
      Print("Sorry but it seems you are trying to pay off your taxes by cheating!");
      return;

    }

  }

  print_header("Welcome to the Ularn Revenue Service district office.  How can we help you?");
  if (outstanding_taxes>0)
  {
    printf_header("You presently owe %d gp in taxes.  ",(long)outstanding_taxes);
  }
  else
  {
    print_header("You do not owe us any taxes.           ");
  }
  set_callback("olrs2");
  add_lrs_options();
}

void olrs2(int ans)
{
	if (ans == 'p')
	{
		print_header("How much? ");
		set_input_type("number");
		set_callback("pay_taxes");
	}
}

void pay_taxes(long amt)
{
    if (amt < 0)
    {
        amt = 0;
    }
    else if (amt > c[GOLD])
    {
        Print("You don't have that much.");
        amt = 0;
    }

    c[GOLD] -= paytaxes((long)amt);
        
    nap(500);
    
    olrs();
}


/* =============================================================================
 *
 * #     #         ######                            ###
 * ##   ##   ####  #     #   ####   #####   ######   ###     ####
 * # # # #  #    # #     #  #    #  #    #  #         #     #
 * #  #  #  #      #     #  #    #  #    #  #####    #       ####
 * #     #  #      #     #  #    #  #####   #                    #
 * #     #  #    # #     #  #    #  #       #               #    #
 * #     #   ####  ######    ####   #       ######           ####
 *
 */

/* =============================================================================
 * Local variables
 */

typedef enum DrugType
{
  DOPE_SPEED,
  DOPE_ACID,
  DOPE_HASH,
  DOPE_MUSHROOMS,
  DOPE_COKE,
  DOPE_COUNT
} DrugType;

struct DopeDataType
{
  char *Name;  /* The name of this dope */
  int   Price; /* The selling price */
  int   Line;  /* The display line for showing in MacDope's inventory*/
  ObjectIdType Item; /* The item id for the drug */
};

/*
 * Flags for what dope has been sold
 *  0 = Dealer has in stock
 *  1 = Sold out
 */
char drug[DOPE_COUNT] =
{ 0, 0, 0, 0, 0 };

static struct DopeDataType DopeData[DOPE_COUNT] =
{
  { "Killer Speed",    100,  8, OSPEED },
  { "Groovy Acid",     250,  9, OACID },
  { "Moster Hash",     500, 10, OHASH },
  { "Trippy Shrooms", 1000, 11, OSHROOMS },
  { "Cool Coke",      5000, 12, OCOKE }
};

/* =============================================================================
 * Local functions
 */

/* =============================================================================
 * FUNCTION: write_pad
 *
 * DESCRIPTION:
 * Function to write the data for McDope's pad to the save file.
 *
 * PARAMETERS:
 *
 *   fp : A pointer to the save file being written.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void write_pad(MemoryFile *fp)
{
  bwrite(fp, drug, DOPE_COUNT * sizeof(char));
}

/* =============================================================================
 * FUNCTION: read_pad
 *
 * DESCRIPTION:
 * Function to read the data for McDope's pad from the save file.
 *
 * PARAMETERS:
 *
 *   fp : a pointer to the save file being read.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void read_pad(MemoryFile *fp)
{
  bread(fp, drug, DOPE_COUNT * sizeof(char));
}

/* =============================================================================
 * FUNCTION: nomore
 *
 * DESCRIPTION:
 * Function to print the out of stock message for McDope's.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void nomore(void)
{
  Print("Sorry man, I ain't got no more of that stuff.");
  nap(2200);
}

/* =============================================================================
 * FUNCTION: nocash
 *
 * DESCRIPTION:
 * Function to print the insufficient funds message for McDope's.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void nocash(void)
{
  Print("Whattaya trying to pull on me? You aint got the cash!");
  nap(1200);
}

/* =============================================================================
 * FUNCTION: pad_hd
 *
 * DESCRIPTION:
 * Function to display the header info for Dealer McDope's pad
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void pad_hd(void)
{
  print_header("Hey man, welcome to Dealer McDope's Pad!  I gots the some of the finest dope ");
  print_header("you'll find anywhere in Ularn -- check it out... ");
}

/* =============================================================================
 * FUNCTION: snag
 *
 * DESCRIPTION:
 * Function to get something from McDope's.
 * Checks the inventory size to make sure the player can carry the item.
 *
 * PARAMETERS:
 *
 *   itm : The item being bought.
 *
 * RETURN VALUE:
 *
 *   0 => no free inventory slot for the item
 *   1 => the player could take the item
 */
static int snag(int itm)
{
  if (pocketfull())
  {
    Print("Hey, you can't carry any more.");
    return(0);
  }
  else 
  {
    Print("Ok, here ya go.");
    take(itm, 0);

    return(1);
  }

}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: opad
 */
void add_pad_options()
{
	char price[20];
	for (int Dope = 0; Dope < DOPE_COUNT; Dope++)
	{
		if (drug[Dope] == 0)
		{
			sprintf(price, "%d gp", DopeData[Dope].Price);
			add_option('a' + Dope, DopeData[Dope].Name, price);
		}
	}
	add_option(-1, "Leave", "");
}

void opad()
{
  pad_hd();

  set_callback("buy_dope");
  add_pad_options();
} /* end pad() */

void buy_dope(int i)
{
    int Dope = i - 'a';

    if (drug[Dope])
    {
		/* None of that dope left */
		nomore();
    }
    else
    {
		if (c[GOLD] < DopeData[Dope].Price)
		{
			/* Player can't afford this */
			nocash();
		}
		else if (snag(DopeData[Dope].Item))
		{
			/* Player can afford and has taken, so pay for it. */
			c[GOLD] -= DopeData[Dope].Price;

			/* Note tht this drug has been bought */
			drug[Dope]++;
			nap(1000);

		}

    }

    opad();
}



/* =============================================================================
 *
 * #     #
 * #     #   ####   #    #  ######
 * #     #  #    #  ##  ##  #
 * #######  #    #  # ## #  #####
 * #     #  #    #  #    #  #
 * #     #  #    #  #    #  #
 * #     #   ####   #    #  ######
 *
 */

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: ohome
 */
void ohome(void)
{
  int i;
  set_callback("ohome2");
  for (i=0; i<IVENSIZE; i++)
    /* remove the potion of cure dianthroritis from inventory */
    if (iven[i]==OPOTION)
      if (ivenarg[i]==PCUREDIANTH)
      {
        iven[i] = ONOTHING;

        print_header("Congratulations.  You found the potion of cure "
             "dianthroritis!");
        print_header("Frankly, No one thought you could do it.");
        print_header("Boy!  Did you surprise them!");
        nap(1000);
        if (gtime>TIMELIMIT) {
          print_header("However... the doctor has the sad duty to "
               "inform you that your daughter has ");
          print_header("died! You didn't make it in time.  In your agony, "
               "you kill the doctor, your ");
          if (sex == 1)
            print_header("wife");
          else
            print_header("husband");
          print_header(" and yourself!");
          nap(5000);
          add_option(DIED_KILLED_FAMILY, "Too bad...", "");
          return;
        }
        else
        {
          print_header("The doctor is now administering the potion and, "
               "in a few moments, ");
          print_header("your daughter should be well on her way to "
               "recovery.");
          nap(6000);
          print_header("The potion is.");
          nap(1000);
          print_header(".");
          nap(1000);
          print_header(".");
          nap(1000);
          print_header(" working!  The doctor thinks that ");
          print_header("your daughter will recover in a few days.");
          UlarnBeep();
          nap(5000);
          add_option(DIED_WINNER, "Congratulations!", "");
          return;
        }
      }

  print_header("Welcome home.");
  print_header("Latest word from the doctor is not good.");

  if (gtime > TIMELIMIT)
  {
    print_header("The doctor has the sad duty to inform you that your "
         "daughter has died!");
    print_header("You didn't make it in time.");
    print_header("In your agony, you kill the doctor, your ");
    if (sex == 1)
      print_header("wife");
    else
      print_header("husband");
    print_header(" and yourself!");
    nap(5000);
    add_option(DIED_KILLED_FAMILY, "Too bad...", "");
    return;
  }

  print_header("The diagnosis is confirmed as dianthroritis.  "
       "He guesses that ");
  printf_header("your daughter has only %d mobuls left in this world.",(long)((TIMELIMIT-gtime+99)/100));
  print_header("It's up to you to find the only hope for your daughter, the very rare ");
  print_header("potion of cure dianthroritis.  It is rumored that only deep "
       "in the ");
  print_header("depths of the caves can this potion be found. ");
  add_option(-1, "Leave", "");
}

void ohome2(int died_reason)
{
    died(died_reason, 0);
}


/* =============================================================================
 * FUNCTION: write_store
 */
void write_store(MemoryFile *fp)
{
  write_dnd_store(fp);
  write_college(fp);
  write_bank(fp);
  write_pad(fp);
}

/* =============================================================================
 * FUNCTION: read_store
 */
void read_store(MemoryFile *fp)
{
  read_dnd_store(fp);
  read_college(fp);
  read_bank(fp);
  read_pad(fp);
}

