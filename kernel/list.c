#include <kernel/list.h>
#include <kernel/interrupt.h>
#include <kernel/debug.h>
#include <lib/string.h>

/**
 * 初始化双向链表
*/
void list_init (struct list* ptr)
{
  ASSERT(ptr != NULL);
  ptr->head.prev = NULL;
  ptr->head.next = &(ptr->tail);
  ptr->tail.next = NULL;
  ptr->tail.prev = &(ptr->head);
} 
/**
 * 插入元素elem到before前
*/
void list_insert_before (struct list_elem* before, struct list_elem* elem)
{
  //关闭中断
  enum int_state old_state = Int_Disable();
  //插入elem
  before->prev->next = elem;
  elem->prev = before->prev;
  elem->next = before;
  before->prev = elem;
  //还原中断
  Int_Set_State(old_state);
} 
/**
 * 增加元素至链表首
*/
void list_push(struct list * plist, struct list_elem* elem)
{
  ASSERT(elem != NULL);
  list_insert_before(plist->head.next,elem);
}

void list_iterate(struct list* plist)
{
}

/**
 * 增加元素至链表尾
*/
void list_append(struct list* plist, struct list_elem* elem)
{
  ASSERT(elem != NULL);
  list_insert_before(&plist->tail,elem);
}
/**
 * 解绑元素
*/
void list_remove(struct list_elem* pelem)
{
  enum int_state old_state = Int_Disable();
  ASSERT(pelem != NULL);
  pelem->prev->next = pelem->next;
  pelem->next->prev = pelem->prev;
  Int_Set_State(old_state);
}
/**
 * 获取链表头节点元素
*/
struct list_elem* list_pop(struct list* plist)
{
  struct list_elem * temp = plist->head.next;
  list_remove(plist->head.next);
  return temp;
}

/**
 * 判断链表是否为空
*/
int list_empty(struct list* plist)
{ 
  return (plist->head.next == &plist->tail) ? 1 : 0;
}
/**
 * 返回链表长度 
*/
uint32_t list_len(struct list* plist)
{
  struct list_elem * temp = plist->head.next;
  uint32_t len = 0;
  while(temp != &plist->tail){
    temp = temp->next;
    len++;
  }
  return len;
}
/**
 * 遍历链表，把元素和arg传给回调函数,返回符合回调函数内部条件的节点
*/
struct list_elem* list_traversal(struct list* plist, list_callback func, int arg)
{
  struct list_elem * temp = plist->head.next;
  if(list_empty(plist) == 0){
    return NULL;
  }
  while (temp != &plist->tail){
    //如果返回1，则表示该元素在回调函数中符合条件，命中，停止遍历
    if(func(temp,arg)){
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}

/**
 * 查找链表中指定元素是否存在
*/
int elem_find(struct list * plist, struct list_elem* obj_elem)
{
  struct list_elem * temp = plist->head.next;
  while (temp != &plist->tail){
    if(temp == obj_elem){
      return 1;
    }
    temp = temp->next;
  }
  return 0;
}









