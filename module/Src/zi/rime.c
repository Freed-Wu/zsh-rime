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
    BUILTIN("rime", 0, rime, 0, -1, 0, "", NULL),
};

#define GET_NUMBER(arg, var)                             \
do {                                                     \
    if (arg == NULL) {                                   \
	fputs("missing " #var, stderr);                  \
	return EXIT_FAILURE;                             \
    }                                                    \
    var = zstrtol(arg, NULL, 0);                         \
} while (0);

#define GET_POSITIVE_NUMBER(arg, var)                    \
do {                                                     \
    GET_NUMBER(arg, var);                                \
    if (var == 0) {                                      \
	fprintf(stderr, "%s is not a legal " #var, arg); \
	return EXIT_FAILURE;                             \
    }                                                    \
} while (0);

/**/
static int
rime(char *nam, char **args, Options ops, UNUSED(int func))
{
    char usage[] = "rime init [arguments...]\n"
		   "rime createSession [session_id]\n"
		   "rime destroySession $session_id\n"
		   "rime getCurrentSchema $session_id [schema_id]\n"
		   "rime getSchemaList [schema_list]\n"
		   "rime selectSchema $session_id $schema_id\n"
		   "rime processKey $session_id $keycode $mask\n"
		   "rime getContext $session_id [context_composition] [context_menu] [context_menu_candidates_text] [context_menu_candidates_comment]\n"
		   "rime getCommit $session_id [commit]\n"
		   "rime commitComposition $session_id\n"
		   "rime clearComposition $session_id";
    if (args[0] == NULL) {
    	puts(usage);
	return EXIT_SUCCESS;
    }
    if (ztrcmp(args[0], "init") == 0) {
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
	    return EXIT_FAILURE;
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
	    return EXIT_FAILURE;
	}

	rime_traits.distribution_name = "Rime";
	rime_traits.distribution_code_name = "zsh-rime";
	rime_traits.distribution_version = "0.0.1";
	rime_traits.app_name = "rime.zsh-rime";
	rime_traits.min_log_level = 3;
	RimeSetup(&rime_traits);
	RimeInitialize(&rime_traits);
    }
    else if (ztrcmp(args[0], "createSession") == 0) {
	RimeSessionId session_id = RimeCreateSession();
	if (session_id == 0) {
	    fputs("failed to create session", stderr);
	    return EXIT_FAILURE;
	}
	setiparam(args[1] ? args[1] : "rime_session_id", session_id);
    }
    else if (ztrcmp(args[0], "destroySession") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	if (!RimeDestroySession(session_id)) {
	    fprintf(stderr, "failed to destroy session %lu", session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "getCurrentSchema") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	char schema_id[DEFAULT_BUFFER_SIZE];
	if (!(RimeGetCurrentSchema(session_id, schema_id, DEFAULT_BUFFER_SIZE))) {
	    fprintf(stderr, "failed to get current schema of session %lu", session_id);
	    return EXIT_FAILURE;
	}
	setsparam(args[2] ? args[2] : "rime_schema_id", ztrdup(schema_id));
    }
    else if (ztrcmp(args[0], "getSchemaList") == 0) {
	RimeSchemaList schema_list;
	if (!RimeGetSchemaList(&schema_list)) {
	    fputs("failed to get schema list", stderr);
	    return EXIT_FAILURE;
	}
	char **val, **ptr = val = (char **)zalloc((schema_list.size + 1) * 2 * sizeof(char *));
	for (size_t i = 0; i < schema_list.size; i++) {
	    *ptr++ = ztrdup(schema_list.list[i].schema_id);
	    *ptr++ = ztrdup_metafy(schema_list.list[i].name);
	}
	*ptr++ = ztrdup(NULL);
	*ptr++ = ztrdup(NULL);
	sethparam(args[1] ? args[1] : "rime_schema_list", val);
	/*RimeFreeSchemaList(&schema_list);*/
    }
    else if (ztrcmp(args[0], "selectSchema") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	if (args[2] == NULL) {
	    fputs("missing schema_id", stderr);
	    return EXIT_FAILURE;
	}
	if (!RimeSelectSchema(session_id, args[2])) {
	    fprintf(stderr, "failed to select schema %s for session %lu", args[2], session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "processKey") == 0) {
	zulong session_id, keycode, mask;
	GET_POSITIVE_NUMBER(args[1], session_id);
	GET_NUMBER(args[2], keycode);
	GET_NUMBER(args[3], mask);
	if (!RimeProcessKey(session_id, keycode, mask)) {
	    fprintf(stderr, "failed to process key %lu with mask %lu for session %lu", keycode, mask, session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "getContext") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	RIME_STRUCT(RimeContext, context);
	if (!(RimeGetContext(session_id, &context))) {
	    fprintf(stderr, "failed to get context of session %lu", session_id);
	    return EXIT_FAILURE;
	}
	char str[DEFAULT_BUFFER_SIZE];
	char **val, **ptr;

	ptr = val = (char **)zalloc((5 + 1) * 2 * sizeof(char *));
	sprintf(str, "%d", context.composition.length);
	*ptr++ = ztrdup("length");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.composition.cursor_pos);
	*ptr++ = ztrdup("cursor_pos");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.composition.sel_start);
	*ptr++ = ztrdup("sel_start");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.composition.sel_end);
	*ptr++ = ztrdup("sel_end");
	*ptr++ = ztrdup(str);
	*ptr++ = ztrdup("preedit");
	*ptr++ = ztrdup_metafy(context.composition.preedit ? context.composition.preedit : "");
	*ptr++ = ztrdup(NULL);
	*ptr++ = ztrdup(NULL);
	sethparam(args[2] ? args[2] : "rime_context_composition", val);

	ptr = val = (char **)zalloc((6 + 1) * 2 * sizeof(char *));
	sprintf(str, "%d", context.menu.page_size);
	*ptr++ = ztrdup("page_size");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.menu.page_no);
	*ptr++ = ztrdup("page_no");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.menu.is_last_page);
	*ptr++ = ztrdup("is_last_page");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.menu.highlighted_candidate_index);
	*ptr++ = ztrdup("highlighted_candidate_index");
	*ptr++ = ztrdup(str);
	sprintf(str, "%d", context.menu.num_candidates);
	*ptr++ = ztrdup("num_candidates");
	*ptr++ = ztrdup(str);
	*ptr++ = ztrdup("select_keys");
	*ptr++ = ztrdup(context.menu.select_keys ? context.menu.select_keys : "");
	*ptr++ = ztrdup(NULL);
	*ptr++ = ztrdup(NULL);
	sethparam(args[3] ? args[3] : "rime_context_menu", val);

	ptr = val = (char **)zalloc((context.menu.num_candidates + 1) * 2 * sizeof(char *));
	for (size_t i = 0; i < context.menu.num_candidates; i++) {
	    *ptr++ = ztrdup_metafy(context.menu.candidates[i].text ? context.menu.candidates[i].text : "");
	}
	*ptr++ = ztrdup(NULL);
	setaparam(args[4] ? args[4] : "rime_context_menu_candidates_text", val);

	ptr = val = (char **)zalloc((context.menu.num_candidates + 1) * 2 * sizeof(char *));
	for (size_t i = 0; i < context.menu.num_candidates; i++) {
	    *ptr++ = ztrdup_metafy(context.menu.candidates[i].comment ? context.menu.candidates[i].comment : "");
	}
	*ptr++ = ztrdup(NULL);
	setaparam(args[5] ? args[5] : "rime_context_menu_candidates_comment", val);

	RimeFreeContext(&context);
    }
    else if (ztrcmp(args[0], "getCommit") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	RIME_STRUCT(RimeCommit, commit);
	if (!(RimeGetCommit(session_id, &commit))) {
	    fprintf(stderr, "failed to get commit of session %lu", session_id);
	    return EXIT_FAILURE;
	}
	char **val, **ptr = val = (char **)zalloc((1 + 1) * 2 * sizeof(char *));
	*ptr++ = ztrdup("text");
	*ptr++ = ztrdup_metafy(commit.text);
	*ptr++ = ztrdup(NULL);
	*ptr++ = ztrdup(NULL);
	sethparam(args[2] ? args[2] : "rime_commit", val);
	RimeFreeCommit(&commit);
    }
    else if (ztrcmp(args[0], "commitComposition") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	return RimeCommitComposition(session_id) == True ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if (ztrcmp(args[0], "clearComposition") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	RimeClearComposition(session_id);
    } else {
    	fputs(usage, stderr);
	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*
 * boot_ is executed when the module is loaded.
 */

static struct conddef cotab[] = {
};

static struct paramdef patab[] = {
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
    return EXIT_SUCCESS;
}
