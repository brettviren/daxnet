/*  =========================================================================
    dax_chirp_msg - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_CHIRP_MSG_H_INCLUDED
#define DAX_CHIRP_MSG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_chirp_msg
DAX_EXPORT dax_chirp_msg_t *
    dax_chirp_msg_new (void);

//  Destroy the dax_chirp_msg
DAX_EXPORT void
    dax_chirp_msg_destroy (dax_chirp_msg_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_chirp_msg_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
