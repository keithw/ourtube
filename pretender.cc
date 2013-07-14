#include <unistd.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <poll.h>
#include <sys/wait.h>

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

    /* make signal file descriptor */
    sigset_t signal_mask;
    sigemptyset( &signal_mask );
    sigaddset( &signal_mask, SIGCHLD );
    sigaddset( &signal_mask, SIGPIPE );

    int signal_fd = signalfd( -1, &signal_mask, 0 );
    if ( signal_fd < 0 ) {
      throw Exception( "signalfd" );
    } else if ( sigprocmask( SIG_BLOCK, &signal_mask, NULL ) < 0 ) {
      throw Exception( "sigprocmask" );
    }

    /* set up poll */
    struct pollfd pollfds[ 2 ];
    pollfds[ 0 ].fd = listener_socket.raw_fd();
    pollfds[ 0 ].events = POLLIN;

    pollfds[ 1 ].fd = signal_fd;
    pollfds[ 1 ].events = POLLIN;

    while ( 1 ) {
      /* wait for client OR signal */
      if ( poll( pollfds, 2, -1 ) <= 0 ) {
	throw Exception( "poll" );
      }

      if ( pollfds[ 0 ].revents & POLLIN ) { /* new client has connected */
	Socket client_socket = listener_socket.accept();
	fprintf( stderr, "Got connection from %s.\n", client_socket.peer_addr().str().c_str() );

	/* fork client handler */
	pid_t pid_of_child = fork();
	if ( pid_of_child < 0 ) {
	  throw Exception( "fork" );
	} else if ( pid_of_child == 0 ) {
	  /* we're the child */
	  //	  handle_request( client_socket );
	  fprintf( stderr, "Child process %d exiting now.\n", getpid() );
	  exit( EXIT_SUCCESS );
	}
      }

      if ( pollfds[ 1 ].revents & POLLIN ) { /* we have a signal */
	signalfd_siginfo delivered_signal;

	if ( read( signal_fd, &delivered_signal, sizeof( signalfd_siginfo ) )
	     != sizeof( signalfd_siginfo ) ) {
	  throw Exception( "read size mismatch" );
	}

	if ( delivered_signal.ssi_signo == SIGCHLD ) {
	  /* reap dead child */
	  pid_t dead_child = waitpid( -1, NULL, 0 );
	  if ( dead_child < 0 ) {
	    throw Exception( "waitpid" );
	  } else if ( dead_child > 0 ) {
	    fprintf( stderr, "Waited for child pid=%d\n", dead_child );
	  }
	}
      }

      if ( (pollfds[ 0 ].revents & POLLERR)
	   || (pollfds[ 1 ].revents & POLLERR) ) {
	throw Exception( "poll error" );
      }
    }
  } catch ( const Exception &e ) {
    e.die();
  }
}
