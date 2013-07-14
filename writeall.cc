#include <unistd.h>

#include "writeall.hh"

using namespace std;

/* blocking write of entire buffer */
static ssize_t writeall( const int fd, const char *buffer, const size_t count )
{
  size_t total_bytes_written = 0;

  while ( total_bytes_written < count ) {
    ssize_t bytes_written = write( fd,
				   buffer + total_bytes_written,
				   count - total_bytes_written );

    if ( bytes_written < 0 ) {
      return bytes_written;
    } else {
      total_bytes_written += bytes_written;
    }
  }

  return total_bytes_written;
}

void writeall( const int fd, const string buf )
{
  if ( writeall( fd, buf.data(), buf.size() ) < 0 ) {
    perror( "writeall" );
    throw errno;
  }
}
