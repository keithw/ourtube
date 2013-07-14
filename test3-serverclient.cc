#include "socket.hh"
#include "exception.hh"

using namespace std;

/* port for server to listen for connections */
const string listen_service = "http-alt";

int main( void )
{
  try {
    /* create listener socket */
    Socket listener_socket;

    /* bind socket */
    listener_socket.bind( Address( "0", listen_service ) );
    fprintf( stderr, "Bound to %s.\n", listener_socket.local_addr().str().c_str() );

    /* mark the socket for listening */
    listener_socket.listen();

    /* wait for client */
    Socket client_socket = listener_socket.accept();

    /* read the request from the client */
    while( 1 ) {
      string buffer = client_socket.read();
      if ( buffer.empty() ) { break; } /* EOF */
      
      
    }

  } catch ( Exception &e ) {
    e.die();
  }

  return 0;
}
