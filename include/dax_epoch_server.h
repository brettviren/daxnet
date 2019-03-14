/*  =========================================================================
    dax_epoch_server - DAX Epoch server

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_epoch_server.xml, or
     * The code generation script that built this file: zproto_server_c
    ************************************************************************
    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_EPOCH_SERVER_H_INCLUDED
#define DAX_EPOCH_SERVER_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  To work with dax_epoch_server, use the CZMQ zactor API:
//
//  Create new dax_epoch_server instance, passing logging prefix:
//
//      zactor_t *dax_epoch_server = zactor_new (dax_epoch_server, "myname");
//
//  Destroy dax_epoch_server instance
//
//      zactor_destroy (&dax_epoch_server);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (dax_epoch_server, "VERBOSE");
//
//  Bind dax_epoch_server to specified endpoint. TCP endpoints may specify
//  the port number as "*" to acquire an ephemeral port:
//
//      zstr_sendx (dax_epoch_server, "BIND", endpoint, NULL);
//
//  Return assigned port number, specifically when BIND was done using an
//  an ephemeral port:
//
//      zstr_sendx (dax_epoch_server, "PORT", NULL);
//      char *command, *port_str;
//      zstr_recvx (dax_epoch_server, &command, &port_str, NULL);
//      assert (streq (command, "PORT"));
//
//  Specify configuration file to load, overwriting any previous loaded
//  configuration file or options:
//
//      zstr_sendx (dax_epoch_server, "LOAD", filename, NULL);
//
//  Set configuration path value:
//
//      zstr_sendx (dax_epoch_server, "SET", path, value, NULL);
//
//  Save configuration data to config file on disk:
//
//      zstr_sendx (dax_epoch_server, "SAVE", filename, NULL);
//
//  Send zmsg_t instance to dax_epoch_server:
//
//      zactor_send (dax_epoch_server, &msg);
//
//  Receive zmsg_t instance from dax_epoch_server:
//
//      zmsg_t *msg = zactor_recv (dax_epoch_server);
//
//  This is the dax_epoch_server constructor as a zactor_fn:
//
void
    dax_epoch_server (zsock_t *pipe, void *args);

//  Self test of this class
void
    dax_epoch_server_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
