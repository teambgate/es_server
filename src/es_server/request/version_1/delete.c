/*
 * Copyright (C) 2017 Manh Tran
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
 */
#include <es_server/es_server.h>
#include "version.h"

#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/server/file_descriptor.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/bytes.h>
#include <cherry/math/math.h>
#include <cherry/server/socket.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <cherry/unistd.h>
#include <smartfox/data.h>

#include <common/command.h>
#include <common/key.h>
#include <common/error.h>

#include <curl/curl.h>

struct write_data {
        struct es_server *p;
        struct sfs_object *obj;
        int fd;
};

static void __response_invalid_data(struct es_server *p, int fd, struct sfs_object *obj, char *msg, size_t msg_len)
{
        struct sfs_object *res = sfs_object_alloc();
        sfs_object_set_long(res, qskey(&__key_request_id__), sfs_object_get_long(obj, qskey(&__key_request_id__), 0));
        sfs_object_set_bool(res, qskey(&__key_result__), 0);
        sfs_object_set_string(res, qskey(&__key_message__), msg, msg_len);
        sfs_object_set_long(res, qskey(&__key_error__), ERROR_DATA_INVALID);

        struct string *d        = sfs_object_to_json(res);
        es_server_send_to_client(p, fd, d->ptr, d->len);
        string_free(d);
        sfs_object_free(res);
}

static size_t func(void *ptr, size_t size, size_t nmemb, struct write_data *data)
{
        struct es_server *p = data->p;
        struct sfs_object *obj = data->obj;
        int fd = data->fd;

        size_t realsize = size * nmemb;

        struct sfs_object *res = sfs_object_alloc();
        sfs_object_set_long(res, qskey(&__key_request_id__), sfs_object_get_long(obj, qskey(&__key_request_id__), 0));
        sfs_object_set_bool(res, qskey(&__key_result__), 1);
        sfs_object_set_string(res, qskey(&__key_message__), qlkey("success"));

        int counter = 0;
        struct sfs_object *result = sfs_object_from_json(ptr, realsize, &counter);
        sfs_object_set_object(res, qskey(&__key_data__), result);

        struct string *d        = sfs_object_to_json(res);
        es_server_send_to_client(p, fd, d->ptr, d->len);
        string_free(d);
        sfs_object_free(res);

        return size * nmemb;
}

void es_server_process_delete_v1(struct es_server *p, int fd, struct sfs_object *obj)
{
        debug("call delete\n");
        struct string *pass             = sfs_object_get_string(obj, qskey(&__key_pass__), SFS_GET_REPLACE_IF_WRONG_TYPE);
        struct string *es_server_pass   = sfs_object_get_string(p->config, qskey(&__key_pass__), SFS_GET_REPLACE_IF_WRONG_TYPE);

        if(strcmp(es_server_pass->ptr, pass->ptr) != 0) {
                __response_invalid_data(p, fd, obj, qlkey("wrong password"));
                goto end;
        }

        struct string *path             = sfs_object_get_string(obj, qskey(&__key_path__), SFS_GET_REPLACE_IF_WRONG_TYPE);
        struct sfs_object *data         = sfs_object_get_object(obj, qskey(&__key_data__), SFS_GET_REPLACE_IF_WRONG_TYPE);

        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if(curl) {
                struct string *temp = string_alloc(0);
                string_cat(temp, qlkey("http://localhost:9200"));
                string_cat(temp, qskey(path));
                curl_easy_setopt(curl, CURLOPT_URL, temp->ptr);
                string_free(temp);

                /* example.com is redirected, so we tell libcurl to follow redirection */
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,"DELETE");
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
                struct write_data wdata;
                wdata.p = p;
                wdata.obj = obj;
                wdata.fd = fd;
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&wdata);

                temp = sfs_object_to_json(data);

                struct curl_slist *headers = NULL;
                curl_slist_append(headers, "Accept: application/json");
                curl_slist_append(headers, "Content-Type: application/json");
                curl_slist_append(headers, "charsets: utf-8");

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temp->ptr);
                //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, temp->len + 1);
                string_free(temp);

                /* Perform the request, res will get the return code */
                res = curl_easy_perform(curl);
                /* Check for errors */
                if(res != CURLE_OK)
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                curl_easy_strerror(res));

                /* always cleanup */
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
        }
end:;
}