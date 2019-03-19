/*  =========================================================================
    dax_timeline_server - DAX Timeline Server

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: dax_timeline_server.xml, or
     * The code generation script that built this file: zproto_server_c
    ************************************************************************
    =========================================================================
*/

#ifndef DAX_TIMELINE_SERVER_H_INCLUDED
#define DAX_TIMELINE_SERVER_H_INCLUDED

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  To work with dax_timeline_server, use the CZMQ zactor API:
//
//  Create new dax_timeline_server instance, passing logging prefix:
//
//      zactor_t *dax_timeline_server = zactor_new (dax_timeline_server, "myname");
//
//  Destroy dax_timeline_server instance
//
//      zactor_destroy (&dax_timeline_server);
//
//  Enable verbose logging of commands and activity:
//
//      zstr_send (dax_timeline_server, "VERBOSE");
//
//  Bind dax_timeline_server to specified endpoint. TCP endpoints may specify
//  the port number as "*" to acquire an ephemeral port:
//
//      zstr_sendx (dax_timeline_server, "BIND", endpoint, NULL);
//
//  Return assigned port number, specifically when BIND was done using an
//  an ephemeral port:
//
//      zstr_sendx (dax_timeline_server, "PORT", NULL);
//      char *command, *port_str;
//      zstr_recvx (dax_timeline_server, &command, &port_str, NULL);
//      assert (streq (command, "PORT"));
//
//  Specify configuration file to load, overwriting any previous loaded
//  configuration file or options:
//
//      zstr_sendx (dax_timeline_server, "LOAD", filename, NULL);
//
//  Set configuration path value:
//
//      zstr_sendx (dax_timeline_server, "SET", path, value, NULL);
//
//  Save configuration data to config file on disk:
//
//      zstr_sendx (dax_timeline_server, "SAVE", filename, NULL);
//
//  Send zmsg_t instance to dax_timeline_server:
//
//      zactor_send (dax_timeline_server, &msg);
//
//  Receive zmsg_t instance from dax_timeline_server:
//
//      zmsg_t *msg = zactor_recv (dax_timeline_server);
//
//  This is the dax_timeline_server constructor as a zactor_fn:
//
void
    dax_timeline_server (zsock_t *pipe, void *args);

//  Self test of this class
void
    dax_timeline_server_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
