#include <lib/bitmap.h>
#include <lib/string.h>
#include <kernel/debug.h>


/**
 * 位图初始化：0有空闲 1占用
*/
void Bitmap_init(struct bitmap* map)
{
  memset(map->bits,0,map->map_lenb);
}

/**
 * 检测位图某一位是否为1 ，是返回1，否返回0
*/
int Bitmap_check_bit(struct bitmap* map,uint32_t bit)
{
  ASSERT(map != NULL);
  ASSERT(bit < (map->map_lenb) * 8);
  uint32_t r = bit / 8;
  uint32_t c = bit % 8;
  return (int)(((map->bits[r]) >> c) & BITMAP_MASK);
}

/**
 * 在位图中申请cnt个连续的位
*/
int  Bitmap_scan(struct bitmap* map,uint32_t cnt)
{
  uint32_t bits_byte = 0;
  ASSERT(map != NULL);
  ASSERT(cnt <= (map->map_lenb) * 8);
  while(map->bits[bits_byte] == 0xff && bits_byte < map->map_lenb){
    bits_byte++;
  }
  if(bits_byte == map->map_lenb) return -1;
  uint8_t _bit = 0; 
  while (((uint8_t)BITMAP_MASK << _bit) & map->bits[bits_byte]){
    _bit++;
  }
  int32_t bit_index_start = bits_byte * 8 + _bit;
  if(cnt == 1) return bit_index_start;
  int32_t free_bits = map->map_lenb * 8 - bit_index_start;  //剩余空闲位
  if(free_bits < cnt){                                  //空间不够分配，返回-1
    return -1;
  }
  uint32_t next_bits = bit_index_start + 1;
  bit_index_start = -1;
  uint32_t count = 1;
  while(free_bits-- > 0){
    if(Bitmap_check_bit(map,next_bits) == 0){
      count++;
    }else{
      count = 0;
    }
    if(count == cnt){
      bit_index_start = next_bits - cnt + 1;
      break;
    }
    next_bits++;
  }
  return bit_index_start;
}

/**
 * 设置位图某一位：0 or 1
*/
void Bitmap_set_bit(struct bitmap* map,uint32_t bit,int8_t var)
{
  ASSERT(map != NULL);
  ASSERT(bit < (map->map_lenb) * 8);
  ASSERT(var == 0 || var == 1);
  uint32_t r = bit / 8;
  uint32_t c = bit % 8;
  if(var){
    map->bits[r] |= BITMAP_MASK << c; 
  }else{
    map->bits[r] &= ~(BITMAP_MASK << c); 
  }
}












