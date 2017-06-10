#include <cherry/stdio.h>
#include <es_server/es_server.h>
#include <cherry/memory.h>

int main(void)
{
begin:;
        struct es_server *s = es_server_alloc();
        es_server_start(s);
        es_server_free(s);

        cache_free();
        dim_memory();
        goto begin;
        return 0;
}
