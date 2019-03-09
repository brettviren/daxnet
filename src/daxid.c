/*  =========================================================================
    daxid - A DAX identity is used to allow the DAX network to know if its participants.

An identity is composed of the following elements:

- a UUID (truly unique)

- a nick name (optional, should be unique, human readable)

- a set of "node endpoints" which are key-value pairs.

More details are in the method documentation.

An application should create a daxid, set various attributes
based on the application's nodes (nodes are abstract roles combined
with their defining configuration) and then start the identity.
Subsequently, the application may call daxid methods to learn
about the state of other identifies on the network.

    LGPL3, boilerplate to come.
    =========================================================================
*/


#include "dax_classes.h"

//  Structure of our class

struct _daxid_t {
    zactor_t* actor;            /* background actor */
};

// This actor services zyre events to collect state about peers
static void s_self_actor (zsock_t* pipe, void* args);

/*
 * API methods
 */

daxid_t *
daxid_new (void)
{
    daxid_t *self = (daxid_t *) zmalloc (sizeof (daxid_t));
    assert (self);

    self->actor = zactor_new(s_self_actor, NULL);
    if (self->actor == NULL) {
        daxid_destroy (&self);
    }

    return self;
}

void
daxid_destroy (daxid_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        daxid_t *self = *self_p;
        zactor_destroy(&self->actor);
        free (self);
        *self_p = NULL;
    }
}

void
daxid_set_nickname (daxid_t *self, const char *nickname)
{
    assert (self);
    zsock_send (self->actor, "sss", "SET", "NICKNAME", nickname);
}

void
daxid_set_endpoint (daxid_t *self, const char *rolename, const char *nodename, const char* endpoint)
{
    assert (self);
    assert (rolename);
    assert (nodename);

    char* string = zsys_sprintf("DAX-%s-%s", rolename, nodename);
    zstr_sendx (self->actor, "SET", "HEADER", string, endpoint, NULL);
    zstr_free (&string);
}

void
daxid_set_verbose (daxid_t *self)
{
    assert (self);
    zstr_sendx (self->actor, "SET", "VERBOSE", "1", NULL);
}

const char *
daxid_nickname (daxid_t *self)
{
    assert (self);
    char *nickname;
    zsock_send (self->actor, "ss", "GET", "NICKNAME");
    zsock_recv (self->actor, "s", &nickname);
    return nickname;
}

zlist_t *
daxid_peers (daxid_t *self)
{
    assert (self);
    zlist_t *peers=0;
    zstr_send (self->actor, "PEERS");
    zsock_recv (self->actor, "p", &peers);
    return peers;               /* fresh */
}

zhash_t *
daxid_peer (daxid_t *self, const char *ident)
{
    assert (self);

    zsock_send(self->actor, "ss", "PEER", ident);
    zframe_t* frame = zframe_recv(self->actor);
    if (!frame) {
        return zhash_new();
    }
    zhash_t* headers = zhash_unpack(frame);
    zframe_destroy(&frame);
    return headers;
}

int
daxid_start (daxid_t *self)
{
    assert (self);
    zsock_send (self->actor, "s", "START");
    return zsock_wait (self->actor);
}



/*
 * Internal bits for the background actor
 */


typedef struct {
    zsock_t* pipe;              /* the actor pipe */
    zpoller_t *poller;          //  Socket poller
    zyre_t* zyre;               /* heavy lifting */
    zhash_t* peers;            /* info about peers */
    bool started;               //  Are we already running?
    bool terminated;
    bool verbose;
} self_t;


static self_t *
s_self_new (zsock_t *pipe, char *directory)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    self->pipe = pipe;
    self->poller = zpoller_new (self->pipe, NULL);
    self->zyre = zyre_new (NULL);
    self->peers = zhash_new ();
    self->started = 0;
    self->verbose = 0;
    self->terminated = 0;

    return self;
}

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        zpoller_destroy (&self->poller);

        zyre_event_t* event = (zyre_event_t*)zhash_first(self->peers);
        while (event) {
            zyre_event_destroy(&event);
            event = (zyre_event_t*)zhash_next(self->peers);
        }
        zhash_destroy (&self->peers);
        zyre_destroy (&self->zyre);
        free (self);
        *self_p = NULL;
    }
}

static void
s_self_set_property (self_t *self, zmsg_t *request)
{
    char *name = zmsg_popstr (request);
    char *value = zmsg_popstr (request);
    if (streq (name, "NICKNAME")) {
        zsys_info("setting nickname to \"%s\"", value ? value : "(none)");
        zyre_set_name(self->zyre, value);
    }
    else if (streq (name, "HEADER")) {
        char* endpoint = zmsg_popstr(request);
        zyre_set_header(self->zyre, value, "%s", endpoint);
        free(endpoint);
    }
    else if (streq (name, "VERBOSE")) {
        zyre_set_verbose (self->zyre);
        self->verbose = 1;
    }
    else {
        zsys_error ("daxid: - invalid SET property: %s", name);
        assert (false);
    }
    free (name);
    free (value);
}

static void
s_self_get_property (self_t *self, zmsg_t *request)
{
    char *name = zmsg_popstr (request);
    if (streq (name, "NICKNAME")) {
        //  Get current nickname from server
        const char *nickname = zyre_name(self->zyre);
        zsock_send (self->pipe, "s", nickname);
    }
    else {
        zsys_error ("daxid: - invalid GET property: %s", name);
        assert (false);
    }
    free (name);
}

static void
s_self_start (self_t *self)
{
    //  Return code for caller
    int rc = 0;
    assert (!self->started);
    
    if (zyre_start (self->zyre) == 0) {
        zpoller_add (self->poller, zyre_socket (self->zyre));
        self->started = true;
    }
    else {
        zsys_info ("daxid: can't start Zyre discovery service");
        zyre_destroy (&self->zyre);
        rc = -1;
    }
    zsock_signal (self->pipe, rc);
}

static void
s_self_peers (self_t *self)
{
    zsock_send(self->pipe, "p", zhash_keys(self->peers));
}

static void
s_self_peer (self_t *self, zmsg_t* request) 
{
    char* ident = zmsg_popstr(request);
    zyre_event_t* event = (zyre_event_t*)zhash_lookup(self->peers, ident);
    free (ident);

    if (event) {
        zhash_t* headers = zyre_event_headers(event);
        if (headers) {
            zframe_t* frame = zhash_pack(headers);
            zframe_send(&frame, self->pipe, 0);
            return;
        }
    }

    zframe_t* empty = zframe_new_empty();
    zframe_send(&empty, self->pipe, 0);
}

static void
s_self_handle_pipe (self_t *self)
{
    //  Get the whole message off the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
        return;                     //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "SET"))
        s_self_set_property (self, request);
    else
    if (streq (command, "GET"))
        s_self_get_property (self, request);
    else
    if (streq (command, "START"))
        s_self_start (self);
    else
    if (streq (command, "PEERS"))
        s_self_peers(self);
    else
    if (streq (command, "PEER"))
        s_self_peer (self, request);
    else
    if (streq (command, "$TERM"))
        self->terminated = true;
    else {
        zsys_error ("daxid: - invalid command: %s", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}

static void
s_self_handle_zyre (self_t *self)
{
    zyre_event_t *event = zyre_event_new (self->zyre);
    assert(event);
    const char* evtype = zyre_event_type(event);
    const char* uuid = zyre_event_peer_uuid (event);
    zsys_info("got zyre event %s, uuid %s", evtype, uuid);
    
    if (streq(evtype,"ENTER")) {
        zyre_event_t* old = (zyre_event_t*)zhash_lookup(self->peers, uuid);
        if (old) {
            zyre_event_destroy(&old);
        }
        zhash_update(self->peers, uuid, event);
        zsys_info("daxid %s welcomes %s [%s]",
                  zyre_name(self->zyre),
                  uuid,
                  zyre_event_peer_name(event));

        // do not destroy the event it's now owned by the zhash
        return;
    }

    if (streq(evtype, "EXIT")) {

        zyre_event_t* old = (zyre_event_t*)zhash_lookup(self->peers, uuid);
        if (old) {
            zhash_delete(self->peers, uuid);
            zyre_event_destroy(&old);
            zsys_info("daxid %s bids adieu to %s [%s]",
                      zyre_name(self->zyre),
                      uuid,
                      zyre_event_peer_name(event));
        }
        else {
            zsys_info("daxid %s later days to unknown %s [%s]",
                      zyre_name(self->zyre),
                      uuid,
                      zyre_event_peer_name(event));
        }
    }
    else {
        zsys_info("daxid unknown event type \"%s\"", evtype);
    }
    zyre_event_destroy (&event);
}

void
s_self_actor (zsock_t *pipe, void *args)
{
    self_t *self = s_self_new (pipe, (char *) args);
    if (self) {
        zsock_signal (pipe, 0);
        while (!self->terminated) {
            zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
            if (which == self->pipe)
                s_self_handle_pipe (self);
            else
            if (which == zyre_socket (self->zyre))
                s_self_handle_zyre (self);
            
            if (zpoller_terminated (self->poller))
                break;          //  Interrupted
        }
    }
    else {
        zsock_signal (pipe, -1);    //  Failed to initialize
    }

    s_self_destroy (&self);
}




//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
daxid_test (bool verbose)
{
    printf (" * daxid: ");

    //  @selftest
    //  Simple create/destroy test
    daxid_t *self = daxid_new ();
    assert (self);

    daxid_set_verbose(self);
    //daxid_set_nickname(self, "nick");
    //assert (streq("nick", daxid_nickname (self)));
    
    daxid_set_endpoint(self, "FAKEROLL", "FAKENODE", "tcp://*:12345");
    daxid_start(self);
    
    for (int n=0; n<300; ++n) {
        if (n%10 == 0) {
            zlist_t* peers = daxid_peers(self);
            zsys_info("got %d peers", zlist_size(peers));
            char* peer = (char*) zlist_first(peers);
            while (peer) {
                
                zsys_info("%03d: %s, getting headers", n, peer);
                zhash_t* headers = daxid_peer(self, peer);
                zsys_info("\tgot %d headers", zhash_size(headers));
                for (char* item = (char*)zhash_first (headers); item != NULL;
                     item = (char*)zhash_next (headers)) {
                    zsys_info("\t%s = %s", zhash_cursor (headers), item);
                }
                zhash_destroy(&headers);

                peer = (char*) zlist_next(peers);
            }
            zlist_destroy(&peers);
        }
        zclock_sleep(100);
    }

    daxid_destroy (&self);
    //  @end
    printf ("OK\n");
}
