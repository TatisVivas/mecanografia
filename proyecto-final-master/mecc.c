#include "mecc.h"
#include "lecc.h"

void confTerm(void){
	setlocale(LC_ALL, "");
	initscr();
	cbreak();

	if (has_colors() == FALSE)
	{
		endwin();
		puts("Tu terminal no soporta color");
		exit(EXIT_FAILURE);
	}

	start_color(); 
	init_pair(PAR_OK, COLOR_GREEN, COLOR_BLACK);
	init_pair(PAR_WAR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(PAR_ERR, COLOR_RED, COLOR_BLACK);
}

void finTerm(WINDOW *campPtr, WINDOW *notfPtr){
	delwin(campPtr);
	delwin(notfPtr);
	endwin();
}
