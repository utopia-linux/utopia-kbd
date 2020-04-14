/*
 * loadunimap.c - aeb
 *
 * Version 1.09
 */
#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "libcommon.h"
#include "kfont.h"

extern char *progname;

static void __attribute__((noreturn))
usage(void)
{
	fprintf(stderr,
	        _("Usage:\n\t%s [-C console] [-o map.orig]\n"), progname);
	exit(1);
}

int main(int argc, char *argv[])
{
	int fd, c, ret;
	char *console = NULL;
	char *outfnam = NULL;
	const char *infnam  = "def.uni";

	set_progname(argv[0]);
	setuplocale();

	if (argc == 2 &&
	    (!strcmp(argv[1], "-V") || !strcmp(argv[1], "--version")))
		print_version_and_exit();

	while ((c = getopt(argc, argv, "C:o:")) != EOF) {
		switch (c) {
			case 'C':
				console = optarg;
				break;
			case 'o':
				outfnam = optarg;
				break;
			default:
				usage();
		}
	}

	if (argc > optind + 1 || (argc == optind && !outfnam))
		usage();

	if ((fd = getfd(console)) < 0)
		kbd_error(EXIT_FAILURE, 0, _("Couldn't get a file descriptor referring to the console"));

	struct kfont_context ctx;
	kfont_init(&ctx);

	if (outfnam) {
		if ((ret = saveunicodemap(&ctx, fd, outfnam)) < 0)
			exit(-ret);
		if (argc == optind)
			exit(0);
	}

	if (argc == optind + 1)
		infnam = argv[optind];
	if ((ret = loadunicodemap(&ctx, fd, infnam)) < 0)
		exit(-ret);
	exit(0);
}
