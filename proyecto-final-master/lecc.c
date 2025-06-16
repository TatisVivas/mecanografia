#include "mecc.h"
#include "lecc.h"

void agregarCaracter(Caracter *c, char a){
	c->carac = a;
	c->tiempo = -1;
	c->error = false;
}

void agregarPalabra(const char pal[], size_t pos, size_t lPal, char cTerm,
		Caracter texto[][LEC_COL]){
		if (cTerm != '\0'){
			for (size_t i = 0; i < lPal; i++, pos++){
				agregarCaracter(&texto[pos / LEC_COL][pos % LEC_COL],
						pal[i]);
			}
		}

		agregarCaracter(&texto[pos / LEC_COL][pos % LEC_COL], cTerm);
}

void generarTexto(const char pals[][PAL_TAM], size_t cPals,
		Caracter texto[][LEC_COL]){
	size_t i = 0;

	while (i < LEC_FIL * LEC_COL){
		size_t rPal = rand() % cPals;
		size_t lPal = strlen(pals[rPal]);

		if ((i % LEC_COL) + lPal + 1 < LEC_COL - 1){
			agregarPalabra(pals[rPal], i, lPal, ' ', texto);
		}
		else if ((i % LEC_COL) + lPal + 1 >= LEC_COL - 1 &&
				i / LEC_COL < LEC_FIL - 1){
			i--;
			agregarPalabra(NULL, i, 0, '\n', texto);
			i = ((i / LEC_COL) + 1) * LEC_COL;
			agregarPalabra(pals[rPal], i, lPal, ' ', texto);
		}
		else{
			i--;
			agregarPalabra(NULL, i, 0, '\0', texto);
			i = LEC_FIL * LEC_COL;
		}

		i += lPal + 1;
	}
}

void cargarLeccion(Leccion *lec, char arch[]){
	FILE * fp = fopen(arch, "r");

	if (fp == NULL){
		perror("fopen");
		exit(EXIT_FAILURE);
	}


	unsigned cantPal;
	char palabras[PAL_MAX][PAL_TAM];
	char format[PAL_TAM];

	sprintf(format, "%%%ds", NOM_TAM - 1);

	if (fscanf(fp, format, &(lec->nom)) != 1){
		fprintf(stderr, "fscanf: error al leer nombre de lección.\n");

		fclose(fp);
		exit(EXIT_FAILURE);
	}

	if (fscanf(fp, "%u %u %u", &(lec->adjWPM), &(lec->penal),
				&cantPal) != 3){
		fprintf(stderr, "fscanf: error al leer adjWPM, penalización "
				"y cantidad de palabras.\n");

		fclose(fp);
		exit(EXIT_FAILURE);
	}

	if (cantPal > PAL_MAX){
		fprintf(stderr, "error: la lección excede el máximo de "
				"palabras.\n");

		fclose(fp);
		exit(EXIT_FAILURE);
	}

	sprintf(format, "%%%ds", PAL_TAM - 1);
	for (size_t i = 0; i < cantPal; i++){
		if (fscanf(fp, format, palabras[i]) != 1){
			fprintf(stderr, "fscanf: error al leer palabra "
					"%zd.\n", i +1);

			fclose(fp);
			exit(EXIT_FAILURE);
		}
	}

	fclose(fp);

	generarTexto((const char(*)[])palabras, cantPal, lec->text);
}

bool menuLeccion(Leccion *lec, WINDOW *campPtr, WINDOW *notfPtr, int pY,
		int pX){
	char arch[NOM_TAM];
	wclear(campPtr);
	wclear(notfPtr);

	mvwprintw(campPtr, pY, pX, "%s\n", " 0. Salir...");
	for (size_t i = 1; i <= LEC_MAX; i++){
		sprintf(arch, "lecciones/%02zd.txt", i);
		FILE *fp = fopen(arch, "r");

		if (fp == NULL){
			perror("fopen");
			fprintf(stderr, "Error al abrir lección: %s\n", arch);
			exit(EXIT_FAILURE);
		}

		char nomLec[NOM_TAM], format[NOM_TAM];
		sprintf(format, "%%%ds", NOM_TAM - 1);
		if (fscanf(fp, format, nomLec) != 1){
			perror("fscanf");
			fprintf(stderr, "Error al leer nombre de lección en: "
					"%s\n", arch);
			fclose(fp);
			exit(EXIT_FAILURE);
		}

		fclose(fp);
		mvwprintw(campPtr,pY + i, pX, "%2zd. %s\n", i, nomLec);
	}

	mvwprintw(notfPtr, 0, 0, "%s", "Selecciona lección: ");
	wnoutrefresh(campPtr);
	wnoutrefresh(notfPtr);
	doupdate();

	short unsigned numLec;

	while (wscanw(notfPtr,"%hu", &numLec) != 1 || numLec > LEC_MAX){
		wclear(notfPtr);
		wattron(notfPtr, A_REVERSE|COLOR_PAIR(PAR_ERR));
		mvwprintw(notfPtr, 1, 0, "%hu%s", numLec, " no se una opción "
				"válida, por favor intenta nuevamente...");
		wattroff(notfPtr, A_REVERSE|COLOR_PAIR(PAR_ERR));
		mvwprintw(notfPtr, 0, 0, "%s", "Selecciona lección: ");
		wnoutrefresh(notfPtr);
		doupdate();
	}

	if (numLec){
		sprintf(arch, "lecciones/%02hu.txt", numLec);
		cargarLeccion(lec, arch);
		return true;
	}
	return false;
}

unsigned imprimirLeccion(const Caracter texto[][LEC_COL], WINDOW *campPtr,
		int pY, int pX){
	wclear(campPtr);
	wmove(campPtr, pY, pX);
	unsigned contC = 0;

	wattrset(campPtr, A_REVERSE|COLOR_PAIR(0));

	for (size_t i = 0; i < LEC_FIL * LEC_COL; i++){
		char c = texto[i / LEC_COL][i % LEC_COL].carac;

		if (c == '\n'){
			i = (((i / LEC_COL) + 1) * LEC_COL) - 1;
			pY += 2;
			wmove(campPtr, pY, pX);
			contC++;
		}
		else if (c == '\0'){
			i = LEC_FIL * LEC_COL;
		}
		else {
			waddch(campPtr, c);
			contC++;
		}
	}

	wnoutrefresh(campPtr);
	doupdate();

	return contC;
}

bool practLeccion(Leccion *lec, WINDOW *campPtr, WINDOW *notfPtr, int pY,
		int pX, unsigned cantC){
	noecho();
	wmove(campPtr, ++pY, pX);

	double maxDT = 60.0 / (lec->adjWPM * 5.0);
	unsigned contE = 0;
	size_t i = 0;
	Caracter *cPtr = &(lec->text[i / LEC_COL][i % LEC_COL]);
	char cEn = wgetch(campPtr);

	if (cEn == 27){
		echo();
		return false;
	}
	else if(cEn == cPtr->carac){
		wattrset(campPtr, A_REVERSE|COLOR_PAIR(PAR_OK));
		mvwaddch(campPtr, pY, pX + (i % LEC_COL),  cEn);
	}
	else {
		wattrset(campPtr, A_REVERSE|COLOR_PAIR(PAR_ERR));
		if (cEn >= 32 && cEn <= 126){
			mvwaddch(campPtr, pY,
					pX + (i % LEC_COL),
					cEn);
		}
		else{
			mvwaddch(campPtr, pY,
					pX + (i % LEC_COL),
					' ');
		}

		contE++;
	}

	i++;

	struct timeval tInit;
	gettimeofday(&tInit, 0);

	while (i < LEC_FIL * LEC_COL){
		cPtr = &(lec->text[i / LEC_COL][i % LEC_COL]);

		if (cPtr->carac != '\0'){
			struct timeval dtInit, dtFin;
			gettimeofday(&dtInit, 0);

			cEn = wgetch(campPtr);

			gettimeofday(&dtFin, 0);

			cPtr->tiempo = dtFin.tv_sec - dtInit.tv_sec +
				((dtFin.tv_usec - dtInit.tv_usec) * 1e-6);

			if (cEn == cPtr->carac && cPtr->tiempo <= maxDT){
				wattrset(campPtr, A_REVERSE|COLOR_PAIR(PAR_OK));
				mvwaddch(campPtr, pY, pX + (i % LEC_COL), cEn);
			}
			else if (cEn == cPtr->carac && cPtr->tiempo > maxDT){
				wattrset(campPtr, A_REVERSE|COLOR_PAIR(PAR_WAR));
				mvwaddch(campPtr, pY, pX + (i % LEC_COL), cEn);
			}
			else if (cEn == 27){
				wattrset(campPtr, A_NORMAL|COLOR_PAIR(0));
				echo();
				return false;
			}
			else {
				cPtr->error = true;
				wattrset(campPtr, A_REVERSE|COLOR_PAIR(PAR_ERR));
				if (cEn >= 32 && cEn <= 126){
					mvwaddch(campPtr, pY,
							pX + (i % LEC_COL),
							cEn);
				}
				else{
					mvwaddch(campPtr, pY,
							pX + (i % LEC_COL),
							' ');
				}

				contE++;
			}

			if (cPtr->carac != '\n'){
				i++;
			}
			else{
				i = (((i / LEC_COL) + 1) * LEC_COL);
				pY += 2;
				wmove(campPtr, pY, pX);
			}
		}
		else {
			i = LEC_FIL * LEC_COL;
		}

		wnoutrefresh(campPtr);
		doupdate();
	}

	wattrset(campPtr, A_NORMAL|COLOR_PAIR(0));

	struct timeval tFin;
	gettimeofday(&tFin, 0);

	double durLec = tFin.tv_sec - tInit.tv_sec +
		((tFin.tv_usec - tInit.tv_usec) * 1e-6);
	unsigned wpm = (60 * cantC)/ (durLec * 5);
	float u = contE / (float) cantC;
	unsigned adjwpm = wpm * pow(1 - u, lec->penal);

	if (u <= 10 && adjwpm >= lec->adjWPM){
		wattrset(notfPtr, A_REVERSE|COLOR_PAIR(PAR_OK));
	}
	else {
		wattrset(notfPtr, A_REVERSE|COLOR_PAIR(PAR_ERR));
	}

	mvwprintw(notfPtr, 0, 0, "WPM:%4u\tU:%6.2f%%\t", wpm, u * 100);
	wprintw(notfPtr, "adjWPM:%4u", adjwpm);

	wattrset(notfPtr, A_NORMAL|COLOR_PAIR(0));
	mvwprintw(notfPtr, 1, 0, "%s", "Pulse cualquier tecla para "
			"continuar...");

	wnoutrefresh(notfPtr);
	doupdate();

	cEn = wgetch(campPtr);

	echo();
	return true;
}
