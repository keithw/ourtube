#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>

#include "writeall.hh"
#include "socket.hh"
#include "exception.hh"

using namespace std;

/* port for server to listen for connections */
const string listen_service = "8080";

int main( void )
{
  try {
    /* create TCP socket */
    Socket listener_socket;

    /* bind socket */
    listener_socket.bind( Address( "0", listen_service ) );
    fprintf( stderr, "Bound to %s.\n", listener_socket.local_addr().str().c_str() );

    /* mark the socket for listening */
    listener_socket.listen();

    /* wait for client connection */
    Socket client_socket = listener_socket.accept();
    fprintf( stderr, "Got connection from %s.\n",
	     client_socket.peer_addr().str().c_str() );

    /* read the request from the client */
    while ( 1 ) {
      string buffer = client_socket.read();
      if ( buffer.empty() ) {
	break;
      }
      writeall( STDOUT_FILENO, buffer );
    }

    fprintf( stderr, "Successful exit.\n" );
  } catch ( Exception &e ) {
    e.die();
  }

  return 0;
}
