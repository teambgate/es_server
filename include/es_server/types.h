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
#ifndef __ES_SERVER_TYPES_H__
#define __ES_SERVER_TYPES_H__

#include <cherry/server/types.h>
#include <smartfox/types.h>

struct es_server;

typedef void(*es_server_delegate)(struct es_server *, int fd, struct smart_object *);

struct client_buffer {
        struct string   *buff;
        u32             requested_len;
};

struct es_server {
        struct file_descriptor_set      *master;
        struct file_descriptor_set      *incomming;
        int                             fdmax;
        int                             listener;

        struct smart_object               *config;
        struct map                      *delegates;
        struct map                      *clients_datas;
};

#endif
