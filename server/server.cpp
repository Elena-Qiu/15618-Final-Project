/**
 * @file server.c
 * @author Chenfei Lou (Mike) <chenfeil@andrew.cmu.edu>
 * @brief A simple server that solves a four-color map problem given the map from clients
 * @date 2022-11-30
 */

#include "src/pipeline.h"
#include "src/csapp.h"

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>

// from tiny.c
#define HOSTLEN 256
#define SERVLEN 8
#define MAXLINE 8192 /* Max text line length */

/* Typedef for convenience */
typedef struct sockaddr SA;

/* Information about a connected client. */
typedef struct {
    struct sockaddr_in addr; // Socket address
    socklen_t addrlen;       // Socket address length
    int connfd;              // Client connection file descriptor
    char host[HOSTLEN];      // Client host
    char serv[SERVLEN];      // Client service (port)
} client_info;

/*
 * Debug macros, which can be enabled by adding -DDEBUG in the Makefile
 */
#ifdef DEBUG
#define dbg_assert(...) assert(__VA_ARGS__)
#define dbg_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dbg_assert(...)
#define dbg_printf(...)
#endif

static void serve(client_info *client);
static bool parse_request(rio_t *riop);
static bool parse_headers(rio_t *riop, size_t *content_lenp);
static bool extract_body(rio_t *riop, char **buf, size_t content_lenp);
static bool construct_response(char *solution, size_t solution_len , char **response, size_t *response_lenp);
static void sigpipe_handler(int sig);


const char *DEFAULT_PORT = "8080";
const char *VALID_URI = "/getMapSolution";



int main(int argc, char **argv) {
    Signal(SIGPIPE, sigpipe_handler); // Handles SIGPIPE signal

    // create a listen queue for clients
    char *port_str = getenv("port");
    if (port_str == NULL) {
        sio_eprintf("port environment variable not found, using default value (%s)\n", DEFAULT_PORT);
        port_str = (char *) DEFAULT_PORT;
    }
    int listenfd = open_listenfd(port_str);
    if (listenfd < 0) {
        sio_eprintf("Failed to listen on port %s\n", port_str);
        return -1;
    }

    // listen to clients and communicate with server periodically
    while (1) {
        /* Allocate space on the stack for client info */
        client_info *client = (client_info *)Malloc(sizeof(client_info));

        /* Initialize the length of the address */
        client->addrlen = sizeof(client->addr);
        client->connfd =
            accept(listenfd, (SA *)&client->addr, &client->addrlen);

        if (client->connfd < 0) {
            perror("accept");
            continue;
        }

        /* Connection is established; serve client sequentially */
        serve(client);
        close(client->connfd);
        Free(client);
    }

    close(listenfd);
}

/**
 * @brief parse the client's request; check for validity; send valid requests
 *        to server, wait for server's responses and return back to client
 *
 * @param client the structure containing client's information
 */
static void serve(client_info *client) {
    rio_t rio;
    rio_readinitb(&rio, client->connfd);

    // check the request
    if (parse_request(&rio) == false) {
        return;
    }

    // check the headers
    size_t bodylen = 0;
    if (parse_headers(&rio, &bodylen) == false) {
        return;
    }
    dbg_printf("request received from client by server, body length = %ld\n", bodylen);

    // extract request body from http request
    char *body = NULL;
    if (extract_body(&rio, &body, bodylen) == false) {
        return;
    }

    // dbg_printf("body extracted: %s\n", body);

    // solve the map coloring problem
    char *solution = NULL;
    size_t solution_len = 0;
    if (solveMap(body, &solution, &solution_len) == false) {
        Free(body);
        return;
    }
    Free(body);

    dbg_printf("solution generated, solution length=%ld\n", solution_len);

    char *response = NULL;
    size_t response_len = 0;
    if (construct_response(solution, solution_len, &response, &response_len) == false) {
        Free(solution);
        return;
    }
    Free(solution);

    // deliver server's reply back to client
    if (rio_writen(client->connfd, response, response_len) < 0) {
        sio_eprintf("error when sending server's reply to client\n");
        Free(response);
        return;
    }
    dbg_printf("reply sent from server to client\n");

    // clean up
    Free(response);
}

/**
 * @brief check the validity of the request stored in rio buffer {@link riop}.
 *
 * @param riop[in] the pointer to the rio buffer storing the request
 * @return true if parsing the request succeeds
 * @return false if parsing the request fails
 */
static bool parse_request(rio_t *riop) {
    /* Read request line */
    char buf[MAXLINE];
    if (rio_readlineb(riop, buf, sizeof(buf)) <= 0) {
        return false;
    }

    /* parse the request line */
    char delim[] = " \r\n\t";
	char *ptr=strtok(buf, delim), *method=NULL, *uri=NULL, *version=NULL;
    int count = 0;
    while(ptr != NULL) {
        if (count == 0) {
            method = ptr;
        } else if (count == 1) {
            uri = ptr;
        } else if (count == 2) {
            version = ptr;
        } else {
            sio_eprintf("invalid request line: %s\n", buf);
            return false;
        }

		ptr = strtok(NULL, delim);
        count ++;
	}
    if (count != 3) {
        sio_eprintf("invalid request line: %s\n", buf);
        return false;
    }
    if (strcmp(method, "POST") != 0) {
        sio_eprintf("only support POST method (get %s)\n", method);
        return false;
    }
    if (strcmp(uri, VALID_URI) != 0) {
        sio_eprintf("only support uri %s (get %s)\n", VALID_URI, uri);
        return false;
    }
    if (strcmp(version, "HTTP/1.0") !=0 && strcmp(version, "HTTP/1.1") !=0) {
        sio_eprintf("http version not valid (get %s)\n", version);
        return false;
    }

    return true;
}

/**
 * @brief check the validity of all http headers stored in rio buffer
 *
 * @param riop[in] the pointer to the rio buffer storing http headers
 * @param content_lenp[out] the pointer to an int that stores request body length
 * @return true if all headers are valid
 * @return false if at least one header is invalid
 */
static bool parse_headers(rio_t *riop, size_t *content_lenp) {
    /* Read headers in a loop */
    char line_buf[MAXLINE];
    while (true) {
        if (rio_readlineb(riop, line_buf, sizeof(line_buf)) <= 0) {
            return false;
        }

        /* Check for end of headers */
        if (strcmp(line_buf, "\r\n") == 0) {
            return true;
        }

        // parse the header line
        char *collon_pos = (char *) strchr(line_buf, ':');
        if (collon_pos == NULL ||
            collon_pos == line_buf ||
            (size_t) (collon_pos - line_buf) == strlen(line_buf))
        {
            sio_eprintf("invalid header: %s\n", line_buf);
            return false;
        }
        
        *collon_pos = 0;
        char *key = line_buf;
        char *val = collon_pos + 1;

        if (strcmp(key, "Content-length") == 0 || strcmp(key, "Content-Length") == 0) {
            *content_lenp = atoi(val);
        }
    }

    // should never go here
    return true;
}

/**
 * @brief extract the body of the http request
 *
 * @param riop rio structure that is bound to client's network socket
 * @param buf[out] the double pointer to a string buffer to store request body
 * @param content_len[in] the length of the request body
 * @return true delivery succeeds
 * @return false delivery fails
 */
static bool extract_body(rio_t *riop, char **buf, size_t content_len) {
    *buf = (char *) Malloc(content_len + 1);
    ssize_t read_length = rio_readnb(riop, *buf, content_len);
    *(*buf + content_len) = 0;
    if ((size_t) read_length != content_len) {
        sio_eprintf("reading %ld bytes (should be %ld bytes)\n", read_length,
                    content_len);
        Free(*buf);
        return false;
    }

    return true;
}

/**
 * @brief construct a response based on the map solver's solution
 *
 * @param solution string representing the solution
 * @param solution_len the length of the solution string
 * @param response a double poniter to the response buffer
 * @param response_len a pointer to the size of the response
 */
static bool construct_response(char *solution, size_t solution_len , char **response, size_t *response_lenp) {
    *response = (char *) Malloc(MAXBUF);

    *response_lenp = snprintf(*response, MAXBUF,
            "HTTP/1.0 200 OK\r\n" \
            "Server: Mike's Server\r\n" \
            "Access-Control-Allow-Origin: *\r\n" \
            "Connection: close\r\n" \
            "Content-Length: %ld\r\n" \
            "Content-Type: text/plain\r\n\r\n", \
            solution_len);
    if (*response_lenp >= MAXBUF) {
        sio_eprintf("header overflow!\n");
        return false;
    }


    size_t new_length = solution_len + *response_lenp;
    if (new_length > MAXBUF) {
        *response = (char *) Realloc(*response, new_length + 1);
    }
    // strcat(*response, solution)
    memcpy(*response + *response_lenp, solution, solution_len);
    // dbg_printf("Response:\n%s", *response);

    *response_lenp = new_length;
    return true;
}

/**
 * @brief handler for SIGPIPE signal
 *
 * @param sig
 */
static void sigpipe_handler(int sig) {
    // save errno
    int old_errno = errno;

    // block all signals
    sigset_t full_mask, old_mask;
    sigfillset(&full_mask);
    sigprocmask(SIG_SETMASK, &full_mask, &old_mask);

    // handler body
    dbg_printf("SIGPIPE received!\n");

    // unblock signals
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    // restore errno
    errno = old_errno;
}