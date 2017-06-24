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
#include <common/cs_server.h>

static void __response_invalid_version(struct cs_server *p, int fd, u32 mask, struct smart_object *obj)
{
        struct smart_object *res = smart_object_alloc();
        smart_object_set_long(res, qskey(&__key_request_id__), smart_object_get_long(obj, qskey(&__key_request_id__), 0));
        smart_object_set_bool(res, qskey(&__key_result__), 0);
        smart_object_set_string(res, qskey(&__key_message__), qlkey(""));
        smart_object_set_long(res, qskey(&__key_error__), ERROR_VERSION_INVALID);
        struct string *cmd = smart_object_get_string(obj, qskey(&__key_cmd__), SMART_GET_REPLACE_IF_WRONG_TYPE);
        smart_object_set_string(res, qskey(&__key_cmd__), qskey(cmd));

        struct string *d        = smart_object_to_json(res);
        cs_server_send_to_client(p, fd, mask, d->ptr, d->len, 0);
        string_free(d);
        smart_object_free(res);
}

#define register_function(func)                                                                 \
void func(struct cs_server *p, int fd, u32 mask, struct smart_object *obj)                      \
{                                                                                               \
        if(!map_has_key(obj->data, qskey(&__key_version__))) {                                  \
                __response_invalid_version(p, fd, mask, obj);                                         \
                return;                                                                         \
        }                                                                                       \
                                                                                                \
        struct string *version = smart_object_get_string(obj, qskey(&__key_version__), 0);        \
                                                                                                \
        if(strcmp(version->ptr, "1") == 0) {                                                    \
                func##_v1(p, fd, mask, obj);                                                          \
                return;                                                                         \
        }                                                                                       \
                                                                                                \
        __response_invalid_version(p, fd, mask, obj);                                                 \
}


register_function(es_server_process_get);
register_function(es_server_process_post);
register_function(es_server_process_put);
register_function(es_server_process_delete);
