/*
 * rime.c - an rime module for zsh
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
#include <dirent.h>
#include <rime_api.h>
#include <wordexp.h>

#include "rime.mdh"
#include "rime.pro"

#define DEFAULT_BUFFER_SIZE 1024
/* parameters */

static struct builtin bintab[] = {
    BUILTIN("rime", 0, rime, 0, -1, 0, "CDglspc", NULL),
};

static zulong session_id;
static char *schema_id;
static char **schema_ids;
static char **schema_names;
static char **candidates;

/**/
static int
rime(char *nam, char **args, Options ops, UNUSED(int func))
{
    bool flag = false;
    unsigned char *c;
    for (c = (unsigned char *)bintab[0].optstr; *c; c++) {
	if (!OPT_ISSET(ops, *c))
	    continue;
	flag = true;
	switch (*c) {
	    case 'C':
		if ((session_id = RimeCreateSession()))
		    break;
		return EXIT_FAILURE;
	    case 'D':
		if (RimeDestroySession(session_id))
		    break;
		return EXIT_FAILURE;
		char buffer[DEFAULT_BUFFER_SIZE];
	    case 'g':
		if (!RimeGetCurrentSchema(session_id, buffer, DEFAULT_BUFFER_SIZE))
		    return EXIT_FAILURE;
		zsfree(schema_id);
		schema_id = ztrdup(buffer);
		break;
		RimeSchemaList schema_list;
	    case 'l':
		if (!RimeGetSchemaList(&schema_list))
		    return EXIT_FAILURE;
		size_t size = (schema_list.size + 1) * sizeof(char *);
		char **ids = zalloc(size);
		for (size_t i = 0; i < schema_list.size; i++)
		    ids[i] = schema_list.list[i].schema_id;
		ids[schema_list.size] = NULL;
		if (schema_ids)
		    freearray(schema_ids);
		schema_ids = zarrdup(ids);
		freearray(ids);

		char **names = zalloc(size);
		for (size_t i = 0; i < schema_list.size; i++)
		    names[i] = metafy(schema_list.list[i].name, strlen(schema_list.list[i].name), META_DUP);
		names[schema_list.size] = NULL;
		if (schema_names)
		    freearray(schema_names);
		schema_names = zarrdup(names);
		freearray(names);
		// double free()
		// RimeFreeSchemaList(&schema_list);
		break;
	    case 's':
		if (args[0] == NULL) {
		    fputs("rime -s schema_id", stderr);
		    return 2;
		}
		if (RimeSelectSchema(session_id, args[0])) {
		    zsfree(schema_id);
		    schema_id = ztrdup(args[0]);
		    return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
		int mask;
	    case 'p':
		mask = args[0] && args[1] ? strtol(args[1], NULL, 0) : 0;
		// mbstowcs() will bring segmentation fault
		if (args[0])
		    for (char *key = args[0]; *key; key++) {
			if (!RimeProcessKey(session_id, *key, mask)) {
			    return EXIT_FAILURE;
			}
		    }
		else
		    // ； = 0xff1b
		    return RimeProcessKey(session_id, 0xff1b, mask) ? EXIT_SUCCESS : EXIT_FAILURE;
		return EXIT_SUCCESS;
		RIME_STRUCT(RimeContext, context);
	    case 'c':
		if (!RimeGetContext(session_id, &context))
		    return EXIT_FAILURE;
		size = (context.menu.num_candidates + 1) * sizeof(char *);
		char **_candidates = zalloc(size);
		for (int i = 0; i < context.menu.num_candidates; i++)
		    _candidates[i] = metafy(context.menu.candidates[i].text, strlen(context.menu.candidates[i].text), META_DUP);
		_candidates[context.menu.num_candidates] = NULL;
		if (candidates)
		    freearray(candidates);
		candidates = zarrdup(_candidates);
		freearray(_candidates);
		return EXIT_SUCCESS;
	}
    }
    if (flag)
    	return EXIT_SUCCESS;
    if (!args[0]) {
	printf("rime [-%s] ...\n", bintab[0].optstr);
	return EXIT_SUCCESS;
    }
    int mask = args[1] ? strtol(args[1], NULL, 0) : 0;
    // mbstowcs() will bring segmentation fault
    for (char *key = args[0]; *key; key++) {
	if (!RimeProcessKey(session_id, *key, mask)) {
	    return EXIT_FAILURE;
	}
    }
    RIME_STRUCT(RimeContext, context);
    if (!RimeGetContext(session_id, &context))
	return EXIT_FAILURE;
    char *_candidates[DEFAULT_BUFFER_SIZE] = {};
    size_t num = 0;
    do {
	for (int i = 0; i < context.menu.num_candidates; i++) {
	    _candidates[num + i] = metafy(context.menu.candidates[i].text, strlen(context.menu.candidates[i].text), META_DUP);
	}
	num += context.menu.num_candidates;
	if (!RimeProcessKey(session_id, '=', 0))
	    break;
	if (!RimeGetContext(session_id, &context))
	    break;
    } while (!context.menu.is_last_page && num + context.menu.page_size < DEFAULT_BUFFER_SIZE);
    if (candidates)
	freearray(candidates);
    candidates = zarrdup(_candidates);
    RimeClearComposition(session_id);
    return EXIT_SUCCESS;
}

/*
 * boot_ is executed when the module is loaded.
 */

static struct conddef cotab[] = {
};

static struct paramdef patab[] = {
    INTPARAMDEF("rime_session_id", &session_id),
    STRPARAMDEF("rime_schema_id", &schema_id),
    ARRPARAMDEF("rime_schema_ids", &schema_ids),
    ARRPARAMDEF("rime_schema_names", &schema_names),
    ARRPARAMDEF("rime_candidates", &candidates),
};

static struct mathfunc mftab[] = {
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
    RIME_STRUCT(RimeTraits, rime_traits);
    char shared_data_dirs[][DEFAULT_BUFFER_SIZE] = {
	"",
	"/run/current-system/sw/share/rime-data",
	"/usr/share/rime-data",
	"/usr/local/share/rime-data",
	"/sdcard/rime-data",
    };
    char *prefix = getenv("PREFIX");
    char log_dir[DEFAULT_BUFFER_SIZE];
    if (prefix) {
	ztrncpy(prefix, log_dir, strlen(prefix));
	ztrncpy("/tmp/zsh-rime", log_dir + strlen(prefix), sizeof("/tmp/zsh-rime") - 1);
	rime_traits.log_dir = log_dir;
	// ignore sucess of failure
	mkdir(rime_traits.log_dir, 0777);
	ztrncpy(prefix, shared_data_dirs[0], strlen(prefix));
	ztrncpy("/share/rime-data", shared_data_dirs[0] + strlen(prefix), sizeof("/share/rime-data") - 1);
    }
    zsfree(prefix);
    unsigned int i;
    for (i = 0; i < sizeof(shared_data_dirs)/sizeof(shared_data_dirs[0]); i++) {
	DIR *dir = opendir(shared_data_dirs[i]);
	if (dir) {
	    closedir(dir);
	    rime_traits.shared_data_dir = shared_data_dirs[i];
	    break;
	}
    }
    if (rime_traits.shared_data_dir == NULL) {
	fputs("no any shared_data_dir!", stderr);
	return EXIT_SUCCESS;
    }

    char user_data_dirs[][DEFAULT_BUFFER_SIZE] = {
	"~/.config/ibus/rime",
	"~/.local/share/fcitx5/rime",
	"~/.config/fcitx/rime",
	"/sdcard/rime",
    };
    for (i = 0; i < sizeof(user_data_dirs)/sizeof(user_data_dirs[0]); i++) {
	wordexp_t exp;
	if (wordexp(user_data_dirs[i], &exp, 0) != 0)
	    continue;
	DIR *dir = opendir(exp.we_wordv[0]);
	if (dir) {
	    closedir(dir);
	    rime_traits.user_data_dir = strdup(exp.we_wordv[0]);
	    wordfree(&exp);
	    break;
	}
	wordfree(&exp);
    }
    if (rime_traits.user_data_dir == NULL) {
	fputs("no any user_data_dir!", stderr);
	return EXIT_SUCCESS;
    }

    rime_traits.distribution_name = "Rime";
    rime_traits.distribution_code_name = "zsh-rime";
    rime_traits.distribution_version = "0.0.1";
    rime_traits.app_name = "rime.zsh-rime";
    rime_traits.min_log_level = 3;
    RimeSetup(&rime_traits);
    RimeInitialize(&rime_traits);
    RimeStartMaintenance(False);
    return EXIT_SUCCESS;
}

/**/
int
features_(Module m, char ***features)
{
    *features = featuresarray(m, &module_features);
    return EXIT_SUCCESS;
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
    return EXIT_SUCCESS;
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
    if (session_id)
	return RimeDestroySession(session_id) ? EXIT_SUCCESS : EXIT_FAILURE;
    return EXIT_SUCCESS;
}
