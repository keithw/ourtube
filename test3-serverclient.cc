#include <poll.h>

#include "socket.hh"
#include "exception.hh"
#include "http_parser.hh"

using namespace std;

/* port for server to listen for connections */
const string listen_service = "http";

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
    fprintf( stderr, "Got connection from %s.\n", client_socket.peer_addr().str().c_str() );

    /* construct HTTP request parser */
    HTTPRequestParser parser;

    /* read the request from the client */
    string pending_client_to_server;
    bool eof = false;

    while( 1 ) {
      fprintf( stderr, "Reading... " );
      string buffer = client_socket.read();
      pending_client_to_server += buffer;
      if ( buffer.empty() ) { eof = true; break; } /* EOF */      

      parser.parse( buffer );
      if ( parser.has_header( "Host" ) ) { break; }
    }

    /* make connection to server */
    if ( eof ) {
      fprintf( stderr, "EOF.\n" );
      exit( EXIT_SUCCESS );
    }

    string host_value = parser.get_header_value( "Host" );
    /* split value into host and (possible) port */
    size_t colon_location = host_value.find( ":" );

    string host, service;
    if ( colon_location == std::string::npos ) {
      host = host_value;
      service = "http";
    } else {
      host = host_value.substr( 0, colon_location );
      service = host_value.substr( colon_location + 1 );
    }

    /* make address of server */
    fprintf( stderr, "Making connection to %s:%s\n", host.c_str(), service.c_str() );
    Address server_address( host, service );

    /* open socket to server */
    Socket server_socket;
    server_socket.connect( server_address );

    /* write the pending data */
    server_socket.write( pending_client_to_server );
    
    /* we've written the client's request,
       (possibly) ONLY up to the Host header
       to the server */

    /* now we need to write everything else the client
       prints to the server, and everything the server prints
       to the client */

    while ( 1 ) {
      /* we need to poll two sockets for possible data */
      struct pollfd pollfds[ 2 ];
      pollfds[ 0 ].fd = client_socket.raw_fd();
      pollfds[ 0 ].events = POLLIN;

      pollfds[ 1 ].fd = server_socket.raw_fd();
      pollfds[ 1 ].events = POLLIN;
      
      /* wait for data in either direction */
      if ( poll( pollfds, 2, -1 ) <= 0 ) {
	throw Exception( "poll" );
      }

      if ( pollfds[ 0 ].revents & POLLIN ) {
	/* data available from client */
	/* send to server */
	server_socket.write( client_socket.read() );
      }

      if ( pollfds[ 1 ].revents & POLLIN ) {
	/* data available from server */
	/* send to client */
	client_socket.write( server_socket.read() );
      }

      if ( (pollfds[ 0 ].revents & POLLERR)
	   || (pollfds[ 1 ].revents & POLLERR) ) {
	break;
      }
    }

  } catch ( Exception &e ) {
    e.die();
  }

  return 0;
}
