/*  =========================================================================
    dax_timeline - A DAX timeline holds the state of multiple epoch declarations.

    LGPL3, boilerplate to come.
    =========================================================================
*/

#ifndef DAX_TIMELINE_H_INCLUDED
#define DAX_TIMELINE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/dax_timeline.xml" to make changes.
//  @interface
//  This API is a draft, and may change without notice.
#ifdef DAX_BUILD_DRAFT_API
//  *** Draft method, for development use, may change without warning ***
//  Create a DAX identity.
DAX_EXPORT dax_timeline_t *
    dax_timeline_new (void);

//  *** Draft method, for development use, may change without warning ***
//  Destroy the DAX identity.  This will cause the network to learn of
//  the loss of this identity.
DAX_EXPORT void
    dax_timeline_destroy (dax_timeline_t **self_p);

//  *** Draft method, for development use, may change without warning ***
//  Return the current epoch as determiend by the given time.
//
//  A subsequent epoch declaration may cause this method to not be idempotent.
DAX_EXPORT void
    dax_timeline_current_epoch (dax_timeline_t *self, int64_t tnow, dax_epoch_t *_);

//  *** Draft method, for development use, may change without warning ***
//  Return the next epoch as determiend by the given time.
//
//  A subsequent epoch declaration may cause this method to not be idempotent.
DAX_EXPORT dax_epoch_t *
    dax_timeline_next_epoch (dax_timeline_t *self, int64_t tnow);

//  *** Draft method, for development use, may change without warning ***
//  Return a socket from which received epoch declaration and
//  termination messages will be forwarded to the application.
//
//  If called, application must read to avoid hitting mute state.
DAX_EXPORT zsock_t *
    dax_timeline_feed (dax_timeline_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Self test of this class.
DAX_EXPORT void
    dax_timeline_test (bool verbose);

#endif // DAX_BUILD_DRAFT_API
//  @end

#ifdef __cplusplus
}
#endif

#endif
