/*  =========================================================================
    dax_chirp_msg - DAX Chirp Message Protocol

    Codec header for dax_chirp_msg.

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

#ifndef DAX_CHIRP_MSG_H_INCLUDED
#define DAX_CHIRP_MSG_H_INCLUDED

/*  These are the dax_chirp_msg messages:

    HELLO - This is a test message for Zyre based protocols.

Messages must specify at least "uuid" and "name" and shouted
messages "group".



Whisper messages will be populated by the identity information of
the peer that is whispering



Must also handle Zyre events
        uuid                string      UUID of the sender
        name                string      Name of the sender
        group               string      Group this message was sent to

    WORLD - This is a test message for Zyre based protocols.

Messages must specify at least "uuid" and "name" and shouted
messages "group".



Whisper messages will be populated by the identity information of
the peer that is whispering



Must also handle Zyre events
        uuid                string      UUID of the sender
        name                string      Name of the sender

    ZYRE_ENTER - A peer has entered the network
        uuid                string      UUID of the sender
        name                string      Name of the sender
        headers             hash        Headers for the entering peer

    ZYRE_JOIN - A peer has joined a group
        uuid                string      UUID of the sender
        name                string      Name of the sender
        group               string      Group the sender is joining

    ZYRE_LEAVE - A peer has left a group
        uuid                string      UUID of the sender
        name                string      Name of the sender

    ZYRE_EXIT - A peer has exited the network
        uuid                string      UUID of the sender
        name                string      Name of the sender

    ZYRE_STOP - This is a test message for Zyre based protocols.

Messages must specify at least "uuid" and "name" and shouted
messages "group".



Whisper messages will be populated by the identity information of
the peer that is whispering



Must also handle Zyre events
        uuid                string      UUID of the sender
        name                string      Name of the sender

    ZYRE_EVASIVE - A peer is being evasive
        uuid                string      UUID of the sender
        name                string      Name of the sender
*/


#define DAX_CHIRP_MSG_HELLO                 1
#define DAX_CHIRP_MSG_WORLD                 2
#define DAX_CHIRP_MSG_ZYRE_ENTER            3
#define DAX_CHIRP_MSG_ZYRE_JOIN             4
#define DAX_CHIRP_MSG_ZYRE_LEAVE            5
#define DAX_CHIRP_MSG_ZYRE_EXIT             6
#define DAX_CHIRP_MSG_ZYRE_STOP             7
#define DAX_CHIRP_MSG_ZYRE_EVASIVE          8

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef DAX_CHIRP_MSG_T_DEFINED
typedef struct _dax_chirp_msg_t dax_chirp_msg_t;
#define DAX_CHIRP_MSG_T_DEFINED
#endif

//  @interface
//  Create a new empty dax_chirp_msg
dax_chirp_msg_t *
    dax_chirp_msg_new (void);

//  Create a new dax_chirp_msg from zpl/zconfig_t *
dax_chirp_msg_t *
    dax_chirp_msg_new_zpl (zconfig_t *config);

//  Destroy a dax_chirp_msg instance
void
    dax_chirp_msg_destroy (dax_chirp_msg_t **self_p);

//  Create a deep copy of a dax_chirp_msg instance
dax_chirp_msg_t *
    dax_chirp_msg_dup (dax_chirp_msg_t *other);

//  Deserialize a dax_chirp_msg from the specified message, popping
//  as many frames as needed. Returns 0 if OK, -1 if the read was
//  interrupted, or -2 if the message is malformed.
int
    dax_chirp_msg_recv (dax_chirp_msg_t *self, zmsg_t *input);

//  Serialize and append the dax_chirp_msg to the specified message
int
    dax_chirp_msg_send (dax_chirp_msg_t *self, zmsg_t *output);



//  Print contents of message to stdout
void
    dax_chirp_msg_print (dax_chirp_msg_t *self);

//  Export class as zconfig_t*. Caller is responsibe for destroying the instance
zconfig_t *
    dax_chirp_msg_zpl (dax_chirp_msg_t *self, zconfig_t* parent);

//  Get/set the message routing id
zframe_t *
    dax_chirp_msg_routing_id (dax_chirp_msg_t *self);
void
    dax_chirp_msg_set_routing_id (dax_chirp_msg_t *self, zframe_t *routing_id);

//  Get the dax_chirp_msg id and printable command
int
    dax_chirp_msg_id (dax_chirp_msg_t *self);
void
    dax_chirp_msg_set_id (dax_chirp_msg_t *self, int id);
const char *
    dax_chirp_msg_command (dax_chirp_msg_t *self);

//  Get/set the uuid field
const char *
    dax_chirp_msg_uuid (dax_chirp_msg_t *self);
void
    dax_chirp_msg_set_uuid (dax_chirp_msg_t *self, const char *value);

//  Get/set the name field
const char *
    dax_chirp_msg_name (dax_chirp_msg_t *self);
void
    dax_chirp_msg_set_name (dax_chirp_msg_t *self, const char *value);

//  Get/set the group field
const char *
    dax_chirp_msg_group (dax_chirp_msg_t *self);
void
    dax_chirp_msg_set_group (dax_chirp_msg_t *self, const char *value);

//  Get a copy of the headers field
zhash_t *
    dax_chirp_msg_headers (dax_chirp_msg_t *self);
//  Get the headers field and transfer ownership to caller
zhash_t *
    dax_chirp_msg_get_headers (dax_chirp_msg_t *self);
//  Set the headers field, transferring ownership from caller
void
    dax_chirp_msg_set_headers (dax_chirp_msg_t *self, zhash_t **hash_p);

//  Self test of this class
void
    dax_chirp_msg_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define dax_chirp_msg_dump  dax_chirp_msg_print

#ifdef __cplusplus
}
#endif

#endif
