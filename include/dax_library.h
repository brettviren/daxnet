/*  =========================================================================
    daxnet - generated layer of public API

    LGPL3, boilerplate to come.

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef DAX_LIBRARY_H_INCLUDED
#define DAX_LIBRARY_H_INCLUDED

//  Set up environment for the application

//  External dependencies
#include <zyre.h>

//  DAX version macros for compile-time API detection
#define DAX_VERSION_MAJOR 0
#define DAX_VERSION_MINOR 0
#define DAX_VERSION_PATCH 0

#define DAX_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define DAX_VERSION \
    DAX_MAKE_VERSION(DAX_VERSION_MAJOR, DAX_VERSION_MINOR, DAX_VERSION_PATCH)

// czmq_prelude.h bits
#if !defined (__WINDOWS__)
#   if (defined WIN32 || defined _WIN32 || defined WINDOWS || defined _WINDOWS)
#       undef __WINDOWS__
#       define __WINDOWS__
#   endif
#endif

// Windows MSVS doesn't have stdbool
#if (defined (_MSC_VER) && !defined (true))
#   if (!defined (__cplusplus) && (!defined (true)))
#       define true 1
#       define false 0
        typedef char bool;
#   endif
#else
#   include <stdbool.h>
#endif
// czmq_prelude.h bits

#if defined (__WINDOWS__)
#   if defined DAX_STATIC
#       define DAX_EXPORT
#   elif defined DAX_INTERNAL_BUILD
#       if defined DLL_EXPORT
#           define DAX_EXPORT __declspec(dllexport)
#       else
#           define DAX_EXPORT
#       endif
#   elif defined DAX_EXPORTS
#       define DAX_EXPORT __declspec(dllexport)
#   else
#       define DAX_EXPORT __declspec(dllimport)
#   endif
#   define DAX_PRIVATE
#elif defined (__CYGWIN__)
#   define DAX_EXPORT
#   define DAX_PRIVATE
#else
#   if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define DAX_PRIVATE __attribute__ ((visibility ("hidden")))
#       define DAX_EXPORT __attribute__ ((visibility ("default")))
#   else
#       define DAX_PRIVATE
#       define DAX_EXPORT
#   endif
#endif

//  Project has no stable classes, so we build the draft API
#undef  DAX_BUILD_DRAFT_API
#define DAX_BUILD_DRAFT_API

//  Opaque class structures to allow forward references
//  These classes are stable or legacy and built in all releases
//  Draft classes are by default not built in stable releases
#ifdef DAX_BUILD_DRAFT_API
typedef struct _dax_ident_t dax_ident_t;
#define DAX_IDENT_T_DEFINED
typedef struct _dax_epoch_t dax_epoch_t;
#define DAX_EPOCH_T_DEFINED
typedef struct _dax_epoch_server_t dax_epoch_server_t;
#define DAX_EPOCH_SERVER_T_DEFINED
typedef struct _dax_epoch_client_t dax_epoch_client_t;
#define DAX_EPOCH_CLIENT_T_DEFINED
typedef struct _dax_chirp_msg_t dax_chirp_msg_t;
#define DAX_CHIRP_MSG_T_DEFINED
typedef struct _dax_chirp_peer_t dax_chirp_peer_t;
#define DAX_CHIRP_PEER_T_DEFINED
typedef struct _timeline_t timeline_t;
#define TIMELINE_T_DEFINED
typedef struct _dax_timeline_t dax_timeline_t;
#define DAX_TIMELINE_T_DEFINED
#endif // DAX_BUILD_DRAFT_API


//  Public classes, each with its own header file
#ifdef DAX_BUILD_DRAFT_API
#include "dax_ident.h"
#include "dax_epoch.h"
#include "dax_epoch_server.h"
#include "dax_epoch_client.h"
#include "dax_chirp_msg.h"
#include "dax_chirp_peer.h"
#include "timeline.h"
#include "dax_timeline.h"
#endif // DAX_BUILD_DRAFT_API

#ifdef DAX_BUILD_DRAFT_API

#ifdef __cplusplus
extern "C" {
#endif

//  Self test for private classes
DAX_EXPORT void
    dax_private_selftest (bool verbose, const char *subtest);

#ifdef __cplusplus
}
#endif
#endif // DAX_BUILD_DRAFT_API

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
