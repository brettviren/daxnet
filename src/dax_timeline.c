/*  =========================================================================
    dax_timeline - A DAX timeline holds what epochs are valid over what time intervals.

    LGPL3, boilerplate to come.
    =========================================================================
*/

/*
@header
    dax_timeline - A DAX timeline holds what epochs are valid over what time intervals.
@discuss
@end
*/

#include "dax_classes.h"

//  Structure of our class

struct _dax_timeline_t {
    int ident;
    int64_t created;
    int64_t sot;
    size_t size;
    int* epochs;
    int64_t* times;   
};


//  --------------------------------------------------------------------------
//  Create a new dax_timeline

dax_timeline_t *
dax_timeline_new (void)
{
    dax_timeline_t *self = (dax_timeline_t *) zmalloc (sizeof (dax_timeline_t));
    assert (self);
    self->ident = 0;
    self->created = 0;
    self->sot= 0;
    self->size = 0;
    self->epochs = NULL;
    self->times = NULL;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the dax_timeline

void
dax_timeline_destroy (dax_timeline_t **self_p)
{
    assert (self_p);
    if (!*self_p) {
        return;                 /* already */
    }
    dax_timeline_t *self = *self_p;

    //  Free class properties here
    if (self->size) {
        free(self->epochs);
        free(self->times);
    }
    //  Free object itself
    free (self);
    *self_p = NULL;
}


int
dax_timeline_ident (dax_timeline_t *self)
{
    return self->ident;
}

time_t
dax_timeline_created (dax_timeline_t *self)
{
    return self->created;
}

int64_t
dax_timeline_sot (dax_timeline_t *self)
{
    return self->sot;
}

size_t
dax_timeline_size (dax_timeline_t *self)
{
    return self->size;
}

int*
dax_timeline_epochs (dax_timeline_t *self)
{
    return self->epochs;
}

int64_t*
dax_timeline_times (dax_timeline_t *self)
{
    return self->times;
}

void
dax_timeline_set_ident (dax_timeline_t *self, int ident)
{
    self->ident = ident;
}


void
dax_timeline_set_created (dax_timeline_t *self, size_t time)
{
    self->created = time;
}


void
dax_timeline_set_sot (dax_timeline_t *self, size_t time)
{
    self->sot = time;
}

void
dax_timeline_set_intervals (dax_timeline_t *self, size_t size, int*  epochs, int64_t*  times)
{
    if (self->size) {
        free(self->epochs);
        free(self->times);
    }
    self->size = size;

    self->epochs = zmalloc(sizeof(int)*self->size);
    memcpy(self->epochs, epochs, sizeof(int)*self->size);
    self->times = zmalloc(sizeof(int64_t)*self->size);
    memcpy(self->times,  times, sizeof(int64_t)*self->size);
}

void
dax_timeline_encode (dax_timeline_t *self, dax_timeline_codec_t *codec)
{
    // this is painful!
    dax_timeline_codec_set_id(codec, DAX_TIMELINE_CODEC_TIMELINE);
    dax_timeline_codec_set_ident(codec, self->ident);
    dax_timeline_codec_set_sot(codec, self->sot);
    
    zchunk_t* epochs = zchunk_new(self->epochs, self->size*sizeof(int));
    zchunk_t* times = zchunk_new(self->times, self->size*sizeof(int64_t));

    dax_timeline_codec_set_epochs(codec, &epochs);    
    dax_timeline_codec_set_times(codec, &times);    
}

dax_timeline_t *
dax_timeline_decode (dax_timeline_codec_t *codec)
{
    dax_timeline_t* self = dax_timeline_new();
    self->ident = dax_timeline_codec_ident(codec);
    self->sot = dax_timeline_codec_sot(codec);
    zchunk_t* epochs = dax_timeline_codec_epochs(codec);
    zchunk_t* times = dax_timeline_codec_times(codec);
    size_t esize = zchunk_size(epochs)/sizeof(int);
    size_t tsize = zchunk_size(times)/sizeof(int64_t);
    assert (esize == tsize);

    self->size = esize;
    if (self->size) {
        self->epochs = zmalloc(sizeof(int)*self->size);
        memcpy(self->epochs, zchunk_data(epochs), sizeof(int)*self->size);
        self->times = zmalloc(sizeof(int64_t)*self->size);
        memcpy(self->times,  zchunk_data(times),  sizeof(int64_t)*self->size);
    }
    return self;
}

dax_timeline_t *
dax_timeline_dup (dax_timeline_t *other)
{
    dax_timeline_t* self = dax_timeline_new();
    self->ident = other->ident;
    self->sot = other->sot;
    self->size = other->size;
    if (self->size) {
        self->epochs = zmalloc(sizeof(int)*self->size);
        memcpy(self->epochs, other->epochs, sizeof(int)*self->size);
        self->times = zmalloc(sizeof(int64_t)*self->size);
        memcpy(self->times,  other->times,  sizeof(int64_t)*self->size);
    }
    return self;
}

void
dax_timeline_print (dax_timeline_t *self)
{
    printf("TIMELINE at: %p\n", self);
    printf("      ident: %d\n", self->ident);
    printf("        sot: %ld\n", self->sot);
    printf("       size: %ld\n", self->size);
    printf("     epochs: %p", self->epochs);
    for (size_t ind=0; ind<self->size; ++ind) {
        printf(" %d", self->epochs[ind]);
    }
    printf("\n");
    printf("      times: %p", self->times);
    for (size_t ind=0; ind<self->size; ++ind) {
        printf(" %ld", self->times[ind]);
    }
    printf("\n");

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
dax_timeline_test (bool verbose)
{
    printf (" * dax_timeline: ");

    //  @selftest
    //  Simple create/destroy test
    dax_timeline_t *self = dax_timeline_new ();
    assert (self);
    dax_timeline_destroy (&self);
    //  @end
    printf ("OK\n");
}
