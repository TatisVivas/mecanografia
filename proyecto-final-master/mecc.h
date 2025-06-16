#ifndef MECC_H
#define MECC_H

#include <stdlib.h>
#include <time.h>
#include <locale.h>
#ifdef _WIN32
  #include "lib/curses.h"
#else
  #include <curses.h>
#endif
#include <sys/time.h>
#include <errno.h>

typedef enum {
        PAR_OK = 1,
        PAR_WAR,
        PAR_ERR,
} ParColor;

void confTerm(void);
void finTerm(WINDOW *campPtr, WINDOW *notfPtr);

#endif
