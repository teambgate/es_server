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
#include "version_1/version.h"

#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <smartfox/data.h>

#include <common/command.h>
#include <common/key.h>
#include <common/error.h>

static void __response_invalid_version(struct es_server *p, int fd, struct sfs_object *obj)
{
        struct sfs_object *res = sfs_object_alloc();
        sfs_object_set_long(res, qskey(&__key_request_id__), sfs_object_get_long(obj, qskey(&__key_request_id__), 0));
        sfs_object_set_bool(res, qskey(&__key_result__), 0);
        sfs_object_set_string(res, qskey(&__key_message__), qlkey(""));
        sfs_object_set_long(res, qskey(&__key_error__), ERROR_VERSION_INVALID);

        struct string *d        = sfs_object_to_json(res);
        es_server_send_to_client(p, fd, d->ptr, d->len);
        string_free(d);
        sfs_object_free(res);
}

#define register_function(func)                                                                 \
void func(struct es_server *p, int fd, struct sfs_object *obj)                                 \
{                                                                                               \
        if(!map_has_key(obj->data, qskey(&__key_version__))) {                                  \
                __response_invalid_version(p, fd, obj);                                         \
                return;                                                                         \
        }                                                                                       \
                                                                                                \
        struct string *version = sfs_object_get_string(obj, qskey(&__key_version__), 0);        \
                                                                                                \
        if(strcmp(version->ptr, "1") == 0) {                                                    \
                func##_v1(p, fd, obj);                                                          \
                return;                                                                         \
        }                                                                                       \
                                                                                                \
        __response_invalid_version(p, fd, obj);                                                 \
}


register_function(es_server_process_get);
register_function(es_server_process_post);
register_function(es_server_process_put);
