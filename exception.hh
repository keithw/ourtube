#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <string>
#include <iostream>
#include <unistd.h>

class Exception
{
private:
  std::string attempt_, error_;

public:
  Exception( const std::string s_attempt, const std::string s_error )
    : attempt_( s_attempt ), error_( s_error )
  {}

  void die( void )
  {
    std::cerr << "Exception: " << attempt_ << ": " << error_ << std::endl;
    exit( EXIT_FAILURE );
  }
};

#endif
