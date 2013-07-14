#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main( int argc, char *argv[] )
{
  /* check arguments */
  if ( argc == 0 ) {
    fprintf( stderr, "ERROR: argv[ 0 ] not provided.\n" );
    exit( EXIT_FAILURE );
  } else if ( argc != 3 ) {
    fprintf( stderr, "Usage: %s HOSTNAME SERVICE\n", argv[ 0 ] );
    exit( EXIT_FAILURE );
  }

  const char *hostname = argv[ 1 ];
  const char *service = argv[ 2 ];

  fprintf( stderr, "Looking up %s... \n", hostname );

  /* hints to guide the resolver */
  struct addrinfo hints;
  memset( &hints, 0, sizeof( struct addrinfo ) );

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  /* the answer */
  struct addrinfo *res;

  /* look up the name or names */
  int gai_ret = getaddrinfo( hostname, service, &hints, &res );
  if ( gai_ret != 0 ) {
    fprintf( stderr, "ERROR: getaddrinfo: %s\n", gai_strerror( gai_ret ) );
    perror( "getaddrinfo" );
    exit( EXIT_FAILURE );
  }

  if ( res == NULL ) {
    /* shouldn't be able to happen */
    fprintf( stderr, "Error: no addresses returned.\n" );
    exit( EXIT_FAILURE );
  }
  
  /* make sure it's an Internet address */
  if ( res->ai_family != AF_INET ) {
    fprintf( stderr, "Bad family.\n" );
    exit( EXIT_FAILURE );
  } else if ( res->ai_socktype != SOCK_STREAM ) {
    fprintf( stderr, "Bad socket type.\n" );
    exit( EXIT_FAILURE );
  } else if ( res->ai_addrlen != sizeof( struct sockaddr_in ) ) {
    fprintf( stderr, "ERROR, res->ai_addrlen = %d, expected %lu.\n",
	     res->ai_addrlen, sizeof( struct sockaddr_in ) );
    exit( EXIT_FAILURE );
  }

  /* we will connect to first address in list */
  struct sockaddr_in *address_ptr = reinterpret_cast<sockaddr_in *>( res->ai_addr );
  fprintf( stderr, "Got an address: %s:%d\n",
	   inet_ntoa( address_ptr->sin_addr ),
	   ntohs( address_ptr->sin_port ) );

  /* create TCP socket */
  int connect_socket = socket( AF_INET, SOCK_STREAM, 0 );
  if ( connect_socket < 0 ) {
    perror( "socket" );
    exit( EXIT_FAILURE );
  }

  /* connect to server */
  if ( connect( connect_socket, res->ai_addr, res->ai_addrlen ) < 0 ) {
    perror( "connect" );
    exit( EXIT_FAILURE );
  }

  /* make an HTTP request */
  

  if ( close( connect_socket ) < 0 ) {
    perror( "close" );
    exit( EXIT_FAILURE );
  }

  freeaddrinfo( res );
  return 0;
}
