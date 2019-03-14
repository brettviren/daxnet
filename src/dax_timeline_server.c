/*  =========================================================================
    dax_timeline_server - class description

    LGPL3, boilerplate to come.
    =========================================================================
*/

/*
@header
    dax_timeline_server -
@discuss
@end
*/

#include "dax_classes.h"

//  Structure of our class

struct _dax_timeline_server_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new dax_timeline_server

dax_timeline_server_t *
dax_timeline_server_new (void)
{
    dax_timeline_server_t *self = (dax_timeline_server_t *) zmalloc (sizeof (dax_timeline_server_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_timeline_server

void
dax_timeline_server_destroy (dax_timeline_server_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        dax_timeline_server_t *self = *self_p;
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
dax_timeline_server_test (bool verbose)
{
    printf (" * dax_timeline_server: ");

    //  @selftest
    //  Simple create/destroy test
    dax_timeline_server_t *self = dax_timeline_server_new ();
    assert (self);
    dax_timeline_server_destroy (&self);
    //  @end
    printf ("OK\n");
}
