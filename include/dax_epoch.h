/*  =========================================================================
    dax_epoch - DAX Epoch Message Codec

    Codec header for dax_epoch.

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

#ifndef DAX_EPOCH_H_INCLUDED
#define DAX_EPOCH_H_INCLUDED

/*  These are the dax_epoch messages:

    DEBUT - Declare an epoch.
        ident               number 4    The epoch identifier.
        dtbeg               number 8    Begin time measured in data clock.
        dtend               number 8    End time measured in data clock.
        declared            number 8    Wall clock time when declared.

    RESCIND - Invalidate a previously declared epoch.
        ident               number 4    The epoch identifier.
        declared            number 8    Wall clock time when declared.

    STATUS - Request current valid epochs.
        dtbeg               number 8    Begin time measured in data clock.

    TIMELINE - A timeline.
        ident               number 4    The epoch identifier.
        epochs              msg         An ordered list of epochs as a message.  One frame per DEBUT.

    ERROR - Command failed for some specific reason
        status              number 2    3-digit status code
        reason              string      Printable explanation
*/

#define DAX_EPOCH_BOT                       0x8000000000000001
#define DAX_EPOCH_EOT                       0x7FFFFFFFFFFFFFFF
#define DAX_EPOCH_SUCCESS                   200
#define DAX_EPOCH_STORED                    201
#define DAX_EPOCH_DELIVERED                 202
#define DAX_EPOCH_NOT_DELIVERED             300
#define DAX_EPOCH_CONTENT_TOO_LARGE         301
#define DAX_EPOCH_TIMEOUT_EXPIRED           302
#define DAX_EPOCH_CONNECTION_REFUSED        303
#define DAX_EPOCH_RESOURCE_LOCKED           400
#define DAX_EPOCH_ACCESS_REFUSED            401
#define DAX_EPOCH_NOT_FOUND                 404
#define DAX_EPOCH_COMMAND_INVALID           500
#define DAX_EPOCH_NOT_IMPLEMENTED           501
#define DAX_EPOCH_INTERNAL_ERROR            502

#define DAX_EPOCH_DEBUT                     1
#define DAX_EPOCH_RESCIND                   2
#define DAX_EPOCH_STATUS                    3
#define DAX_EPOCH_TIMELINE                  4
#define DAX_EPOCH_ERROR                     5

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef DAX_EPOCH_T_DEFINED
typedef struct _dax_epoch_t dax_epoch_t;
#define DAX_EPOCH_T_DEFINED
#endif

//  @interface
//  Create a new empty dax_epoch
dax_epoch_t *
    dax_epoch_new (void);

//  Create a new dax_epoch from zpl/zconfig_t *
dax_epoch_t *
    dax_epoch_new_zpl (zconfig_t *config);

//  Destroy a dax_epoch instance
void
    dax_epoch_destroy (dax_epoch_t **self_p);

//  Create a deep copy of a dax_epoch instance
dax_epoch_t *
    dax_epoch_dup (dax_epoch_t *other);

//  Receive a dax_epoch from the socket. Returns 0 if OK, -1 if
//  the read was interrupted, or -2 if the message is malformed.
//  Blocks if there is no message waiting.
int
    dax_epoch_recv (dax_epoch_t *self, zsock_t *input);

//  Send the dax_epoch to the output socket, does not destroy it
int
    dax_epoch_send (dax_epoch_t *self, zsock_t *output);



//  Print contents of message to stdout
void
    dax_epoch_print (dax_epoch_t *self);

//  Export class as zconfig_t*. Caller is responsibe for destroying the instance
zconfig_t *
    dax_epoch_zpl (dax_epoch_t *self, zconfig_t* parent);

//  Get/set the message routing id
zframe_t *
    dax_epoch_routing_id (dax_epoch_t *self);
void
    dax_epoch_set_routing_id (dax_epoch_t *self, zframe_t *routing_id);

//  Get the dax_epoch id and printable command
int
    dax_epoch_id (dax_epoch_t *self);
void
    dax_epoch_set_id (dax_epoch_t *self, int id);
const char *
    dax_epoch_command (dax_epoch_t *self);

//  Get/set the ident field
uint32_t
    dax_epoch_ident (dax_epoch_t *self);
void
    dax_epoch_set_ident (dax_epoch_t *self, uint32_t ident);

//  Get/set the dtbeg field
uint64_t
    dax_epoch_dtbeg (dax_epoch_t *self);
void
    dax_epoch_set_dtbeg (dax_epoch_t *self, uint64_t dtbeg);

//  Get/set the dtend field
uint64_t
    dax_epoch_dtend (dax_epoch_t *self);
void
    dax_epoch_set_dtend (dax_epoch_t *self, uint64_t dtend);

//  Get/set the declared field
uint64_t
    dax_epoch_declared (dax_epoch_t *self);
void
    dax_epoch_set_declared (dax_epoch_t *self, uint64_t declared);

//  Get a copy of the epochs field
zmsg_t *
    dax_epoch_epochs (dax_epoch_t *self);
//  Get the epochs field and transfer ownership to caller
zmsg_t *
    dax_epoch_get_epochs (dax_epoch_t *self);
//  Set the epochs field, transferring ownership from caller
void
    dax_epoch_set_epochs (dax_epoch_t *self, zmsg_t **msg_p);

//  Get/set the status field
uint16_t
    dax_epoch_status (dax_epoch_t *self);
void
    dax_epoch_set_status (dax_epoch_t *self, uint16_t status);

//  Get/set the reason field
const char *
    dax_epoch_reason (dax_epoch_t *self);
void
    dax_epoch_set_reason (dax_epoch_t *self, const char *value);

//  Self test of this class
void
    dax_epoch_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define dax_epoch_dump      dax_epoch_print

#ifdef __cplusplus
}
#endif

#endif
