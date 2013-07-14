#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* port for server to listen for connections */
const uint16_t listen_port = 80;

int main( void )
{
  /* create TCP socket */
  int listener_socket = socket( AF_INET, SOCK_STREAM, 0 );
  if ( listener_socket < 0 ) {
    perror( "socket" );
    exit( EXIT_FAILURE );
  }

  /* make local address to listen on */
  struct sockaddr_in port80;
  port80.sin_family = AF_INET;
  port80.sin_port = htons( listen_port );
  port80.sin_addr.s_addr = INADDR_ANY;

  return 0;
}
