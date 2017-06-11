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
#ifndef __ES_SERVER_ES_SERVER_H__
#define __ES_SERVER_ES_SERVER_H__

#include <es_server/types.h>

struct es_server *es_server_alloc();

void es_server_start(struct es_server *p);

void es_server_free(struct es_server *p);

void es_server_send_to_client(struct es_server *p, int fd, char *ptr, int len);

void es_server_process_get(struct es_server *p, int fd, struct sfs_object *obj);
void es_server_process_post(struct es_server *p, int fd, struct sfs_object *obj);
void es_server_process_put(struct es_server *p, int fd, struct sfs_object *obj);
void es_server_process_delete(struct es_server *p, int fd, struct sfs_object *obj);

#endif
