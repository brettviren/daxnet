/*  =========================================================================
    dax_epoch_client - DAX Epoch Client

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_epoch_client.xml, or
     * The code generation script that built this file: zproto_client_c
    ************************************************************************
    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_EPOCH_CLIENT_H_INCLUDED
#define DAX_EPOCH_CLIENT_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef DAX_EPOCH_CLIENT_T_DEFINED
typedef struct _dax_epoch_client_t dax_epoch_client_t;
#define DAX_EPOCH_CLIENT_T_DEFINED
#endif

//  @interface
//  Create a new dax_epoch_client, return the reference if successful, or NULL
//  if construction failed due to lack of available memory.
dax_epoch_client_t *
    dax_epoch_client_new (void);

//  Destroy the dax_epoch_client and free all memory used by the object.
void
    dax_epoch_client_destroy (dax_epoch_client_t **self_p);

//  Return actor, when caller wants to work with multiple actors and/or
//  input sockets asynchronously.
zactor_t *
    dax_epoch_client_actor (dax_epoch_client_t *self);

//  Return message pipe for asynchronous message I/O. In the high-volume case,
//  we send methods and get replies to the actor, in a synchronous manner, and
//  we send/recv high volume message data to a second pipe, the msgpipe. In
//  the low-volume case we can do everything over the actor pipe, if traffic
//  is never ambiguous.
zsock_t *
    dax_epoch_client_msgpipe (dax_epoch_client_t *self);

//  Return true if client is currently connected, else false. Note that the
//  client will automatically re-connect if the server dies and restarts after
//  a successful first connection.
bool
    dax_epoch_client_connected (dax_epoch_client_t *self);

//  Enable verbose tracing (animation) of state machine activity.
void
    dax_epoch_client_set_verbose (dax_epoch_client_t *self, bool verbose);

//  Self test of this class
void
    dax_epoch_client_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
