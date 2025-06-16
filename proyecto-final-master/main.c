#include "mecc.h"
#include "lecc.h"

int main(void){
	confTerm();
	Leccion lec;

	int pX, pY;
	int notfTam = 2;
	getmaxyx(stdscr, pY, pX);
	WINDOW *campPtr = newwin(pY - notfTam, pX, 0, 0);
	WINDOW *notfPtr = newwin(notfTam, pX, pY - notfTam, 0);

	pY *= .05;

	while (menuLeccion(&lec, campPtr, notfPtr, pY, (pX - NOM_TAM + 4) / 2)){
		srand(time(NULL));

		unsigned cantC = imprimirLeccion((const Caracter (*)[])lec.text,
				campPtr, pY, (pX - LEC_COL) / 2);
		practLeccion(&lec, campPtr, notfPtr, pY, (pX - LEC_COL) / 2,
				cantC);
	}

	finTerm(campPtr, notfPtr);

	exit(EXIT_SUCCESS);
}
