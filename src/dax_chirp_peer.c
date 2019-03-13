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
    zyre_t* zyre;
    dax_chirp_msg_t *message;   //  Message to/from other peer
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
s_slurp(zsock_t* sock)
{
    zmsg_t* msg = zmsg_recv(sock);
    if (!msg) return;
    char* first = zmsg_popstr(msg);
    if (first) {
        zsys_debug("msg to API: [%s]", first);
        free(first);
    }
    zmsg_destroy(&msg);
}

void
dax_chirp_peer_test (bool verbose)
{
    printf (" * dax_chirp_peer: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    // TODO: fill this out
    dax_chirp_peer_t *peer1 = dax_chirp_peer_new ("peer1");
    assert (peer1);
    dax_chirp_peer_set_verbose(peer1, verbose);

    // simulate some application "work"
    //zclock_sleep(100);

    dax_chirp_peer_t *peer2 = dax_chirp_peer_new ("peer2");
    assert (peer2);
    dax_chirp_peer_set_verbose(peer2, verbose);

    // simulate some application "work"
    //zclock_sleep(100);

    dax_chirp_peer_t *peer3 = dax_chirp_peer_new ("peer3");
    assert (peer3);
    dax_chirp_peer_set_verbose(peer3, verbose);

    // simulate some application "work"
    zclock_sleep(100);

    zactor_t *a1 = dax_chirp_peer_actor(peer1);
    zactor_t *a2 = dax_chirp_peer_actor(peer2);
    zactor_t *a3 = dax_chirp_peer_actor(peer3);

    assert (a1 && a2 && a3);

    zsock_t* p1 = dax_chirp_peer_msgpipe(peer1);
    zsock_t* p2 = dax_chirp_peer_msgpipe(peer2);
    zsock_t* p3 = dax_chirp_peer_msgpipe(peer3);

    assert (p1 && p2 && p3);

    zpoller_t* poller = zpoller_new(zactor_sock(a1),
                                    zactor_sock(a2),
                                    zactor_sock(a3),
                                    p1,p2,p3,NULL);

    while (!zsys_interrupted) {
        void* which = zpoller_wait(poller, 1000);
        if (!which) break;
        s_slurp((zsock_t*)which);
    }

    dax_chirp_peer_destroy (&peer1);
    // simulate some application "work"
    //zclock_sleep(100);

    dax_chirp_peer_destroy (&peer2);
    dax_chirp_peer_destroy (&peer3);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  action method generate_chat_message
//

static void
generate_chat_message (peer_t *self)
{
    zsys_debug("action generate_chat_message not implemented");
    dax_chirp_msg_set_message(self->message, "this is generated");
}


//  ---------------------------------------------------------------------------
//  action method output_chat_message
//

static void
output_chat_message (peer_t *self)
{
    zsys_debug("action output_chat_message not implemented");
    dax_chirp_msg_print(self->message);
}
