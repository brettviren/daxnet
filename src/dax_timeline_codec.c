/*  =========================================================================
    dax_timeline_codec - DAX Timline Message Codec

    Codec class for dax_timeline_codec.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_timeline_codec.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    =========================================================================
*/

/*
@header
    dax_timeline_codec - DAX Timline Message Codec
@discuss
@end
*/

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../include/dax_timeline_codec.h"

//  Structure of our class

struct _dax_timeline_codec_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  dax_timeline_codec message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    uint32_t ident;                     //  A number monotonically increasing with wall clock time and which uniquely identifies this timeline.
    uint64_t created;                   //  The wall clock time in seconds from the Unix epoch (time_t).
    uint64_t sot;                       //  The start of timeline time measured as ticks of the 64 bit data clock.
    zchunk_t *epochs;                   //  The number of the epoch which applies to each interval, packed into an array of 32 bit ints.
    zchunk_t *times;                    //  The transition times as packed array of 64 bit data clocks. They give the end time for each interval.
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
        zsys_warning ("dax_timeline_codec: GET_OCTETS failed"); \
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
        zsys_warning ("dax_timeline_codec: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("dax_timeline_codec: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("dax_timeline_codec: GET_NUMBER4 failed"); \
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
        zsys_warning ("dax_timeline_codec: GET_NUMBER8 failed"); \
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
        zsys_warning ("dax_timeline_codec: GET_STRING failed"); \
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
        zsys_warning ("dax_timeline_codec: GET_LONGSTR failed"); \
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
//  Create a new dax_timeline_codec

dax_timeline_codec_t *
dax_timeline_codec_new (void)
{
    dax_timeline_codec_t *self = (dax_timeline_codec_t *) zmalloc (sizeof (dax_timeline_codec_t));
    return self;
}

//  --------------------------------------------------------------------------
//  Create a new dax_timeline_codec from zpl/zconfig_t *

dax_timeline_codec_t *
    dax_timeline_codec_new_zpl (zconfig_t *config)
{
    assert (config);
    dax_timeline_codec_t *self = NULL;
    char *message = zconfig_get (config, "message", NULL);
    if (!message) {
        zsys_error ("Can't find 'message' section");
        return NULL;
    }

    if (streq ("DAX_TIMELINE_CODEC_TIMELINE", message)) {
        self = dax_timeline_codec_new ();
        dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_TIMELINE);
    }
    else
    if (streq ("DAX_TIMELINE_CODEC_SEND_TIMELINE", message)) {
        self = dax_timeline_codec_new ();
        dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_SEND_TIMELINE);
    }
    else
    if (streq ("DAX_TIMELINE_CODEC_ERROR", message)) {
        self = dax_timeline_codec_new ();
        dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_ERROR);
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
            dax_timeline_codec_destroy (&self);
            return NULL;
        }
        zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
        free (bvalue);
        self->routing_id = frame;
    }


    zconfig_t *content = NULL;
    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "ident", NULL);
            if (!s) {
                zsys_error ("content/ident not found");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/ident: %s is not a number", s);
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            self->ident = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "created", NULL);
            if (!s) {
                zsys_error ("content/created not found");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/created: %s is not a number", s);
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            self->created = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "sot", NULL);
            if (!s) {
                zsys_error ("content/sot not found");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/sot: %s is not a number", s);
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            self->sot = uvalue;
            }
            {
            char *s = zconfig_get (content, "epochs", NULL);
            if (!s) {
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            zchunk_t *chunk = zchunk_new (bvalue, strlen (s) / 2);
            free (bvalue);
            self->epochs = chunk;
            }
            {
            char *s = zconfig_get (content, "times", NULL);
            if (!s) {
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            zchunk_t *chunk = zchunk_new (bvalue, strlen (s) / 2);
            free (bvalue);
            self->times = chunk;
            }
            break;
        case DAX_TIMELINE_CODEC_SEND_TIMELINE:
            break;
        case DAX_TIMELINE_CODEC_ERROR:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "status", NULL);
            if (!s) {
                zsys_error ("content/status not found");
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/status: %s is not a number", s);
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            self->status = uvalue;
            }
            {
            char *s = zconfig_get (content, "reason", NULL);
            if (!s) {
                dax_timeline_codec_destroy (&self);
                return NULL;
            }
            strncpy (self->reason, s, 255);
            }
            break;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_timeline_codec

void
dax_timeline_codec_destroy (dax_timeline_codec_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_timeline_codec_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        zchunk_destroy (&self->epochs);
        zchunk_destroy (&self->times);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create a deep copy of a dax_timeline_codec instance

dax_timeline_codec_t *
dax_timeline_codec_dup (dax_timeline_codec_t *other)
{
    assert (other);
    dax_timeline_codec_t *copy = dax_timeline_codec_new ();

    // Copy the routing and message id
    dax_timeline_codec_set_routing_id (copy, dax_timeline_codec_routing_id (other));
    dax_timeline_codec_set_id (copy, dax_timeline_codec_id (other));


    // Copy the rest of the fields
    dax_timeline_codec_set_ident (copy, dax_timeline_codec_ident (other));
    dax_timeline_codec_set_created (copy, dax_timeline_codec_created (other));
    dax_timeline_codec_set_sot (copy, dax_timeline_codec_sot (other));
    {
        zchunk_t *dup_chunk = zchunk_dup (dax_timeline_codec_epochs (other));
        dax_timeline_codec_set_epochs (copy, &dup_chunk);
    }
    {
        zchunk_t *dup_chunk = zchunk_dup (dax_timeline_codec_times (other));
        dax_timeline_codec_set_times (copy, &dup_chunk);
    }
    dax_timeline_codec_set_status (copy, dax_timeline_codec_status (other));
    dax_timeline_codec_set_reason (copy, dax_timeline_codec_reason (other));

    return copy;
}

//  --------------------------------------------------------------------------
//  Receive a dax_timeline_codec from the socket. Returns 0 if OK, -1 if
//  the recv was interrupted, or -2 if the message is malformed.
//  Blocks if there is no message waiting.

int
dax_timeline_codec_recv (dax_timeline_codec_t *self, zsock_t *input)
{
    assert (input);
    int rc = 0;
    zmq_msg_t frame;
    zmq_msg_init (&frame);

    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("dax_timeline_codec: no routing ID");
            rc = -1;            //  Interrupted
            goto malformed;
        }
    }
    int size;
    size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        zsys_warning ("dax_timeline_codec: interrupted");
        rc = -1;                //  Interrupted
        goto malformed;
    }
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);


    //  Get and check protocol signature
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("dax_timeline_codec: invalid signature");
        rc = -2;                //  Malformed
        goto malformed;
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            GET_NUMBER4 (self->ident);
            GET_NUMBER8 (self->created);
            GET_NUMBER8 (self->sot);
            {
                size_t chunk_size;
                GET_NUMBER4 (chunk_size);
                if (self->needle + chunk_size > (self->ceiling)) {
                    zsys_warning ("dax_timeline_codec: epochs is missing data");
                    rc = -2;    //  Malformed
                    goto malformed;
                }
                zchunk_destroy (&self->epochs);
                self->epochs = zchunk_new (self->needle, chunk_size);
                self->needle += chunk_size;
            }
            {
                size_t chunk_size;
                GET_NUMBER4 (chunk_size);
                if (self->needle + chunk_size > (self->ceiling)) {
                    zsys_warning ("dax_timeline_codec: times is missing data");
                    rc = -2;    //  Malformed
                    goto malformed;
                }
                zchunk_destroy (&self->times);
                self->times = zchunk_new (self->needle, chunk_size);
                self->needle += chunk_size;
            }
            break;

        case DAX_TIMELINE_CODEC_SEND_TIMELINE:
            break;

        case DAX_TIMELINE_CODEC_ERROR:
            GET_NUMBER2 (self->status);
            GET_STRING (self->reason);
            break;

        default:
            zsys_warning ("dax_timeline_codec: bad message ID");
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
//  Send the dax_timeline_codec to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
dax_timeline_codec_send (dax_timeline_codec_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID

    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            frame_size += 4;            //  ident
            frame_size += 8;            //  created
            frame_size += 8;            //  sot
            frame_size += 4;            //  Size is 4 octets
            if (self->epochs)
                frame_size += zchunk_size (self->epochs);
            frame_size += 4;            //  Size is 4 octets
            if (self->times)
                frame_size += zchunk_size (self->times);
            break;
        case DAX_TIMELINE_CODEC_ERROR:
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
    size_t nbr_frames = 1;              //  Total number of frames to send

    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            PUT_NUMBER4 (self->ident);
            PUT_NUMBER8 (self->created);
            PUT_NUMBER8 (self->sot);
            if (self->epochs) {
                PUT_NUMBER4 (zchunk_size (self->epochs));
                memcpy (self->needle,
                        zchunk_data (self->epochs),
                        zchunk_size (self->epochs));
                self->needle += zchunk_size (self->epochs);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            if (self->times) {
                PUT_NUMBER4 (zchunk_size (self->times));
                memcpy (self->needle,
                        zchunk_data (self->times),
                        zchunk_size (self->times));
                self->needle += zchunk_size (self->times);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            break;

        case DAX_TIMELINE_CODEC_ERROR:
            PUT_NUMBER2 (self->status);
            PUT_STRING (self->reason);
            break;

    }
    //  Now send the data frame
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);

    return 0;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
dax_timeline_codec_print (dax_timeline_codec_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            zsys_debug ("DAX_TIMELINE_CODEC_TIMELINE:");
            zsys_debug ("    ident=%ld", (long) self->ident);
            zsys_debug ("    created=%ld", (long) self->created);
            zsys_debug ("    sot=%ld", (long) self->sot);
            zsys_debug ("    epochs=[ ... ]");
            zsys_debug ("    times=[ ... ]");
            break;

        case DAX_TIMELINE_CODEC_SEND_TIMELINE:
            zsys_debug ("DAX_TIMELINE_CODEC_SEND_TIMELINE:");
            break;

        case DAX_TIMELINE_CODEC_ERROR:
            zsys_debug ("DAX_TIMELINE_CODEC_ERROR:");
            zsys_debug ("    status=%ld", (long) self->status);
            zsys_debug ("    reason='%s'", self->reason);
            break;

    }
}

//  --------------------------------------------------------------------------
//  Export class as zconfig_t*. Caller is responsibe for destroying the instance

zconfig_t *
dax_timeline_codec_zpl (dax_timeline_codec_t *self, zconfig_t *parent)
{
    assert (self);

    zconfig_t *root = zconfig_new ("dax_timeline_codec", parent);

    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
        {
            zconfig_put (root, "message", "DAX_TIMELINE_CODEC_TIMELINE");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "ident", "%ld", (long) self->ident);
            zconfig_putf (config, "created", "%ld", (long) self->created);
            zconfig_putf (config, "sot", "%ld", (long) self->sot);
            {
            char *hex = NULL;
            STR_FROM_BYTES (hex, zchunk_data (self->epochs), zchunk_size (self->epochs));
            zconfig_putf (config, "epochs", "%s", hex);
            zstr_free (&hex);
            }
            {
            char *hex = NULL;
            STR_FROM_BYTES (hex, zchunk_data (self->times), zchunk_size (self->times));
            zconfig_putf (config, "times", "%s", hex);
            zstr_free (&hex);
            }
            break;
            }
        case DAX_TIMELINE_CODEC_SEND_TIMELINE:
        {
            zconfig_put (root, "message", "DAX_TIMELINE_CODEC_SEND_TIMELINE");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            break;
            }
        case DAX_TIMELINE_CODEC_ERROR:
        {
            zconfig_put (root, "message", "DAX_TIMELINE_CODEC_ERROR");

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
dax_timeline_codec_routing_id (dax_timeline_codec_t *self)
{
    assert (self);
    return self->routing_id;
}

void
dax_timeline_codec_set_routing_id (dax_timeline_codec_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the dax_timeline_codec id

int
dax_timeline_codec_id (dax_timeline_codec_t *self)
{
    assert (self);
    return self->id;
}

void
dax_timeline_codec_set_id (dax_timeline_codec_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
dax_timeline_codec_command (dax_timeline_codec_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_TIMELINE_CODEC_TIMELINE:
            return ("TIMELINE");
            break;
        case DAX_TIMELINE_CODEC_SEND_TIMELINE:
            return ("SEND_TIMELINE");
            break;
        case DAX_TIMELINE_CODEC_ERROR:
            return ("ERROR");
            break;
    }
    return "?";
}


//  --------------------------------------------------------------------------
//  Get/set the ident field

uint32_t
dax_timeline_codec_ident (dax_timeline_codec_t *self)
{
    assert (self);
    return self->ident;
}

void
dax_timeline_codec_set_ident (dax_timeline_codec_t *self, uint32_t ident)
{
    assert (self);
    self->ident = ident;
}


//  --------------------------------------------------------------------------
//  Get/set the created field

uint64_t
dax_timeline_codec_created (dax_timeline_codec_t *self)
{
    assert (self);
    return self->created;
}

void
dax_timeline_codec_set_created (dax_timeline_codec_t *self, uint64_t created)
{
    assert (self);
    self->created = created;
}


//  --------------------------------------------------------------------------
//  Get/set the sot field

uint64_t
dax_timeline_codec_sot (dax_timeline_codec_t *self)
{
    assert (self);
    return self->sot;
}

void
dax_timeline_codec_set_sot (dax_timeline_codec_t *self, uint64_t sot)
{
    assert (self);
    self->sot = sot;
}


//  --------------------------------------------------------------------------
//  Get the epochs field without transferring ownership

zchunk_t *
dax_timeline_codec_epochs (dax_timeline_codec_t *self)
{
    assert (self);
    return self->epochs;
}

//  Get the epochs field and transfer ownership to caller

zchunk_t *
dax_timeline_codec_get_epochs (dax_timeline_codec_t *self)
{
    zchunk_t *epochs = self->epochs;
    self->epochs = NULL;
    return epochs;
}

//  Set the epochs field, transferring ownership from caller

void
dax_timeline_codec_set_epochs (dax_timeline_codec_t *self, zchunk_t **chunk_p)
{
    assert (self);
    assert (chunk_p);
    zchunk_destroy (&self->epochs);
    self->epochs = *chunk_p;
    *chunk_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get the times field without transferring ownership

zchunk_t *
dax_timeline_codec_times (dax_timeline_codec_t *self)
{
    assert (self);
    return self->times;
}

//  Get the times field and transfer ownership to caller

zchunk_t *
dax_timeline_codec_get_times (dax_timeline_codec_t *self)
{
    zchunk_t *times = self->times;
    self->times = NULL;
    return times;
}

//  Set the times field, transferring ownership from caller

void
dax_timeline_codec_set_times (dax_timeline_codec_t *self, zchunk_t **chunk_p)
{
    assert (self);
    assert (chunk_p);
    zchunk_destroy (&self->times);
    self->times = *chunk_p;
    *chunk_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the status field

uint16_t
dax_timeline_codec_status (dax_timeline_codec_t *self)
{
    assert (self);
    return self->status;
}

void
dax_timeline_codec_set_status (dax_timeline_codec_t *self, uint16_t status)
{
    assert (self);
    self->status = status;
}


//  --------------------------------------------------------------------------
//  Get/set the reason field

const char *
dax_timeline_codec_reason (dax_timeline_codec_t *self)
{
    assert (self);
    return self->reason;
}

void
dax_timeline_codec_set_reason (dax_timeline_codec_t *self, const char *value)
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
dax_timeline_codec_test (bool verbose)
{
    printf (" * dax_timeline_codec: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    //  Simple create/destroy test
    zconfig_t *config;
    dax_timeline_codec_t *self = dax_timeline_codec_new ();
    assert (self);
    dax_timeline_codec_destroy (&self);
    //  Create pair of sockets we can send through
    //  We must bind before connect if we wish to remain compatible with ZeroMQ < v4
    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    int rc = zsock_bind (output, "inproc://selftest-dax_timeline_codec");
    assert (rc == 0);

    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    rc = zsock_connect (input, "inproc://selftest-dax_timeline_codec");
    assert (rc == 0);


    //  Encode/send/decode and verify each message type
    int instance;
    self = dax_timeline_codec_new ();
    dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_TIMELINE);
    dax_timeline_codec_set_ident (self, 123);
    dax_timeline_codec_set_created (self, 123);
    dax_timeline_codec_set_sot (self, 123);
    zchunk_t *timeline_epochs = zchunk_new ("Captcha Diem", 12);
    dax_timeline_codec_set_epochs (self, &timeline_epochs);
    zchunk_t *timeline_times = zchunk_new ("Captcha Diem", 12);
    dax_timeline_codec_set_times (self, &timeline_times);
    // convert to zpl
    config = dax_timeline_codec_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_timeline_codec_send (self, output);
    dax_timeline_codec_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_timeline_codec_t *self_temp = self;
        if (instance < 2)
            dax_timeline_codec_recv (self, input);
        else {
            self = dax_timeline_codec_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_timeline_codec_routing_id (self));
        assert (dax_timeline_codec_ident (self) == 123);
        assert (dax_timeline_codec_created (self) == 123);
        assert (dax_timeline_codec_sot (self) == 123);
        assert (memcmp (zchunk_data (dax_timeline_codec_epochs (self)), "Captcha Diem", 12) == 0);
        if (instance == 2)
            zchunk_destroy (&timeline_epochs);
        assert (memcmp (zchunk_data (dax_timeline_codec_times (self)), "Captcha Diem", 12) == 0);
        if (instance == 2)
            zchunk_destroy (&timeline_times);
        if (instance == 2) {
            dax_timeline_codec_destroy (&self);
            self = self_temp;
        }
    }
    dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_SEND_TIMELINE);
    // convert to zpl
    config = dax_timeline_codec_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_timeline_codec_send (self, output);
    dax_timeline_codec_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_timeline_codec_t *self_temp = self;
        if (instance < 2)
            dax_timeline_codec_recv (self, input);
        else {
            self = dax_timeline_codec_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_timeline_codec_routing_id (self));
        if (instance == 2) {
            dax_timeline_codec_destroy (&self);
            self = self_temp;
        }
    }
    dax_timeline_codec_set_id (self, DAX_TIMELINE_CODEC_ERROR);
    dax_timeline_codec_set_status (self, 123);
    dax_timeline_codec_set_reason (self, "Life is short but Now lasts for ever");
    // convert to zpl
    config = dax_timeline_codec_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_timeline_codec_send (self, output);
    dax_timeline_codec_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        dax_timeline_codec_t *self_temp = self;
        if (instance < 2)
            dax_timeline_codec_recv (self, input);
        else {
            self = dax_timeline_codec_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_timeline_codec_routing_id (self));
        assert (dax_timeline_codec_status (self) == 123);
        assert (streq (dax_timeline_codec_reason (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_timeline_codec_destroy (&self);
            self = self_temp;
        }
    }

    dax_timeline_codec_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
