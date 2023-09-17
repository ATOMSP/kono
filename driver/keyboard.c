#include <driver/keyboard.h>
#include <lib/print.h>
#include <kernel/interrupt.h>
#include <kernel/stdint.h>
#include <lib/io.h>

/* 键盘buffer register port */
#define KED_BUF_PORT  0x60

 /* 部分控制字符 */
#define esc       '\033' 
#define backspace '\b' 
#define tab       '\t' 
#define enter     '\r'
#define delete    '\177'

/* 不可见字符一律ascii为0 */
#define char_invisible    0 
#define ctrl_l_char       char_invisible 
#define ctrl_r_char       char_invisible 
#define shift_l_char      char_invisible 
#define shift_r_char      char_invisible 
#define alt_l_char        char_invisible
#define alt_r_char        char_invisible 
#define caps_lock_char    char_invisible 

/* 定义部分控制字符的通断码 */
#define ctrl_l_make       0x1d
#define ctrl_r_make       0xe01d 
#define shift_l_make      0x2a 
#define shift_r_make      0x36 
#define alt_l_make        0x38
#define alt_r_make        0xe038 
#define alt_r_break       0xe0b8 
#define ctrl_r_break      0xe09d 
#define caps_lock_make    0x3a 

//定义按键按下状态标志
static uint8_t ctrl_status;
static uint8_t shift_status;
static uint8_t alt_status;
static uint8_t caps_lock_status;
//记录makecode是否以0xe0开头
static uint8_t ext_scancode;

/* 以8042通码为索引的ascii二维数组 */
static char keymap[][2] = {
  {0,0},{esc,esc},{'1','!'},{'2','@'},{'3','#'},{'4','$'},{'5','%'},{'6','^'},{'7','&'},{'8','*'},
  {'9','('},{'0',')'},{'-','_'},{'=','+'},{backspace,backspace},{tab,tab},{'q','Q'},{'w','W'},{'e','E'},{'r','R'},
  {'t','T'},{'y','Y'},{'u','U'},{'i','I'},{'o','O'},{'p','P'},{'[','{'},{']','}'},{enter,enter},{ctrl_l_char,ctrl_l_char},
  {'a','A'},{'s','S'},{'d','D'},{'f','F'},{'g','G'},{'h','H'},{'j','J'},{'k','K'},{'l','L'},{';',':'},
  {'\'','"'},{'`','~'},{shift_l_char,shift_l_char},{'\\','|'},{'z','Z'},{'x','X'},{'c','C'},{'v','V'},{'b','B'},{'n','N'},
  {'m','M'},{',','<'},{'.','>'},{'/','?'},{shift_r_char,shift_r_char},{'*','*'},{alt_l_char,alt_l_char},{' ',' '},{caps_lock_char,caps_lock_char}
};




/* 键盘中断处理程序 */
static void intr_keyboard_handler(void)
{
  uint8_t is_break;
  //得到上一次的状态
  uint8_t last_shift_status = shift_status;
  uint8_t last_ctrl_status = ctrl_status;
  uint8_t last_caps_lock_status = caps_lock_status;
  uint16_t scancode = inb(KED_BUF_PORT);
  //如果是拓展码
  if(scancode == 0xe0){
    ext_scancode = 1;
    return;
  } 
  //如果上一次是拓展码
  if(ext_scancode){
    ext_scancode = 0;
    scancode = (0xe000 | scancode);
  }
  is_break = (scancode & 0x80);
  //如果为断码
  if(is_break){
    uint16_t make_code = (scancode &= 0xff7f);
    if(make_code == shift_l_make || make_code == shift_r_make){
      shift_status = 0;
    }else if(make_code == ctrl_l_make || make_code == ctrl_r_make){
      ctrl_status = 0;
    }else if(make_code == alt_l_make || make_code == alt_r_make){
      alt_status = 0;
    }
    return;
    //不是断码，则是通码
  }else if((scancode > 0x00 && scancode < 0x3b) || (scancode == alt_r_make) ||(scancode == ctrl_r_make)){
    uint8_t shift = 0;
    /* 判断键盘中有两种字符的按键*/
    if((scancode < 0x0e) || (scancode == 0x29)||
       (scancode == 0x1a) || (scancode == 0x1b)||
       (scancode == 0x2b) || (scancode == 0x27)||
       (scancode == 0x28) || (scancode == 0x33)||
       (scancode == 0x34) || (scancode == 0x35)
    ){
      //如果上一次按下了shift
      if(last_shift_status){
        shift = 1;
      }
      //如果是普通单字符
    }else{
      //如果shift和caps都按下
      if(last_shift_status && last_caps_lock_status){
        shift = 0;
      }
      //如果随便按下其中一个
      else if(last_shift_status || last_caps_lock_status){
        shift = 1;
      }else{
        shift = 0;
      }
    }
    //取低字节
    uint16_t index = (scancode & 0x00ff);
    char c = keymap[index][shift];
    //只显示可显示字符，为0的就是不可显示字符
    if(c){
      s_putchar(c);
      return;
    }
    if(scancode == ctrl_l_make || scancode == ctrl_r_make){
      ctrl_status = 1;
    }else if(scancode == shift_l_make || scancode == shift_r_make){
      shift_status = 1;
    }else if(scancode == alt_l_make || scancode == alt_r_make){
      alt_status = 1;
    /*对于caps键特殊操作*/
    }else if(scancode == caps_lock_make){
      caps_lock_status = !caps_lock_status;
    }
    //暂不支持的按键
  }else{
    s_putstr("Unknown keyboard code\n");
  }
}


/**
 * 键盘初始化
*/
void keyboard_init(void)
{
  s_putstr("init keyboard...\n");
  intr_handle_register(0x21,intr_keyboard_handler);
  s_putstr("init keyboard done\n");
}










