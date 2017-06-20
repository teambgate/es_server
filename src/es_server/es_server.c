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
#include <common/cs_server.h>
#include <common/command.h>
#include <common/key.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <smartfox/data.h>
#include <cherry/map.h>
#include <cherry/array.h>

struct es_server *es_server_alloc()
{
        struct es_server *p     = smalloc(sizeof(struct es_server));
        INIT_LIST_HEAD(&p->server);

        struct cs_server *c     = cs_server_alloc(0);
        list_add_tail(&c->user_head, &p->server);

        c->config               = smart_object_from_json_file("res/config.json", FILE_INNER);

        map_set(c->delegates, qskey(&__cmd_get__), &(cs_server_delegate){es_server_process_get});
        map_set(c->delegates, qskey(&__cmd_post__), &(cs_server_delegate){es_server_process_post});
        map_set(c->delegates, qskey(&__cmd_put__), &(cs_server_delegate){es_server_process_put});
        map_set(c->delegates, qskey(&__cmd_delete__), &(cs_server_delegate){es_server_process_delete});

        return p;
}

void es_server_start(struct es_server *p)
{
        if(!list_singular(&p->server)) {
                struct cs_server *c     = (struct cs_server *)
                        ((char *)p->server.next - offsetof(struct cs_server, user_head));
                u16 port                = smart_object_get_short(c->config, qlkey("service_port"), SMART_GET_REPLACE_IF_WRONG_TYPE);

                cs_server_start(c, port);
        }
}

void es_server_free(struct es_server *p)
{
        if(!list_singular(&p->server)) {
                struct cs_server *c     = (struct cs_server *)
                        ((char *)p->server.next - offsetof(struct cs_server, user_head));
                cs_server_free(c);
        }

        sfree(p);
}
