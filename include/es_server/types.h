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
#include <pthread.h>

struct es_server {
        struct list_head server;
};

// struct es_server;
//
// typedef void(*es_server_delegate)(struct es_server *, int fd, u32 mask, struct sobj *);
//
// struct client_buffer {
//         struct string   *buff;
//         u32             requested_len;
// };
//
// struct client_step {
//         int fd;
//         u64 step;
// };
//
// struct es_server {
//         struct file_descriptor_set      *master;
//         struct file_descriptor_set      *incomming;
//         int                             fdmax;
//         int                             listener;
//
//         struct array                    *fd_mask;
//
//         struct array                    *fd_invalids;
//
//         pthread_mutex_t                 client_data_mutex;
//
//         u64                             step;
//
//         struct sobj             *config;
//         struct map                      *delegates;
//         struct map                      *clients_datas;
// };

#endif
