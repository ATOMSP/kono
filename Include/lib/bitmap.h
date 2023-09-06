#ifndef __LIB_BITMAP_H
#define __LIB_BITMAP_H

#ifdef __cplusplus
extern"C"{
#endif


#include <kernel/stdint.h>

#define BITMAP_MASK 1

struct bitmap
{
  uint32_t map_lenb; //位图字节长度
  uint8_t* bits;
};

/**
 * 位图初始化：0有空闲 1占用
*/
void Bitmap_init(struct bitmap* map);
/**
 * 检测位图某一位是否为1 ，是返回1，否返回0
*/
int Bitmap_check_bit(struct bitmap* map,uint32_t bit);
/**
 * 在位图中申请cnt个连续的位
*/
int  Bitmap_scan(struct bitmap* map,uint32_t cnt);
/**
 * 设置位图某一位：0 or 1
*/
void Bitmap_set_bit(struct bitmap* map,uint32_t bit,int8_t var);











#ifdef __cplusplus
}
#endif










#endif


