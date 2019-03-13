/*  =========================================================================
    dax_chirp_msg - DAX Chirp Message Protocol

    Codec class for dax_chirp_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_chirp_msg.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    =========================================================================
*/

/*
@header
    dax_chirp_msg - DAX Chirp Message Protocol
@discuss
@end
*/

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../include/dax_chirp_msg.h"

//  Structure of our class

struct _dax_chirp_msg_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  dax_chirp_msg message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    char uuid [256];                    //  UUID of the sender
    char name [256];                    //  Name of the sender
    char group [256];                   //  Group this message was sent to
    zhash_t *headers;                   //  Headers for the entering peer
    size_t headers_bytes;               //  Size of hash content
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
        zsys_warning ("dax_chirp_msg: GET_OCTETS failed"); \
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
        zsys_warning ("dax_chirp_msg: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("dax_chirp_msg: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("dax_chirp_msg: GET_NUMBER4 failed"); \
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
        zsys_warning ("dax_chirp_msg: GET_NUMBER8 failed"); \
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
        zsys_warning ("dax_chirp_msg: GET_STRING failed"); \
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
        zsys_warning ("dax_chirp_msg: GET_LONGSTR failed"); \
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
//  Create a new dax_chirp_msg

dax_chirp_msg_t *
dax_chirp_msg_new (void)
{
    dax_chirp_msg_t *self = (dax_chirp_msg_t *) zmalloc (sizeof (dax_chirp_msg_t));
    return self;
}

//  --------------------------------------------------------------------------
//  Create a new dax_chirp_msg from zpl/zconfig_t *

dax_chirp_msg_t *
    dax_chirp_msg_new_zpl (zconfig_t *config)
{
    assert (config);
    dax_chirp_msg_t *self = NULL;
    char *message = zconfig_get (config, "message", NULL);
    if (!message) {
        zsys_error ("Can't find 'message' section");
        return NULL;
    }

    if (streq ("DAX_CHIRP_MSG_HELLO", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_HELLO);
    }
    else
    if (streq ("DAX_CHIRP_MSG_WORLD", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_WORLD);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_ENTER", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_ENTER);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_JOIN", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_JOIN);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_LEAVE", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_LEAVE);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_EXIT", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_EXIT);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_STOP", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_STOP);
    }
    else
    if (streq ("DAX_CHIRP_MSG_ZYRE_EVASIVE", message)) {
        self = dax_chirp_msg_new ();
        dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_EVASIVE);
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
            dax_chirp_msg_destroy (&self);
            return NULL;
        }
        zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
        free (bvalue);
        self->routing_id = frame;
    }


    zconfig_t *content = NULL;
    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            {
            char *s = zconfig_get (content, "group", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->group, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_WORLD:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_ENTER:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            {
            zconfig_t *zhash = zconfig_locate (content, "headers");
            if (zhash) {
                zhash_t *hash = zhash_new ();
                zhash_autofree (hash);
                zconfig_t *child;
                for (child = zconfig_child (zhash);
                                child != NULL;
                                child = zconfig_next (child))
                {
                    zhash_update (hash, zconfig_name (child), zconfig_value (child));
                }
                self->headers = hash;
            }
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_JOIN:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            {
            char *s = zconfig_get (content, "group", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->group, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_EXIT:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_STOP:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            break;
        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            {
            char *s = zconfig_get (content, "uuid", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->uuid, s, 255);
            }
            {
            char *s = zconfig_get (content, "name", NULL);
            if (!s) {
                dax_chirp_msg_destroy (&self);
                return NULL;
            }
            strncpy (self->name, s, 255);
            }
            break;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_chirp_msg

void
dax_chirp_msg_destroy (dax_chirp_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_chirp_msg_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        zhash_destroy (&self->headers);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create a deep copy of a dax_chirp_msg instance

dax_chirp_msg_t *
dax_chirp_msg_dup (dax_chirp_msg_t *other)
{
    assert (other);
    dax_chirp_msg_t *copy = dax_chirp_msg_new ();

    // Copy the routing and message id
    dax_chirp_msg_set_routing_id (copy, dax_chirp_msg_routing_id (other));
    dax_chirp_msg_set_id (copy, dax_chirp_msg_id (other));


    // Copy the rest of the fields
    dax_chirp_msg_set_uuid (copy, dax_chirp_msg_uuid (other));
    dax_chirp_msg_set_name (copy, dax_chirp_msg_name (other));
    dax_chirp_msg_set_group (copy, dax_chirp_msg_group (other));
    {
        zhash_t *dup_hash = zhash_dup (dax_chirp_msg_headers (other));
        dax_chirp_msg_set_headers (copy, &dup_hash);
    }

    return copy;
}

//  --------------------------------------------------------------------------
//  Deserialize a dax_chirp_msg from the specified message, popping
//  as many frames as needed. Returns 0 if OK, -1 if the recv was interrupted,
//  or -2 if the message is malformed.
int
dax_chirp_msg_recv (dax_chirp_msg_t *self, zmsg_t *input)
{
    assert (input);
    int rc = 0;

    zframe_t *frame = zmsg_pop (input);
    if (!frame) {
        zsys_warning ("dax_chirp_msg: missing frames in message");
        rc = -1;                //  Interrupted
        goto malformed;
    }
    //  Get and check protocol signature
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);

    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("dax_chirp_msg: invalid signature");
        rc = -2;                //  Malformed
        goto malformed;
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            GET_STRING (self->group);
            break;

        case DAX_CHIRP_MSG_WORLD:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_ENTER:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                zhash_destroy (&self->headers);
                self->headers = zhash_new ();
                zhash_autofree (self->headers);
                while (hash_size--) {
                    char key [256];
                    char *value = NULL;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->headers, key, value);
                    free (value);
                }
            }
            break;

        case DAX_CHIRP_MSG_ZYRE_JOIN:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            GET_STRING (self->group);
            break;

        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EXIT:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_STOP:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            GET_STRING (self->uuid);
            GET_STRING (self->name);
            break;

        default:
            zsys_warning ("dax_chirp_msg: bad message ID");
            rc = -2;            //  Malformed
            goto malformed;
    }
    zframe_destroy (&frame);
    //  Successful return
    return rc;

    //  Error returns
    malformed:
        zframe_destroy (&frame);
        return rc;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Serialize and append the dax_chirp_msg to the specified message
int
dax_chirp_msg_send (dax_chirp_msg_t *self, zmsg_t *output)
{
    assert (self);
    assert (output);
    size_t frame_size = 2 + 1;          //  Signature and message ID

    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            frame_size += 1 + strlen (self->group);
            break;
        case DAX_CHIRP_MSG_WORLD:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            break;
        case DAX_CHIRP_MSG_ZYRE_ENTER:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            frame_size += 4;            //  Size is 4 octets
            if (self->headers) {
                self->headers_bytes = 0;
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    self->headers_bytes += 1 + strlen (zhash_cursor (self->headers));
                    self->headers_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            frame_size += self->headers_bytes;
            break;
        case DAX_CHIRP_MSG_ZYRE_JOIN:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            frame_size += 1 + strlen (self->group);
            break;
        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            break;
        case DAX_CHIRP_MSG_ZYRE_EXIT:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            break;
        case DAX_CHIRP_MSG_ZYRE_STOP:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            break;
        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            frame_size += 1 + strlen (self->uuid);
            frame_size += 1 + strlen (self->name);
            break;
    }
    //  Now serialize message into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = zframe_data (frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);


    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            PUT_STRING (self->group);
            break;

        case DAX_CHIRP_MSG_WORLD:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_ENTER:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            if (self->headers) {
                PUT_NUMBER4 (zhash_size (self->headers));
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    PUT_STRING (zhash_cursor (self->headers));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty hash
            break;

        case DAX_CHIRP_MSG_ZYRE_JOIN:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            PUT_STRING (self->group);
            break;

        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EXIT:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_STOP:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            PUT_STRING (self->uuid);
            PUT_STRING (self->name);
            break;

    }
    //  Now store the frame data
    zmsg_append (output, &frame);

    return 0;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
dax_chirp_msg_print (dax_chirp_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            zsys_debug ("DAX_CHIRP_MSG_HELLO:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            zsys_debug ("    group='%s'", self->group);
            break;

        case DAX_CHIRP_MSG_WORLD:
            zsys_debug ("DAX_CHIRP_MSG_WORLD:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_ENTER:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_ENTER:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            zsys_debug ("    headers=");
            if (self->headers) {
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->headers), item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case DAX_CHIRP_MSG_ZYRE_JOIN:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_JOIN:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            zsys_debug ("    group='%s'", self->group);
            break;

        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_LEAVE:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EXIT:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_EXIT:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_STOP:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_STOP:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            break;

        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            zsys_debug ("DAX_CHIRP_MSG_ZYRE_EVASIVE:");
            zsys_debug ("    uuid='%s'", self->uuid);
            zsys_debug ("    name='%s'", self->name);
            break;

    }
}

//  --------------------------------------------------------------------------
//  Export class as zconfig_t*. Caller is responsibe for destroying the instance

zconfig_t *
dax_chirp_msg_zpl (dax_chirp_msg_t *self, zconfig_t *parent)
{
    assert (self);

    zconfig_t *root = zconfig_new ("dax_chirp_msg", parent);

    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_HELLO");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            zconfig_putf (config, "group", "%s", self->group);
            break;
            }
        case DAX_CHIRP_MSG_WORLD:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_WORLD");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_ENTER:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_ENTER");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            if (self->headers) {
                zconfig_t *hash = zconfig_new ("headers", config);
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    zconfig_putf (hash, zhash_cursor (self->headers), "%s", item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_JOIN:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_JOIN");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            zconfig_putf (config, "group", "%s", self->group);
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_LEAVE:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_LEAVE");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_EXIT:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_EXIT");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_STOP:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_STOP");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            break;
            }
        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
        {
            zconfig_put (root, "message", "DAX_CHIRP_MSG_ZYRE_EVASIVE");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "uuid", "%s", self->uuid);
            zconfig_putf (config, "name", "%s", self->name);
            break;
            }
    }
    return root;
}

//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
dax_chirp_msg_routing_id (dax_chirp_msg_t *self)
{
    assert (self);
    return self->routing_id;
}

void
dax_chirp_msg_set_routing_id (dax_chirp_msg_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the dax_chirp_msg id

int
dax_chirp_msg_id (dax_chirp_msg_t *self)
{
    assert (self);
    return self->id;
}

void
dax_chirp_msg_set_id (dax_chirp_msg_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
dax_chirp_msg_command (dax_chirp_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case DAX_CHIRP_MSG_HELLO:
            return ("HELLO");
            break;
        case DAX_CHIRP_MSG_WORLD:
            return ("WORLD");
            break;
        case DAX_CHIRP_MSG_ZYRE_ENTER:
            return ("ZYRE_ENTER");
            break;
        case DAX_CHIRP_MSG_ZYRE_JOIN:
            return ("ZYRE_JOIN");
            break;
        case DAX_CHIRP_MSG_ZYRE_LEAVE:
            return ("ZYRE_LEAVE");
            break;
        case DAX_CHIRP_MSG_ZYRE_EXIT:
            return ("ZYRE_EXIT");
            break;
        case DAX_CHIRP_MSG_ZYRE_STOP:
            return ("ZYRE_STOP");
            break;
        case DAX_CHIRP_MSG_ZYRE_EVASIVE:
            return ("ZYRE_EVASIVE");
            break;
    }
    return "?";
}


//  --------------------------------------------------------------------------
//  Get/set the uuid field

const char *
dax_chirp_msg_uuid (dax_chirp_msg_t *self)
{
    assert (self);
    return self->uuid;
}

void
dax_chirp_msg_set_uuid (dax_chirp_msg_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->uuid)
        return;
    strncpy (self->uuid, value, 255);
    self->uuid [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the name field

const char *
dax_chirp_msg_name (dax_chirp_msg_t *self)
{
    assert (self);
    return self->name;
}

void
dax_chirp_msg_set_name (dax_chirp_msg_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->name)
        return;
    strncpy (self->name, value, 255);
    self->name [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the group field

const char *
dax_chirp_msg_group (dax_chirp_msg_t *self)
{
    assert (self);
    return self->group;
}

void
dax_chirp_msg_set_group (dax_chirp_msg_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->group)
        return;
    strncpy (self->group, value, 255);
    self->group [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get the headers field without transferring ownership

zhash_t *
dax_chirp_msg_headers (dax_chirp_msg_t *self)
{
    assert (self);
    return self->headers;
}

//  Get the headers field and transfer ownership to caller

zhash_t *
dax_chirp_msg_get_headers (dax_chirp_msg_t *self)
{
    zhash_t *headers = self->headers;
    self->headers = NULL;
    return headers;
}

//  Set the headers field, transferring ownership from caller

void
dax_chirp_msg_set_headers (dax_chirp_msg_t *self, zhash_t **headers_p)
{
    assert (self);
    assert (headers_p);
    zhash_destroy (&self->headers);
    self->headers = *headers_p;
    *headers_p = NULL;
}



//  --------------------------------------------------------------------------
//  Selftest

void
dax_chirp_msg_test (bool verbose)
{
    printf (" * dax_chirp_msg: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    //  Simple create/destroy test
    zconfig_t *config;
    dax_chirp_msg_t *self = dax_chirp_msg_new ();
    assert (self);
    dax_chirp_msg_destroy (&self);
    zmsg_t *output = zmsg_new ();
    assert (output);

    zmsg_t *input = zmsg_new ();
    assert (input);


    //  Encode/send/decode and verify each message type
    int instance;
    self = dax_chirp_msg_new ();
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_HELLO);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_group (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_group (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_WORLD);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_ENTER);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zhash_t *zyre_enter_headers = zhash_new ();
    zhash_insert (zyre_enter_headers, "Name", "Brutus");
    dax_chirp_msg_set_headers (self, &zyre_enter_headers);
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        zhash_t *headers = dax_chirp_msg_get_headers (self);
        // Order of values is not guaranted
        assert (zhash_size (headers) == 1);
        assert (streq ((char *) zhash_first (headers), "Brutus"));
        assert (streq ((char *) zhash_cursor (headers), "Name"));
        zhash_destroy (&headers);
        if (instance == 2)
            zhash_destroy (&zyre_enter_headers);
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_JOIN);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_group (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_group (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_LEAVE);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_EXIT);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_STOP);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }
    dax_chirp_msg_set_id (self, DAX_CHIRP_MSG_ZYRE_EVASIVE);
    dax_chirp_msg_set_uuid (self, "Life is short but Now lasts for ever");
    dax_chirp_msg_set_name (self, "Life is short but Now lasts for ever");
    zmsg_destroy (&output);
    output = zmsg_new ();
    assert (output);
    // convert to zpl
    config = dax_chirp_msg_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    dax_chirp_msg_send (self, output);
    dax_chirp_msg_send (self, output);

    zmsg_destroy (&input);
    input = zmsg_dup (output);
    assert (input);
    for (instance = 0; instance < 3; instance++) {
        dax_chirp_msg_t *self_temp = self;
        if (instance < 2)
            dax_chirp_msg_recv (self, input);
        else {
            self = dax_chirp_msg_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (dax_chirp_msg_routing_id (self) == NULL);
        assert (streq (dax_chirp_msg_uuid (self), "Life is short but Now lasts for ever"));
        assert (streq (dax_chirp_msg_name (self), "Life is short but Now lasts for ever"));
        if (instance == 2) {
            dax_chirp_msg_destroy (&self);
            self = self_temp;
        }
    }

    dax_chirp_msg_destroy (&self);
    zmsg_destroy (&input);
    zmsg_destroy (&output);
#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
