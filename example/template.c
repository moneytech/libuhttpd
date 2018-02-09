/*
 * Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uhttpd.h>
#include <libubox/ulog.h>

static void hello_action(struct uh_client *cl)
{
    uh_template(cl);
}

static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [option]\n"
        "          -p port  # Default port is 8080\n"
        "          -s       # SSl on\n"
        "          -v       # verbose\n", prog);
    exit(1);
}

int main(int argc, char **argv)
{
    struct uh_server *srv = NULL;
    bool verbose = false;
    bool ssl = false;
    int port = 8080;
    int opt;

    while ((opt = getopt(argc, argv, "p:vs")) != -1) {
        switch (opt)
        {
        case 'p':
            port = atoi(optarg);
            break;
        case 's':
            ssl = true;
            break;
        case 'v':
            verbose = true;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (!verbose)
        ulog_threshold(LOG_ERR);
    
    ULOG_INFO("libuhttpd version: %s\n", UHTTPD_VERSION_STRING);

    uloop_init();

    srv = uh_server_new("0.0.0.0", port);
    if (!srv)
        goto done;

#if (!UHTTPD_SSL_SUPPORT)
    if (ssl)
        ULOG_ERR("SSl is not compiled in\n");
#else
    if (ssl && srv->ssl_init(srv, "server-key.pem", "server-cert.pem") < 0)
        goto done;
#endif

    ULOG_INFO("Listen on: %s *:%d\n", srv->ssl ? "https" : "http", port);

    srv->add_action(srv, "/template.html", hello_action);
    
    uloop_run();
done:
    uloop_done();
    srv->free(srv);
    
    return 0;
}