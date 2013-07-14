#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

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

  if ( bind( listener_socket,
	     reinterpret_cast<sockaddr *>( &listen_addr ),
	     sizeof( listen_addr ) ) < 0 ) {
    perror( "bind" );
    exit( EXIT_FAILURE );
  }

  return 0;
}
