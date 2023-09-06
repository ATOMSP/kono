#include <kernel/mm.h>
#include <kernel/stdint.h>
#include <lib/bitmap.h>



struct pool
{
  struct bitmap pool_map;
  size_t addr;
  size_t capacity;
};






void init_mem(void)
{

}







