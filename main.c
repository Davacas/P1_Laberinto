/*
Programa que encuentra los caminos a la meta de un laberinto usando una búsqueda por árbol DFS.
Hecho por Castillo Montes Alan y Vargas Castro Daniel 
para la clase de Algoritmos y Estructuras de Datos de la FI de la UNAM en marzo de 2016.
*/
#include <stdio.h>
#include <stdlib.h>
//DECLARACIÓN DE VARIABLES GLOBALES
typedef struct puntosVisitados {
//Estructura que contendrá las coordenadas de los puntos visitados
	int coordX;
	int coordY;
} PuntoVisitado;
int indiceVisitado = 0; //Variable que recorrerá los índices del arreglo de puntos visitados

typedef struct puntosCamino {
//Estructura que contendrá las coordenadas del camino a la meta
	int coordX;
	int coordY;
} PuntoActual;
int indicePActual = 0; 	//Variable que recorrerá los índices del arreglo con el camino a la meta

FILE *archivoLaberinto = NULL;	//Apuntador a archivo que contiene el laberinto
FILE *archivoCaminos = NULL;	//Apuntador a archivo que almacenará los caminos
FILE *archivoCaminoCorto = NULL;//Apuntador a archivo que almacenará el camino más corto
PuntoActual *camino = NULL;		//Apuntador a estructura con coordenadas del camino
PuntoVisitado *visitados = NULL;//Apuntador a estructura con coordenadas de puntos visitados
char **laberintoEnMatriz = NULL;//Apuntador de apuntadores donde se almacenará el laberinto leído del archivo
int renglones = 0; 				//Para conocer las dimensiones de la matriz con el laberinto
int columnas = 0; 				//Para conocer las dimensiones de la matriz con el laberinto
char posicionEnArchivo = 0; 	//Variable que recorrá caracter por caracter un archivo durante su lectura
int i = 0, j = 0; 				//Contadores para usos múltiples, principalmente lectura y escritura de arreglos
int todoRevisado = 0;			//Variable que indicará si ya se tomaron todos los caminos posibles
int totalDePuntos = 0;			//Variable que contará la cantidad de puntos que no son pared del laberinto
int NORTEVisitado = 0;			//Variable que indica si se ha visitado el punto al NORTE del actual o no
int ESTEVisitado = 0;			//Variable que indica si se ha visitado el punto al ESTE del actual o no
int SURVisitado = 0;			//Variable que indica si se ha visitado el punto al SUR del actual o no
int OESTEVisitado = 0;			//Variable que indica si se ha visitado el punto al OESTE del actual o no
int caminoMasCorto = 2147483647;//Variable que almacena el tamaño del camino más corto encontrado
int renglonCMC = 0;				//Variable que ubica en qué renglon del archivo de caminos está el camino más corto
int comparaRenglonCMC = 0;		//Variable contra la que se compara el renglón
int contaReasignaVI = 0;		//Variable que cuenta las veces que se ha vuelto a asignar el punto inicial para eitar loops infinitos

//PROTOTIPOS DE FUNCIONES
void imprimeLaberinto();
void caminoNORTE();
void caminoESTE();
void caminoSUR();
void caminoOESTE();
void agregarVisitado();
void asignaCaminoMasCorto();

int main(int argc, char *argv[]) {
//VERIFICACIÓN DE ARGUMENTOS PASADOS POR CONSOLA
    if (argc < 2) {
       	printf ("Por favor, ingrese el nombre del archivo.\n");
       	exit (0);
    }
    else if (argc < 4) {
  	 	printf ("Ingrese las coordenadas correctamente.\n");
      	exit (0);
    }
    if (argc > 5) {
        printf("Se pasaron demasiados argumentos.\n");
        exit (0);
    }
 
//SE CARGA EL ARCHIVO CON EL LABERINTO AL PROGRAMA

	archivoLaberinto = fopen(argv[1], "r"); //El nombre del archivo se almacena como el argumento 1 pasado por consola
	if (archivoLaberinto == NULL) {
		printf("No se pudo cargar el archivo. El programa se cerrará.\n");
		exit(0);
	}

//CUENTA RENGLONES Y COLUMNAS DEL ARCHIVO CON EL LABERINTO PARA DESPUÉS ASIGNAR UNA MATRIZ DINÁMICA DE ESE TAMAÑO
	posicionEnArchivo = fgetc(archivoLaberinto);
	while (feof(archivoLaberinto) == 0) {
		if (posicionEnArchivo == '0' || posicionEnArchivo == '1')
			columnas ++;
		else if (posicionEnArchivo == '\n') {
			renglones ++;
			columnas = 0;
		}
		posicionEnArchivo = fgetc(archivoLaberinto);
	}
	renglones ++; //Para compensar por el renglón que no lee por el salto de línea
	rewind(archivoLaberinto);

//CREACIÓN DE MATRIZ DINÁMICA CON LAS DIMENSIONES OBTENIDAS ANTERIORMENTE
	laberintoEnMatriz = (char**)malloc(renglones*sizeof(char*));
	if(laberintoEnMatriz == NULL) { 
    	printf("No se pudo reservar la memoria. El programa se cerrará.\n");
    	exit(0);
    }
	for (i = 0; i < renglones; i++)	{
    	laberintoEnMatriz[i] = (char*)malloc(columnas*sizeof(char));
    	if (laberintoEnMatriz[i] == NULL) {
			printf("No se pudo reservar la memoria. El programa se cerrará.\n");
	  		exit(0);
  		}
	}
	renglones = 0;
	columnas = 0;

//COPIA DE LABERINTO EN ARCHIVO A UNA MATRIZ
    while (feof(archivoLaberinto) == 0)	{
		posicionEnArchivo = fgetc(archivoLaberinto);
		if (posicionEnArchivo == '0' || posicionEnArchivo == '1') {
			laberintoEnMatriz[renglones][columnas] = posicionEnArchivo;
			columnas ++;
			if (posicionEnArchivo == '0')
				totalDePuntos ++;
		}
		else if (posicionEnArchivo == '\n') {
			renglones ++;
			columnas = 0;
		}
		else if (posicionEnArchivo == -1)
			renglones ++;
	} 
	fclose(archivoLaberinto);

//CREACIÓN DE ARREGLOS QUE CONTENDRÁN EL CAMINO Y LOS PUNTOS VISITADOS
	visitados = (PuntoVisitado*) malloc((indiceVisitado+1)*sizeof(PuntoVisitado));
	if (visitados == NULL) {
		printf("No se pudo reservar la memoria. El programa se cerrará.\n");
		exit(0);
	}
	camino = (PuntoActual*) malloc((indicePActual+1)*sizeof(PuntoActual));
	if (visitados == NULL) {
		printf("No se pudo reservar la memoria. El programa se cerrará.\n");
		exit(0);
	}

//VERIFICACIÓN DE QUE EXISTA EL PUNTO INICIAL INGRESADO. LO ASIGNA SI EXISTE
	camino[0].coordX = atoi(argv[2]); //La coordenada X del punto inicial se toma del segundo argumento del programa convertido a int
	camino[0].coordY = atoi(argv[3]); //La coordenada Y del punto inicial se toma del tercer argumento del programa convertido a int
	visitados[0].coordX = camino[0].coordX; //Se marca como visitado el punto inicial
	visitados[0].coordY = camino[0].coordY;

	if (laberintoEnMatriz[camino[0].coordX][camino[0].coordY] != '0') {
	//Si el punto ingresado es pared o se sale del arreglo, se cierra el programa
		printf("No existe este punto en el laberinto. El programa se cerrará.\n");
		exit(0);
	}
	printf("Inicio: %d, %d\n", camino[0].coordX, camino[0].coordY);
	printf("Se agregó (%d, %d) al arreglo de visitados. Se han visitado %d de %d puntos. \n", visitados[indiceVisitado].coordX, visitados[indiceVisitado].coordY, indiceVisitado+1, totalDePuntos);
	imprimeLaberinto();
	getchar();

//SE CREA UN ARCHIVO PARA ALMACENAR LOS CAMINOS A LA META
	archivoCaminos = fopen("Caminos.txt", "w+");
	if (archivoCaminos == NULL)	{
		printf("No se pudo abrir un archivo. El programa se cerrará.\n");
		exit(0);
	}

//VERIFICACIÓN DE SI EL PUNTO INICIAL ES UNA META
	if (camino[0].coordX-1 < 0) {
		printf("El punto inicial es una meta. Se relocalizará el punto inicial.\n"); 	//Se encontró una nueva meta
		fprintf(archivoCaminos, "(%d, %d) ", camino[0].coordX, camino[0].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		camino[0].coordX = atoi(argv[2])+1; //La coordenada X del punto inicial se toma del segundo argumento del programa convertido a int
		camino[0].coordY = atoi(argv[3]);
		agregarVisitado();
		imprimeLaberinto();
		asignaCaminoMasCorto();
		getchar();
	}

	else if (camino[0].coordY+1 > (columnas-1)) {//Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("El punto inicial es una meta. Se relocalizará el punto inicial.\n");
		fprintf(archivoCaminos, "(%d, %d) ", camino[0].coordX, camino[0].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		camino[0].coordX = atoi(argv[2]); //La coordenada X del punto inicial se toma del segundo argumento del programa convertido a int
		camino[0].coordY = atoi(argv[3])-1;
		agregarVisitado();
		imprimeLaberinto();
		asignaCaminoMasCorto();
		getchar();
	}

	else if (camino[0].coordX+1 > (renglones-1)) { //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("El punto inicial es una meta. Se relocalizará el punto inicial.\n");
		fprintf(archivoCaminos, "(%d, %d) ", camino[0].coordX, camino[0].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		camino[0].coordX = atoi(argv[2])-1; //La coordenada X del punto inicial se toma del segundo argumento del programa convertido a int
		camino[0].coordY = atoi(argv[3]);
		agregarVisitado();
		imprimeLaberinto();
		asignaCaminoMasCorto();
		getchar();
	}

	else if (camino[indicePActual].coordY-1 < 0) { //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("El punto inicial es una meta. Se relocalizará el punto inicial.\n");
		fprintf(archivoCaminos, "(%d, %d) ", camino[0].coordX, camino[0].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		camino[0].coordX = atoi(argv[2]); //La coordenada X del punto inicial se toma del segundo argumento del programa convertido a int
		camino[0].coordY = atoi(argv[3])+1;
		agregarVisitado();
		imprimeLaberinto();
		asignaCaminoMasCorto();
		getchar();
	}

//*************** BUSCADOR DE CAMINOS EN LABERINTO POR ALGORITMO "DEPTH FIRST SEARCH" (DFS) ***************
	while (todoRevisado == 0) { 	//Se repite todo hasta que se hayan revisado todos los caminos
		archivoCaminos = fopen("Caminos.txt", "a+");
		if (archivoCaminos == NULL) {
			printf("No se pudo abrir un archivo. El programa se cerrará.\n");
			exit(0);
		}

		NORTEVisitado = 0;
		ESTEVisitado = 0;
		SURVisitado = 0;
		OESTEVisitado = 0;

	//Se revisa si ya se revisó todo el laberinto

		for(i = 0; i <= indiceVisitado; i++) {
			if (camino[indicePActual].coordX-1 == visitados[i].coordX && camino[indicePActual].coordY == visitados[i].coordY)
				NORTEVisitado = 1;
			if (camino[indicePActual].coordX == visitados[i].coordX && camino[indicePActual].coordY+1 == visitados[i].coordY)
				ESTEVisitado = 1;
			if (camino[indicePActual].coordX+1 == visitados[i].coordX && camino[indicePActual].coordY == visitados[i].coordY)
				SURVisitado = 1;
			if (camino[indicePActual].coordX == visitados[i].coordX && camino[indicePActual].coordY-1 == visitados[i].coordY)
				OESTEVisitado = 1;
		}

		if (camino[indicePActual].coordX-1 < 0)
			caminoNORTE();
		else if (camino[indicePActual].coordY+1 > (columnas-1))
			caminoESTE();
		else if (camino[indicePActual].coordX+1 > (renglones-1))
			caminoSUR();
		else if (camino[indicePActual].coordY-1 < 0)
			caminoOESTE();

		else if (NORTEVisitado == 0 && camino[indicePActual].coordX-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0') {
			if (camino[indicePActual].coordX-1 < 0)
				caminoNORTE();
			else if (camino[indicePActual].coordX-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0')
				caminoNORTE();
			else if (camino[indicePActual].coordY+1 > (columnas-1))
				caminoESTE();
			else if (camino[indicePActual].coordY+1 <= (columnas-1) && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0')
				caminoESTE();
			else if (camino[indicePActual].coordX+1 > (renglones-1))
				caminoSUR();
			else if (camino[indicePActual].coordX+1 <= (renglones-1) && laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0')
				caminoSUR();
			else if (camino[indicePActual].coordY-1 < 0)
				caminoOESTE();
			else if (camino[indicePActual].coordY-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0')
				caminoOESTE();
		}

		else if (ESTEVisitado == 0 && camino[indicePActual].coordY+1 <= (columnas-1) && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0')	{
			if (camino[indicePActual].coordY+1 > (columnas-1))
				caminoESTE();
			else if (camino[indicePActual].coordY+1 <= (columnas-1) && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0')
				caminoESTE();
			else if (camino[indicePActual].coordX+1 > (renglones-1))
				caminoSUR();
			else if (camino[indicePActual].coordX+1 <= (renglones-1) && laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0')
				caminoSUR();
			else if (camino[indicePActual].coordY-1 < 0)
				caminoOESTE();
			else if (camino[indicePActual].coordY-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0')
				caminoOESTE();
			else if (camino[indicePActual].coordX-1 < 0)
				caminoNORTE();
			else if (camino[indicePActual].coordX-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0')
				caminoNORTE();
		}
		
		else if (SURVisitado == 0 && camino[indicePActual].coordX+1 <= (renglones-1) && laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0') {
			if (camino[indicePActual].coordX+1 > (renglones-1))
				caminoSUR();
			else if (camino[indicePActual].coordX+1 <= (renglones-1) && laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0')
				caminoSUR();
			else if (camino[indicePActual].coordY-1 < 0)
				caminoOESTE();
			else if (camino[indicePActual].coordY-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0')
				caminoOESTE();
			else if (camino[indicePActual].coordX-1 < 0)
				caminoNORTE();
			else if (camino[indicePActual].coordX-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0')
				caminoNORTE();
			else if (camino[indicePActual].coordY+1 > (columnas-1))
				caminoESTE();
			else if (camino[indicePActual].coordY+1 <= (columnas-1) && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0')
				caminoESTE();
		}

		else if (OESTEVisitado == 0 && camino[indicePActual].coordY-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0') {
			if (camino[indicePActual].coordY-1 < 0) 
				caminoOESTE();
			else if (camino[indicePActual].coordY-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0')
				caminoOESTE();
			else if (camino[indicePActual].coordX-1 < 0)
				caminoNORTE();
			else if (camino[indicePActual].coordX-1 >= 0 && laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0')
				caminoNORTE();
			else if (camino[indicePActual].coordY+1 > (columnas-1))
				caminoESTE();
			else if (camino[indicePActual].coordY+1 <= (columnas-1) && laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0')
				caminoESTE();
			else if (camino[indicePActual].coordX+1 > (renglones-1))
				caminoSUR();
			else if (camino[indicePActual].coordX+1 <= (renglones-1) && laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0')
				caminoSUR();
		}

		else {
			printf("Ningún punto nuevo disponible. Realizando backtrack.\n");
			indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado (bakctrack)
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
		}

	//Se revisa si el arreglo con el camino está vacío y se rellena con el punto inicial si lo está
		if (indicePActual < 0) {
			printf("Se reasignó el punto inicial.\n");
			indicePActual ++;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));			
			camino[0].coordX = atoi(argv[2]);
			camino[0].coordY = atoi(argv[3]);
			contaReasignaVI ++;
		}

	//Si el punto inicial se ha tenido que recolocar varias veces, se termina el algoritmo por seguridad
		if (contaReasignaVI >= 100) {
			printf("Existen caminos bloqueados por paredes. No se podrá calcular el camino más corto.\n Revise 'Caminos.txt' para ver todos los caminos encontrados hasta ahora.\n");
			exit(0);
		}

	//Se imprime lo que va recorriendo el laberinto	
		imprimeLaberinto();

	//Se revisa si ya se visitaron todos los puntos para terminar el algoritmo
		if (indiceVisitado+1 == totalDePuntos) {
		//Última revisión de metas por si el último punto visitado es una meta
			if (camino[indicePActual].coordX-1 < 0)
				caminoNORTE();
			else if (camino[indicePActual].coordY+1 > (columnas-1))
				caminoESTE();
			else if (camino[indicePActual].coordX+1 > (renglones-1))
				caminoSUR();
			else if (camino[indicePActual].coordY-1 < 0) 
				caminoOESTE();
			todoRevisado = 1;
		}
		getchar();
	}

//LIBERACIÓN DE MEMORIA Y CIERRE DE ARCHIVOS
	for (i = 0; i < renglones; i++)
    	free(laberintoEnMatriz[i]);
    free(laberintoEnMatriz);
    free(visitados);
    free(camino);
	printf("Se encontraron todos los caminos y se han almacenado en el archivo 'Caminos.txt'.\nEl camino más corto se encuentra en el archivo 'CaminoCorto.txt'.\n");
    return 0;
}

//Función que verifica si al NORTE de un punto existe camino
void caminoNORTE() {
	//Se revisa si existe camino al norte del punto actual
	if (camino[indicePActual].coordX-1 < 0){ //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("Meta encontrada. Realizando backtrack y almacenando en archivo.\n"); 	//Se encontró una nueva meta
		for (i = 0; i <= indicePActual; i++)
			fprintf(archivoCaminos, "(%d, %d) ", camino[i].coordX, camino[i].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		asignaCaminoMasCorto();
		indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado (bakctrack)
		camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
	}
	else if (laberintoEnMatriz[camino[indicePActual].coordX-1][camino[indicePActual].coordY] == '0') {
		printf("Ir al NORTE: (%d, %d) a (%d, %d)\n", camino[indicePActual].coordX, camino[indicePActual].coordY, camino[indicePActual].coordX-1, camino[indicePActual].coordY);
		if (NORTEVisitado == 1) { //Si el punto ya se visitó
			printf("Ese punto ya fue visitado. Realizando backtrack.\n");
			indicePActual --;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
		}
		else if (NORTEVisitado == 0) {
			indicePActual ++;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
			camino[indicePActual].coordX = camino[indicePActual-1].coordX-1;
			camino[indicePActual].coordY = camino[indicePActual-1].coordY;
			agregarVisitado();
		}
	}
}

//Función que verifica si al ESTE de un punto existe camino
void caminoESTE() {
	if (camino[indicePActual].coordY+1 > (columnas-1)) { //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta 
		printf("Meta encontrada. Realizando backtrack y almacenando en archivo.\n"); 	//Se encontró una nueva meta
		for (i = 0; i <= indicePActual; i++)
			fprintf(archivoCaminos, "(%d, %d) ", camino[i].coordX, camino[i].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		asignaCaminoMasCorto();
		indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado (bakctrack)
		camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
	}
	else if (laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY+1] == '0') {
		printf("Ir al ESTE: (%d, %d) a (%d, %d)\n", camino[indicePActual].coordX, camino[indicePActual].coordY, camino[indicePActual].coordX, camino[indicePActual].coordY+1);
		if (ESTEVisitado == 1) {
			printf("Ese punto ya fue visitado. Realizando backtrack.\n");
			indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
		}
		else if (ESTEVisitado == 0) {
			indicePActual ++;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
			camino[indicePActual].coordX = camino[indicePActual-1].coordX;
			camino[indicePActual].coordY = camino[indicePActual-1].coordY+1;
			agregarVisitado();		
		}
	}
}

//Función que verifica si al SUR de un punto existe camino
void caminoSUR() {
	if (camino[indicePActual].coordX+1 > (renglones-1)) { //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("Meta encontrada. Realizando backtrack y almacenando en archivo.\n"); 	//Se encontró una nueva meta
		for (i = 0; i <= indicePActual; i++)
			fprintf(archivoCaminos, "(%d, %d) ", camino[i].coordX, camino[i].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		asignaCaminoMasCorto();
		indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado (bakctrack)
		camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
	}
	else if (laberintoEnMatriz[camino[indicePActual].coordX+1][camino[indicePActual].coordY] == '0') {
		printf("Ir al SUR: (%d, %d) a (%d, %d)\n", camino[indicePActual].coordX, camino[indicePActual].coordY, camino[indicePActual].coordX+1, camino[indicePActual].coordY);
		if (SURVisitado == 1) {
			printf("Ese punto ya fue visitado. Realizando backtrack.\n");
			indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
		}
		else if (SURVisitado == 0) {
			indicePActual ++;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
			camino[indicePActual].coordX = camino[indicePActual-1].coordX+1;
			camino[indicePActual].coordY = camino[indicePActual-1].coordY;
			agregarVisitado();		
		}
	}
}

//Función que verifica si al OESTE de un punto existe camino
void caminoOESTE() {
	if (camino[indicePActual].coordY-1 < 0) { //Si el punto al norte se sale de los índices del arreglo, se habría encontrado una meta
		printf("Meta encontrada. Realizando backtrack y almacenando en archivo.\n"); 	//Se encontró una nueva meta
		for (i = 0; i <= indicePActual; i++)
			fprintf(archivoCaminos, "(%d, %d) ", camino[i].coordX, camino[i].coordY); //Se vuelca la memoria
		fprintf(archivoCaminos, "\n"); //Se imprime salto de línea en archivo por si se almacenan más caminos
		fclose(archivoCaminos);
		asignaCaminoMasCorto();
		indicePActual --; //Se reduce el tamaño del arreglo que contiene el camino, borrando el último dato ingresado (bakctrack)
		camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
	}
	else if (laberintoEnMatriz[camino[indicePActual].coordX][camino[indicePActual].coordY-1] == '0') {
		printf("Ir al OESTE:  (%d, %d) a (%d, %d)\n", camino[indicePActual].coordX, camino[indicePActual].coordY, camino[indicePActual].coordX, camino[indicePActual].coordY-1);
		if (OESTEVisitado == 1) {
			printf("Ese punto ya fue visitado. Realizando backtrack.\n");
			indicePActual --;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
		}
		else if (OESTEVisitado == 0) {
			indicePActual ++;
			camino = (PuntoActual*) realloc(camino, (indicePActual+1)*sizeof(PuntoActual));
			camino[indicePActual].coordX = camino[indicePActual-1].coordX;
			camino[indicePActual].coordY = camino[indicePActual-1].coordY-1;
			agregarVisitado();		
		}
	}
}

//FUNCIÓN QUE AGREGA ELEMENTOS AL ARREGLO DE VISITADOS
void agregarVisitado() {
	indiceVisitado ++;
	visitados = (PuntoVisitado*) realloc(visitados, (indiceVisitado+1)*sizeof(PuntoVisitado));
	visitados[indiceVisitado].coordX = camino[indicePActual].coordX;
	visitados[indiceVisitado].coordY = camino[indicePActual].coordY;
	if (visitados == NULL) {
		printf("No se pudo reservar memoria para almacenar puntos visitados. El programa se cerrará.\n");
		exit (0);
	}
	printf("Se agregó (%d, %d) al arreglo de visitados. Se han visitado %d de %d puntos. \n", visitados[indiceVisitado].coordX, visitados[indiceVisitado].coordY, indiceVisitado+1, totalDePuntos);
}

//FUNCIÓN QUE CALCULA Y ALMACENA EL CAMINO MÁS CORTO EN UN ARCHIVO
void asignaCaminoMasCorto() {
	if (indicePActual < caminoMasCorto)	{
		caminoMasCorto = indicePActual;
		archivoCaminoCorto = fopen("CaminoCorto.txt", "w+");
		if (archivoCaminos == NULL) {
			printf("No se pudo crear un archivo. El programa se cerrara.\n");
			exit(0);
		}
		
		for (i = 0; i <= indicePActual; i++)
			fprintf(archivoCaminoCorto, "(%d, %d) ", camino[i].coordX, camino[i].coordY);
		printf("Se encontró un camino más corto. Guardado en archivo.\n");
		fclose(archivoCaminoCorto);
	}
}

//FUNCIÓN QUE IMPRIME EL LABERINTO
void imprimeLaberinto() {
	for(i = 0; i < renglones; i++) {
	  	for(j = 0; j < columnas; j++)
	   		if (i == camino[indicePActual].coordX && j == camino[indicePActual].coordY)
	   			printf ("X ");
	   		else
	   			printf("%c ", laberintoEnMatriz[i][j]);
		printf("\n");
	}
}