/*  =========================================================================
    dax_timeline_codec - DAX Timline Message Codec

    Codec header for dax_timeline_codec.

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

#ifndef DAX_TIMELINE_CODEC_H_INCLUDED
#define DAX_TIMELINE_CODEC_H_INCLUDED

/*  These are the dax_timeline_codec messages:

    TIMELINE - A timeline.
        ident               number 4    A number monotonically increasing with wall clock time and which uniquely identifies this timeline.
        created             number 8    The wall clock time in seconds from the Unix epoch (time_t).
        sot                 number 8    The start of timeline time measured as ticks of the 64 bit data clock.
        epochs              chunk       The number of the epoch which applies to each interval, packed into an array of 32 bit ints.
        times               chunk       The transition times as packed array of 64 bit data clocks. They give the end time for each interval.

    SEND_TIMELINE - Request the most recently produced timeline.

    ERROR - Command failed for some specific reason
        status              number 2    3-digit status code
        reason              string      Printable explanation
*/

#define DAX_TIMELINE_CODEC_BOT              0x0
#define DAX_TIMELINE_CODEC_EOT              0x7FFFFFFFFFFFFFFF
#define DAX_TIMELINE_CODEC_SUCCESS          200
#define DAX_TIMELINE_CODEC_STORED           201
#define DAX_TIMELINE_CODEC_DELIVERED        202
#define DAX_TIMELINE_CODEC_NOT_DELIVERED    300
#define DAX_TIMELINE_CODEC_CONTENT_TOO_LARGE  301
#define DAX_TIMELINE_CODEC_TIMEOUT_EXPIRED  302
#define DAX_TIMELINE_CODEC_CONNECTION_REFUSED  303
#define DAX_TIMELINE_CODEC_RESOURCE_LOCKED  400
#define DAX_TIMELINE_CODEC_ACCESS_REFUSED   401
#define DAX_TIMELINE_CODEC_NOT_FOUND        404
#define DAX_TIMELINE_CODEC_COMMAND_INVALID  500
#define DAX_TIMELINE_CODEC_NOT_IMPLEMENTED  501
#define DAX_TIMELINE_CODEC_INTERNAL_ERROR   502

#define DAX_TIMELINE_CODEC_TIMELINE         1
#define DAX_TIMELINE_CODEC_SEND_TIMELINE    2
#define DAX_TIMELINE_CODEC_ERROR            3

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef DAX_TIMELINE_CODEC_T_DEFINED
typedef struct _dax_timeline_codec_t dax_timeline_codec_t;
#define DAX_TIMELINE_CODEC_T_DEFINED
#endif

//  @interface
//  Create a new empty dax_timeline_codec
dax_timeline_codec_t *
    dax_timeline_codec_new (void);

//  Create a new dax_timeline_codec from zpl/zconfig_t *
dax_timeline_codec_t *
    dax_timeline_codec_new_zpl (zconfig_t *config);

//  Destroy a dax_timeline_codec instance
void
    dax_timeline_codec_destroy (dax_timeline_codec_t **self_p);

//  Create a deep copy of a dax_timeline_codec instance
dax_timeline_codec_t *
    dax_timeline_codec_dup (dax_timeline_codec_t *other);

//  Receive a dax_timeline_codec from the socket. Returns 0 if OK, -1 if
//  the read was interrupted, or -2 if the message is malformed.
//  Blocks if there is no message waiting.
int
    dax_timeline_codec_recv (dax_timeline_codec_t *self, zsock_t *input);

//  Send the dax_timeline_codec to the output socket, does not destroy it
int
    dax_timeline_codec_send (dax_timeline_codec_t *self, zsock_t *output);



//  Print contents of message to stdout
void
    dax_timeline_codec_print (dax_timeline_codec_t *self);

//  Export class as zconfig_t*. Caller is responsibe for destroying the instance
zconfig_t *
    dax_timeline_codec_zpl (dax_timeline_codec_t *self, zconfig_t* parent);

//  Get/set the message routing id
zframe_t *
    dax_timeline_codec_routing_id (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_routing_id (dax_timeline_codec_t *self, zframe_t *routing_id);

//  Get the dax_timeline_codec id and printable command
int
    dax_timeline_codec_id (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_id (dax_timeline_codec_t *self, int id);
const char *
    dax_timeline_codec_command (dax_timeline_codec_t *self);

//  Get/set the ident field
uint32_t
    dax_timeline_codec_ident (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_ident (dax_timeline_codec_t *self, uint32_t ident);

//  Get/set the created field
uint64_t
    dax_timeline_codec_created (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_created (dax_timeline_codec_t *self, uint64_t created);

//  Get/set the sot field
uint64_t
    dax_timeline_codec_sot (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_sot (dax_timeline_codec_t *self, uint64_t sot);

//  Get a copy of the epochs field
zchunk_t *
    dax_timeline_codec_epochs (dax_timeline_codec_t *self);
//  Get the epochs field and transfer ownership to caller
zchunk_t *
    dax_timeline_codec_get_epochs (dax_timeline_codec_t *self);
//  Set the epochs field, transferring ownership from caller
void
    dax_timeline_codec_set_epochs (dax_timeline_codec_t *self, zchunk_t **chunk_p);

//  Get a copy of the times field
zchunk_t *
    dax_timeline_codec_times (dax_timeline_codec_t *self);
//  Get the times field and transfer ownership to caller
zchunk_t *
    dax_timeline_codec_get_times (dax_timeline_codec_t *self);
//  Set the times field, transferring ownership from caller
void
    dax_timeline_codec_set_times (dax_timeline_codec_t *self, zchunk_t **chunk_p);

//  Get/set the status field
uint16_t
    dax_timeline_codec_status (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_status (dax_timeline_codec_t *self, uint16_t status);

//  Get/set the reason field
const char *
    dax_timeline_codec_reason (dax_timeline_codec_t *self);
void
    dax_timeline_codec_set_reason (dax_timeline_codec_t *self, const char *value);

//  Self test of this class
void
    dax_timeline_codec_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define dax_timeline_codec_dump  dax_timeline_codec_print

#ifdef __cplusplus
}
#endif

#endif
