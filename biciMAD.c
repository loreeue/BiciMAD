#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int usuariosTotales, numEstaciones, numHuecos, tiempoMinEspera, tiempoMaxEspera, tiempoMinMontando, tiempoMaxMontando;
//creo el mutex
pthread_mutex_t *mutex_estaciones; //mutex para controlar el acceso a las estaciones

FILE *archivoSalida; //fichero donde se escribe la salida

struct Usuario { //estructura para los usuarios
    	int id;
    	int estacion_origen;
    	int estacion_destino;
    	int tiene_bicicleta; //para controlar que un usuario que tiene una bici no pueda coger otra sin haber dejado la anterior
};

struct Estacion { //estructura para las estaciones
    	int id;
    	int bicis_disponibles;
    	int huecos_disponibles;
    	pthread_cond_t condBicis;  //condición para coger las bicis
    	pthread_cond_t condHuecos; //condición para dejar las bicis
};

struct Estacion *estaciones; //array de estaciones

void mostrarEstadoEstacionActual(int estacionId) { //función para mostrar el estado de una estación concreta
	int i;
	printf("\n");
    	printf("Estado de la Estación %d: ", estacionId);
    	fprintf(archivoSalida, "Estado de la Estación %d: ", estacionId);
    	for (i = 0; i < numHuecos; i++) {
        	if (i < estaciones[estacionId].huecos_disponibles) { //imprime 0 en los huecos libres
         		printf("0 ");
         		fprintf(archivoSalida, "0 ");
        	}
        	else { //imprime 1 en los huecos ocupados
            		printf("1 ");
            		fprintf(archivoSalida, "1 ");
        	}
    	}
    	printf("\n\n");
    	fprintf(archivoSalida, "\n\n");
}

void* funcion_threads(void* arg) {
	struct Usuario *usuario = (struct Usuario*) arg;

	srand(time(NULL) + usuario->id);

	while(1) {
		//inicializo valores del usuario
    		usuario->estacion_origen = rand() % numEstaciones;
    		usuario->estacion_destino = rand() % numEstaciones;
    		usuario->tiene_bicicleta = 0;
    	
    		sleep(rand() % (tiempoMaxEspera - tiempoMinEspera + 1) + tiempoMinEspera); //tiempo aleatorio para coger una bici
		if (usuario->tiene_bicicleta == 0) {
			printf("Usuario %d quiere coger bici de Estación %d\n", usuario->id, usuario->estacion_origen); //el usuario intenta coger una bici
			fprintf(archivoSalida, "Usuario %d quiere coger bici de Estación %d\n", usuario->id, usuario->estacion_origen);
			pthread_mutex_lock(&mutex_estaciones[usuario->estacion_origen]);

			while (estaciones[usuario->estacion_origen].bicis_disponibles == 0) { //si no hay bicis disponibles para coger-> espera
				pthread_cond_wait(&estaciones[usuario->estacion_origen].condBicis, &mutex_estaciones[usuario->estacion_origen]);
			}
			
    			//si hay bicis disponibles para coger
        		estaciones[usuario->estacion_origen].bicis_disponibles--; //disminuyo bicis disponibles
            		estaciones[usuario->estacion_origen].huecos_disponibles++; //aumento huecos disponibles
            		pthread_cond_signal(&estaciones[usuario->estacion_origen].condHuecos); //vuelvo a comprobar por si un usuario puede dejar ya su bici

        		printf("Usuario %d coge bici de Estación %d\n", usuario->id, usuario->estacion_origen);
        		fprintf(archivoSalida, "Usuario %d coge bici de Estación %d\n", usuario->id, usuario->estacion_origen);
        		usuario->tiene_bicicleta = 1;
        		mostrarEstadoEstacionActual(usuario->estacion_origen);

        		pthread_mutex_unlock(&mutex_estaciones[usuario->estacion_origen]);

			printf("Usuario %d montando en bici...\n", usuario->id);
			fprintf(archivoSalida, "Usuario %d montando en bici...\n", usuario->id);
        		sleep(rand() % (tiempoMaxMontando - tiempoMinMontando + 1) + tiempoMinMontando); //tiempo aleatorio para montar en una bici

        		printf("Usuario %d quiere dejar bici en Estación %d\n", usuario->id, usuario->estacion_destino);
        		fprintf(archivoSalida, "Usuario %d quiere dejar bici en Estación %d\n", usuario->id, usuario->estacion_destino);
        		pthread_mutex_lock(&mutex_estaciones[usuario->estacion_destino]);

        		while (estaciones[usuario->estacion_destino].huecos_disponibles == 0) { //si no hay huecos disponibles para dejar la bici-> espera
					pthread_cond_wait(&estaciones[usuario->estacion_destino].condHuecos, &mutex_estaciones[usuario->estacion_destino]);
			}
        		
        		//si hay huecos libres para dejar la bici
            		estaciones[usuario->estacion_destino].huecos_disponibles--; //disminuyo huecos disponibles
            		estaciones[usuario->estacion_destino].bicis_disponibles++; //aumento bicis disponibles
            		pthread_cond_signal(&estaciones[usuario->estacion_destino].condBicis); //vuelvo a comprobar por si un usuario puede coger ya su bici

            		printf("Usuario %d deja bici en Estación %d\n", usuario->id, usuario->estacion_destino);
            		fprintf(archivoSalida, "Usuario %d deja bici en Estación %d\n", usuario->id, usuario->estacion_destino);
            		usuario->tiene_bicicleta = 0;
            		mostrarEstadoEstacionActual(usuario->estacion_destino);

			pthread_mutex_unlock(&mutex_estaciones[usuario->estacion_destino]);
    		}
    		else { //si ya tiene una bicicleta
    			pthread_mutex_unlock(&mutex_estaciones[usuario->estacion_origen]);
    		}
    	}
	fflush(archivoSalida);
    	pthread_exit(NULL); //para que el hilo salga de esta función
}

char* obtenerHoraFecha() { //obtener la hora que luego se añadirá al nombre del fichero de salida
	char *horaFecha;
	time_t tiempoActual;
    	struct tm *infoTiempo;

    	horaFecha = (char *)malloc(128);
    	time(&tiempoActual);
    	infoTiempo = localtime(&tiempoActual);
    	strftime(horaFecha, 128, "%Y%m%d%H%M%S", infoTiempo);
    	return horaFecha;
}

int main(int argc, char *argv[]) {
    	char *fecha = (char *)malloc(1024 * sizeof(char));
    	char *nombre = (char *)malloc(1024 * sizeof(char));
    	char *txt = (char *)malloc(1024 * sizeof(char));

    	char *fechaHora;

    	FILE *archivoEntrada;

	struct Usuario *usuarios;
	int i;
	int hilo;

	pthread_t *threads;

	fechaHora = obtenerHoraFecha();
    	if (argc == 1) { //NO FICHEROS
        	//fichero de entrada -> entrada_BiciMAD.txt
        	//crear un fichero de salida con nombre fichero<hora y fecha>.txt
        	archivoEntrada = fopen("entrada_BiciMAD.txt", "r");
    		if (archivoEntrada == NULL) {
        		printf("Error al abrir el archivo de entrada");
        		return EXIT_FAILURE;
    		}
    		strcpy(nombre, "salida_sim_BiciMAD");
    		strcpy(txt, ".txt");
    		strcat(nombre, fechaHora);
    		strcat(nombre, txt);
    		archivoSalida = fopen(nombre, "w");
    		if (archivoSalida == NULL) {
        		perror("Error al abrir el archivo de salida");
        		fclose(archivoEntrada);
        		return EXIT_FAILURE;
    		}
    	}
    	else if (argc == 2) { //FICHERO ENTRADA
        	//crear un fichero de salida con nombre fichero<hora y fecha>.txt
        	archivoEntrada = fopen(argv[1], "r");
    		if (archivoEntrada == NULL) {
        		printf("Error al abrir el archivo de entrada");
        		return EXIT_FAILURE;
    		}
    		strcpy(nombre, "salida_sim_BiciMAD");
    		strcpy(txt, ".txt");
    		strcat(nombre, fechaHora);
    		strcat(nombre, txt);
    		archivoSalida = fopen(nombre, "w");
    		if (archivoSalida == NULL) {
        		perror("Error al abrir el archivo de salida");
        		fclose(archivoEntrada);
        		return EXIT_FAILURE;
    		}
    	}
    	else if (argc == 3) { //FICHERO ENTRADA Y FICHERO SALIDA
    		//crear otro fichero de salida con nombre <hora y fecha>fichero.txt
    		archivoEntrada = fopen(argv[1], "r");
    		if (archivoEntrada == NULL) {
        		printf("Error al abrir el archivo de entrada");
        		return EXIT_FAILURE;
    		}
    		strcat(fechaHora, argv[2]);
    		archivoSalida = fopen(fechaHora, "w");
    		if (archivoSalida == NULL) {
        		perror("Error al abrir el archivo de salida");
        		fclose(archivoEntrada);
        		return EXIT_FAILURE;
    		}
    	}
    	else {
    		fprintf(stderr, "Uso: %s archivo.txt\n", argv[0]);
        	return 1;
    	}

    	//libero memoria dinámica
    	free(fechaHora);
    	free(fecha);
    	free(nombre);
    	free(txt);

    	//leo valores del fichero
    	fscanf(archivoEntrada, "%d", &usuariosTotales);
    	fscanf(archivoEntrada, "%d", &numEstaciones);
    	fscanf(archivoEntrada, "%d", &numHuecos);
    	fscanf(archivoEntrada, "%d", &tiempoMinEspera);
    	fscanf(archivoEntrada, "%d", &tiempoMaxEspera);
    	fscanf(archivoEntrada, "%d", &tiempoMinMontando);
    	fscanf(archivoEntrada, "%d", &tiempoMaxMontando);

    	//cierro fichero entrada
    	fclose(archivoEntrada);
    	printf("\nBiciMAD: CONFIGURACIÓN INICIAL\n");
    	printf("Usuarios: %d\n", usuariosTotales);
    	printf("Número de estaciones: %d\n", numEstaciones);
    	printf("Número de huecos por estación: %d\n", numHuecos);
    	printf("Tiempo mínimo de espera para decidir coger una bici: %d\n", tiempoMinEspera);
    	printf("Tiempo máximo de espera para decidir coger una bici: %d\n", tiempoMaxEspera);
    	printf("Tiempo mínimo que pasa un usuario montando una bici: %d\n", tiempoMinMontando);
    	printf("Tiempo máximo que pasa un usuario montando una bici: %d\n", tiempoMaxMontando);
    	printf("\nSIMULACIÓN FUNCIONAMIENTO BiciMAD\n");

	//reservo memoria para las estaciones y para los usuarios
    	estaciones = (struct Estacion*)malloc(numEstaciones * sizeof(struct Estacion));
    	usuarios = (struct Usuario*)malloc(usuariosTotales * sizeof(struct Usuario));

    	fprintf(archivoSalida, "\nBiciMAD: CONFIGURACIÓN INICIAL\n");
    	fprintf(archivoSalida, "Usuarios: %d\n", usuariosTotales);
    	fprintf(archivoSalida, "Número de estaciones: %d\n", numEstaciones);
    	fprintf(archivoSalida, "Número de huecos por estación: %d\n", numHuecos);
    	fprintf(archivoSalida, "Tiempo mínimo de espera para decidir coger una bici: %d\n", tiempoMinEspera);
    	fprintf(archivoSalida, "Tiempo máximo de espera para decidir coger una bici: %d\n", tiempoMaxEspera);
    	fprintf(archivoSalida, "Tiempo mínimo que pasa un usuario montando una bici: %d\n", tiempoMinMontando);
    	fprintf(archivoSalida, "Tiempo máximo que pasa un usuario montando una bici: %d\n", tiempoMaxMontando);
    	fprintf(archivoSalida, "\nSIMULACIÓN FUNCIONAMIENTO BiciMAD\n");
    	
    	fflush(archivoSalida);
    	
    	//reservo memoria para los threads
    	threads = (pthread_t *)malloc(usuariosTotales * sizeof(pthread_t));

	//reservo memoria para el mutex
	mutex_estaciones = malloc(numEstaciones * sizeof(pthread_mutex_t));
	
	//inicializo el mutex
	for (i = 0; i < numEstaciones; i++) {
        	if (pthread_mutex_init(&mutex_estaciones[i], NULL) != 0) {
            		perror("Error al inicializar el mutex de huecos");
            		return 1;
        	}
    	}

	//inicializo las estaciones (todos sus campos)
    	for (i = 0; i < numEstaciones; i++) {
        	estaciones[i].id = i;
        	estaciones[i].bicis_disponibles = (3 * numHuecos) / 4;
        	estaciones[i].huecos_disponibles = numHuecos / 4;
        	pthread_cond_init(&estaciones[i].condBicis, NULL);
    		pthread_cond_init(&estaciones[i].condHuecos, NULL);
    	}

	//creo todos los hilos
    	for(i = 0; i < usuariosTotales; i++) {
    		usuarios[i].id = i;
    		
    		hilo = pthread_create(&threads[i], NULL, funcion_threads, (void*)&usuarios[i]);
    		if (hilo != 0) {
        		fprintf(stderr, "Error al crear el hilo %d: %s\n", i, strerror(hilo));
        		exit(EXIT_FAILURE);
    		}
    	}
    	
    	//esperar a que los hilos terminen
    	for (i = 0; i < usuariosTotales; i++) {
    		pthread_join(threads[i], NULL);
	}

	fflush(archivoSalida);

	//cierro fichero salida
	fclose(archivoSalida);

	//libero memoria dinámica
	free(threads);
	free(estaciones);
	free(usuarios);

	//destruyo mutex y mutex con condición
	for (int i = 0; i < numEstaciones; i++) {
    		pthread_mutex_destroy(&mutex_estaciones[i]);
	}

	for (int i = 0; i < numEstaciones; i++) {
    		pthread_cond_destroy(&estaciones[i].condBicis);
    		pthread_cond_destroy(&estaciones[i].condHuecos);
	}

	//libero memoria dinámica
	free(mutex_estaciones);

    	return EXIT_SUCCESS;
}