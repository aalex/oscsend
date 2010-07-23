/*
 * oscdump - Receive and dump OpenSound Control messages.
 *
 * Copyright (C) 2008 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <lo/lo.h>

void usage(void)
{
	printf("oscdump version %s\n"
		   "Copyright (C) 2008 Kentaro Fukuchi\n\n"
		   "Usage: oscdump port [--tcp] \n"
		   "Receive OpenSound Control messages via UDP and dump to standard output.\n\n"
		   "Arguments description\n"
		   " * port    : specifies the listening port number.\n\n"
		   "Options\n"
		   " * --help    : prints help and exits.\n\n"
		   " * --version    : prints version and exits.\n\n",
		   VERSION);
}

void errorHandler(int num, const char *msg, const char *where)
{
	printf("liblo server error %d in path %s: %s\n", num, where, msg);
}

int messageHandler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
	int i;

	printf("%s %s", path, types);

	for(i=0; i<argc; i++) {
		printf(" ");
		lo_arg_pp((lo_type)types[i], argv[i]);
	}
	printf("\n");

	return 0;
}

void print_version()
{
    printf("%s\n", VERSION);
}

int main(int argc, char **argv)
{
	lo_server server;
    int proto = 0;
	if(argc < 2) {
		usage();
		exit(1);
	}
    if (argc == 3) {
        if (strncmp(argv[2], "--tcp", 5) == 0) {
            proto = LO_TCP;
            printf("using TCP\n");
        } else {
            proto = LO_UDP;
        }
    }
    if (strncmp(argv[1], "--help", 6) == 0) {
        usage();
        exit(0);
    }
    if (strncmp(argv[1], "--version", 9) == 0) {
        print_version();
        exit(0);
    }

	//server = lo_server_new(argv[1], errorHandler);
    
	server = lo_server_new_with_proto(argv[1], proto, errorHandler);
	if(server == NULL) {
		printf("Could not start a server with port %s\n", argv[1]);
		exit(1);
	} else {
		printf("Running a server with port %s\n", argv[1]);
    }

	lo_server_add_method(server, NULL, NULL, messageHandler, NULL);


	for(;;) {
		lo_server_recv_noblock(server, 10); // how long to wait in milli
        //printf(".");
	}
    // TODO: handle signals such as in http://github.com/aalex/proto-toonloop/blob/signals/sigs.c
    lo_server_free(server);

	return 0;
}
