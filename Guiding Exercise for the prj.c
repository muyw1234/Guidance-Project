#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char *argv[])
{
	// Parte de inicializacion de conexion
	int sock_conn, sock_listen,ret;
	struct sockaddr_in serv_adr;
	char peticion[600];
	char respuesta[600];
	
	//abrir socket
	// este if es un socket de escucha lo que hace es esperar una conexion desde cliente
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		printf("Error al crear socket");
	}
	
	//inicializa a zero serv_adr
	memset(&serv_adr, 0, sizeof(serv_adr));
	//que tipo de protocolo va a usar
	serv_adr.sin_family = AF_INET;
	
	//cual es la direccion ip donde va a escuchar
	serv_adr.sin_addr.s_addr= htonl(INADDR_ANY);//INADDR_ANY: va a escuchar cualquier direccion 
	//se va a escuchar desde puerto 9050
	serv_adr.sin_port = htons(9060);
	//asociar el socoet de escucha con los protocolos
	if (bind(sock_listen, (struct sockaddr *) & serv_adr, sizeof(serv_adr))<0)
	{
		printf("error al bind");
	}
	
	//la cola de ser atentida no puede ser mas de 3
	if (listen(sock_listen,3)<0)
	{
		printf("error en el listen");
	}
	
	int i;
	//para que no se desconecte hasta que el cliente ha decidido desconectarse(clicando el boton de desconectar)
	for(;;) //bucle infinito
	{
		printf("escuchando\n");
		//accepta el la peticion sock_listen y crear una connecion, y a traves de eso se puede comunicar con el cliente
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("he recibido conexion\n");
		
		int terminar=0;
		while(terminar==0)
		{
			//recoger la peticion
			//una operacion de lectura a traves del sock_conn, y la peticion la guarda en el char peticion
			//es una funcion que lee una mensaje de texto en el que llega a traves del socket y lo guarda en el variable peticion
			//y ademas me da el tamaño del mensaje
			ret = read(sock_conn, peticion, sizeof(peticion));
			printf("recibido\n");
			
			//poner el fin en el buffer: fin de linea
			peticion[ret] = '\0';
			
			printf("peticion : %s\n", peticion);
			
			//respuesta de la peticion:
			char nombre[20];
			//coger los caracteres hasta primer /
			char *p = strtok(peticion, "/");
			// convertir estos caractere a int 
			int codigo = atoi (p);
			
			if (codigo !=0)
			{
				// coger el siguiente parabla antes de / que es el userID
				p= strtok(NULL, "/");
				
				// lo copia a un nuevo variable nombre
				strcpy(nombre, p);
				
				printf("code: %d, user id: %s\n", codigo, nombre);
			}
			
			if (codigo == 0)
			{
				terminar=1;
			}
			else if(codigo ==1)
			{
				sprintf (respuesta,"%d",strlen (nombre));
				
			}
			else if (codigo ==2)
			{
				if((nombre[0]=='M') || (nombre[0]== 'S'))
				{
					strcpy(respuesta, "SI");
				}
				else
				{
					strcpy(respuesta, "NO");
				}
			}
			else
			{
				p=strtok(NULL, "/");
				float altura = atof (p);
				if( altura >1.7)
				{
					sprintf(respuesta, "%s: eres alto", nombre);
				}
				else{
					sprintf(respuesta, "%s: eres bajo", nombre);
				}
				
			}
			if (codigo !=0)
			{
				printf ("respuesta: %s\n", respuesta);
				// enviar la respuesta al cliente
				write (sock_conn, respuesta, strlen(respuesta));
			}
		}
		close (sock_conn);
	}
}

