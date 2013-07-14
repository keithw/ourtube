#include <sys/socket.h>
#include <string.h>

#include "socket.hh"
#include "exception.hh"

Socket::Socket()
  : fd_( socket( AF_INET, SOCK_STREAM, 0 ) )
{
  if ( fd_ < 0 ) {
    throw Exception( "socket", strerror( errno ) );
  }
}

Socket::~Socket()
{
  if ( close( fd_ ) < 0 ) {
    throw Exception( "close", strerror( errno ) );
  }
}
