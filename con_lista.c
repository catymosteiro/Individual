#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>

//Declaracion de estructuras
typedef struct{
	char nombre[20];
	int socket;
}Conectado;

typedef struct {
	Conectado conectados[100];
	int num;
}ListaConectados;

//Declaracion de variables globales
int contador_servicios;
ListaConectados miLista2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Funciones
int Pon (ListaConectados *lista, char nombre[20], int socket){
	//Anade conectados a la lista
	if(lista ->num ==100)
		return -1;
	else{
		strcpy(lista->conectados[lista->num].nombre, nombre);
		lista->conectados[lista->num].socket = socket;
		lista->num ++;
		printf ("Conectado: %s\n", lista->conectados[lista->num -1].nombre);
		return 3;
	}
}
int DameSocket (ListaConectados *lista, char nombre[20]){
	//Devuelve el socket del nombre introducido que esta en la lista
	int i=0;
	int encontrado=0;
	while ((i<lista->num)&&!encontrado){
		if (strcmp(lista->conectados[i].nombre,nombre)==0)
			encontrado =1;
		else
			i=i+1;
	}
	if (encontrado)
		return lista->conectados[i].socket;
	else
		return -1;
}
int DamePosicion (ListaConectados *lista, char nombre[20]){
	//Devuelve la posicion del nombre introducido que esta en la lista
	int i=0;
	int encontrado=0;
	while ((i<lista->num)&&!encontrado){
		if (strcmp(lista->conectados[i].nombre,nombre)==0)
			encontrado =1;
		else
			i=i+1;
	}
	if (encontrado)
		return i;
	else
		return -1;
}

int Elimina (ListaConectados *lista, char nombre[20]){
	//Elimina en nombre de la lista de conectados
	int pos =DamePosicion (lista, nombre);
	if (pos==-1)
		return -1;
	else {
		int i;
		for (i= pos; i<lista->num-1; i++)
		{
			lista-> conectados [i] = lista->conectados[i+1];
			//strcpy(lista->conectados[i].nombre = lista->conectados[i+1].nombre, nombre);
			//lista->conectados[i].socket =lista->conectados[i+1].socket;
		}
		lista ->num--;
		return 0;
	}
}

void DameConectados (ListaConectados *lista, char conectados[300]){
	//Pone en un vector los nombres de conectados separados por una bcoma
	//Primero pone el número de conectados. Ej: 3,Maria,Juan,Pedro
	sprintf (conectados, "%d", lista->num);
	printf ("%d",lista->num);
	int i;
	for (i=0; i<lista->num; i++)
		sprintf (conectados, "%s,%s", conectados, lista->conectados[i].nombre);
}

void DameTodosSockets (ListaConectados *lista, char conectados[300], char sockets[300]){
	//Pone en un vector los sockets de conectados separados por una coma. Ej: 2,3,4
	int i;
	int o =0;
	char socket[10];
	char nombre[20];
	char *p = strtok (conectados, ",");
	int n = atoi (p);
	p = strtok (NULL, ",");
	strcpy (nombre, p);
	
	for (i=0;o<n;i++)
		if (strcmp (lista->conectados[i].nombre, nombre)==0){
			sprintf(socket, "%d", lista->conectados[i].socket);
			strcat (sockets, socket); 
			if (o< n-1){
				strcat (sockets, ",");
				p = strtok (NULL, ",");
				strcpy (nombre, p);
			}
			o++;
	}
}

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s = (int * ) socket;
	sock_conn = *s;
	int ret;
	char peticion[512];
	char respuesta[512];
	
	int ya;
	int terminar =0;
	ya = terminar;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	while (terminar ==0)
	{
		char respuesta[512];		
		// Ahora recibimos la petici?n
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que anadir marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		printf ("Peticion: %s\n",peticion);
		
		//Analizamos la peticion
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		// Ya tenemos el c?digo de la petici?n
		char nombre[20];
		p = strtok( NULL, "/");
		strcpy (nombre, p);
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		
		if (codigo ==4444)
		{
			// Ya tenemos el nombre
			if (ya ==0){
				ya = 1;
				pthread_mutex_lock(&mutex); //No interrumpir
				int poner = Pon (&miLista2, nombre, miLista2.num);
				pthread_mutex_unlock(&mutex); //Ahora si se puede interrumpir
				if (poner == 3)
					printf("Usuario añadido a la lista de conectados. \n");
				else
					printf("Error al introducir al usuario a la lista de conectados. \n");
			}
		}
		
		else if (codigo ==10) //Peticion de desconexion
		{
			if (p!= NULL)
			{
				ya = 0;
				pthread_mutex_lock(&mutex); //No interrumpir
				int eliminar = Elimina (&miLista2, nombre);
				pthread_mutex_unlock(&mutex); //Ahora si se puede interrumpir
				if (eliminar==0)
					printf("Usuario eliminado de la lista de conectados. \n");
				else
					printf("Error al eliminar el usuario de la lista de conectados. \n");
			}
			terminar=1;
		}
		
		else if (codigo ==1) //Solicitan saber la longitd del nombre
		{
			sprintf (respuesta,"%d",strlen (nombre));
		}
		
		else if (codigo ==2)
		{	
			// Quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
			{
				strcpy (respuesta,"SI");
			}
			
			else
			{
				strcpy (respuesta,"NO");
			}
		}
		
		else if (codigo==3) //Para saber si es alt@
		{
			p = strtok( NULL, "/");
			float altura =  atof (p);
			if (altura > 1.70)
				sprintf (respuesta, "%s: eres alt@",nombre);
			else
				sprintf (respuesta, "%s: eres baj@",nombre);
		}
		else if (codigo ==4)
		{
			// Quieren saber si el nombre es polindromo
			int i = 0;
			int o = strlen (nombre) - 1;
			int fallo = 0;
			while ((i < o) && (fallo !=1))
			{
				nombre[0] = tolower(nombre[0]);
				if(nombre[i]==nombre[o-i])
					fallo = 0;
				
				else
					fallo = 1;
				i++;
			}
			if (fallo==0)
				strcpy (respuesta,"Si, tu nombre es palindromo");
			else
				strcpy (respuesta,"No, tu nombre no es palindromo");
		}
		
		else if (codigo ==5) //Nombre en mayusculas
		{
			for (int in = 0; nombre[in] != '\0'; in++)
			{
				nombre[in] = toupper(nombre[in]);
			}
			sprintf (respuesta,"%s",nombre);
		}
		
		else if (codigo ==27)
		{
			char misConectados [300];
			DameConectados (&miLista2, misConectados);
			strcpy (respuesta, misConectados);
			printf("Resp: %s\n",respuesta);
		}
		
		else if (codigo ==6) //Solicitan saber el numero de servicios realizados
		{
			sprintf (respuesta,"%d",contador_servicios);
		}
		
		if (codigo !=10)
		{
			if (codigo !=4444){
				printf ("Respuesta: %s\n", respuesta);
				// Enviamos respuesta
				write (sock_conn,respuesta, strlen(respuesta));
			}
		}
		
		if ((codigo ==1)||(codigo ==2)||(codigo ==3)||(codigo ==4)||(codigo ==5))
		{
			pthread_mutex_lock(&mutex); //No interrumpir
			contador_servicios = contador_servicios +1;
			pthread_mutex_unlock(&mutex); //Ahora si se puede interrumpir
		}
	}
	//Finaliza el servicio para este cliente
	close(sock_conn); 
}


int main(int argc, char *argv[])
{
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;

	// Abrimos el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	memset(&serv_adr, 0, sizeof(serv_adr));// Inicializa
	serv_adr.sin_family = AF_INET;
	// Asocia el socket a cualquiera de las IP de la maquina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9200);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	pthread_t thread[100];
	
	//Inicializacion de variables globales
	contador_servicios = 0;
	miLista2.num =0;
	
	// Bucle infinito 
	for (;;){
		printf ("Escuchando...\n");
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		sockets[i] = sock_conn;
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]);

	}
}

