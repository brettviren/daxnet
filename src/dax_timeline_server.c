/*  =========================================================================
    dax_timeline_server - dax_timeline_server

    =========================================================================
*/

/*
@header
    Description of class for man page.
@discuss
    Detailed discussion of the class, if any.
@end
*/

#include "daxproto_classes.h"
//  TODO: Change these to match your project's needs
#include "../include/dax_timeline_codec.h"
#include "../include/dax_timeline_server.h"

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

    zsock_t* pub;               /* emit new timelines */
    dax_timeline_t* timeline;   /* current timeline */
};

//  ---------------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    dax_timeline_codec_t *message;  //  Message in and out
    uint  unique_id;            //  Client identifier (for correlation purpose with the engine)

    //  TODO: Add specific properties for your application
};

//  Include the generated server engine
#include "dax_timeline_server_engine.inc"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    // fixme: make default config file if missing.

    self->pub = zsock_new(ZMQ_PUB);
    self->timeline = NULL;

    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    if (self->timeline) {
        dax_timeline_destroy(&self->timeline);
    }
    zsock_destroy(&self->pub);
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    if (streq(method, "BIND PUB")) {
        char* endpoint = zmsg_popstr(msg);
        int port = zsock_bind(self->pub, "%s", endpoint);
        zmsg_t* reply = zmsg_new();
        if (port < 0) {
            zmsg_addstr(reply, "FAILURE");
            zmsg_addmem(reply, &port, sizeof(int));
        }
        else {
            zmsg_addstr(reply, zsock_endpoint(self->pub));
        }
        free(endpoint);
        return reply;

        if (self->timeline) {
            zsys_debug("Sending after PUB");
            dax_timeline_codec_t* tlmsg = dax_timeline_codec_new();
            dax_timeline_encode(self->timeline, tlmsg);
            dax_timeline_codec_send(tlmsg, self->pub);
            dax_timeline_codec_destroy(&tlmsg);
        }
    }

    if (streq(method, "NEW TIMELINE")) {
        zframe_t* frame = zmsg_pop(msg);
        if (!frame) {
            zsys_error("failed to send frame");
        }
        if (zframe_size(frame) != sizeof(void*)) {
            zsys_error("wrong size");
        }
        /* scary */
        dax_timeline_t* tl = *((dax_timeline_t**)zframe_data(frame));
        if (!tl) {
            zsys_error("failed to get pointer");
        }
        zframe_destroy(&frame);
        if (self->timeline) {
            dax_timeline_destroy(&self->timeline);
        }
        self->timeline = tl;
        if (self->pub) {

            dax_timeline_codec_t* tlmsg = dax_timeline_codec_new();
            dax_timeline_encode(self->timeline, tlmsg);
            dax_timeline_codec_send(tlmsg, self->pub);
            dax_timeline_codec_destroy(&tlmsg);
            zsys_debug("Published after NEW TIMELINE");

        }

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
dax_timeline_server_test (bool verbose)
{
    printf (" * dax_timeline_server: ");
    if (verbose)
        printf ("\n");

    zsock_t* sub = zsock_new_sub("ipc://@/dax_timeline_publisher","");
    assert(sub);

    zactor_t *server = zactor_new (dax_timeline_server, "server");
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "LOAD", "dax_timeline.cfg", NULL);
    zstr_sendx (server, "BIND", "ipc://@/dax_timeline_server", NULL);
    zstr_sendx (server, "BIND PUB", "ipc://@/dax_timeline_publisher", NULL);
    char* endpoint = zstr_recv(server);
    zsys_debug("PUB on %s", endpoint);
    free(endpoint);

    dax_timeline_t* tl = dax_timeline_new();
    dax_timeline_set_sot(tl, 1234);
    dax_timeline_set_ident(tl, 42);
    int epochs[4] = {39,40,41,42};
    int64_t times[4] = {2000, 3000, 4000, 5000};
    dax_timeline_set_intervals(tl, 4,epochs,times);
    zsys_debug("The test timeline");
    dax_timeline_print(tl);

    zsys_warning("SLEEPING FOR TEST");
    zclock_sleep(1000);

    zsys_debug("The test copy timeline");

    dax_timeline_t* tl2 = dax_timeline_dup(tl);
    dax_timeline_print(tl2);
    dax_timeline_destroy(&tl2);

    zsock_send(server, "sp", "NEW TIMELINE", tl);
    tl = NULL;
    
    zclock_sleep(1000);

    zsys_debug("Making codec for sub:");
    dax_timeline_codec_t* fromsub = dax_timeline_codec_new();
    assert(fromsub);
    zsys_debug("Try to receive from publisher:");
    zmsg_t* submsg = zmsg_recv(sub);
    zmsg_print(submsg);
    zmsg_destroy(&submsg);
    /* int rc = dax_timeline_codec_recv(fromsub, sub); */
    /* assert(rc == 0); */
    /* dax_timeline_t* tlfromsub = dax_timeline_decode(fromsub); */
    /* zsys_debug("From SUB:"); */
    /* dax_timeline_print(tlfromsub); */
    zsock_destroy(&sub);
    dax_timeline_codec_destroy(&fromsub);

    zsys_debug("Making dealer");
    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    zsock_connect (client, "ipc://@/dax_timeline_server");
    
    //  TODO: fill this out
    dax_timeline_codec_t *request = dax_timeline_codec_new ();
    dax_timeline_codec_set_id(request, DAX_TIMELINE_CODEC_SEND_TIMELINE);
    dax_timeline_codec_send(request, client);
    dax_timeline_codec_recv(request, client);
    assert (dax_timeline_codec_id(request) == DAX_TIMELINE_CODEC_TIMELINE);

    dax_timeline_t* got = dax_timeline_decode(request);
    zsys_debug("Got back:");
    dax_timeline_print(got);
    dax_timeline_destroy(&got);

    dax_timeline_codec_destroy (&request);

    zsock_destroy (&client);
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}



//  ---------------------------------------------------------------------------
//  signal_command_invalid
//

static void
signal_command_invalid (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  check_current
//

static void
check_current (client_t *self)
{
    if (!self->server->timeline) {
        dax_timeline_codec_set_status(self->message, DAX_TIMELINE_CODEC_NOT_FOUND);
        dax_timeline_codec_set_reason(self->message, "No timeline");
        engine_set_exception(self, no_timeline_event);
        return;
    }
    if (dax_timeline_codec_ident(self->message)
        != dax_timeline_ident(self->server->timeline)) {
        dax_timeline_encode(self->server->timeline, self->message);
    }
    zsys_debug("check_current:");
    dax_timeline_print(self->server->timeline);
}
