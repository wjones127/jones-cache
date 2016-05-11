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

void add_std_headers(evhtp_request_t *req)
{
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
}
/*
  HEAD /k: Just respond with header
  Test w/: curl -i -X HEAD localhost:8081/4
*/
void handler_header(evhtp_request_t *req, void *a)
{
    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg
}

/*
  GET /k: Get entry with key k
  Test w/: curl -i -X GET localhost:8081/4
*/
void handler_get(evhtp_request_t *req, void *a)
{
    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg
}

/*
  DELETE /k: Delete entry with key k
  Test w/: curl -i -X DELETE localhost:8081/4
*/
void handler_delete(evhtp_request_t *req, void *a)
{
    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg
}

void handler_main(evhtp_request_t *req, void *a)
{
   if (evhtp_request_get_method(req) == htp_method_GET)
       handler_get(req, a);
   else if (evhtp_request_get_method(req) == htp_method_HEAD)
       handler_header(req, a);
   else if (evhtp_request_get_method(req) == htp_method_DELETE)
       handler_delete(req, a);
   else
       respond_not_found(req);
}

/*
  PUT /k/v: Set entry k with value v
  Test w/: curl -i -X PUT localhost:8081/4/32f4
*/
void handler_set(evhtp_request_t *req, void *a)
{
    if (evhtp_request_get_method(req) != htp_method_PUT) {
        respond_not_found(req);
        return;
    }

    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg    
}

/*
  POST /shutdown: Shutdown server
  Test w/: curl -i -X POST localhost:8081/shutdown
*/
void handler_shutdown(evhtp_request_t *req, void *a)
{
    if (evhtp_request_get_method(req) != htp_method_POST) {
        respond_not_found(req);
        return;
    }

    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg    
}

/*
  POST /memsize/value: Start server with memsize of value
  Test w/: curl -i -X POST localhost:8081/memsize/4302
*/
void handler_setup(evhtp_request_t *req, void *a)
{
    if (evhtp_request_get_method(req) != htp_method_POST) {
        respond_not_found(req);
        return;
    }

    add_std_headers(req);

    evhtp_send_reply(req, EVHTP_RES_OK); // Response code is 2nd arg    
}


int main(int argc, char ** argv)
{
    evbase_t * evbase = event_base_new();
    evhtp_t  * htp    = evhtp_new(evbase, NULL);

    evhtp_set_cb(htp, "/", handler_greeting, NULL);
    evhtp_set_regex_cb(htp, "^/[0-9]+$", handler_main, NULL);
    evhtp_set_regex_cb(htp, "^/[0-9]+/[A-Za-z0-9]+$", handler_set, NULL);
    evhtp_set_cb(htp, "/shutdown", handler_shutdown, NULL);
    evhtp_set_regex_cb(htp, "^/memsize/[0-9]+$", handler_setup, NULL);
    
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
