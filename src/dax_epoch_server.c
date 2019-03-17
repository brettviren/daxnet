/*  =========================================================================
    dax_epoch_server - dax_epoch_server

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
#include "../include/dax_epoch_server.h"

//  ---------------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;

//  This structure defines the context for each running server. Store
//  whatever properties and structures you need for the server.

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zsock_t *pipe;              //  Actor pipe back to caller
    zconfig_t *config;          //  Current loaded configuration

    //  TODO: Add any properties you need here
    zsock_t* pub;               /* Socket to send epoch debuts and rescinds */
    const char* pub_endpoint;   /* its endpoint */
    int last_epoch;             /* number of last epoch to see/send */
};

//  ---------------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    dax_epoch_t *message;       //  Message in and out
    uint  unique_id;            //  Client identifier (for correlation purpose with the engine)

    //  TODO: Add specific properties for your application
};

//  Include the generated server engine
#include "dax_epoch_server_engine.inc"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    self->pub = zsock_new_pub("tcp://*:*");
    assert(self->pub);
    self->pub_endpoint = zsock_endpoint(self->pub);
    
    self->last_epoch = -1;

    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    zsock_destroy(&self->pub);
    self->pub_endpoint = NULL;
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    zmsg_t *reply = NULL;
    if (streq(method, "DEBUT")) {

    }

    return NULL;
}

//  Apply new configuration.

static void
server_configuration (server_t *self, zconfig_t *config)
{
    ZPROTO_UNUSED(self);
    ZPROTO_UNUSED(config);
    //  Apply new configuration
}

//  Allocate properties and structures for a new client connection and
//  optionally engine_set_next_event (). Return 0 if OK, or -1 on error.

static int
client_initialize (client_t *self)
{
    ZPROTO_UNUSED(self);
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a client connection

static void
client_terminate (client_t *self)
{
    ZPROTO_UNUSED(self);
    //  Destroy properties here
}

//  ---------------------------------------------------------------------------
//  Selftest

void
dax_epoch_server_test (bool verbose)
{
    printf (" * dax_epoch_server: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zactor_t *server = zactor_new (dax_epoch_server, "server");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "BIND", "ipc://@/dax_epoch_server", NULL);

    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    zsock_connect (client, "ipc://@/dax_epoch_server");

    //  TODO: fill this out
    dax_epoch_t *request = dax_epoch_new ();
    dax_epoch_destroy (&request);

    zsock_destroy (&client);
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}


//  ---------------------------------------------------------------------------
//  send_timeline
//

static void
send_timeline (client_t *self)
{
    
}


//  ---------------------------------------------------------------------------
//  signal_command_invalid
//

static void
signal_command_invalid (client_t *self)
{

}
