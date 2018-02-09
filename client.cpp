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

#define MY_PORT 3000

int main( int argc, char ** argv )
{
	if ( argc < 1 ) {

		cout<<"\n Missing IP Adress (x.x.x.x) " << endl;
		exit(EXIT_SUCCESS);
	}

	/** buffer, sent message */
	char msg[256];
	/** socket handler */
	int  sock;
	int  retval;

	fd_set rfds;

	/** sockets, server and client */
	struct sockaddr_in srv_addr;
	struct sockaddr_in cl_addr;
	struct timeval     tv;

	/** socket reserved */
	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {

		printf("\n socket : error \n");
		exit(-1);
	}

	/** socket type sys socket*/
	srv_addr.sin_family      = AF_INET;

	/** socket port */
	srv_addr.sin_port        = htons(MY_PORT);

	/** socket address */
	srv_addr.sin_addr.s_addr = inet_addr(argv[1]);

	/** set to zero */
	memset( &( srv_addr.sin_zero ), '\0', 8 );

	cout<<"Server connection (" << argv[1] << ")...";

	/** co necting to server */
	if( ( connect( sock, ( struct sockaddr * ) &srv_addr, sizeof( struct sockaddr ) ) ) == -1 )
	{
		printf("\n connect : error \n");
		exit(-1);
	};

	cout<<"\n OK " << endl;

	while(1) {

	/* Monitoring entries sys types */
	FD_ZERO( &rfds );

	/** sockets */
	FD_SET( sock, &rfds ); /* socket */

	/** keyboard inputs */
	FD_SET( 0, &rfds );

	/** time */
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	retval = select( sock+1, &rfds, NULL, NULL, &tv );

		switch( retval ) {

			case -1:
				printf("\n select : error \n");
				exit(-1);
				break;

			default:
			if( FD_ISSET( sock, &rfds ) ) {

				if( ( recv( sock, msg, sizeof( msg ), 0 ) ) == -1 ) {

					printf("\n recv : error \n");
					exit(-1);
				}

				//cout << "\n Server says : " << msg << endl;

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
					if( !strcmp( msg, "exit" ) ) {

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
