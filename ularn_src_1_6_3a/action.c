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

void wield(int i) {
  int it = i - 'a';
  c[WIELD] = it;
  Printf("You wield %s", objectname[(int)iven[it]]);
  show_plusses(ivenarg[it]);
  Printc('.');
  recalc();
}

void wear(int i) {
  int it = i - 'a';
  switch (iven[it]) {
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
      if (c[WIELD] == it) {
        c[WIELD] = -1;
      }
      Printf("You put on your %s", objectname[(int)iven[it]]);
      show_plusses(ivenarg[it]);
      Printc('.');
      break;
    case OSHIELD:
      c[SHIELD] = it;
      if (c[WIELD] == it) {
        c[WIELD] = -1;
      }
      Print("You put on your shield");
      show_plusses(ivenarg[it]);
      Printc('.');
      break;
  }
  recalc();
}

void dropobj(int i) {
  if (c[TIMESTOP]) {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }
  if (i == '.') {
    char* p = &item[playerx][playery];
    if (*p != ONOTHING && *p != OPIT) {
      Print("There's something here already!");
      return;
    }
    print_header("How much gold do you drop?");
    set_input_type("number");
    set_callback("drop_gold");
  } else {
    drop_object(i - 'a');
  }
}

void drop_gold(long amt) {
  if (amt <= 0) {
    return;
  }
  if (amt > c[GOLD]) {
    Print("You don't have that much!");
    return;
  }
	int i;
  char p;
  if (amt <= 32767) {
    p = OGOLDPILE;
    i = (int)amt;
  } else if (amt <= 327670L) {
    p = ODGOLD;
    i = (int)(amt / 10);
    amt = 10L * i;
  } else if (amt <= 3276700L) {
    p = OMAXGOLD;
    i = (int)(amt / 100);
    amt = 100L * i;
  } else if (amt <= 32767000L) {
    p = OKGOLD;
    i = (int)(amt / 1000);
    amt = 1000L * i;
  } else {
    p = OKGOLD;
    i = (int)32767;
    amt = 32767000L;
  }
  c[GOLD] -= amt;
  Printf("You drop %d gold piece%s.", (long)amt, plural(amt));
  if (item[playerx][playery] == OPIT) {
    Print("The gold disappears down the pit.");
  } else {
    item[playerx][playery] = p;
    iarg[playerx][playery] = (short)i;
  }
  dropflag = 1;
}

void readscr(int i) {
  if (c[BLINDCOUNT]) {
    Print("You can't read anything when you're blind!");
    return;
  }
  if (c[TIMESTOP]) {
    Print("Nothing can be moved while time is stopped!");
    return;
  }
  if (iven[i - 'a'] == OSCROLL) {
    read_scroll(ivenarg[i - 'a']);
    iven[i - 'a'] = ONOTHING;
  } else if (iven[i - 'a'] == OBOOK) {
    readbook(ivenarg[i - 'a']);
    iven[i - 'a'] = ONOTHING;
  }
}

void eatcookie(int i) {
  if (c[TIMESTOP]) {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }
  Print("The cookie was delicious.");
  iven[i - 'a'] = ONOTHING;
  if (!c[BLINDCOUNT]) {
    Print("Inside you find a scrap of paper that says:");
    Print(fortune(fortfile));
  }
}

void quaff(int i) {
  if (c[TIMESTOP]) {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }
  quaffpotion(ivenarg[i - 'a']);
  iven[i - 'a'] = ONOTHING;
}

void closedoor() {
  if (c[TIMESTOP]) {
    Printf("Nothing can be moved while time is stopped!");
    return;
  }
  Print("The door closes.");
  forget();
  item[playerx][playery] = OCLOSEDDOOR;
  iarg[playerx][playery] = 0;
  dropflag = 1;
}

void takeoff(int i) {
	if (c[SHIELD] == i - 'a') {
		c[SHIELD] = -1;
		Print("Your shield is off.");
		recalc();
	} else if (c[WEAR] == i - 'a') {
		c[WEAR] = -1;
		Print("Your armor is off.");
		recalc();
	}
}

void unwield() {
	c[WIELD] = -1;
	Print("You unwield your weapon.");
	recalc();
}
