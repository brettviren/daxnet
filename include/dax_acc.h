/*  =========================================================================
    dax_acc - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_ACC_H_INCLUDED
#define DAX_ACC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new dax_acc
DAX_EXPORT dax_acc_t *
    dax_acc_new (void);

//  Destroy the dax_acc
DAX_EXPORT void
    dax_acc_destroy (dax_acc_t **self_p);

//  Self test of this class
DAX_EXPORT void
    dax_acc_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
