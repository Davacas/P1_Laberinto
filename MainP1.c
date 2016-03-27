#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
//DECLARACIÓN DE VARIABLES
	FILE *archivoLaberinto = NULL;
	int **laberintoEnMatriz = NULL;
	int renglones = 0;
	int columnas = 0;
	int posicion = 2;
	int i = 0, j = 0;

//SE CARGA EL ARCHIVO AL PROGRAMA
	archivoLaberinto = fopen(argv[1], "r"); //El nombre del archivo se almacena como el argumento 1 pasado por consola
	if (archivoLaberinto == NULL)
    {
		printf("No se pudo cargar el archivo. El programa se cerrara");
		exit(0);
	}

//CUENTA RENGLONES Y COLUMNAS PARA ASIGNAR MATRIZ DINÁMICA
	posicion = fgetc(archivoLaberinto);
	while (posicion != '\n')
	{
		if (posicion == '0' || posicion == '1')
			columnas++;
		posicion = fgetc(archivoLaberinto);
	}
	rewind(archivoLaberinto); 
	while (feof(archivoLaberinto) == 0)
	{
		posicion = fgetc(archivoLaberinto);
		if (posicion == '\n' || posicion == -1)
			renglones++;
	}
	rewind(archivoLaberinto);

//CREACIÓN DE MATRIZ DINÁMICA
	laberintoEnMatriz = (int**)malloc(renglones*sizeof(int*));
	if(laberintoEnMatriz == NULL)
    { 
    	printf("No se pudo reservar la memoria. El programa se cerrara");
    	exit(0);
    }
	for (i = 0; i < renglones; i++)
    	laberintoEnMatriz[i] = (int*)malloc(columnas*sizeof(int));
    for (i = 0; i < renglones; i++)
   		if(laberintoEnMatriz[i] == NULL)
		{
			printf("No se pudo reservar la memoria. El programa se cerrara");
	  		exit(0);
  		}
    renglones = 0;
	columnas = 0;

//COPIA DE LABERINTO A MATRIZ
    while (feof(archivoLaberinto) == 0)
	{
		posicion = fgetc(archivoLaberinto);
		if (posicion == '0' || posicion == '1')
		{
			laberintoEnMatriz[renglones][columnas] = posicion;
			columnas++;
		}
		if (posicion == '\n')
		{
			renglones++;
			columnas = 0;
		}
		if (posicion == -1)
			renglones++;
	} 
    			
//IMPRESIÓN EN PANTALLA DEL LABERINTO PORQUE SÍ
    for(i = 0; i < renglones; i++)
    {
    	for(j = 0; j < columnas; j++)
    		printf("%c ", laberintoEnMatriz[i][j]);
		printf("\n");
	}
	printf ("%c", laberintoEnMatriz[0][4]);

//LIBERACIÓN DE MEMORIA
	for (i = 0; i < renglones; i++)
    	free(laberintoEnMatriz[i]);
    free(laberintoEnMatriz); 

    fclose (archivoLaberinto);
    return 0;
}