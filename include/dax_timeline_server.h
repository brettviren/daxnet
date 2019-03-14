/*  =========================================================================
    dax_timeline_server - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_TIMELINE_SERVER_H_INCLUDED
#define DAX_TIMELINE_SERVER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_timeline_server
DAX_EXPORT dax_timeline_server_t *
    dax_timeline_server_new (void);

//  Destroy the dax_timeline_server
DAX_EXPORT void
    dax_timeline_server_destroy (dax_timeline_server_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_timeline_server_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
