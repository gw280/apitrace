/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include "glproc.hpp"


#if !defined(_WIN32)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for dladdr
#endif
#include <dlfcn.h>
#endif


/*
 * Handle to the true OpenGL library.
 * XXX: Not really used yet.
 */
#if defined(_WIN32)
HINSTANCE __libGlHandle = NULL;
#else
void *__libGlHandle = NULL;
void *__libEglHandle = NULL;
#endif

#define EGL_LIB "libEGL.so"
#define GL_LIB "libGLESv2.so"

#if defined(_WIN32)

#error Unsupported

#elif defined(__APPLE__)

#error Unsupported

#else

/*
 * Absolutely make sure we use the real dlopen here
 */
typedef void * (*PFNDLOPEN)(const char *, int);

static void *__dlopen(const char *filename, int flag)
{
    typedef void * (*PFNDLOPEN)(const char *, int);
    static PFNDLOPEN dlopen_ptr = NULL;

    if (!dlopen_ptr) {
#if defined(ANDROID)
        // Android doesn't support RTLD_NEXT
        dlopen_ptr = (PFNDLOPEN)dlsym(RTLD_DEFAULT, "dlopen");
#else
        dlopen_ptr = (PFNDLOPEN)dlsym(RTLD_NEXT, "dlopen");
#endif
        if (!dlopen_ptr) {
            os::log("apitrace: error: dlsym(RTLD_NEXT, \"dlopen\") failed\n");
            return NULL;
        }
    }

    return dlopen_ptr(filename, flag);
}

/* 
 * Initialise dlopen handles for the GL library and the EGL 
 * library
 */
bool
__dlopenLibraries()
{
    // We use __dlopen to ensure we don't catch apitrace's dlopen
    // which will intercept a dlopen call to any GL/EGL library
    if (!__libEglHandle) {
        __libEglHandle = __dlopen(EGL_LIB, RTLD_LOCAL | RTLD_LAZY);

        if (!__libEglHandle) {
            os::log("apitrace: error: couldn't load %s\n", EGL_LIB);
            return false;
        }
    }

    if (!__libGlHandle) {
        __libGlHandle = __dlopen(GL_LIB, RTLD_LOCAL | RTLD_LAZY);

        if (!__libGlHandle) {
            os::log("apitrace: error: couldn't load %s\n", GL_LIB);
            return false;
        }
    }

    return true;
}


/*
 * Lookup a public EGL/GL/GLES symbol
 *
 * The spec states that eglGetProcAddress should only be used for non-core
 * (extensions) entry-points.  Core entry-points should be taken directly from
 * the API specific libraries.
 *
 * We cannot tell here which API a symbol is meant for here (as some are
 * exported by many).  So this code assumes that the appropriate shared
 * libraries have been loaded previously (either dlopened with RTLD_GLOBAL, or
 * as part of the executable dependencies), and that their symbols available
 * for quering via dlsym(RTLD_NEXT, ...).
 */
void *
__getPublicProcAddress(const char *procName)
{
    void *proc = NULL;

    if (__dlopenLibraries()) {
        proc = dlsym(__libEglHandle, procName);

        if (!proc)
            proc = dlsym(__libGlHandle, procName);
    }

    return proc;
}

/*
 * Lookup a private EGL/GL/GLES symbol
 *
 * Private symbols should always be available through eglGetProcAddress, and
 * they are guaranteed to work with any context bound (regardless of the API).
 *
 * However, per issue#57, eglGetProcAddress returns garbage on some
 * implementations, and the spec states that implementations may choose to also
 * export extension functions publicly, so we always attempt dlsym before
 * eglGetProcAddress to mitigate that.
 */
void *
__getPrivateProcAddress(const char *procName)
{
    void *proc = NULL;

    if (__dlopenLibraries()) {
        proc = dlsym(__libEglHandle, procName);

        if (!proc)
            proc = dlsym(__libGlHandle, procName);
    }

    if (!proc && procName[0] == 'g' && procName[1] == 'l')
        proc = (void *) __eglGetProcAddress(procName);

    return proc;
}

#endif
