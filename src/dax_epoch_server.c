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

// the timeline
typedef struct _debut_t {
    int ident;
    int64_t tbeg, tend, decl;
} debut_t;

debut_t* debut_new(int ident, int64_t tbeg, int64_t tend, int64_t decl)
{
    debut_t* self = (debut_t *) zmalloc (sizeof (debut_t));
    self->ident = ident;
    self->tbeg = tbeg;
    self->tend = tend;
    self->decl = decl;
    return self;
}
void debut_destroy(debut_t** pdebut)
{
    free(*pdebut);
    *pdebut = NULL;
}


dax_epoch_t* debut2epoch(debut_t* debut)
{
    dax_epoch_t* emsg = dax_epoch_new();
    dax_epoch_set_id(emsg, DAX_EPOCH_DEBUT);
    dax_epoch_set_ident(emsg, debut->ident);
    dax_epoch_set_dtbeg(emsg, debut->tbeg);
    dax_epoch_set_dtend(emsg, debut->tend);
    dax_epoch_set_declared(emsg, debut->decl);
    return emsg;
}

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zsock_t *pipe;              //  Actor pipe back to caller
    zconfig_t *config;          //  Current loaded configuration

    zlist_t* timeline;          /* list of epoch debuts */
    int last_epoch;             /* last epoch number. */

    zsock_t* pub;               /* publish new debuts and rescinds */
    int pub_port;
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
    self->timeline = zlist_new();
    self->pub = zsock_new(ZMQ_PUB);
    self->pub_port = -1;
    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    zsock_destroy(&self->pub);
    self->pub_port = -1;
    dax_epoch_t* emsg = (dax_epoch_t*)zlist_first(self->timeline);
    while (emsg) {
        dax_epoch_destroy(&emsg);
        emsg = zlist_next(self->timeline);
    }
    zlist_destroy(&self->timeline);
}

//  Process server API method, return reply message if any

// add epoch, takes ownership
static void
s_add_epoch(server_t* self, debut_t* debut)
{
    zlist_append(self->timeline, debut);
    self->last_epoch = debut->ident;

    dax_epoch_t* emsg = debut2epoch(debut);
    zsock_send(self->pub, "m", emsg);
    dax_epoch_destroy(&emsg);
}

// del epoch
static void
s_del_epoch(server_t* self, int ident, int64_t decl)
{
    // ident, tbeg, tend, declared
    dax_epoch_t* emsg = dax_epoch_new();
    dax_epoch_set_id(emsg, DAX_EPOCH_RESCIND);
    dax_epoch_set_ident(emsg, ident);
    dax_epoch_set_declared(emsg, decl);

    // delete from store
    zlist_t* timeline = self->timeline;
    dax_epoch_t* one = (dax_epoch_t*)zlist_first(timeline);
    while (one) {
        int other = dax_epoch_ident(one);
        if (other == ident) {
            zlist_remove(timeline, one);
            break;
        }
        one = (dax_epoch_t*)zlist_next(timeline);
    }

    // tell the world
    zsock_send(self->pub, "m", emsg);

    dax_epoch_destroy(&emsg);
}

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    int ident = *(int*)zframe_data(zmsg_first(msg));

    if (streq(method, "DEBUT")) {
        int64_t tbeg = *(int64_t*)zframe_data(zmsg_next(msg));
        int64_t tend = *(int64_t*)zframe_data(zmsg_next(msg));
        int64_t decl = *(int64_t*)zframe_data(zmsg_next(msg));
        s_add_epoch(self, debut_new(ident, tbeg, tend, decl));
    }
    else if (streq(method, "RESCIND")) {
        int64_t decl = *(int64_t*)zframe_data(zmsg_next(msg));
        s_del_epoch(self, ident, decl);
    }
    else if (streq(method, "PUBLISH")) {
        char *endpoint = zmsg_popstr (msg);
        self->pub_port = zsock_bind (self->pub, "%s", endpoint);
        if (self->pub_port == -1) {
            zsys_warning ("could not bind to %s", endpoint);
        }
        zstr_free (&endpoint);
    }
    else if (streq (method, "PUB PORT")) {
        zstr_sendm (self->pipe, "PUB PORT");
        zstr_sendf (self->pipe, "%d", self->pub_port);
    }

    else {
        zsys_warning ("unknown server method '%s' - ignored", method);
    }

    return NULL;
}

//  Apply new configuration.

static void
server_configuration (server_t *self, zconfig_t *config)
{
    zconfig_t *section = zconfig_locate (self->config, "dax_epoch_server");
    while (section) { 
        if (streq(zconfig_name(section), "epochs")) {
            // debut = 1
            //     tbeg = ...
            //     tend = ...
            //     declared = ...
            // rescind = 1
            //     declared = ...
            zconfig_t* sub = zconfig_child(section);
            while (sub) {
                // fixme: do some input validation, you lout.
                if (streq(zconfig_name(sub), "debut")) {
                    int ident = atoi(zconfig_get(sub, "debut", "-1"));
                    int64_t tbeg = atol(zconfig_get(sub, "debut/tbeg", "0"));
                    int64_t tend = atol(zconfig_get(sub, "debut/tend", "0"));
                    int64_t decl = atol(zconfig_get(sub, "debut/declared", "0"));
                    s_add_epoch(self, debut_new(ident, tbeg, tend, decl));
                    continue;
                }
                if (streq(zconfig_name(sub), "rescind")) {
                    int ident = atoi(zconfig_get(sub, "rescind", "-1"));
                    int64_t decl = atol(zconfig_get(sub, "rescind/declared", "0"));
                    s_del_epoch(self, ident, decl);
                    continue;
                }
                sub = zconfig_next(sub);
            }
        }
    }
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
//  set_timeline
//

static void
set_timeline (client_t *self)
{
    dax_epoch_t* emsg = self->message;

    if (dax_epoch_upto(emsg) == self->server->last_epoch) {
        return;                 /* no change */
    }

    // clear old, one frame per debut
    zmsg_t* debuts = dax_epoch_epochs(emsg);
    zframe_t* frame = zmsg_pop(debuts);
    while (frame) {
        zframe_destroy(&frame);
        frame = zmsg_pop(debuts);
    }

    zmsg_t* msg = zmsg_new();

    debut_t* debut = (debut_t*)zlist_first(self->server->timeline);
    while (debut) {

        zmsg_t* one = zmsg_new();
        zmsg_addmem(one, &debut->ident, 4);
        zmsg_addmem(one, &debut->tbeg, 8);
        zmsg_addmem(one, &debut->tend, 8);
        zmsg_addmem(one, &debut->decl, 8);

        frame = zmsg_encode(one);
        zmsg_append(msg, &frame);

        debut = (debut_t*)zlist_next(self->server->timeline);
    }

    // set new timeline
    dax_epoch_set_epochs(emsg, &msg);
}


//  ---------------------------------------------------------------------------
//  signal_command_invalid
//

static void
signal_command_invalid (client_t *self)
{

}


//  ---------------------------------------------------------------------------
//  store_debut
//

static void
store_debut (client_t *self)
{
    dax_epoch_t* emsg = self->message;
    s_add_epoch(self->server, debut_new(
                    dax_epoch_ident(emsg),
                    dax_epoch_dtbeg(emsg),
                    dax_epoch_dtend(emsg),
                    dax_epoch_declared(emsg)));
}


//  ---------------------------------------------------------------------------
//  store_rescind
//

static void
store_rescind (client_t *self)
{
    int ident = dax_epoch_ident(self->message);
    zlist_t* timeline = self->server->timeline;
    dax_epoch_t* one = (dax_epoch_t*)zlist_first(timeline);
    while (one) {
        int other = dax_epoch_ident(one);
        if (other == ident) {
            zlist_remove(timeline, one);
            break;
        }
        one = (dax_epoch_t*)zlist_next(timeline);
    }
}
