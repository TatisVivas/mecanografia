#include "minunit/minunit.h"
#include "../mecc.c"
#include "../lecc.c"

MU_TEST(test_agregarCaracter) {
	puts("\nPrueba agregarCaracter():");

	Caracter c;
	char a = 'x';

	agregarCaracter(&c, a);

	mu_check(c.carac == a);
	mu_check(c.tiempo == -1);
	mu_check(c.error == false);
}

MU_TEST(test_agregarPalabra) {
	puts("\nPrueba agregarPalabra():");

	Caracter text[LEC_FIL][LEC_COL];
	const char pal[PAL_TAM] = "prueba";
	char cTerm = ' ';

	agregarPalabra(pal, 0, 6, cTerm, text);

	for (size_t i = 0; i < 6; i++){
		mu_check(text[0][i].carac == pal[i]);
		mu_check(text[0][i].tiempo == -1);
		mu_check(text[0][i].error == false);
	}

	mu_check(text[0][6].carac == cTerm);
	mu_check(text[0][6].tiempo == -1);
	mu_check(text[0][6].error == false);

	cTerm = '\n';
	agregarPalabra(pal, 7, 6, cTerm, text);

	for (size_t i = 0; i < 6; i++){
		mu_check(text[0][i + 7].carac == pal[i]);
		mu_check(text[0][i + 7].tiempo == -1);
		mu_check(text[0][i + 7].error == false);
	}

	mu_check(text[0][13].carac == cTerm);
	mu_check(text[0][13].tiempo == -1);
	mu_check(text[0][13].error == false);


	cTerm = '\0';
	agregarPalabra(NULL, 14, 0, cTerm, text);

	mu_check(text[0][14].carac == cTerm);
	mu_check(text[0][14].tiempo == -1);
	mu_check(text[0][14].error == false);


}

MU_TEST(test_generarTexto){
	puts("\nPrueba generarTexto():");

	size_t cPals = 4;
	const char pals[PAL_MAX][PAL_TAM] = {
		"cero",
		"uno",
		"dos",
		"tres"
	};
	Caracter text[LEC_FIL][LEC_COL];

	srand(1024);
	generarTexto(pals, cPals, text);

	srand(1024);
	size_t rPal = rand() % cPals;
	size_t i = 0;

	puts("\nPárrafo generado:");
	while ( i < LEC_FIL * LEC_COL){

		if (text[i / LEC_COL][i % LEC_COL].carac == ' '){
			i++;
		}
		else if (text[i / LEC_COL][i % LEC_COL].carac == '\n'){
			putchar('\n');
			i = ((i / LEC_COL) + 1) * LEC_COL;
		}
		else if (text[i / LEC_COL][i % LEC_COL].carac == '\0'){
			i = LEC_FIL * LEC_COL;
		}
		else {
			size_t lPal = strlen(pals[rPal]), j;
			char tPal[PAL_TAM];

			for(j = 0; j < lPal; j++, i++){
				putchar(text[i / LEC_COL][i % LEC_COL].carac);
				tPal[j] = text[i / LEC_COL][i % LEC_COL].carac; 
			}
			tPal[j] = '\0'; 

			mu_assert_string_eq( pals[rPal], tPal);
			rPal = rand() % cPals;
		}
	}
}

MU_TEST(test_cargarLeccion) {
	puts("\nPrueba cargarLeccion():");

	Leccion lec;

	cargarLeccion(&lec, "lecciones/01.txt");

	mu_assert_string_eq("Fila_central_I", lec.nom);
	mu_check(lec.adjWPM == 3);
	mu_check(lec.penal == 1);
}

MU_TEST(test_menuLeccion){
	puts("\nPrueba menuLección():");

	confTerm();
	Leccion lec;

	int pX, pY;
	int notfTam = 2;
	getmaxyx(stdscr, pY, pX);
	WINDOW *campPtr = newwin(pY - notfTam, pX, 0, 0);
	WINDOW *notfPtr = newwin(notfTam, pX, pY - notfTam, 0);

	mu_check(menuLeccion(&lec, campPtr, notfPtr, pY * .05,
				(pX - NOM_TAM + 4) / 2) == true);
	mu_assert_string_eq("Fila_central_I", lec.nom);
	mu_check(lec.adjWPM == 3);
	mu_check(lec.penal == 1);

	mu_check(menuLeccion(&lec, campPtr, notfPtr, pY * .05,
				(pX - NOM_TAM + 4) / 2) == false);

	finTerm(campPtr, notfPtr);
}

MU_TEST(test_imprimirLeccion){
	puts("\nPrueba imprimirLeccion():");

	confTerm();

	size_t cPals = 4;
	const char pals[PAL_MAX][PAL_TAM] = {
		"cero",
		"uno",
		"dos",
		"tres"
	};
	Caracter text[LEC_FIL][LEC_COL];

	int pX, pY;
	int notfTam = 2;
	getmaxyx(stdscr, pY, pX);
	WINDOW *campPtr = newwin(pY - notfTam, pX, 0, 0);
	WINDOW *notfPtr = newwin(notfTam, pX, pY - notfTam, 0);

	srand(1024);
	generarTexto(pals, cPals, text);

	pY *= .05;
	pX = (pX - LEC_COL) / 2;

	unsigned cantC = imprimirLeccion((const Caracter (*)[])text, campPtr,
			pY, pX);

	size_t i = 0;
	unsigned contC = 0;

	while ( i < LEC_FIL * LEC_COL){
		char c = text[i / LEC_COL][i % LEC_COL].carac;
		if (c == '\n'){
			i = ((i / LEC_COL) + 1) * LEC_COL;
			pY += 2;
			contC++;
		}
		else if (c == '\0'){
			i = LEC_FIL * LEC_COL;
		}
		else {
			char e = mvwinch(campPtr, pY, pX + (i % LEC_COL))
				& A_CHARTEXT;
			mu_check(c == e);
			i++;
			contC++;
		}
	}

	mvwprintw(notfPtr, 0, 0, "%s", "Pulsa enter para continuar...");
	wgetch(notfPtr);

	wnoutrefresh(notfPtr);
	doupdate();

	finTerm(campPtr, notfPtr);

	printf("\n%s%u\n", "Cantidad de caracteres:", cantC);
	mu_check(cantC == contC);
}

MU_TEST(test_practLeccion){
	puts("\nPrueba practLeccion():");

	confTerm();

	size_t cPals = 1;
	const char pals[PAL_MAX][PAL_TAM] = {
		"h"
	};
	Leccion lec;
	lec.adjWPM = 120;
	lec.penal = 3;

	int pX, pY;
	int notfTam = 2;
	getmaxyx(stdscr, pY, pX);
	WINDOW *campPtr = newwin(pY - notfTam, pX, 0, 0);
	WINDOW *notfPtr = newwin(notfTam, pX, pY - notfTam, 0);

	srand(1024);
	generarTexto(pals, cPals, lec.text);

	pY *= .05;
	pX = (pX - LEC_COL) / 2;

	unsigned cantC = imprimirLeccion((const Caracter (*)[])lec.text,
			campPtr, pY, pX);

	practLeccion(&lec, campPtr, notfPtr, pY, pX, cantC);

	finTerm(campPtr, notfPtr);

}

MU_TEST_SUITE(test_caracter_suite) {
	puts("\nPruebas Carater:");
	MU_RUN_TEST(test_agregarCaracter);
	MU_RUN_TEST(test_agregarPalabra);
}

MU_TEST_SUITE(test_leccion_suite) {
	puts("\nPruebas Leccion:");
	MU_RUN_TEST(test_cargarLeccion);
	MU_RUN_TEST(test_generarTexto);
	MU_RUN_TEST(test_menuLeccion);
	MU_RUN_TEST(test_imprimirLeccion);
	MU_RUN_TEST(test_practLeccion);
}

int main(void) {
	MU_RUN_SUITE(test_caracter_suite);
	MU_RUN_SUITE(test_leccion_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
