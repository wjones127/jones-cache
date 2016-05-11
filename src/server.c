/*#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>*/

//#include "cache.h"
#include <event2/event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "evhtp.h"

void testcb(evhtp_request_t * req, void * a)
{
    const char * str = a;

    evbuffer_add(req->buffer_out, str, strlen(str));
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void handler_greeting(evhtp_request_t * req, void * a)
{
    const char *str = "<html><body><h1>Hello! Welcome to jones-cache. Here's the API:</h1>\n\n"
        "<ul><li>GET /k: returns JSON tuple of item in cache with key k</li>\n"
        "<li>PUT /k/v: set key k with value v in cache</li>\n"
        "<li>DELETE /k: deletes entry with k</li>\n"
        "<li>HEAD /k: returns just header</li>\n"
        "<li>POST /shutdown: shuts the server down</li>\n"
        "<li>POST /memsize/value: resets cache with new maxvalue</li>\n"
        "</ul></body></html>";

    // Add Content-Type
    evhtp_kv_t *header = evhtp_header_new("Content-Type",  "text/html", 0, 1);
    evhtp_headers_add_header(req->headers_out, header);

    evbuffer_add(req->buffer_out, str, strlen(str));
    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg
}

void respond_not_found(evhtp_request_t *req)
{
    const char *str = "Not found.";
    evbuffer_add(req->buffer_out, str, strlen(str));
    evhtp_send_reply(req, EVHTP_RES_NOTFOUND);
}

// Test w/: curl -i -X HEAD localhost:8081/4
void handler_header(evhtp_request_t *req, void *a)
{
    // check for correction HTTP request type
    if (evhtp_request_get_method(req) != htp_method_HEAD) {
        respond_not_found(req);
        return;
    }
    
    // Add Content-Type
    evhtp_kv_t *header = evhtp_header_new("Content-Type",  "application/json", 0, 1);
    evhtp_headers_add_header(req->headers_out, header);

    // Add Accept
    header = evhtp_header_new("Accept",  "text/plain", 0, 1);
    evhtp_headers_add_header(req->headers_out, header);
    
    // Add Date
    char time_string[80];
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    strftime(time_string, sizeof(time_string)-1, "%a, %d %b %Y %T %z", timeinfo);

    header = evhtp_header_new("Date", time_string, 0, 1);
    evhtp_headers_add_header(req->headers_out, header);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg
}


void issue161cb(evhtp_request_t * req, void * a)
{
    struct evbuffer * b = evbuffer_new();

    if (evhtp_request_get_proto(req) == EVHTP_PROTO_10) {
        evhtp_request_set_keepalive(req, 0);
    }

    evhtp_send_reply_start(req, EVHTP_RES_OK);

    evbuffer_add(b, "foo", 3);
    evhtp_send_reply_body(req, b);

    evbuffer_add(b, "bar\n\n", 5);
    evhtp_send_reply_body(req, b);

    evhtp_send_reply_end(req);

    evbuffer_free(b);
}

int
main(int argc, char ** argv) {
    evbase_t * evbase = event_base_new();
    evhtp_t  * htp    = evhtp_new(evbase, NULL);

    //evhtp_set_cb(htp, "/", handler_greeting, NULL);
    evhtp_set_regex_cb(htp, "/[0-9]+$", handler_header, NULL);
    evhtp_set_cb(htp, "/simple/", testcb, "simple");
    evhtp_set_cb(htp, "/1/ping", testcb, "one");
    evhtp_set_cb(htp, "/1/ping.json", testcb, "two");
    evhtp_set_cb(htp, "/issue161", issue161cb, NULL);
    #ifndef EVHTP_DISABLE_EVTHR
    evhtp_use_threads(htp, NULL, 8, NULL);
    #endif
    evhtp_bind_socket(htp, "0.0.0.0", 8081, 2048);

    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_free(htp);
    event_base_free(evbase);

    return 0;
}
