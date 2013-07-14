#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* port for server to listen for connections */
const uint16_t listen_port = 8080;

int main( void )
{
  /* create TCP socket */
  int listener_socket = socket( AF_INET, SOCK_STREAM, 0 );
  if ( listener_socket < 0 ) {
    perror( "socket" );
    exit( EXIT_FAILURE );
  }

  /* make local address to listen on */
  struct sockaddr_in listen_addr;
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons( listen_port );
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  /* bind the socket to listen_addr */
  if ( bind( listener_socket,
	     reinterpret_cast<const sockaddr *>( &listen_addr ),
	     sizeof( listen_addr ) ) < 0 ) {
    perror( "bind" );
    exit( EXIT_FAILURE );
  }

  /* mark the socket for listening */
  if ( listen( listener_socket, 1 ) < 0 ) {
    perror( "listen" );
    exit( EXIT_FAILURE );
  }

  /* make new socket address for connection */
  struct sockaddr_in client_addr;
  socklen_t client_addr_size = sizeof( client_addr );

  fprintf( stderr, "Listening on %s:%d...",
	   inet_ntoa( listen_addr.sin_addr ),
	   ntohs( listen_addr.sin_port ) );

  /* wait for client connection */
  int client_socket = accept( listener_socket,
			      reinterpret_cast<sockaddr *>( &client_addr ),
			      &client_addr_size );

  return client_socket;
}
