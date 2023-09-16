#ifndef __KERNEL_LIST_H   
#define __KERNEL_LIST_H   

#ifdef __cplusplus
extern "C"{
#endif


#include <kernel/stdint.h>

//得到结构体成员内部偏移量
#define offset (struct_type, member) (int) ( & ( (struct_type*) 0) ->member) 
//根据结构体成员获取结构体头指针
#define elem2entry(struct_type, struct_member_name, elem_ptr) \ 
 (struct type*) ((int) elem_ptr - offset (struct_type, struct_member_name)) 

//双向链表前序指针和后序指针
struct list_elem { 
  struct list_elem* prev; 
  struct list_elem* next; 
} ; 
//链表数据结构：包含头节点和尾节点
struct list { 
  struct list_elem head; 
  struct list_elem tail; 
}; 

typedef int (*list_callback)(struct list_elem * e, int arg); 

void list_init (struct list*); 
void list_insert_before (struct list_elem* before, struct list_elem* elem); 
void list_push(struct list* plist, struct list_elem* elem); 
void list_iterate(struct list* plist); 
void list_append(struct list* plist, struct list_elem* elem); 
void list_remove(struct list_elem* pelem); 
struct list_elem* list_pop(struct list* plist); 
int list_empty(struct list* plist); 
uint32_t list_len(struct list* plist); 
struct list_elem* list_traversal(struct list* plist, list_callback func, int arg); 
int elem_find(struct list * plist, struct list_elem* obj_elem);


#ifdef __cplusplus
}
#endif



#endif