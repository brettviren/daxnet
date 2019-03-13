/*  =========================================================================
    dax_chirp_peer - DAX Chirp peer

    =========================================================================
*/

/*
@header
    Description of class for man page.
@discuss
    Detailed discussion of the class, if any.
@end
*/

#include "dax_classes.h"
//  TODO: Change these to match your project's needs
#include "../include/dax_chirp_msg.h"
#include "../include/dax_chirp_peer.h"

//  Forward reference to method arguments structure
typedef struct _peer_args_t peer_args_t;

//  This structure defines the context for a peer connection
typedef struct {
    //  These properties must always be present in the peer_t
    //  and are set by the generated engine. The cmdpipe gets
    //  messages sent to the actor; the msgpipe may be used for
    //  faster asynchronous message flows.
    zsock_t *cmdpipe;           //  Command pipe to/from caller API
    zsock_t *msgpipe;           //  Message pipe to/from caller API
    //zsock_t *dealer;            //  Socket to talk to server
    zyre_t* zyre;
    dax_chirp_msg_t *message;   //  Message to/from server
    peer_args_t *args;        //  Arguments from methods

    //  TODO: Add specific properties for your application
} peer_t;

//  Include the generated peer engine
#include "dax_chirp_peer_engine.inc"

//  Allocate properties and structures for a new peer instance.
//  Return 0 if OK, -1 if failed

static int
peer_initialize (peer_t *self)
{
    return 0;
}

//  Free properties and structures for a peer instance

static void
peer_terminate (peer_t *self)
{
    //  Destroy properties here
}


//  ---------------------------------------------------------------------------
//  Selftest

void
dax_chirp_peer_test (bool verbose)
{
    printf (" * dax_chirp_peer: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    // TODO: fill this out
    dax_chirp_peer_t *peer1 = dax_chirp_peer_new ("test1");
    dax_chirp_peer_t *peer2 = dax_chirp_peer_new ("test2");
    dax_chirp_peer_set_verbose(peer1, verbose);
    dax_chirp_peer_set_verbose(peer2, verbose);

    // simulate some application "work"
    zclock_sleep(2);

    dax_chirp_peer_destroy (&peer1);
    dax_chirp_peer_destroy (&peer2);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  join
//

static void
join (peer_t *self)
{
}
