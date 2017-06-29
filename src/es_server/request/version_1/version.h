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
#ifndef __ES_SERVER_REQUEST_VERSION_1_VERSION_H__
#define __ES_SERVER_REQUEST_VERSION_1_VERSION_H__

#include <es_server/types.h>

void es_server_process_get_v1(struct cs_server *p, int fd, u32 mask, struct sobj *obj);
void es_server_process_post_v1(struct cs_server *p, int fd, u32 mask, struct sobj *obj);
void es_server_process_put_v1(struct cs_server *p, int fd, u32 mask, struct sobj *obj);
void es_server_process_delete_v1(struct cs_server *p, int fd, u32 mask, struct sobj *obj);

#endif
