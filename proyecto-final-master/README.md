# Proyecto Final - Curso de Mecanografía

[[_TOC_]]

## Enunciado

Escribe en `C/C++` un programa que enseñe mecanografía[^mecano][^monkeyMec] a
sus usuarios. El programa debe ofrecer al menos `12` lecciones (3 lecciones por
cada fila de teclado) que debe leer desde archivos de texto con el siguiente
formato:

[^mecano]: TypingClub, «Aprende escritura al tacto gratis».
  https://www.typingclub.com/dactilografia, 2022. 

[^monkeyMec]: Miodec, «Monkeytype». https://monkeytype.com/, 2022.

```text
Fila_central_I
3 1 10
f
j
fj
jf
asdf
jkl
lkj
fdsa
hg
gh
```

La primera línea del archivo de texto contiene el nombre de la lección. La
siguiente línea muestra el mínimo de palabras por minuto ajustado $`AdjWPM`$
(`3`) requerido para aprobar la lección, el exponente de penalización $`a`$
(`1`) y la cantidad de palabras que contiene la lección (`10`)[^1]. A
continuación, cada línea de texto contiene una palabra **únicamente con
caracteres ASCII y sin espacios**.

[^1]: La cantidad de palabras no debe exceder el máximo de palabras por lección
  determinado en su programa.

El ejercicio ofrecido al usuario debe ser una _combinación aleatoria_ de las
palabras contenidas en el archivo de texto que define la lección sin sobrepasar
un máximo de filas y columnas definidos en su programa.

Usando las funcionalidades ofrecidas por la librería `curses`[^pdcurses] y la
medición de tiempo mediante `sys/time.h`[^sysTime] debe aplicar el siguiente
código de colores por cada carácter ingresado por el usuario mientras practica
una lección:

[^pdcurses]: W. McBrine, «PDCurses Documentation». https://pdcurses.org/docs/,
  2022.

[^sysTime]: T. O. Group, «The Single UNIX ® Specification, Version 2 -
  sys/time.h - time types».
  https://pubs.opengroup.org/onlinepubs/7908799/xsh/systime.h.html, 1997.

- Carácter errado: fondo rojo.
- Carácter lento[^2]: fondo amarillo.
- Carácter rápido: fondo verde.

[^2]: Se considera lento el carácter con $`\Delta t`$ inferior al esperado para
  alcanzar el $`WPM`$ exigido por la lección.

Al finalizar la lección, se debe reportar:

- Palabras por minuto ($`WPM`$):

   ```math
   WPM = \frac{|T|-1}{S}\times 60 \times \frac{1}{5}
   ```

   > $`T`$ is the final transcribed string entered by the subject, and $`|T|`$
   > is the length of this string... The $`S`$ term is seconds, measured from
   > the entry of the first character to the entry of the last, including
   > backspaces. The $`60`$ is seconds per minute and the $`1/5`$ is words per
   > character[^textMeasures].

- Proporción de errores ($`U`$):

   ```math
   $$U = \frac{E}{|T|}$$
   ```

   > $`U`$ is the uncorrected error rate ranging from 0.00 to 1.00,
   > inclusive[^textMeasures].

   _**Nota**: El programa no debe permitir al usuario borrar y/o reintentar el
   ingreso de un carácter._

- Palabras por minuto ajustado ($`AdjWPM`$):

   ```math
   AdjWPM = WPM\times (1 - U)^a
   ```

   >  The variable $`a`$ might be called the “penalty exponent” and is usually
   >  set to 1.0, but may be increased in order to more severely penalize WPM
   >  in light of uncorrected errors[^textMeasures].

- Estado de aprobación de la lección.

[^textMeasures]: J. O. Wobbrock, «Measures of Text Entry Performance», en Text
  Entry Systems, K. T.-I. I. Scott MacKenzie, Ed. San Francisco: Morgan
  Kaufmann, 2010, pp. 47-74.

# Datos

## Común: `mecc.h`

### Enumeración: `ParColor`

- `PAR_OK`: igual a `1`.
- `PAR_WAR`
- `PAR_ERR`

## Lección: `lecc.h`

### Constante Simbólica: `PAL_TAM`

Máxima cantidad de caracteres que puede tener una palabra: `32`

### Constante Simbólica: `NOM_TAM`

Máxima cantidad de caracteres que puede tener un nombre de lección: `PAL_TAM *
3`

### Constante Simbólica: `LEC_MAX`

Máxima cantidad de lecciones a cargar.

### Constante Simbólica: `PAL_MAX`

Máxima cantidad de palabras por lección a cargar: `64`

### Constante Simbólica: `LEC_FIL`

Cantidad de lineas de tiene una lección: `5`

### Constante Simbólica: `LEC_COL`

Cantidad de caracteres que tiene una línea de texto de una lección: `80`

### Estructura: `Caracter`

- `carac`: carácter exigido por la lección.
- `tiempo`: $`\Delta t`$ de intento (valor negativo representa _vacío_).
- `error`: `true` representa que el intento fue errado.

### Estructura: `Leccion`

- `nom`: arreglo de caracteres de tamaño `NOM_TAM`.
- `adjWPM`: mínimo de palabras por minuto ajustado para pasar la lección.
- `penal`: exponente de penalización (dificultad de lección).
- `text`: matriz con elementos de tipo `Caracter` de tamaño `TEC_FIL` por
  `TEC_COL`.

# Funciones

## Lección

### Función: `agregarCaracter`

Inicializa una estructura de tipo `Caracter` (referencia) a partir de carácter.

#### Entradas

- `c`: estructura de tipo `Caracter` (referencia).
- `a`: carácter`char` a agregar.

#### Salidas

Ninguna.

#### Pseudo-código
1. Asigno al miembro `carac` de `c` el valor de `a`.
1. Asigno al miembro `tiempo` de `c` el valor `-1`.
1. Asigno al miembro `error` de `c` el valor `false`.

### Función: `agregarPalabra`

Agrega palabra inicializando cada carácter a partir de la posición dada y
finalizando con espacio `' '`, cambio de línea `'\n\'` o final de texto `'\0'`.
Retorna la posición final del cursor.

#### Entradas

- `pal`: arreglo constante de tipo `char` de tamaño `PAL_TAM`.
- `pos`: posición inicial a insertar.
- `lPal`: cantidad de caracteres en palabra.
- `cTerm`: carácter con el cual terminar la palabra.
- `texto`: matriz de tipo `Caracter` con `LEC_COL` columnas.

#### Salidas

Ninguna.

#### Pseudo-código

1. Si `cTerm` es diferente de `\0` (carácter nulo):
   1. Para `i` igual a `0`; hasta `i` menor a `lPal`; incremento `i` y `pos`
      en `1`:
      1. Invoco `agregarCaracter()` para elemento en posición fila `pos /
         LEC_COL` y columna `pos % LEC_COL` de matriz `texto` y caracter en
         posición `i` de arreglo `pal`.
1. Invoco `agregarCaracter()` para elemento en posición fila `pos / LEC_COL` y
   columna `pos % LEC_COL` de matriz `texto` y carácter `cTerm`.


### Función: `generarTexto`

A partir de una matriz de palabras, genera texto que no exceda `LEC_FIL` y
`LEC_COL`.

#### Entradas

- `pals`: matriz constante de tipo `char` con `PAL_TAM` columnas.
- `cPals`: cantidad de palabras (filas).
- `texto`: matriz de tipo `Caracter` con `LEC_COL` columnas.

#### Salidas

Ninguna.

#### Pseudo-código

1. Declaro `i` y asigno `0`.
1. Mientras `i` sea menor a `LEC_FIL * LEC_COL`.
   1. Declaro `rPal` y asigno el módulo `cPals` de lo retornado por
      `rand()`.
   1. Declaro `lPal` y asigno el retorno de `strlen()` para la palabra en
      fila `rPal` de matriz `pals`.
   1. Si columna actual `i % LEC_COL` mas longitud de palabra `lPal + 1` es
      menor a `LEC_COL - 1`:
      1. Invoco `agregarPalabra()` para palabra en fila `rPal`, con
         longitud `lPal`, posición inicial `i` y terminación con `' '`
         (espacio).
   1. De lo contrario, si columna actual `i % LEC_COL` mas longitud de palabra
      `lPal + 1` es mayor o igual a `LEC_COL - 1` y fila `i / LEC_COL` es
      menor a última fila `LEC_FIL - 1`:
      1. Decremento `i` en `1` (sobrescribe último espacio).
      1. Invoco `agregarPalabra()` para palabra `NULL`, con longitud `0`,
         posición inicial `i` y terminación con `'\n'` (cambio de línea).
      1. Asigno a `i` a nueva línea `((i / LEC_COL) + 1) * LEC_COL`.
      1. Invoco `agregarPalabra()` para palabra en fila `rPal`, con
         longitud `lPal`, posición inicial `i` y terminación con `' '`
         (espacio).
   1. De lo contrario (fin de texto):
      1. Decremento `i` en `1` (sobrescribe último espacio).
      1. Invoco `agregarPalabra()` para palabra `NULL`, con longitud `0`,
         posición inicial `i` y terminación con `'\0'` (fin de texto).
      1. Asigno a `i` el valor de`LEC_FIL * LEC_COL`.
   1. Incremento a `i` `lPal + 1`.

### Función: `cargarLeccion`

Lee de un archivo de texto la lección.

#### Entradas

- `lec`: puntero a estructura de tipo `Leccion` (referencia).
- `arch`: arreglo con cadena de caracteres que representa nombre de archivo.

#### Salida

Ninguna.

#### Pseudo-código

1. Declaro apuntador `fp` de tipo `FILE` y asigno retorno de `fopen()` en modo
   solo lectura para archivo ubicado en ruta `arch`.
1. Si `fp` es igual a `NULL`:
   1. Imprimo error en pantalla.
   1. Finalizo ejecución de programa.
1. Declaro entero positivo `cantPal` (cantidad de palabras que contiene la
   lección).
1. Declaro matriz `palabras` de tipo `char` de tamaño `PAL_MAX` por `PAL_TAM`.
1. Declaro arreglo `format` de tipo `char` y tamaño `PAL_TAM`.
1. Invoco `sprintf()` para generar cadena de caracteres sobre arreglo `format`
   con formato a usar en `fscanf()` como una cadena de caracteres con `NOM_TAM
   - 1` elementos.
1. Si `fscanf()` para puntero a archivo `fp`, formato `format` y miembro `nom`
de estructura `lec` (tipo `Leccion`); retorna algo distinto a `1`:
   1. Imprimo error en pantalla.
   1. Cierro acceso a archivo `fp`.
   1. Finalizo ejecución de programa.
1. Si `fscanf()` para puntero a archivo `fp`, formato para obtener tres números
sin signo, los miembros `adjWPM` y `penal` de estructura `lec` (tipo `Leccion`)
y `cantPal`; retorna algo distinto a `3`:
   1. Imprimo error en pantalla.
   1. Cierro acceso a archivo `fp`.
   1. Finalizo ejecución de programa.
1. Si cantidad de palabras `cantPal` es mayor a máximo de palabras `PAL_MAX`:
   1. Imprimo error en pantalla.
   1. Cierro acceso a archivo `fp`.
   1. Finalizo ejecución de programa.
1. Invoco `sprintf()` para generar cadena de caracteres sobre arreglo `format`
con formato a usar en `fscanf()` como una cadena de caracteres con `PAL_TAM -
1` elementos.
1. Para `i` igual a `0`; hasta `i < cantPal`; incremento `i` en `1`:
   1. Si `fscanf()` para puntero a archivo `fp`, formato `format` para obtener
   una cadena de caracteres y puntero a la fila `i` de matriz `palabras`;
   retorna algo distinto a `1`:
      1. Imprimo error en pantalla.
      1. Cierro acceso a archivo `fp`.
      1. Finalizo ejecución de programa.
1. Invoca `generarTexto()`.

### Función: `menuLeccion`

Muestra en pantalla lecciones disponibles e invoca `cargarLeccion` para lección
seleccionada.

#### Entradas

- `lec`: estructura de tipo `Leccion` (referencia).
- `campPtr`: estructura de tipo `WINDOW` (referencia) con ventada de campo.
- `notf`: estructura de tipo `WINDOW` (referencia) con ventada de notificación.
- `pY`: posición vertical inicial de cursor.
- `pX`: posición horizontal inicial de cursor.

#### Salida

- `false` si usuario decide finalizar programa, de lo contrario `true`.

#### Pseudo-código

1. Declaro arreglo de caracteres `arch` de tamaño `NOM_TAM`.
1. Limpio contenido de pantalla.
1. Imprimo en pantalla `"0. Salir"` (ventana de campo) en posición `pY` y `pX`.
1. Para `i` igual a `1`; hasta `i` menor igual a `LEC_MAX`; incremento `i` en
   `1`:
   1. Invoco `sprintf()` para generar cadena de caracteres sobre arreglo `arch`
      con ruta a archivo de texto que contiene lección número `i`.
   1. Declaro apuntador `fp` de tipo `FILE` y asigno retorno de `fopen()` en
      modo solo lectura para archivo ubicado en ruta `arch`.
   1. Si `fp` es igual a `NULL`:
      1. Imprimo error en pantalla.
      1. Finalizo ejecución de programa.
   1. Declaro arreglo de caracteres `nomLec` de tamaño `NOM_TAM`.
   1. Declaro arreglo de caracteres `format` de tamaño `NOM_TAM`.
   1. Invoco `sprintf()` para generar cadena de caracteres sobre arreglo
      `format` con formato a usar en `fscanf()` como una cadena de carateres
      con `NOM_TAM - 1` elementos.
   1. Si `fscanf()` para puntero a archivo `fp`, formato `format` y  arreglo de
      caracteres `nomLec`; retorna algo distinto a `1`:
      1. Imprimo error en pantalla.
      1. Cierro acceso a archivo `fp`.
      1. Finalizo ejecución de programa.
   1. Cierro acceso a archivo `fp`.
   1. Imprimo en pantalla `i + 1`, `nomLec` y un cambio de línea (ventana de
      campo) en posición `pY + i` y `pX`.
1. Pregunto a usuario por número de lección (ventana de notificaciones).
1. Actualizo contenido de pantalla.
1. Declaro entero positivo `numLec`.
1. Mientras retorno de `wscanfw()` sobre `numLec` sea diferente a `1` o valor
   de `numLec` mayor a `LEC_MAX`:
   1. Imprimo en pantalla error y rango esperado (ventana de notificaciones).
1. Si `numLec` en diferente de `0`:
   1. Invoco `sprintf()` para generar cadena de caracteres sobre arreglo `arch`
      con ruta a archivo de texto que contiene lección número `numLec`.
   1. Invoco `cargarLeccion` para `lec` y `arch`.
   1. Retorno `true`.
1. Retorno `false`.


### Función: `imprimirLeccion`

Imprime en lineas impares del texto guía de la lección.

#### Entradas

- `texto`: matriz constante de tipo `Caracter` de tamaño `LEC_FIL` por
  `LEC_COL`.
- `campPtr`: estructura de tipo `WINDOW` (referencia) con ventada de campo.
- `pY`: posición vertical inicial de cursor.
- `pX`: posición horizontal inicial de cursor.

#### Salida

Cantidad de caracteres que contiene la lección impresa.

#### Pseudo-código

1. Limpio ventana de campo.
1. Invoco `wmove()` para la ventana `campPtr` y posición `pY` y `pX`.
1. Declaro `contC` para contar cantidad de caracteres impresos y asigno `0`.
1. Para `i` igual a `0`; hasta `i` menor a `LEC_FIL * LEC_COL`; incremento `i`
   en `1`:
   1. Declaro `c` y asigno carácter en `texto`, fila `i / LEC_COL` y columna `i
      % LEC_COL`.
   1. Si `c` es igual a `'\n'`:
      1. Asigno a `i` posición de primer elemento de siguiente fila `-1`.
      1. Incremento `pY` en `2`.
      1. Invoco `wmove()` para la ventana `campPtr` y posición `pY` y `pX`.
      1. Incremento `contC` en `1`.
   1. De lo contrario, si `c` es igual a `'\0'`:
      1. Asigno a `i` el resultado `LEC_FIL * LEC_COL`.
   1. De lo contrario:
      1. Imprimir `c`.
      1. Incremento `contC` en `1`.
1. Actualizo ventana `campPtr`.
1. Retorno `contC`.

### Función: `practLeccion`

Obtiene entrada de teclado, inserta datos en estructura, analiza carácter
ingresado e imprime en pantalla código de colores.

#### Entradas

- `lec`: estructura de tipo `Leccion` con caracteres inicializados.
- `campPtr`: estructura de tipo `WINDOW` (referencia) con ventada de campo.
- `notfPtr`: estructura de tipo `WINDOW` (referencia) con ventada de notificación.
- `pY`: posición vertical inicial de cursor.
- `pX`: posición horizontal inicial de cursor.
- `cantC`: cantidad de caracteres que contiene la lección.

#### Salida

`true` si lección finaliza correctamente, `false` si usuario oprime `ESC` para
salir de la lección anticipadamente.

#### Pseudo-código

1. Activo modo `noecho()` de terminal.
1. Muevo cursor a posición `++pY` y `pX`.
1. Declaro `maxDT` como límite de tiempo de carácter lento y asigno `(60 *
   cantC) / (lec->adjWPM * 5)`.
1. Declaro contador `contE` para contar errores y asigno `0`.
1. Declaro `i` para posición en texto de lección y asigno `0`.
1. Declaro `cPtr` y asigno apuntador a carácter en `texto`, fila `i / LEC_COL`
   y columna `i % LEC_COL`.
1. Declaro `cEn` para almacenar carácter ingresado por usuario.
1. Obtengo carácter y asigno a `cEn`.
1. Si `cEn` es igual a `27`:
   1. Retorno `false`.
1. De lo contrario, si `cEn` es igual a `cPtr->carac`:
   1. Imprimo `cEn` en modo ok (verde).
1. De lo contrario:
   1. Imprimo `cEn` en modo error (rojo).
1. Incremento `i` en `1`.
1. Declaro estructura de tiempo `tInit` y asigno tiempo actual.
1. Mientras `i` es menor a `LEC_FIL * LEC_COL`:
   1. Asigno a `cPtr` apuntador a carácter en `texto`, fila `i / LEC_COL` y
      columna `i % LEC_COL`.
   1. Si `cPtr->carac` en diferente de `\0`:
      1. Declaro estructuras de tiempo `dtInit` y `dtFin`.
      1. Asigno tiempo actual a `dtInit`.
      1. Obtengo carácter y asigno a `cEn`.
      1. Asigno tiempo actual a `dtFin`.
      1. Asigno $`\Delta t`$ de ingreso de caracter a miembro `tiempo` de
	 `cPtr`.
      1. Si `cEn` es igual a `cPtr->carac` y `cPtr->tiempo` es menor o igual a
	 `maxDT`:
	 1. Imprimo `cEn` en modo ok (verde).
      1. De lo contrario, si `cEn` es igual a `cPtr->carac` y `cPtr->tiempo` es
	 mayor a `maxDT`:
	 1. Imprimo `cEn` en modo warn (amarillo).
      1. De lo contrario, si `cEn` es igual a `27`:
         1. Retorno `false`.
      1. De lo contrario:
	 1. Asigno `true` a miembro `error` de `cPtr`.
	 1. Imprimo `cEn` en modo error (rojo).
	 1. Incremento `contE` en `1`.
      1. Si `cPtr->carac` es diferente de `'\n'`
         1. Incremento `i` en `1`.
      1. De lo contrario, :
         1. Asigno a `i` posición de primer elemento de siguiente fila.
         1. Incremento `pY` en `2`.
         1. Invoco `wmove()` para la ventana `campPtr` y posición `pY` y `pX`.
   1. De lo contrario:
      1. Asigno a `i` el resultado `LEC_FIL * LEC_COL`.
   1. Actualizo ventana `campPtr`.
1. Declaro estructura de tiempo `tFin` y asigno tiempo actual.
1. Declaro `durLec` y asigno duración total en segundos que tomó la lección.
1. Declaro `wpm` y asigno `(60 * cantC)/ (durLec * 5)`.
1. Declaro `u` y asigno `contE / cantC`.
1. Declaro `adjwpm` y asigno `wpm * pow(1 - u, lec->penal)`.
1. Imprimo en ventana `notfPtr` valores de `wpm` y `u`.
1. Si `adjwpm` es mayor o igual a `lec-adjWPM`:
   1. Imprimo en ventana `notfPtr` valor de `adjwpm` en verde.
1. De lo contrario:
   1. Imprimo en ventana `notfPtr` valor de `adjwpm` en rojo.
1. Imprimo en ventana `notfPtr`: "Pulse cualquier tecla para continuar..."
1. Actualizo ventana `notfPtr`.
1. Obtengo carácter y asigno a `cEn`.
1. Revierto modo `echo()` de terminal.
1. Retorno `true`.

# Referencias
