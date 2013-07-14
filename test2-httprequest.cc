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

/* Convert string to integer with error checking */
long int myatoi( const char *str )
{
  char *end;

  errno = 0;
  long int ret = strtol( str, &end, 10 );

  if ( ( errno != 0 )
       || ( end != str + strlen( str ) ) ) {
    fprintf( stderr, "Bad integer [%s].\n", str );
    exit( EXIT_FAILURE );
  }

  return ret;
}

/* what port we want */
const char service[] = "http";

int main( int argc, char *argv[] )
{
  /* check arguments */
  if ( argc == 0 ) {
    fprintf( stderr, "ERROR: argv[ 0 ] not provided.\n" );
    exit( EXIT_FAILURE );
  } else if ( argc != 3 ) {
    fprintf( stderr, "Usage: %s HOSTNAME PORT\n", argv[ 0 ] );
    exit( EXIT_FAILURE );
  }

  const char *hostname = argv[ 1 ];
  const char *port = argv[ 2 ];

  /* first, convert the port */
  const long int port_integer = myatoi( port );

  if ( (port_integer <= 0) || port_integer > USHRT_MAX ) {
    fprintf( stderr, "Bad port number [%ld].\n", port_integer );
    exit( EXIT_FAILURE );
  }

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
    fprintf( stderr, "Error: no addresses returned.\n" );
    exit( EXIT_FAILURE );
  }

  /* print out the addresses */
  while ( res ) {
    /* print out the first one */
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

    struct sockaddr_in *address_ptr = reinterpret_cast<sockaddr_in *>( res->ai_addr );
    fprintf( stderr, "Got an address: %s\n", inet_ntoa( address_ptr->sin_addr ) );

    res = res->ai_next;
  }

  freeaddrinfo( res );

  return 0;
}
