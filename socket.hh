#ifndef SOCKET_HH
#define SOCKET_HH

class Socket
{
private:
  int fd_;

public:
  Socket();
  ~Socket();

  int fd( void ) { return fd_; }
};

#endif
