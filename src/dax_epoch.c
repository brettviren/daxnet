/*  =========================================================================
    dax_epoch - DAX Epoch Message Codec

    Codec class for dax_epoch.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_epoch.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    =========================================================================
*/

/*
@header
    dax_epoch - DAX Epoch Message Codec
@discuss
@end
*/

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../include/dax_epoch.h"

//  Structure of our class

struct _dax_epoch_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  dax_epoch message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    uint32_t ident;                     //  The epoch identifier.
    uint64_t dtbeg;                     //  Begin time measured in data clock.
    uint64_t dtend;                     //  End time measured in data clock.
    uint64_t declared;                  //  Wall clock time when declared.
    zmsg_t *epochs;                     //  An ordered list of epochs as a message.  One frame per DEBUT.
    uint16_t status;                    //  3-digit status code
    char reason [256];                  //  Printable explanation
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) { \
        zsys_warning ("dax_epoch: GET_OCTETS failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (byte) (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) { \
        zsys_warning ("dax_epoch: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("dax_epoch: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("dax_epoch: GET_NUMBER4 failed"); \
        goto malformed; \
    } \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) { \
        zsys_warning ("dax_epoch: GET_NUMBER8 failed"); \
        goto malformed; \
    } \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("dax_epoch: GET_STRING failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("dax_epoch: GET_LONGSTR failed"); \
        goto malformed; \
    } \
    free ((host)); \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  --------------------------------------------------------------------------
//  bytes cstring conversion macros

// create new array of unsigned char from properly encoded string
// len of resulting array is strlen (str) / 2
// caller is responsibe for freeing up the memory
#define BYTES_FROM_STR(dst, _str) { \
    char *str = (char*) (_str); \
    if (!str || (strlen (str) % 2) != 0) \
        return NULL; \
\
    size_t strlen_2 = strlen (str) / 2; \
    byte *mem = (byte*) zmalloc (strlen_2); \
    size_t i; \
\
    for (i = 0; i != strlen_2; i++) \
    { \
        char buff[3] = {0x0, 0x0, 0x0}; \
        strncpy (buff, str, 2); \
        unsigned int uint; \
        sscanf (buff, "%x", &uint); \
        assert (uint <= 0xff); \
        mem [i] = (byte) (0xff & uint); \
        str += 2; \
    } \
    dst = mem; \
}

// convert len bytes to hex string
// caller is responsibe for freeing up the memory
#define STR_FROM_BYTES(dst, _mem, _len) { \
    byte *mem = (byte*) (_mem); \
    size_t len = (size_t) (_len); \
    char* ret = (char*) zmalloc (2*len + 1); \
    char* aux = ret; \
    size_t i; \
    for (i = 0; i != len; i++) \
    { \
        sprintf (aux, "%02x", mem [i]); \
        aux+=2; \
    } \
    dst = ret; \
}

//  --------------------------------------------------------------------------
//  Create a new dax_epoch

dax_epoch_t *
dax_epoch_new (void)
{
    dax_epoch_t *self = (dax_epoch_t *) zmalloc (sizeof (dax_epoch_t));
    return self;
}

//  --------------------------------------------------------------------------
//  Create a new dax_epoch from zpl/zconfig_t *

dax_epoch_t *
    dax_epoch_new_zpl (zconfig_t *config)
{
    assert (config);
    dax_epoch_t *self = NULL;
    char *message = zconfig_get (config, "message", NULL);
    if (!message) {
        zsys_error ("Can't find 'message' section");
        return NULL;
    }

    if (streq ("DAX_EPOCH_DEBUT", message)) {
        self = dax_epoch_new ();
        dax_epoch_set_id (self, DAX_EPOCH_DEBUT);
    }
    else
    if (streq ("DAX_EPOCH_RESCIND", message)) {
        self = dax_epoch_new ();
        dax_epoch_set_id (self, DAX_EPOCH_RESCIND);
    }
    else
    if (streq ("DAX_EPOCH_STATUS", message)) {
        self = dax_epoch_new ();
        dax_epoch_set_id (self, DAX_EPOCH_STATUS);
    }
    else
    if (streq ("DAX_EPOCH_TIMELINE", message)) {
        self = dax_epoch_new ();
        dax_epoch_set_id (self, DAX_EPOCH_TIMELINE);
    }
    else
    if (streq ("DAX_EPOCH_ERROR", message)) {
        self = dax_epoch_new ();
        dax_epoch_set_id (self, DAX_EPOCH_ERROR);
    }
    else
       {
        zsys_error ("message=%s is not known", message);
        return NULL;
       }

    char *s = zconfig_get (config, "routing_id", NULL);
    if (s) {
        byte *bvalue;
        BYTES_FROM_STR (bvalue, s);
        if (!bvalue) {
            dax_epoch_destroy (&self);
            return NULL;
        }
        zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
        free (bvalue);
        self->routing_id = frame;
    }


    zconfig_t *content = NULL;
    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_epoch_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "ident", NULL);
            if (!s) {
                zsys_error ("content/ident not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/ident: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->ident = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "dtbeg", NULL);
            if (!s) {
                zsys_error ("content/dtbeg not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/dtbeg: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->dtbeg = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "dtend", NULL);
            if (!s) {
                zsys_error ("content/dtend not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/dtend: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->dtend = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "declared", NULL);
            if (!s) {
                zsys_error ("content/declared not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/declared: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->declared = uvalue;
            }
            break;
        case DAX_EPOCH_RESCIND:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_epoch_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "ident", NULL);
            if (!s) {
                zsys_error ("content/ident not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/ident: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->ident = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "declared", NULL);
            if (!s) {
                zsys_error ("content/declared not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/declared: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->declared = uvalue;
            }
            break;
        case DAX_EPOCH_STATUS:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_epoch_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "dtbeg", NULL);
            if (!s) {
                zsys_error ("content/dtbeg not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/dtbeg: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->dtbeg = uvalue;
            }
            break;
        case DAX_EPOCH_TIMELINE:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_epoch_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "ident", NULL);
            if (!s) {
                zsys_error ("content/ident not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/ident: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->ident = uvalue;
            }
            {
            char *s = zconfig_get (content, "epochs", NULL);
            if (!s) {
                dax_epoch_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                dax_epoch_destroy (&self);
                return NULL;
            }
#if CZMQ_VERSION_MAJOR == 4
            zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
            zmsg_t *msg = zmsg_decode (frame);
            zframe_destroy (&frame);
#else
            zmsg_t *msg = zmsg_decode (bvalue, strlen (s) / 2);
#endif
            free (bvalue);
            self->epochs = msg;
            }
            break;
        case DAX_EPOCH_ERROR:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_epoch_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "status", NULL);
            if (!s) {
                zsys_error ("content/status not found");
                dax_epoch_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/status: %s is not a number", s);
                dax_epoch_destroy (&self);
                return NULL;
            }
            self->status = uvalue;
            }
            {
            char *s = zconfig_get (content, "reason", NULL);
            if (!s) {
                dax_epoch_destroy (&self);
                return NULL;
            }
            strncpy (self->reason, s, 255);
            }
            break;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_epoch

void
dax_epoch_destroy (dax_epoch_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_epoch_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        zmsg_destroy (&self->epochs);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create a deep copy of a dax_epoch instance

dax_epoch_t *
dax_epoch_dup (dax_epoch_t *other)
{
    assert (other);
    dax_epoch_t *copy = dax_epoch_new ();

    // Copy the routing and message id
    dax_epoch_set_routing_id (copy, dax_epoch_routing_id (other));
    dax_epoch_set_id (copy, dax_epoch_id (other));


    // Copy the rest of the fields
    dax_epoch_set_ident (copy, dax_epoch_ident (other));
    dax_epoch_set_dtbeg (copy, dax_epoch_dtbeg (other));
    dax_epoch_set_dtend (copy, dax_epoch_dtend (other));
    dax_epoch_set_declared (copy, dax_epoch_declared (other));
    {
        zmsg_t *dup_msg = zmsg_dup (dax_epoch_epochs (other));
        dax_epoch_set_epochs (copy, &dup_msg);
    }
    dax_epoch_set_status (copy, dax_epoch_status (other));
    dax_epoch_set_reason (copy, dax_epoch_reason (other));

    return copy;
}

//  --------------------------------------------------------------------------
//  Receive a dax_epoch from the socket. Returns 0 if OK, -1 if
//  the recv was interrupted, or -2 if the message is malformed.
//  Blocks if there is no message waiting.

int
dax_epoch_recv (dax_epoch_t *self, zsock_t *input)
{
    assert (input);
    int rc = 0;
    zmq_msg_t frame;
    zmq_msg_init (&frame);

    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("dax_epoch: no routing ID");
            rc = -1;            //  Interrupted
            goto malformed;
        }
    }
    int size;
    size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        zsys_warning ("dax_epoch: interrupted");
        rc = -1;                //  Interrupted
        goto malformed;
    }
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);


    //  Get and check protocol signature
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("dax_epoch: invalid signature");
        rc = -2;                //  Malformed
        goto malformed;
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            GET_NUMBER4 (self->ident);
            GET_NUMBER8 (self->dtbeg);
            GET_NUMBER8 (self->dtend);
            GET_NUMBER8 (self->declared);
            break;

        case DAX_EPOCH_RESCIND:
            GET_NUMBER4 (self->ident);
            GET_NUMBER8 (self->declared);
            break;

        case DAX_EPOCH_STATUS:
            GET_NUMBER8 (self->dtbeg);
            break;

        case DAX_EPOCH_TIMELINE:
            GET_NUMBER4 (self->ident);
            //  Get zero or more remaining frames
            zmsg_destroy (&self->epochs);
            if (zsock_rcvmore (input))
                self->epochs = zmsg_recv (input);
            else
                self->epochs = zmsg_new ();
            break;

        case DAX_EPOCH_ERROR:
            GET_NUMBER2 (self->status);
            GET_STRING (self->reason);
            break;

        default:
            zsys_warning ("dax_epoch: bad message ID");
            rc = -2;            //  Malformed
            goto malformed;
    }
    //  Successful return
    zmq_msg_close (&frame);
    return rc;

    //  Error returns
    malformed:
        zmq_msg_close (&frame);
        return rc;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Send the dax_epoch to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
dax_epoch_send (dax_epoch_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID

    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            frame_size += 4;            //  ident
            frame_size += 8;            //  dtbeg
            frame_size += 8;            //  dtend
            frame_size += 8;            //  declared
            break;
        case DAX_EPOCH_RESCIND:
            frame_size += 4;            //  ident
            frame_size += 8;            //  declared
            break;
        case DAX_EPOCH_STATUS:
            frame_size += 8;            //  dtbeg
            break;
        case DAX_EPOCH_TIMELINE:
            frame_size += 4;            //  ident
            break;
        case DAX_EPOCH_ERROR:
            frame_size += 2;            //  status
            frame_size += 1 + strlen (self->reason);
            break;
    }
    //  Now serialize message into the frame
    zmq_msg_t frame;
    zmq_msg_init_size (&frame, frame_size);
    self->needle = (byte *) zmq_msg_data (&frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);
    bool have_epochs = false;
    size_t nbr_frames = 1;              //  Total number of frames to send

    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            PUT_NUMBER4 (self->ident);
            PUT_NUMBER8 (self->dtbeg);
            PUT_NUMBER8 (self->dtend);
            PUT_NUMBER8 (self->declared);
            break;

        case DAX_EPOCH_RESCIND:
            PUT_NUMBER4 (self->ident);
            PUT_NUMBER8 (self->declared);
            break;

        case DAX_EPOCH_STATUS:
            PUT_NUMBER8 (self->dtbeg);
            break;

        case DAX_EPOCH_TIMELINE:
            PUT_NUMBER4 (self->ident);
            nbr_frames += self->epochs? zmsg_size (self->epochs): 1;
            have_epochs = true;
            break;

        case DAX_EPOCH_ERROR:
            PUT_NUMBER2 (self->status);
            PUT_STRING (self->reason);
            break;

    }
    //  Now send the data frame
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);

    //  Now send the epochs if necessary
    if (have_epochs) {
        if (self->epochs) {
            zframe_t *frame = zmsg_first (self->epochs);
            while (frame) {
                zframe_send (&frame, output, ZFRAME_REUSE + (--nbr_frames? ZFRAME_MORE: 0));
                frame = zmsg_next (self->epochs);
            }
        }
        else
            zmq_send (zsock_resolve (output), NULL, 0, 0);
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
dax_epoch_print (dax_epoch_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            zsys_debug ("DAX_EPOCH_DEBUT:");
            zsys_debug ("    ident=%ld", (long) self->ident);
            zsys_debug ("    dtbeg=%ld", (long) self->dtbeg);
            zsys_debug ("    dtend=%ld", (long) self->dtend);
            zsys_debug ("    declared=%ld", (long) self->declared);
            break;

        case DAX_EPOCH_RESCIND:
            zsys_debug ("DAX_EPOCH_RESCIND:");
            zsys_debug ("    ident=%ld", (long) self->ident);
            zsys_debug ("    declared=%ld", (long) self->declared);
            break;

        case DAX_EPOCH_STATUS:
            zsys_debug ("DAX_EPOCH_STATUS:");
            zsys_debug ("    dtbeg=%ld", (long) self->dtbeg);
            break;

        case DAX_EPOCH_TIMELINE:
            zsys_debug ("DAX_EPOCH_TIMELINE:");
            zsys_debug ("    ident=%ld", (long) self->ident);
            zsys_debug ("    epochs=");
            if (self->epochs)
                zmsg_print (self->epochs);
            else
                zsys_debug ("(NULL)");
            break;

        case DAX_EPOCH_ERROR:
            zsys_debug ("DAX_EPOCH_ERROR:");
            zsys_debug ("    status=%ld", (long) self->status);
            zsys_debug ("    reason='%s'", self->reason);
            break;

    }
}

//  --------------------------------------------------------------------------
//  Export class as zconfig_t*. Caller is responsibe for destroying the instance

zconfig_t *
dax_epoch_zpl (dax_epoch_t *self, zconfig_t *parent)
{
    assert (self);

    zconfig_t *root = zconfig_new ("dax_epoch", parent);

    switch (self->id) {
        case DAX_EPOCH_DEBUT:
        {
            zconfig_put (root, "message", "DAX_EPOCH_DEBUT");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "ident", "%ld", (long) self->ident);
            zconfig_putf (config, "dtbeg", "%ld", (long) self->dtbeg);
            zconfig_putf (config, "dtend", "%ld", (long) self->dtend);
            zconfig_putf (config, "declared", "%ld", (long) self->declared);
            break;
            }
        case DAX_EPOCH_RESCIND:
        {
            zconfig_put (root, "message", "DAX_EPOCH_RESCIND");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "ident", "%ld", (long) self->ident);
            zconfig_putf (config, "declared", "%ld", (long) self->declared);
            break;
            }
        case DAX_EPOCH_STATUS:
        {
            zconfig_put (root, "message", "DAX_EPOCH_STATUS");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "dtbeg", "%ld", (long) self->dtbeg);
            break;
            }
        case DAX_EPOCH_TIMELINE:
        {
            zconfig_put (root, "message", "DAX_EPOCH_TIMELINE");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "ident", "%ld", (long) self->ident);
            {
            char *hex = NULL;
#if CZMQ_VERSION_MAJOR == 4
            zframe_t *frame = zmsg_encode (self->epochs);
            STR_FROM_BYTES (hex, zframe_data (frame), zframe_size (frame));
            zconfig_putf (config, "epochs", "%s", hex);
            zstr_free (&hex);
            zframe_destroy (&frame);
#else
            byte *buffer;
            size_t size = zmsg_encode (self->epochs, &buffer);
            STR_FROM_BYTES (hex, buffer, size);
            zconfig_putf (config, "epochs", "%s", hex);
            zstr_free (&hex);
            free (buffer); buffer= NULL;
#endif
            }
            break;
            }
        case DAX_EPOCH_ERROR:
        {
            zconfig_put (root, "message", "DAX_EPOCH_ERROR");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "status", "%ld", (long) self->status);
            zconfig_putf (config, "reason", "%s", self->reason);
            break;
            }
    }
    return root;
}

//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
dax_epoch_routing_id (dax_epoch_t *self)
{
    assert (self);
    return self->routing_id;
}

void
dax_epoch_set_routing_id (dax_epoch_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the dax_epoch id

int
dax_epoch_id (dax_epoch_t *self)
{
    assert (self);
    return self->id;
}

void
dax_epoch_set_id (dax_epoch_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
dax_epoch_command (dax_epoch_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_EPOCH_DEBUT:
            return ("DEBUT");
            break;
        case DAX_EPOCH_RESCIND:
            return ("RESCIND");
            break;
        case DAX_EPOCH_STATUS:
            return ("STATUS");
            break;
        case DAX_EPOCH_TIMELINE:
            return ("TIMELINE");
            break;
        case DAX_EPOCH_ERROR:
            return ("ERROR");
            break;
    }
    return "?";
}


//  --------------------------------------------------------------------------
//  Get/set the ident field

uint32_t
dax_epoch_ident (dax_epoch_t *self)
{
    assert (self);
    return self->ident;
}

void
dax_epoch_set_ident (dax_epoch_t *self, uint32_t ident)
{
    assert (self);
    self->ident = ident;
}


//  --------------------------------------------------------------------------
//  Get/set the dtbeg field

uint64_t
dax_epoch_dtbeg (dax_epoch_t *self)
{
    assert (self);
    return self->dtbeg;
}

void
dax_epoch_set_dtbeg (dax_epoch_t *self, uint64_t dtbeg)
{
    assert (self);
    self->dtbeg = dtbeg;
}


//  --------------------------------------------------------------------------
//  Get/set the dtend field

uint64_t
dax_epoch_dtend (dax_epoch_t *self)
{
    assert (self);
    return self->dtend;
}

void
dax_epoch_set_dtend (dax_epoch_t *self, uint64_t dtend)
{
    assert (self);
    self->dtend = dtend;
}


//  --------------------------------------------------------------------------
//  Get/set the declared field

uint64_t
dax_epoch_declared (dax_epoch_t *self)
{
    assert (self);
    return self->declared;
}

void
dax_epoch_set_declared (dax_epoch_t *self, uint64_t declared)
{
    assert (self);
    self->declared = declared;
}


//  --------------------------------------------------------------------------
//  Get the epochs field without transferring ownership

zmsg_t *
dax_epoch_epochs (dax_epoch_t *self)
{
    assert (self);
    return self->epochs;
}

//  Get the epochs field and transfer ownership to caller

zmsg_t *
dax_epoch_get_epochs (dax_epoch_t *self)
{
    zmsg_t *epochs = self->epochs;
    self->epochs = NULL;
    return epochs;
}

//  Set the epochs field, transferring ownership from caller

void
dax_epoch_set_epochs (dax_epoch_t *self, zmsg_t **msg_p)
{
    assert (self);
    assert (msg_p);
    zmsg_destroy (&self->epochs);
    self->epochs = *msg_p;
    *msg_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the status field

uint16_t
dax_epoch_status (dax_epoch_t *self)
{
    assert (self);
    return self->status;
}

void
dax_epoch_set_status (dax_epoch_t *self, uint16_t status)
{
    assert (self);
    self->status = status;
}


//  --------------------------------------------------------------------------
//  Get/set the reason field

const char *
dax_epoch_reason (dax_epoch_t *self)
{
    assert (self);
    return self->reason;
}

void
dax_epoch_set_reason (dax_epoch_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->reason)
        return;
    strncpy (self->reason, value, 255);
    self->reason [255] = 0;
}



//  --------------------------------------------------------------------------
//  Selftest

void
dax_epoch_test (bool verbose)
{
    printf (" * dax_epoch: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    //  Simple create/destroy test
    zconfig_t *config;
    dax_epoch_t *self = dax_epoch_new ();
    assert (self);
    dax_epoch_destroy (&self);
    //  Create pair of sockets we can send through
    //  We must bind before connect if we wish to remain compatible with ZeroMQ < v4
    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    int rc = zsock_bind (output, "inproc://selftest-dax_epoch");
    assert (rc == 0);

    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    rc = zsock_connect (input, "inproc://selftest-dax_epoch");
    assert (rc == 0);


    //  Encode/send/decode and verify each message type
    int instance;
    self = dax_epoch_new ();
    dax_epoch_set_id (self, DAX_EPOCH_DEBUT);
    dax_epoch_set_ident (self, 123);
    dax_epoch_set_dtbeg (self, 123);
    dax_epoch_set_dtend (self, 123);
    dax_epoch_set_declared (self, 123);
    // convert to zpl
    config = dax_epoch_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_epoch_send (self, output);
    dax_epoch_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_epoch_t *self_temp = self;
        if (instance < 2)
            dax_epoch_recv (self, input);
        else {
            self = dax_epoch_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_epoch_routing_id (self));
        assert (dax_epoch_ident (self) == 123);
        assert (dax_epoch_dtbeg (self) == 123);
        assert (dax_epoch_dtend (self) == 123);
        assert (dax_epoch_declared (self) == 123);
        if (instance == 2) {
            dax_epoch_destroy (&self);
            self = self_temp;
        }
    }
    dax_epoch_set_id (self, DAX_EPOCH_RESCIND);
    dax_epoch_set_ident (self, 123);
    dax_epoch_set_declared (self, 123);
    // convert to zpl
    config = dax_epoch_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_epoch_send (self, output);
    dax_epoch_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_epoch_t *self_temp = self;
        if (instance < 2)
            dax_epoch_recv (self, input);
        else {
            self = dax_epoch_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_epoch_routing_id (self));
        assert (dax_epoch_ident (self) == 123);
        assert (dax_epoch_declared (self) == 123);
        if (instance == 2) {
            dax_epoch_destroy (&self);
            self = self_temp;
        }
    }
    dax_epoch_set_id (self, DAX_EPOCH_STATUS);
    dax_epoch_set_dtbeg (self, 123);
    // convert to zpl
    config = dax_epoch_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_epoch_send (self, output);
    dax_epoch_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_epoch_t *self_temp = self;
        if (instance < 2)
            dax_epoch_recv (self, input);
        else {
            self = dax_epoch_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_epoch_routing_id (self));
        assert (dax_epoch_dtbeg (self) == 123);
        if (instance == 2) {
            dax_epoch_destroy (&self);
            self = self_temp;
        }
    }
    dax_epoch_set_id (self, DAX_EPOCH_TIMELINE);
    dax_epoch_set_ident (self, 123);
    zmsg_t *timeline_epochs = zmsg_new ();
    dax_epoch_set_epochs (self, &timeline_epochs);
    zmsg_addstr (dax_epoch_epochs (self), "Captcha Diem");
    // convert to zpl
    config = dax_epoch_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_epoch_send (self, output);
    dax_epoch_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_epoch_t *self_temp = self;
        if (instance < 2)
            dax_epoch_recv (self, input);
        else {
            self = dax_epoch_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_epoch_routing_id (self));
        assert (dax_epoch_ident (self) == 123);
        assert (zmsg_size (dax_epoch_epochs (self)) == 1);
        char *content = zmsg_popstr (dax_epoch_epochs (self));
        assert (streq (content, "Captcha Diem"));
        zstr_free (&content);
        if (instance == 2)
            zmsg_destroy (&timeline_epochs);
        if (instance == 2) {
            dax_epoch_destroy (&self);
            self = self_temp;
        }
    }
    dax_epoch_set_id (self, DAX_EPOCH_ERROR);
    dax_epoch_set_status (self, 123);
    dax_epoch_set_reason (self, "Life is short but Now lasts for ever");
    // convert to zpl
    config = dax_epoch_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_epoch_send (self, output);
    dax_epoch_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_epoch_t *self_temp = self;
        if (instance < 2)
            dax_epoch_recv (self, input);
        else {
            self = dax_epoch_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_epoch_routing_id (self));
        assert (dax_epoch_status (self) == 123);
        assert (streq (dax_epoch_reason (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_epoch_destroy (&self);
            self = self_temp;
        }
    }

    dax_epoch_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
