#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

#include "writeall.hh"
#include "socket.hh"

using namespace std;

/* port for server to listen for connections */
const string listen_service = "8080";

int main( void )
{
  /* create TCP socket */
  Socket listener_socket;

  /* bind socket */
  listener_socket.bind( listen_service );

  /* mark the socket for listening */
  listener_socket.listen();

  /* make new socket address for connection */
  struct sockaddr_in client_addr;
  socklen_t client_addr_size = sizeof( client_addr );

  /* wait for client connection */
  int client_socket = accept( listener_socket.fd(),
			      reinterpret_cast<sockaddr *>( &client_addr ),
			      &client_addr_size );

  if ( client_socket < 0 ) {
    perror( "accept" );
    exit( EXIT_FAILURE );
  }

  if ( client_addr_size != sizeof( client_addr ) ) {
    fprintf( stderr, "ERROR, client_addr_size = %u, expected %lu.\n",
	     client_addr_size, sizeof( client_addr ) );
    exit( EXIT_FAILURE );
  }

  /* we got a successful connection */
  fprintf( stderr, "Got connection from %s:%d.\n",
	   inet_ntoa( client_addr.sin_addr ),
	   ntohs( client_addr.sin_port ) );

  /* read the request from the client */
  while ( 1 ) {
    const size_t buffer_size = 4096;
    char buffer[ buffer_size ];

    ssize_t bytes_read = read( client_socket, &buffer, buffer_size );
    if ( bytes_read == 0 ) {
      /* end of file = client has closed their side of connection */
      fprintf( stderr, "End of file, closing connection.\n" );
      break;
    } else if ( bytes_read < 0 ) {
      perror( "read" );
      break;
    } else {
      /* successful read */
      /* write to the terminal */
      writeall( STDOUT_FILENO, string( buffer, bytes_read ) );
    }
  }

  /* close client socket */
  if ( close( client_socket ) < 0 ) {
    perror( "close" );
    exit( EXIT_FAILURE );
  }

  fprintf( stderr, "Successful exit.\n" );

  return 0;
}
