/*  =========================================================================
    dax_timeline - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_TIMELINE_H_INCLUDED
#define DAX_TIMELINE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_timeline
DAX_EXPORT dax_timeline_t *
    dax_timeline_new (void);

//  Destroy the dax_timeline
DAX_EXPORT void
    dax_timeline_destroy (dax_timeline_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_timeline_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
