#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_SECCOMP
#include <seccomp.h>
#include <stdio.h>

int installSyscallFilter (void)
{
	// Use SCMP_ACT_TRAP to get a core dump.
	scmp_filter_ctx ctx = seccomp_init (SCMP_ACT_KILL);
	if (ctx == NULL)
	{
		return 1;
	}

	// Memory allocation.
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (mmap), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (brk), 0);

	// I/O
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (read), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (write), 0);

	// Clean exit
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (exit), 0);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (exit_group), 0);

	// The bowels of stdio want to know the size of a file, even for stdout.
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (fstat), 0);

	// seems unnecessary, but this comes from
	// main/parse.c:2764 : tagFilePosition (&tagfpos);
	seccomp_rule_add (ctx, SCMP_ACT_ALLOW, SCMP_SYS (lseek), 0);

	int err = seccomp_load (ctx);
	if (err < 0)
	{
		fprintf (stderr, "seccomp_load: %d", err);
	}

	seccomp_release (ctx);

	return err;
}

/*
   TODO: on OSX, Seatbelt
   (https://dev.chromium.org/developers/design-documents/sandbox/osx-sandboxing-design)
   should be used for equivalent functionality.
 */
#endif
