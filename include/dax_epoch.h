/*  =========================================================================
    dax_epoch - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_EPOCH_H_INCLUDED
#define DAX_EPOCH_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_epoch
DAX_EXPORT dax_epoch_t *
    dax_epoch_new (void);

//  Destroy the dax_epoch
DAX_EXPORT void
    dax_epoch_destroy (dax_epoch_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_epoch_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
