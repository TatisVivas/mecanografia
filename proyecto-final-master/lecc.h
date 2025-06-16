#ifndef LECC_H
#define LECC_H

#include <string.h>
#include <math.h>

#define PAL_TAM 16
#define NOM_TAM PAL_TAM * 3
#define PAL_MAX 64
#define LEC_MAX 13
#define LEC_FIL 5
#define LEC_COL 80

typedef struct {
	char carac;
	double tiempo;
	bool error;
} Caracter;

typedef struct {
	char nom[NOM_TAM];
	unsigned adjWPM;
	unsigned penal;
	Caracter text[LEC_FIL][LEC_COL];
} Leccion;

void agregarCaracter(Caracter *c, char a);
void agregarPalabra(const char pal[], size_t pos, size_t lPal, char cTerm,
		Caracter texto[][LEC_COL]);
void generarTexto(const char pals[][PAL_TAM], size_t cPals,
		Caracter texto[][LEC_COL]);
void cargarLeccion(Leccion *lec, char arch[]);
bool menuLeccion(Leccion *lec, WINDOW *campPtr, WINDOW *notfPtr, int pY,
		int pX);
unsigned imprimirLeccion(const Caracter texto[][LEC_COL], WINDOW *campPtr,
		int pY, int pX);
bool practLeccion(Leccion *lec, WINDOW *campPtr, WINDOW *notfPtr, int pY,
		int pX, unsigned cantC);

#endif
