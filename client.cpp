#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define MY_PORT 7000

int main (int argc,char ** argv )
{
	if ( argc < 1 )
	{
		cout<<"\n Parametre manquant,adresse ip format: x.x.x.x\n";
		exit(EXIT_SUCCESS);
	}

	char msg[256];			// buffer
	int  sock;		// handles de socket
	int  retval;

	fd_set rfds,wfds;

	struct sockaddr_in srv_addr;
	struct sockaddr_in cl_addr;
	struct timeval     tv;

	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )	// reserve socket
	{
		printf("\n socket : error \n");
		exit(-1);
	}

	srv_addr.sin_family      = AF_INET;		// type
	srv_addr.sin_port        = htons(MY_PORT);	// port 3000 (arbitraire)
	srv_addr.sin_addr.s_addr = inet_addr(argv[1]);	// mon addresse
	memset( &( srv_addr.sin_zero ), '\0', 8 );	// mise a 0

	cout<<"Server connection (" << argv[1] << ")...";

	// connecte au serveur
	if( ( connect( sock, ( struct sockaddr * ) &srv_addr, sizeof( struct sockaddr ) ) ) == -1 )
	{
		printf("\n connect : error \n");
		exit(-1);
	};

	cout<<" OK \n";

	while(1) {
	/* Surveiller les entrées */
	FD_ZERO( &rfds );
	FD_SET( sock, &rfds ); /* socket */
	FD_SET( 0, &rfds );    /* clavier */
	tv.tv_sec = 0; 	     /* temps */
	tv.tv_usec = 0;
	retval = select( sock+1, &rfds, NULL, NULL, &tv );
	/* Considérer tv comme indéfini maintenant ! */

		switch( retval ) {

			case -1:
				printf("\n select : error \n");
				exit(-1);
				break;

			default:

				if( FD_ISSET( sock, &rfds ) ) {

					if( (recv( sock, msg, sizeof( msg ), 0 ) ) == -1) {

						printf("\n recv : error \n");
						exit(-1);
					}

					cout<<"\n Server says : " << msg;

					if( !strcmp( msg, "exit" ) ) {
						goto quit;
					}
					break;
				}
				if( FD_ISSET( 0, &rfds ) ) {

					cin.getline( msg, 256 );

				  if( ( send( sock, msg, sizeof( msg ), 0 ) ) == -1) {

						printf("\n send : error \n");
						exit(-1);
					}
					if( !strcmp( msg,"exit" ) ) {

						goto quit;
					}
				  if( !strcmp( msg, "clear" ) ) {

						system("clear");
					}
					break;
				}
			}
	}

	quit:
		cout<<"\n Disconnecting ....";
		close(sock);
		cout<<" Disconnected !\n";

	return EXIT_SUCCESS;
}
