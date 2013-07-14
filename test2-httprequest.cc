#include "ezio.hh"
#include "address.hh"
#include "socket.hh"
#include "exception.hh"

using namespace std;

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

  try {
    /* resolve server address */
    Address server_address( hostname, service );

    /* create TCP socket */
    Socket server_socket;

    /* connect to server */
    server_socket.connect( server_address );

    /* make an HTTP request */
    server_socket.write( "GET / HTTP/1.1\r\n" );
    server_socket.write( string( "Host: " ) + hostname + ":" + to_string( server_address.port() ) + "\r\n\r\n" );

    /* read and print the response */
    while ( 1 ) {
      string buffer = server_socket.read();
      if ( buffer.empty() ) { break; } /* EOF */
      writeall( STDOUT_FILENO, buffer );
    }
  } catch ( Exception &e ) {
    e.die();
  }

  return 0;
}
