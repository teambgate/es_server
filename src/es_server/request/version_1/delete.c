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
#include <common/cs_server.h>

#include <curl/curl.h>

struct write_data {
        struct cs_server *p;
        struct sobj *obj;
        int fd;
        u32 mask;
};

static void __response_invalid_data(struct cs_server *p, int fd, u32 mask, struct sobj *obj, char *msg, size_t msg_len)
{
        struct sobj *res = sobj_alloc();
        sobj_set_i64(res, qskey(&__key_request_id__), sobj_get_i64(obj, qskey(&__key_request_id__), 0));
        sobj_set_u8(res, qskey(&__key_result__), 0);
        sobj_set_str(res, qskey(&__key_message__), msg, msg_len);
        sobj_set_i64(res, qskey(&__key_error__), ERROR_DATA_INVALID);
        struct string *cmd = sobj_get_str(obj, qskey(&__key_cmd__), RPL_TYPE);
        sobj_set_str(res, qskey(&__key_cmd__), qskey(cmd));

        struct string *d        = sobj_to_json(res);
        cs_server_send_to_client(p, fd, mask, d->ptr, d->len, 0);
        string_free(d);
        sobj_free(res);
}

static size_t func(void *ptr, size_t size, size_t nmemb, struct write_data *data)
{
        struct cs_server *p = data->p;
        struct sobj *obj = data->obj;
        int fd = data->fd;
        u32 mask = data->mask;

        size_t realsize = size * nmemb;

        struct sobj *res = sobj_alloc();
        sobj_set_i64(res, qskey(&__key_request_id__), sobj_get_i64(obj, qskey(&__key_request_id__), 0));
        sobj_set_u8(res, qskey(&__key_result__), 1);
        sobj_set_str(res, qskey(&__key_message__), qlkey("success"));
        struct string *cmd = sobj_get_str(obj, qskey(&__key_cmd__), RPL_TYPE);
        sobj_set_str(res, qskey(&__key_cmd__), qskey(cmd));

        int counter = 0;
        struct sobj *result = sobj_from_json(ptr, realsize, &counter);
        sobj_set_obj(res, qskey(&__key_data__), result);

        struct string *d        = sobj_to_json(res);
        cs_server_send_to_client(p, fd, mask, d->ptr, d->len, 0);
        string_free(d);
        sobj_free(res);

        return size * nmemb;
}

void es_server_process_delete_v1(struct cs_server *p, int fd, u32 mask, struct sobj *obj)
{
        debug("call delete\n");
        struct string *pass             = sobj_get_str(obj, qskey(&__key_pass__), RPL_TYPE);
        struct string *es_server_pass   = sobj_get_str(p->config, qskey(&__key_pass__), RPL_TYPE);

        if(strcmp(es_server_pass->ptr, pass->ptr) != 0) {
                __response_invalid_data(p, fd, mask, obj, qlkey("wrong password"));
                goto end;
        }

        struct string *path             = sobj_get_str(obj, qskey(&__key_path__), RPL_TYPE);
        struct sobj *data         = sobj_get_obj(obj, qskey(&__key_data__), RPL_TYPE);

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
                wdata.mask = mask;
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&wdata);

                temp = sobj_to_json(data);

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
                if(res != CURLE_OK) {
                        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                                curl_easy_strerror(res));
                        __response_invalid_data(p, fd, mask, obj, qlkey("database error"));
                }

                /* always cleanup */
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
        }
end:;
}
