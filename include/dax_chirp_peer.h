/*  =========================================================================
    dax_chirp_peer - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_CHIRP_PEER_H_INCLUDED
#define DAX_CHIRP_PEER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_chirp_peer
DAX_EXPORT dax_chirp_peer_t *
    dax_chirp_peer_new (void);

//  Destroy the dax_chirp_peer
DAX_EXPORT void
    dax_chirp_peer_destroy (dax_chirp_peer_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_chirp_peer_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
