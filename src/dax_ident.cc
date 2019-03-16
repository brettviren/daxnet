/*  =========================================================================
    dax_ident - A DAX identity is used to allow the DAX network to know if its participants.

An identity is composed of the following elements:

- a UUID (truly unique)

- a nick name (optional, should be unique, human readable)

- a set of "node endpoints" which are key-value pairs.

More details are in the method documentation.

An application should create a dax_ident, set various attributes
based on the application's nodes (nodes are abstract roles combined
with their defining configuration) and then start the identity.
Subsequently, the application may call dax_ident methods to learn
about the state of other identifies on the network.

    LGPL3, boilerplate to come.
    =========================================================================
*/

/*
@header
    dax_ident - A DAX identity is used to allow the DAX network to know if its participants.

An identity is composed of the following elements:

- a UUID (truly unique)

- a nick name (optional, should be unique, human readable)

- a set of "node endpoints" which are key-value pairs.

More details are in the method documentation.

An application should create a dax_ident, set various attributes
based on the application's nodes (nodes are abstract roles combined
with their defining configuration) and then start the identity.
Subsequently, the application may call dax_ident methods to learn
about the state of other identifies on the network.
@discuss
@end
*/

#include "dax_classes.h"

//  Structure of our class

struct _dax_ident_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new dax_ident

dax_ident_t *
dax_ident_new (void)
{
    dax_ident_t *self = (dax_ident_t *) zmalloc (sizeof (dax_ident_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_ident

void
dax_ident_destroy (dax_ident_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_ident_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
dax_ident_test (bool verbose)
{
    printf (" * dax_ident: ");

    //  @selftest
    //  Simple create/destroy test
    dax_ident_t *self = dax_ident_new ();
    assert (self);
    dax_ident_destroy (&self);
    //  @end
    printf ("OK\n");
}
