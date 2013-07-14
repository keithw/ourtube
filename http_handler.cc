#include <poll.h>

#include "exception.hh"
#include "http_handler.hh"

using namespace std;

void HTTPHandler::handle_request( void )
{
  try {
    /* parse up to the host header */
    read_request_up_to_host_header();

    /* open connection to server */
    connect_to_server();

    /* write pending data from client to server */
    server_socket_.write( pending_client_to_server_ );

    /* ferry data in both directions until EOF from either */
    two_way_connection();
  } catch ( const Exception &e ) {
    e.die();
  }
}

void HTTPHandler::read_request_up_to_host_header( void )
{
  while ( 1 ) {
    string buffer = client_socket_.read();
    if ( buffer.empty() ) { client_eof_ = true; return; }

    /* save this data for later replay to server */
    pending_client_to_server_ += buffer;

    parser_.parse( buffer );
    if ( parser_.has_header( "Host" ) ) {
      /* found it */
      return;
    }
  }
}

void HTTPHandler::connect_to_server( void )
{
  /* find host and service */
  string header_value = parser_.get_header_value( "Host" );

  /* split value into host and (possible) port */
  size_t colon_location = header_value.find( ":" );

  string host, service;
  if ( colon_location == std::string::npos ) {
    host = header_value;
    service = "http";
  } else {
    host = header_value.substr( 0, colon_location );
    service = header_value.substr( colon_location + 1 );
  }

  Address server_addr( host, service );
  fprintf( stderr, "Opening connection to %s... ",
	   server_addr.str().c_str() );

  server_socket_.connect( server_addr );

  fprintf( stderr, "done.\n" );
}

void HTTPHandler::two_way_connection( void )
{
  struct pollfd pollfds[ 2 ];
  pollfds[ 0 ].fd = client_socket_.raw_fd();
  
  pollfds[ 1 ].fd = server_socket_.raw_fd();

  while ( 1 ) {
    if ( client_eof_ && server_eof_ ) {
      break;
    }

    pollfds[ 0 ].events = client_eof_ ? 0 : POLLIN;
    pollfds[ 1 ].events = server_eof_ ? 0 : POLLIN;

    /* wait for data in either direction */
    if ( poll( pollfds, 2, -1 ) <= 0 ) {
      throw Exception( "poll" );
    }

    if ( pollfds[ 0 ].revents & POLLIN ) {
      /* data available from client */
      /* send to server */
      string buffer = client_socket_.read();
      if ( buffer.empty() ) {
	client_eof_ = true;
      } else {
	server_socket_.write( buffer );
      }
    }

    if ( pollfds[ 1 ].revents & POLLIN ) {
      /* data available from server */
      /* send to client */
      string buffer = server_socket_.read();
      if ( buffer.empty() ) {
	server_eof_ = true;
      } else {
	client_socket_.write( buffer );
      }
    }

    if ( (pollfds[ 0 ].revents & POLLERR)
	 || (pollfds[ 1 ].revents & POLLERR) ) {
      throw Exception( "poll error" );
    }
  }
}
