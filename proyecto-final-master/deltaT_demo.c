#include "mecc.h"

int main(void) {
	confTerm();

	int pX, pY;
	int notfTam = 2;
	getmaxyx(stdscr, pY, pX);
	WINDOW *campPtr = newwin(pY - notfTam, pX, 0, 0);
	WINDOW *notfPtr = newwin(notfTam, pX, pY - notfTam, 0); 

	demoDeltaT(campPtr, notfPtr);

	finTerm(campPtr, notfPtr);

	return 0;
}

void confTerm(void){
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();

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

void demoDeltaT(WINDOW *campPtr, WINDOW *notfPtr){
	struct timeval begin, end;
	char c;

	wattron(notfPtr, A_REVERSE);
	wprintw(notfPtr, "Escriba texto, pulse Esc para salir...");
	wattroff(notfPtr, A_REVERSE);

	wnoutrefresh(campPtr);
	wnoutrefresh(notfPtr);
	doupdate();

	do
	{
		gettimeofday(&begin, 0);
		c = wgetch(campPtr);
		gettimeofday(&end, 0);

		double deltaT = end.tv_sec - begin.tv_sec +
			((end.tv_usec - begin.tv_usec) * 1e-6);
		ParColor selColor;

		if (deltaT > 1.5)
		{
			selColor = PAR_ERR;
		}
		else if (deltaT > 0.8)
		{
			selColor = PAR_WAR;
		}
		else if (deltaT > 0.2)
		{
			selColor = 0;
		}
		else
		{
			selColor = PAR_OK;
		}

		wattron(campPtr, A_REVERSE|COLOR_PAIR(selColor));
		waddch(campPtr, c);
		wattroff(campPtr, A_REVERSE|COLOR_PAIR(selColor));

		wattron(notfPtr, A_REVERSE|COLOR_PAIR(selColor));
		mvwprintw(notfPtr, 1, 0, "Δt: %.3f", deltaT);
		wattroff(notfPtr, A_REVERSE|COLOR_PAIR(selColor));

		wnoutrefresh(campPtr);
		wnoutrefresh(notfPtr);
		doupdate();
	} while (c != 27);
}

void finTerm(WINDOW *campPtr, WINDOW *notfPtr){
	delwin(campPtr);
	delwin(notfPtr);
	endwin();
}
