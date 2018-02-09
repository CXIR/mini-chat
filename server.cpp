#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#define MY_PORT 3000

int main( int argc, char **argv )
{
	//strcpy(msg,"malik\0");

	/** sockets handlers */
	int  sock;
	int  socket_client;

	/** buffer (incoming message */
	char msg[256];

	int  yes         = 1;
	int  max_clients = 20;
	int  retval;

	fd_set rfds,wfds;

	struct timeval tv;

	/** adresses: server and client */
	struct sockaddr_in srv_addr;
	struct sockaddr_in cl_addr;

	int size = sizeof( struct sockaddr_in );

	cout << "\n Starting Server .... " << endl;

	/** reserved socket */
	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )	 {

		printf( "\n socket : error \n" );
		exit(-1);
	}
	if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &yes , sizeof( int ) ) == -1 ) {

		printf( "\n set sock option : error \n" );
		exit(-1);
	}

	/** server socket type */
	srv_addr.sin_family      = AF_INET;

	/** server socket port */
	srv_addr.sin_port        = htons(MY_PORT);

	/** server socket address */
	srv_addr.sin_addr.s_addr = INADDR_ANY;

	/** set to zero */
	memset( &( srv_addr.sin_zero ), '\0', 8 );

	if( bind( sock, ( struct sockaddr * ) &srv_addr, sizeof( struct sockaddr ) ) == -1 ) {

		printf("\n bind : error \n");
		exit(-1);
	}

	if( listen( sock, max_clients ) == -1 ) {

		printf("\n listen : error\n");
		exit(-1);
	}

	cout << "\n OK " << endl;

	while(true) {
		cout << "\n Waiting for a client ...." << endl;

		if( ( socket_client = accept( sock, (struct sockaddr *) & cl_addr,( socklen_t* ) &size ) ) == -1 ) {

			printf("\n accept : error \n");
			exit(-1);
		}

		if( !fork() ) {

			cout << "\n A Client is connected " << endl;

			while(true) {

				/** Monitoring entries */
				FD_ZERO( &rfds );

				/** Monitoring sockets */
				FD_SET( socket_client, &rfds );

				/** monitoring keyboards */
				FD_SET( 0, &rfds );

				/** time */
				tv.tv_sec  = 0;
				tv.tv_usec = 0;

				retval = select( socket_client + 1, &rfds, NULL, NULL, &tv );

				switch( retval ) {

					case -1:
						printf("\n select : error \n");
						exit(-1);
						break;

					default:
						if( FD_ISSET( socket_client, &rfds ) ) {

							if( ( recv( socket_client, msg, sizeof( msg ), 0 ) ) == -1 ) {

								printf("\n recv : error \n");
								exit(-1);
							}

							cout << "\n Client says : " << msg << endl;

							if( !strcmp( msg, "exit" ) ) {

								cout << "\n Disconnecting .... " << endl;
								close( socket_client );

								cout << "OK\n" << endl;
								exit(0);
							}
							break;
						}
						if( FD_ISSET( 0, &rfds ) ) {

							cin.getline( msg, 256 );
						}
						if( ( send( socket_client, msg, sizeof( msg ), 0 ) ) == -1 ) {

							printf("\n send : error \n");
							exit(-1);
						}
						if( !strcmp( msg, "exit" ) ) {

							cout << "\n Disonnecting ...." << endl;
							close( socket_client) ;

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
	return EXIT_SUCCESS;
}
