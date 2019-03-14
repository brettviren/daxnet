/*  =========================================================================
    dax_timeline_client - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_TIMELINE_CLIENT_H_INCLUDED
#define DAX_TIMELINE_CLIENT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_timeline_client
DAX_EXPORT dax_timeline_client_t *
    dax_timeline_client_new (void);

//  Destroy the dax_timeline_client
DAX_EXPORT void
    dax_timeline_client_destroy (dax_timeline_client_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_timeline_client_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
