/*-
 * Copyright (c) 2003 Mike Barcroft <mike@FreeBSD.org>
 * Copyright (c) 2007 Bill Moran/Collaborative Fusion
 * Copyright (c) 2015 Nicholas Kiraly/Intermedix
 * Copyright (c) 2015 Steve Polyack/Intermedix
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ''AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Intermedix.
 *
 * $FreeBSD: src/usr.sbin/jexec/jexec.c,v 1.2 2003/07/04 19:14:27 bmilekic Exp $
 */

#include <sys/param.h>
#include <sys/jail.h>
#include <jail.h>

#include <err.h>
#include <errno.h>
#include <login_cap.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>

static void	usage(void);

int
main(int argc, char *argv[])
{
	int jid, ngroups;
	uid_t huid;
	struct passwd *husername, *jusername;
	gid_t *groups = NULL;
	login_cap_t *lcap;
	long ngroups_max;

	if (argc < 3)
		usage();

	ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
	if ((groups = malloc(sizeof(gid_t) * ngroups_max)) == NULL)
		err(1, "malloc");

	/* Get the jail ID using jail_getid, which also supports jail names */
	jid = jail_getid(argv[1]);
	if (jid < 0)
		err(1, "%s", jail_errmsg);

	/* Get the current user ID and user name in the host system */
	huid = getuid();
	husername = getpwuid(huid);
	/* Get the user name in the jail */
	if (jail_attach(jid) == -1)
		err(1, "jail_attach: %d", jid);
	jusername = getpwuid(huid);
	if (jusername == NULL)
		err(1, "UID mapping failed");
	if (strcmp(husername->pw_name, jusername->pw_name) != 0)
		err(1, "Username mapping failed");
	/* Gather additional user info */
	if (chdir(jusername->pw_dir) == -1)
		err(1, "chdir: %s", jusername->pw_dir);
	lcap = login_getpwclass(jusername);
	if (lcap == NULL)
		err(1, "getpwclass: %s", jusername->pw_name);
	ngroups = ngroups_max;
	if (getgrouplist(jusername->pw_name, jusername->pw_gid, groups, &ngroups) != 0)
		err(1, "getgrouplist: %s", jusername->pw_name);
	if (setgroups(ngroups, groups) != 0)
		err(1, "setgroups");
	if (setgid(jusername->pw_gid) != 0)
		err(1, "setgid");
	if (setusercontext(lcap, jusername, jusername->pw_uid,
	    LOGIN_SETALL & ~LOGIN_SETGROUP & ~LOGIN_SETLOGIN) != 0)
		err(1, "setusercontext");
	login_close(lcap);
	if (execvp(argv[2], argv + 2) == -1)
		err(1, "execvp: %s", argv[2]);
	exit(0);
}

static void
usage(void)
{

	fprintf(stderr, "usage: jailme jid|jailname command [...]\n");
	exit(1);
}
