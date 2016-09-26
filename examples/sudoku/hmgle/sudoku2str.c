#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "debug_print.h"

static void show_usage(const char *pro_name)
{
	fprintf(stderr, "Usage: %s [Options] [inputfile]\n"
			"\n"
			"Options:\n"
			"        -h show this help\n"
			"\n", pro_name);
}

int main(int argc, char **argv)
{
	char str[81 + 1];
	int opt;
	FILE *in = stdin;

	while ((opt = getopt(argc, argv, "h?")) != -1) {
		switch (opt) {
		case 'h':
		default:
			show_usage(argv[0]);
			exit(1);
		}
	}
	if (argc > optind) {
		in = fopen(argv[optind], "r");
		if (!in) {
			fprintf(stderr,
				"Unable to open input file \"%s\": %s\n",
				argv[optind], strerror(errno));
			return 1;
		}
	}
	memset(str, '0', 81);
	str[81] = '\0';
	char ch;
	int n = 0;
	while (n < 81 && (ch = fgetc(in)) != EOF) {
		if ((ch >= '0' && ch <= '9') ||
		    ch == '*' || ch == '.' ||
		    ch == 'x' || ch == 'X') {
			str[n++] = ch;
		}
	}
	printf("%s\n", str);
	if (in != stdin)
		fclose(in);
	return 0;
}
