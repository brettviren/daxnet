/*  =========================================================================
    dax_chirp_peer_engine - DAX Chirp peer engine

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_chirp_peer.xml, or
     * The code generation script that built this file: zproto_zyrepeer_c
    ************************************************************************
    =========================================================================
*/


//  ---------------------------------------------------------------------------
//  State machine constants

typedef enum {
    start_state = 1,
    chatting_state = 2
} state_t;

typedef enum {
    NULL_event = 0,
    zyre_enter_event = 1,
    zyre_join_event = 2,
    hello_event = 3,
    world_event = 4,
    send_chat_event = 5,
    chat_event = 6
} event_t;

//  Names for state machine logging and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "start",
    "chatting"
};

static char *
s_event_name [] = {
    "(NONE)",
    "ZYRE_ENTER",
    "ZYRE_JOIN",
    "HELLO",
    "WORLD",
    "SEND_CHAT",
    "CHAT"
};


//  ---------------------------------------------------------------------------
//  Context for the peer. This embeds the application-level peer context
//  at its start (the entire structure, not a reference), so we can cast a
//  pointer between peer_t and s_peer_t arbitrarily.

//  These are the different method arguments we manage automatically
struct _peer_args_t {
    char *message;
};

typedef struct {
    peer_t peer;                //  Application-level peer context
    zsock_t *cmdpipe;           //  Get/send commands from caller API
    zsock_t *msgpipe;           //  Get/send messages from caller API
    //zsock_t *dealer;            //  Socket to talk to server
    zyre_t *zyre;               // The zyre node
    zloop_t *loop;              //  Listen to pipe and zyre
    dax_chirp_msg_t *message;   //  Message received or sent
    peer_args_t args;         //  Method arguments structure
    bool verbose;               //  True if we're tracing state machine
    bool connected;             //  True if peer is connected
    bool terminated;            //  True if peer is shutdown
    bool fsm_stopped;           //  "terminate" action called
    size_t expiry;              //  Expiry timer, msecs
    size_t heartbeat;           //  Heartbeat timer, msecs
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
    int expiry_timer;           //  zloop timer for expiry
    int wakeup_timer;           //  zloop timer for alarms
    int heartbeat_timer;        //  zloop timer for heartbeat
    event_t wakeup_event;       //  Wake up with this event
    char log_prefix [41];       //  Log prefix string
} s_peer_t;

static int
    peer_initialize (peer_t *self);
static void
    peer_terminate (peer_t *self);
static void
    s_peer_destroy (s_peer_t **self_p);
static void
    s_peer_execute (s_peer_t *self, event_t event);
static int
    s_peer_handle_wakeup (zloop_t *loop, int timer_id, void *argument);
static void
    s_satisfy_pedantic_compilers (void);
static void
    join (peer_t *self);
static void
    generate_chat_message (peer_t *self);
static void
    output_chat_message (peer_t *self);

//  Create a new peer connection

static s_peer_t *
s_peer_new (zsock_t *cmdpipe, zsock_t *msgpipe)
{
    s_peer_t *self = (s_peer_t *) zmalloc (sizeof (s_peer_t));
    if (self) {
        assert ((s_peer_t *) &self->peer == self);
        self->cmdpipe = cmdpipe;
        self->msgpipe = msgpipe;
        self->state = start_state;
        self->event = NULL_event;
        snprintf (self->log_prefix, sizeof (self->log_prefix),
            "%6d:%-33s", randof (1000000), "dax_chirp_peer");
        //self->dealer = zsock_new (ZMQ_DEALER);
        //if (self->dealer)
        self->zyre = zyre_new(NULL);
        if (self->zyre)
            self->message = dax_chirp_msg_new ();
        if (self->message)
            self->loop = zloop_new ();
        if (self->loop) {
            //  Give application chance to initialize and set next event
            self->peer.cmdpipe = self->cmdpipe;
            self->peer.msgpipe = self->msgpipe;
            //self->peer.dealer = self->dealer;
            self->peer.zyre = self->zyre;
            self->peer.message = self->message;
            self->peer.args = &self->args;
            if (peer_initialize (&self->peer))
                s_peer_destroy (&self);
        }
        else
            s_peer_destroy (&self);
    }
    s_satisfy_pedantic_compilers ();
    return self;
}

//  Destroy the peer connection

static void
s_peer_destroy (s_peer_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        s_peer_t *self = *self_p;
        zstr_free (&self->args.message);
        peer_terminate (&self->peer);
        dax_chirp_msg_destroy (&self->message);
        zsock_destroy (&self->msgpipe);
        //zsock_destroy (&self->dealer);
        zyre_destroy(&self->zyre);
        zloop_destroy (&self->loop);
        free (self);
        *self_p = NULL;
    }
}

//  ---------------------------------------------------------------------------
//  These methods are an internal API for actions

//  Set the next event, needed in at least one action in an internal
//  state; otherwise the state machine will wait for a message on the
//  dealer socket and treat that as the event.

static void
engine_set_next_event (peer_t *peer, event_t event)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        self->next_event = event;
    }
}

//  Raise an exception with 'event', halting any actions in progress.
//  Continues execution of actions defined for the exception event.

static void
engine_set_exception (peer_t *peer, event_t event)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        self->exception = event;
    }
}

//  Set wakeup alarm after 'delay' msecs. The next state should handle the
//  wakeup event. The alarm is cancelled on any other event.

static void
engine_set_wakeup_event (peer_t *peer, size_t delay, event_t event)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        if (self->wakeup_timer) {
            zloop_timer_end (self->loop, self->wakeup_timer);
            self->wakeup_timer = 0;
        }
        self->wakeup_timer = zloop_timer (
            self->loop, delay, 1, s_peer_handle_wakeup, self);
        self->wakeup_event = event;
    }
}

//  Set a heartbeat timer. The interval is in msecs and must be
//  non-zero. The state machine must handle the "heartbeat" event.
//  The heartbeat happens every interval no matter what traffic the
//  peer is sending or receiving.

static void
engine_set_heartbeat (peer_t *peer, size_t heartbeat)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        self->heartbeat = heartbeat;
    }
}


//  Set expiry timer. Setting a non-zero expiry causes the state machine
//  to receive an "expired" event if there is no incoming traffic for that many
//  milliseconds. This cycles over and over until/unless the code sets a
//  zero expiry. The state machine must handle the "expired" event.

//  Macro to support deprecated name: remove after 2016-07-31
#define engine_set_timeout engine_set_expiry

static void
engine_set_expiry (peer_t *peer, size_t expiry)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        self->expiry = expiry;
    }
}

//  Poll socket for activity, invoke handler on any received message.
//  Handler must be a CZMQ zloop_fn function; receives peer as arg.

static void
engine_handle_socket (peer_t *peer, zsock_t *sock, zloop_reader_fn handler)
{
    if (peer && sock) {
        s_peer_t *self = (s_peer_t *) peer;
        if (handler != NULL) {
            int rc = zloop_reader (self->loop, sock, handler, self);
            assert (rc == 0);
            zloop_reader_set_tolerant (self->loop, sock);
        }
        else
            zloop_reader_end (self->loop, sock);
    }
}

//  Set connected to true/false. The peer must call this if it wants
//  to provide the API with the connected status.

static void
engine_set_connected (peer_t *peer, bool connected)
{
    if (peer) {
        s_peer_t *self = (s_peer_t *) peer;
        self->connected = connected;
    }
}

//  Pedantic compilers don't like unused functions, so we call the whole
//  API, passing null references. It's nasty and horrid and sufficient.

static void
s_satisfy_pedantic_compilers (void)
{
    engine_set_next_event (NULL, NULL_event);
    engine_set_exception (NULL, NULL_event);
    engine_set_heartbeat (NULL, 0);
    engine_set_expiry (NULL, 0);
    engine_set_wakeup_event (NULL, 0, NULL_event);
    engine_handle_socket (NULL, 0, NULL);
    engine_set_connected (NULL, 0);
}


//  ---------------------------------------------------------------------------
//  Generic methods on protocol messages
//  TODO: replace with lookup table, since ID is one byte

static event_t
s_protocol_event (s_peer_t *self, dax_chirp_msg_t *message)
{
    assert (message);
    switch (dax_chirp_msg_id (message)) {
        case DAX_CHIRP_MSG_HELLO:
            return hello_event;
            break;
        case DAX_CHIRP_MSG_SEND_CHAT:
            return send_chat_event;
            break;
        case DAX_CHIRP_MSG_CHAT:
            return chat_event;
            break;
        case DAX_CHIRP_MSG_WORLD:
            return world_event;
            break;
        case DAX_CHIRP_MSG_ZYRE_ENTER:
            return zyre_enter_event;
            break;
        case DAX_CHIRP_MSG_ZYRE_JOIN:
            return zyre_join_event;
            break;
        default:
            zsys_error ("%s: unknown command %s, halting",
                        self->log_prefix, dax_chirp_msg_command (message));
            self->terminated = true;
            return NULL_event;
    }
}


//  Execute state machine as long as we have events; if event is NULL_event,
//  or state machine is stopped, do nothing.

static void
s_peer_execute (s_peer_t *self, event_t event)
{
    self->next_event = event;
    //  Cancel wakeup timer, if any was pending
    if (self->wakeup_timer) {
        zloop_timer_end (self->loop, self->wakeup_timer);
        self->wakeup_timer = 0;
    }
    while (!self->terminated                    //  Actor is dying
        && !self->fsm_stopped                   //  FSM has finished
        && self->next_event != NULL_event) {
        self->event = self->next_event;
        self->next_event = NULL_event;
        self->exception = NULL_event;
        if (self->verbose) {
            zsys_debug ("%s: %s:",
                self->log_prefix, s_state_name [self->state]);
            zsys_debug ("%s:     %s",
                self->log_prefix, s_event_name [self->event]);
        }
        switch (self->state) {
            case start_state:
                if (self->event == zyre_enter_event) {
                    if (!self->exception) {
                        if (self->verbose)
                            zsys_debug ("%s:         $ join : %s", self->log_prefix,"CHAT");
                        zyre_join(self->zyre,"CHAT");
                    }
                    if (!self->exception)
                        self->state = start_state;
                }
                else
                if (self->event == zyre_join_event) {
                    if (!self->exception) {
                        //  send HELLO
                        if (self->verbose)
                            zsys_debug ("%s:         $ send HELLO",
                                self->log_prefix);
                        dax_chirp_msg_set_id (self->message, DAX_CHIRP_MSG_HELLO);
                        zsys_debug ("%s: Send message to other peer", self->log_prefix);
                        dax_chirp_msg_print (self->message);
                        //dax_chirp_msg_send (self->message, self->dealer);
                        zmsg_t* encoded_msg = zmsg_new();
                        dax_chirp_msg_send (self->message, encoded_msg); //Encode the message to allow zyre to wrap it
                        zyre_shout(self->zyre,dax_chirp_msg_group(self->message),&encoded_msg);
                        //dax_chirp_msg_send (self->message, self->dealer);
                    }
                    if (!self->exception)
                        self->state = chatting_state;
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("%s: unhandled event %s in %s",
                        self->log_prefix,
                        s_event_name [self->event],
                        s_state_name [self->state]);
                    assert (false);
                }
                break;

            case chatting_state:
                if (self->event == hello_event) {
                    if (!self->exception) {
                        //  send WORLD
                        if (self->verbose)
                            zsys_debug ("%s:         $ send WORLD",
                                self->log_prefix);
                        dax_chirp_msg_set_id (self->message, DAX_CHIRP_MSG_WORLD);
                        zsys_debug ("%s: Send message to other peer", self->log_prefix);
                        dax_chirp_msg_print (self->message);
                        //dax_chirp_msg_send (self->message, self->dealer);
                        zmsg_t* encoded_msg = zmsg_new();
                        dax_chirp_msg_send (self->message, encoded_msg); //Encode the message to allow zyre to wrap it
                        //dax_chirp_msg_send (self->message, self->zyre);
                        zyre_whisper(self->zyre,dax_chirp_msg_uuid(self->message),&encoded_msg);
                        //dax_chirp_msg_send (self->message, self->dealer);
                    }
                }
                else
                if (self->event == world_event) {
                }
                else
                if (self->event == send_chat_event) {
                    if (!self->exception) {
                        //  generate chat message
                        if (self->verbose)
                            zsys_debug ("%s:         $ generate chat message", self->log_prefix);
                        generate_chat_message (&self->peer);
                    }
                    if (!self->exception) {
                        //  send CHAT
                        if (self->verbose)
                            zsys_debug ("%s:         $ send CHAT",
                                self->log_prefix);
                        dax_chirp_msg_set_id (self->message, DAX_CHIRP_MSG_CHAT);
                        zsys_debug ("%s: Send message to other peer", self->log_prefix);
                        dax_chirp_msg_print (self->message);
                        //dax_chirp_msg_send (self->message, self->dealer);
                        zmsg_t* encoded_msg = zmsg_new();
                        dax_chirp_msg_send (self->message, encoded_msg); //Encode the message to allow zyre to wrap it
                        zyre_shout(self->zyre,dax_chirp_msg_group(self->message),&encoded_msg);
                        //dax_chirp_msg_send (self->message, self->dealer);
                    }
                }
                else
                if (self->event == chat_event) {
                    if (!self->exception) {
                        //  output chat message
                        if (self->verbose)
                            zsys_debug ("%s:         $ output chat message", self->log_prefix);
                        output_chat_message (&self->peer);
                    }
                }
                else {
                    //  Handle unexpected internal events
                    zsys_warning ("%s: unhandled event %s in %s",
                        self->log_prefix,
                        s_event_name [self->event],
                        s_state_name [self->state]);
                    assert (false);
                }
                break;
        }
        //  If we had an exception event, interrupt normal programming
        if (self->exception) {
            if (self->verbose)
                zsys_debug ("%s:         ! %s",
                    self->log_prefix, s_event_name [self->exception]);
            self->next_event = self->exception;
        }
        else
        if (self->verbose)
            zsys_debug ("%s:         > %s",
                    self->log_prefix, s_state_name [self->state]);
    }
}

//  zloop callback when peer wakeup timer expires

static int
s_peer_handle_wakeup (zloop_t *loop, int timer_id, void *argument)
{
    s_peer_t *self = (s_peer_t *) argument;
    s_peer_execute (self, self->wakeup_event);
    return 0;
}


//  Handle command pipe to/from calling API

static int
s_peer_handle_cmdpipe (zloop_t *loop, zsock_t *reader, void *argument)
{
    s_peer_t *self = (s_peer_t *) argument;
    char *method = zstr_recv (self->cmdpipe);
    if (!method)
        return -1;                  //  Interrupted; exit zloop
    if (self->verbose)
        zsys_debug ("%s:     API command=%s", self->log_prefix, method);

    if (streq (method, "$TERM"))
        self->terminated = true;    //  Shutdown the engine
    else
    if (streq (method, "$CONNECTED"))
        zsock_send (self->cmdpipe, "i", self->connected);
    else
    if (streq (method, "SET VERBOSE"))
        zsock_recv (self->cmdpipe, "i", &self->verbose);
    else if (streq (method, "SET NAME")) {
        char* zname=0;
        zsock_recv (self->cmdpipe, "s", &zname);
        zyre_set_name(self->zyre, zname);
        free (zname);
    }
    else
    if (streq (method, "SEND CHAT")) {
        zstr_free (&self->args.message);
        zsock_recv (self->cmdpipe, "s", &self->args.message);
        s_peer_execute (self, send_chat_event);
    }
    //  Cleanup pipe if any argument frames are still waiting to be eaten
    if (zsock_rcvmore (self->cmdpipe)) {
        zsys_error ("%s: trailing API command frames (%s)",
            self->log_prefix, method);
        zmsg_t *more = zmsg_recv (self->cmdpipe);
        zmsg_print (more);
        zmsg_destroy (&more);
    }
    zstr_free (&method);
    return self->terminated? -1: 0;
}


//  Handle message pipe to/from calling API

static int
s_peer_handle_msgpipe (zloop_t *loop, zsock_t *reader, void *argument)
{
    s_peer_t *self = (s_peer_t *) argument;

    //  We will process as many messages as we can, to reduce the overhead
    //  of polling and the reactor:
    while (zsock_events (self->msgpipe) & ZMQ_POLLIN) {
        char *method = zstr_recv (self->msgpipe);
        if (!method)
            return -1;              //  Interrupted; exit zloop
        if (self->verbose)
            zsys_debug ("%s:     API message=%s", self->log_prefix, method);

        //  Front-end shuts down msgpipe before cmdpipe, this little
        //  handshake just ensures all traffic on the msgpipe has been
        //  flushed before the calling thread continues with destroying
        //  the actor.
        if (streq (method, "$FLUSH"))
            zsock_signal (self->cmdpipe, 0);
        //  Cleanup pipe if any argument frames are still waiting to be eaten
        if (zsock_rcvmore (self->msgpipe)) {
            zsys_error ("%s: trailing API message frames (%s)", self->log_prefix, method);
            zmsg_t *more = zmsg_recv (self->msgpipe);
            zmsg_print (more);
            zmsg_destroy (&more);
        }
        zstr_free (&method);
    }
    return 0;
}


//  Handle a message (a protocol reply) from another peer

static int
s_peer_handle_protocol (zloop_t *loop, zsock_t *reader, void *argument)
{
    s_peer_t *self = (s_peer_t *) argument;

    //  We will process as many messages as we can, to reduce the overhead
    //  of polling and the reactor:
    //while (zsock_events (self->dealer) & ZMQ_POLLIN) {
    //    if (dax_chirp_msg_recv (self->message, self->dealer))
    //        return -1;              //  Interrupted; exit zloop
   while (zsock_events (zyre_socket(self->zyre)) & ZMQ_POLLIN) {
        zyre_event_t * event = zyre_event_new(self->zyre);
        if (!event)
            return -1;         //  Interrupted; exit zloop

        zsys_debug ("Peer message");
        dax_chirp_msg_print (self->message);
        //Translate the zyre event into a dax_chirp_msg message
        const char* evtype = zyre_event_type(event);
        if (streq("SHOUT", evtype)) {
            //Use the payload of the shout as our message body
            dax_chirp_msg_recv(self->message,zyre_event_msg(event));

            //Copy over the relevant zyre data
            dax_chirp_msg_set_group(self->message,strdup(zyre_event_group(event)));
        }
        else if (streq("WHISPER", evtype)) {
            //Use the payload message of the whisper as our message body
            dax_chirp_msg_recv(self->message,zyre_event_msg(event));
        }
        else if (streq("ENTER", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_ENTER);
            zhash_t* hash = zhash_dup(zyre_event_headers(event));
            dax_chirp_msg_set_headers(self->message,&hash);
        }
        else if (streq("JOIN", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_JOIN);
            dax_chirp_msg_set_group(self->message,strdup(zyre_event_group(event)));
        }
        else if (streq("LEAVE", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_LEAVE);
        }
        else if (streq("EXIT", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_EXIT);
        }
        else if (streq("STOP", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_STOP);
        }
        else if (streq("EVASIVE", evtype)) {
            dax_chirp_msg_set_id(self->message,DAX_CHIRP_MSG_ZYRE_EVASIVE);
        }
        // fixme: what to do if it's something unknown?

        dax_chirp_msg_set_uuid (self->message,strdup(zyre_event_peer_uuid(event)));
        dax_chirp_msg_set_name (self->message,strdup(zyre_event_peer_name(event)));
        s_peer_execute (self, s_protocol_event (self, self->message));
        zyre_event_destroy(&event);
        if (self->terminated)
            return -1;
    }
    return 0;
}


//  ---------------------------------------------------------------------------
//  This is the peer actor, which polls its two sockets and processes
//  incoming messages

void
dax_chirp_peer (zsock_t *cmdpipe, void *msgpipe)
{
    //  Initialize
    s_peer_t *self = s_peer_new (cmdpipe, (zsock_t *) msgpipe);
    if (self && zyre_start(self->zyre) == 0) {
        zsock_signal (cmdpipe, 0);

        //  Set up handler for the sockets the peer uses
        engine_handle_socket ((peer_t *) self, self->cmdpipe, s_peer_handle_cmdpipe);
        engine_handle_socket ((peer_t *) self, self->msgpipe, s_peer_handle_msgpipe);
        //engine_handle_socket ((peer_t *) self, self->dealer, s_peer_handle_protocol);
        engine_handle_socket ((peer_t *) self, zyre_socket (self->zyre), s_peer_handle_protocol);
        //  Run reactor until there's a termination signal
        zloop_start (self->loop);

        //  Reactor has ended
        s_peer_destroy (&self);
    }
    else
        zsock_signal (cmdpipe, -1);
}


//  ---------------------------------------------------------------------------
//  Class interface

struct _dax_chirp_peer_t {
    zactor_t *actor;            //  Peer actor
    zsock_t *msgpipe;           //  Pipe for async message flow
    bool connected;             //  Peer currently connected or not
};


//  ---------------------------------------------------------------------------
//  Create a new dax_chirp_peer

dax_chirp_peer_t *
dax_chirp_peer_new (const char* zname)
{
    dax_chirp_peer_t *self = (dax_chirp_peer_t *) zmalloc (sizeof (dax_chirp_peer_t));
    if (!self) return self;

    zsock_t *backend;
    self->msgpipe = zsys_create_pipe (&backend);
    if (self->msgpipe)
        self->actor = zactor_new (dax_chirp_peer, backend);
    if (!self->actor)
        dax_chirp_peer_destroy (&self);
    if (zname)
        zstr_sendx(self->actor, "SET NAME", zname, NULL);

    return self;
}


//  ---------------------------------------------------------------------------
//  Destroy the dax_chirp_peer

void
dax_chirp_peer_destroy (dax_chirp_peer_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_chirp_peer_t *self = *self_p;
        if (self->actor && !zsys_interrupted) {
            //  Before destroying the actor we have to flush any pending
            //  traffic on the msgpipe, otherwise it gets lost in a fire and
            //  forget scenario. We do this by sending $FLUSH to the msgpipe
            //  and waiting for a signal back on the cmdpipe.
            if (zstr_send (self->msgpipe, "$FLUSH") == 0)
                zsock_wait (self->actor);
        }
        zactor_destroy (&self->actor);
        zsock_destroy (&self->msgpipe);
        free (self);
        *self_p = NULL;
    }
}


//  ---------------------------------------------------------------------------
//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.

zactor_t *
dax_chirp_peer_actor (dax_chirp_peer_t *self)
{
    assert (self);
    return self->actor;
}


//  ---------------------------------------------------------------------------
//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In
//  the low-volume case we can do everything over the actor pipe, if traffic
//  is never ambiguous.

zsock_t *
dax_chirp_peer_msgpipe (dax_chirp_peer_t *self)
{
    assert (self);
    return self->msgpipe;
}


//  ---------------------------------------------------------------------------
//  Client interface for sending a message into the state machine to broadcast to
//  the group

int
dax_chirp_peer_send_chat (dax_chirp_peer_t *self, const char *message)
{
    assert (self);

    zsock_send (self->actor, "ss", "SEND CHAT", message);
    return 0;
}


//  ---------------------------------------------------------------------------
//  Enable verbose tracing (animation) of state machine activity.

void
dax_chirp_peer_set_verbose (dax_chirp_peer_t *self, bool verbose)
{
    assert (self);
    zsock_send (self->actor, "si", "SET VERBOSE", verbose);
}
