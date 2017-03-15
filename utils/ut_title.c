/*
 * Description: 
 *     History: yang@haipo.me, 2016/03/26, create
 */

# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include <stdio.h>
# include <stddef.h>
# include <stdlib.h>
# include <stdarg.h>
# include <string.h>
# include <errno.h>

# include "ut_title.h"

extern char **environ;

static char *title_base;
static char *title_tail;

void process_title_init(int argc, char *argv[])
{
    if (title_base)
        return;

    char *base = argv[0];
    char *tail = argv[argc-1] + strlen(argv[argc-1]) + 1;

    char **envp = environ;
    for (int i = 0; envp[i]; ++i) {
        if (envp[i] < tail)
            continue;
        tail = envp[i] + strlen(envp[i]) + 1;
    }

    /* dup program name */
    program_invocation_name = strdup(program_invocation_name);
    program_invocation_short_name = strdup(program_invocation_short_name);

    /* dup argv */
    for (int i = 0; i < argc; ++i) {
        argv[i] = strdup(argv[i]);
    }

    /* dup environ */
    clearenv();
    for (int i = 0; envp[i]; ++i) {
        char *eq = strchr(envp[i], '=');
        if (eq == NULL)
            continue;

        *eq = '\0';
        setenv(envp[i], eq + 1, 1);
        *eq = '=';
    }

    title_base = base;
    title_tail = tail;
    memset(title_base, 0, title_tail - title_base);
}

void process_title_set(const char *fmt, ...)
{
    if (!title_base)
        return;

    char buf[256];
	va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    int title_len = len < (title_tail - title_base) ? len : (title_tail - title_base - 1);
    memcpy(title_base, buf, title_len);
    title_base[title_len] = '\0';

    return;
}

