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
#include <stdio.h>
#include <rime_api.h>

#include "rime.mdh"
#include "rime.pro"

#define DEFAULT_BUFFER_SIZE 1024
/* parameters */

static struct builtin bintab[] = {
    BUILTIN("rime", 0, rime, 0, -1, 0, "", NULL),
};

#define CHECK_NOT_NULL(arg, var)                                \
do {                                                            \
    if (arg == NULL) {                                          \
	fputs("missing " #var, stderr);                         \
	return EXIT_FAILURE;                                    \
    }                                                           \
} while (0);

#define GET_NUMBER(arg, var)                                    \
do {                                                            \
    char *_arg = arg;                                           \
    CHECK_NOT_NULL(_arg, var);                                  \
    var = zstrtol(_arg, NULL, 0);                               \
} while (0);

#define GET_POSITIVE_NUMBER(arg, var)                           \
do {                                                            \
    char *__arg = arg;                                          \
    GET_NUMBER(__arg, var);                                     \
    if (var == 0) {                                             \
	fprintf(stderr, "%s is not a legal " #var "\n", __arg); \
	return EXIT_FAILURE;                                    \
    }                                                           \
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

  unsigned int i = 1;
  while (args[i]) {
    if (ztrcmp(args[i], "shared_data_dir") == 0) {
      rime_traits.shared_data_dir = args[++i];
    }
    else if (ztrcmp(args[i], "user_data_dir") == 0) {
      rime_traits.user_data_dir = args[++i];
    }
    else if (ztrcmp(args[i], "log_dir") == 0) {
      rime_traits.log_dir = args[++i];
    }
    else if (ztrcmp(args[i], "distribution_name") == 0) {
      rime_traits.distribution_name = args[++i];
    }
    else if (ztrcmp(args[i], "distribution_code_name") == 0) {
      rime_traits.distribution_code_name = args[++i];
    }
    else if (ztrcmp(args[i], "distribution_version") == 0) {
      rime_traits.distribution_version = args[++i];
    }
    else if (ztrcmp(args[i], "app_name") == 0) {
      rime_traits.app_name = args[++i];
    }
    else if (ztrcmp(args[i], "min_log_level") == 0) {
      GET_NUMBER(args[++i], rime_traits.min_log_level);
    }
    else {
      fprintf(stderr, "%s is unknown\n", args[i]);
    }
    i++;
  }
  CHECK_NOT_NULL(rime_traits.shared_data_dir, shared_data_dir);
  CHECK_NOT_NULL(rime_traits.user_data_dir, user_data_dir);
  CHECK_NOT_NULL(rime_traits.log_dir, log_dir);
  CHECK_NOT_NULL(rime_traits.distribution_name, distribution_name);
  CHECK_NOT_NULL(rime_traits.distribution_code_name, distribution_code_name);
  CHECK_NOT_NULL(rime_traits.distribution_version, distribution_version);
  CHECK_NOT_NULL(rime_traits.app_name, app_name);
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
	    fprintf(stderr, "failed to destroy session %lu\n", session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "getCurrentSchema") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	char schema_id[DEFAULT_BUFFER_SIZE];
	if (!(RimeGetCurrentSchema(session_id, schema_id, DEFAULT_BUFFER_SIZE))) {
	    fprintf(stderr, "failed to get current schema of session %lu\n", session_id);
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
	    fprintf(stderr, "failed to select schema %s for session %lu\n", args[2], session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "processKey") == 0) {
	zulong session_id, keycode, mask;
	GET_POSITIVE_NUMBER(args[1], session_id);
	GET_NUMBER(args[2], keycode);
	GET_NUMBER(args[3], mask);
	if (!RimeProcessKey(session_id, keycode, mask)) {
	    fprintf(stderr, "failed to process key %lu with mask %lu for session %lu\n", keycode, mask, session_id);
	    return EXIT_FAILURE;
	}
    }
    else if (ztrcmp(args[0], "getContext") == 0) {
	zulong session_id;
	GET_POSITIVE_NUMBER(args[1], session_id);
	RIME_STRUCT(RimeContext, context);
	if (!(RimeGetContext(session_id, &context))) {
	    fprintf(stderr, "failed to get context of session %lu\n", session_id);
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
	    fprintf(stderr, "failed to get commit of session %lu\n", session_id);
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
