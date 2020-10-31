#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <pthread.h>

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s = (int * ) socket;
	sock_conn = *s;
	
	int ret;
	char peticion[512];
	char respuesta[512];
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//hasta que se desconecte
	while (terminar ==0)
	{
		// Ahora recibimos la petici?n
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que a?adirle la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		
		printf ("Peticion: %s\n",peticion);
		
		// vamos a ver que quieren
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		// Ya tenemos el c?digo de la petici?n
		char nombre[20];
		
		if (codigo !=0)
		{
			p = strtok( NULL, "/");
			
			strcpy (nombre, p);
			// Ya tenemos el nombre
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		
		if (codigo ==0) //petici?n de desconexi?n
			terminar=1;
		
		else if (codigo ==1) //piden la longitd del nombre
			sprintf (respuesta,"%d",strlen (nombre));
		
		else if (codigo ==2)
		{	
			// quieren saber si el nombre es bonito
			if((nombre[0]=='M') || (nombre[0]=='S'))
				strcpy (respuesta,"SI");
			
			else
				strcpy (respuesta,"NO");
		}
		
		else if (codigo==3)//quiere saber si es alto
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
		
		if (codigo !=0)
		{
			printf ("Respuesta: %s\n", respuesta);
			// Enviamos respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn); 
}



int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
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
	
	// Bucle infinito seria for(;;)
	for (i=0; i<5;i++){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		sockets[i] = sock_conn;
		pthread_create (&thread[i], NULL, AtenderCliente, &sockets[i]);

	}
}

