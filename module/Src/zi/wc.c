/*
 * wc.c - an wc module for zsh
 *
 * This file is part of zsh, the Z shell.
 *
 * Copyright (c) 1996-1997 Zoltán Hidvégi
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and to distribute modified versions of this software for any
 * purpose, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * In no event shall Zoltán Hidvégi or the Zsh Development Group be liable
 * to any party for direct, indirect, special, incidental, or consequential
 * damages arising out of the use of this software and its documentation,
 * even if Zoltán Hidvégi and the Zsh Development Group have been advised of
 * the possibility of such damage.
 *
 * Zoltán Hidvégi and the Zsh Development Group specifically disclaim any
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose.  The software
 * provided hereunder is on an "as is" basis, and Zoltán Hidvégi and the
 * Zsh Development Group have no obligation to provide maintenance,
 * support, updates, enhancements, or modifications.
 *
 */

#include "wc.mdh"
#include "wc.pro"

/**/
static mnumber
math_wcswidth(UNUSED(char *name), char *arg, UNUSED(int id))
{
    mnumber ret;

    ret.type = MN_INTEGER;
#define DEFAULT_BUFFER_SIZE 1024
    wchar_t wc[DEFAULT_BUFFER_SIZE] = L"";
    arg = unmetafy(arg, NULL);
    size_t wc_len = mbstowcs(wc, arg, strlen(arg));
    ret.u.l = wcswidth(wc, wc_len);;

    return ret;
}

/*
 * boot_ is executed when the module is loaded.
 */

static struct builtin bintab[] = {
};

static struct conddef cotab[] = {
};

static struct paramdef patab[] = {
};

static struct mathfunc mftab[] = {
    STRMATHFUNC("wcswidth", math_wcswidth, 0),
};

static struct features module_features = {
    bintab, sizeof(bintab)/sizeof(*bintab),
    cotab, sizeof(cotab)/sizeof(*cotab),
    mftab, sizeof(mftab)/sizeof(*mftab),
    patab, sizeof(patab)/sizeof(*patab),
    0
};

/**/
int
setup_(UNUSED(Module m))
{
    fflush(stdout);
    return 0;
}

/**/
int
features_(Module m, char ***features)
{
    *features = featuresarray(m, &module_features);
    return 0;
}

/**/
int
enables_(Module m, int **enables)
{
    return handlefeatures(m, &module_features, enables);
}

/**/
int
boot_(Module m)
{
    return 0;
}

/**/
int
cleanup_(Module m)
{
    return setfeatureenables(m, &module_features, NULL);
}

/**/
int
finish_(UNUSED(Module m))
{
    return 0;
}
