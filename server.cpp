#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define MY_PORT 7000

int main( int argc, char **argv )
{
	//strcpy(msg,"malik\0");

	int  sock;
	int  sockCHL;		// handles de sockets
	char msg[256];			// buffer
	int  yes         = 1;
	int  max_clients = 20;
	int  retval;

	fd_set rfds,wfds;

	struct timeval     tv;
	struct sockaddr_in srv_addr;	// addresse serveur
	struct sockaddr_in cl_addr;	// addresse client

	int size = sizeof( struct sockaddr_in );

	cout << "\n Starting Server .... " << endl;
	// reserve socket
	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )	 {
		printf("\n socket : error \n");
		exit(-1);
	}
	if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes , sizeof( int ) ) == -1 ) {
		printf("\nsetsockopt:erreur\n");
		exit(-1);
	}

	srv_addr.sin_family      = AF_INET;		// type
	srv_addr.sin_port        = htons(MY_PORT);	// port 3000 (arbitraire)
	srv_addr.sin_addr.s_addr = INADDR_ANY;	// mon addresse
	memset( &( srv_addr.sin_zero ), '\0', 8 );	// mise a 0

	if( bind( sock, ( struct sockaddr * ) &srv_addr, sizeof( struct sockaddr ) ) == -1 ) {
		printf("\n bind : error \n");
		exit(-1);
	}

	if( listen( sock, max_clients ) == -1 ) {
		printf("\n listen : error\n");
		exit(-1);
	}

	cout << "OK";

	while(true) {
		cout << "\n Attente de connexion d'un client ...." << endl;

		if((sockCHL=accept(sock,(struct sockaddr *) & cl_addr,(socklen_t*)&size))== -1) {
			printf("\naccept:erreur\n");
			exit(-1);
		}

		if( !fork() ) {
			cout << "\n A Client is connected" << endl;

			while(1) {
				/* Surveiller les entrees */
				FD_ZERO( &rfds );
				FD_SET( sockCHL, &rfds ); /* socket  */
				FD_SET( 0, &rfds ); 	/* clavier */
				tv.tv_sec  = 0; 		/* temps */
				tv.tv_usec = 0;

				retval = select( sockCHL + 1, &rfds, NULL, NULL, &tv);
				/* Considérer tv comme indéfini maintenant ! */
				switch( retval ) {
					case -1:
						printf("\n select : error \n");
						exit(-1);
						break;

					default:
						if(FD_ISSET( sockCHL, &rfds ) ) {

							if( (recv(sockCHL,msg,sizeof(msg),0 )) == -1) {
								printf("\nrecv:erreur\n");
								exit(-1);
							}

							cout<<"\nClient dit :"<<msg;

							if( !strcmp( msg, "exit" ) ) {

								cout << "\n Disonnecting .... " << endl;
								close( sockCHL );

								cout << "OK\n" << endl;
								exit(0);
							}
							break;
						}
						if( FD_ISSET( 0, &rfds ) ) {

							cin.getline(msg,256);
						}
						if( ( send( sockCHL, msg, sizeof( msg ), 0 ) ) == -1 ) {

							printf("\n send : error \n");
							exit(-1);
						}
						if( !strcmp( msg, "exit" ) ) {

							cout << "\n Disonnecting ...." << endl;
							close( sockCHL) ;

							cout << "OK\n" << endl;
							exit(0);
						}
						if( !strcmp( msg, "clear" ) ) {

							system("clear");
						}
						break;
					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}
