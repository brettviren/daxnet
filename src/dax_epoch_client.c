/*  =========================================================================
    dax_epoch_client - DAX Epoch Client

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
#include "../include/dax_epoch.h"
#include "../include/dax_epoch_client.h"

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
    dax_epoch_t *message;       //  Message to/from server
    client_args_t *args;        //  Arguments from methods

    zsock_t* sub;               /* listen for updates */

} client_t;

//  Include the generated client engine
#include "dax_epoch_client_engine.inc"


// Handle messages on the SUB socket

static int
s_client_handle_publish (zloop_t *loop, zsock_t *reader, void *argument)
{
    client_t *self = (client_t *) argument;

    // call:
    // engine_set_next_event (self, new_epoch);

    return 0;
}

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
    zsock_destroy(&self->sub);
}


//  ---------------------------------------------------------------------------
//  Selftest

void
dax_epoch_client_test (bool verbose)
{
    printf (" * dax_epoch_client: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    // TODO: fill this out
    dax_epoch_client_t *client = dax_epoch_client_new ();
    dax_epoch_client_set_verbose(client, verbose);
    dax_epoch_client_destroy (&client);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  connect_to_server
//

static void
connect_to_server (client_t *self)
{
    // This is called between connect and the automated call for STATUS
}


//  ---------------------------------------------------------------------------
//  new_timeline
//

static void
new_timeline (client_t *self)
{
//    zsock_send(self->msgpipe, "sp", "TIMELINE", dax_epoch_timeline......);
}


//  ---------------------------------------------------------------------------
//  subscribe
//

static void
subscribe (client_t *self)
{
    /* self->sub = zsock_new_sub(self->args->publisher); */
    /* if (!self->sub) { */
    /*     return -1; */
    /* } */
    /* engine_handle_socket (self, self->sub, s_client_handle_publish); */
}


//  ---------------------------------------------------------------------------
//  forward_debut
//

static void
forward_debut (client_t *self)
{
//    zsock_send(self->msgpipe, "sp", "DEBUT", ....);
}


//  ---------------------------------------------------------------------------
//  forward_rescind
//

static void
forward_rescind (client_t *self)
{
//    zsock_send(self->msgpipe, "sp", "RESCIND", ....);
}
