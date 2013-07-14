#ifndef EZIO_HH
#define EZIO_HH

#include <string>

std::string num2str( const int num );
std::string readall( const int fd );
void writeall( const int fd, const std::string & buf );

#endif /* EZIO_HH */
