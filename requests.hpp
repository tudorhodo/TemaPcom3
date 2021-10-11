#ifndef _REQUESTS_
#define _REQUESTS_
#include "json.hpp"
using json=nlohmann::json;

// computes and returns a GET request string
char *compute_get_request(char *host, char *url, char *query_params, char **cookies, int cookies_count, char* token);

// computes and returns a POST request string
char *compute_post_request(char *host, char *url, char* content_type, json data, char *token);

// computes and retuns a DELETE request string
char *compute_delete_request(char *host, char *url, char *token);

#endif
