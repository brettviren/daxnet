/*  =========================================================================
    dax_timeline_client - DAX Timeline Client

    LGPL3, boilerplate to come.
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
#include "../include/dax_timeline_codec.h"
#include "../include/dax_timeline_client.h"

//  Forward reference to method arguments structure
typedef struct _client_args_t client_args_t;

//  This structure defines the context for a client connection
typedef struct {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine. The cmdpipe gets
    //  messages sent to the actor; the msgpipe may be used for
    //  faster asynchronous message flows.
    zsock_t *cmdpipe;           //  Command pipe to/from caller API
    zsock_t *msgpipe;           //  Message pipe to/from caller API
    zsock_t *dealer;            //  Socket to talk to server
    dax_timeline_codec_t *message;  //  Message to/from server
    client_args_t *args;        //  Arguments from methods

    //  TODO: Add specific properties for your application
} client_t;

//  Include the generated client engine
#include "dax_timeline_client_engine.inc"

//  Allocate properties and structures for a new client instance.
//  Return 0 if OK, -1 if failed

static int
client_initialize (client_t *self)
{
    return 0;
}

//  Free properties and structures for a client instance

static void
client_terminate (client_t *self)
{
    //  Destroy properties here
}


//  ---------------------------------------------------------------------------
//  Selftest

void
dax_timeline_client_test (bool verbose)
{
    printf (" * dax_timeline_client: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    // TODO: fill this out
    dax_timeline_client_t *client = dax_timeline_client_new ();
    dax_timeline_client_set_verbose(client, verbose);
    dax_timeline_client_destroy (&client);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  connect_to_server
//

static void
connect_to_server (client_t *self)
{
}


//  ---------------------------------------------------------------------------
//  forward_timeline
//

static void
forward_timeline (client_t *self)
{
}


//  ---------------------------------------------------------------------------
//  signal_internal_error
//

static void
signal_internal_error (client_t *self)
{
}
