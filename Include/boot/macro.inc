; load setup into the memory 
MBR_BASE_ADDR equ 0x7c00                                ; 主引导扇区加载地址
SETUP_BASE_ADDR equ 0x0900                              ; setup程序加载地址
SETUP_START_SECTOR equ 2                                ; setup程序扇区编号
SETUP_LOAD_SECTOR_NUM  equ 4                            ; setup占扇区数 

; GDT全局描述符表参数
DESC_G_4K equ 1_0_0_0_0000_0_00_0_0000_00000000b        ; G标志位选择粒度为：4K

DESC_D_32 equ 0_1_0_0_0000_0_00_0_0000_00000000b        ; D/B标志位选择使用32位操作数

DESC_L_32 equ 0_0_0_0_0000_0_00_0_0000_00000000b        ; L标志位表示处于32位代码段

DESC_AVL  equ 0_0_0_0_0000_0_00_0_0000_00000000b        ; 暂时未用

DESC_LIMIT_CODE2 equ 0_0_0_0_1111_0_00_0_0000_00000000b ; 代码段界限第二部分
DESC_LIMIT_DATA2 equ DESC_LIMIT_CODE2                   ; 数据段界限第二部分
DESC_LIMIT_VIDEO2 equ 0_0_0_0_0000_0_00_0_0000_00000000b; 显卡段界限第二部分

DESC_P equ 0_0_0_0_0000_1_00_0_0000_00000000b           ; P标志位：该段是否在内存中

DESC_DPL_0 equ 0_0_0_0_0000_0_00_0_0000_00000000b       ; 特权级0
DESC_DPL_1 equ 0_0_0_0_0000_0_01_0_0000_00000000b       ; 特权级1
DESC_DPL_2 equ 0_0_0_0_0000_0_10_0_0000_00000000b       ; 特权级2
DESC_DPL_3 equ 0_0_0_0_0000_0_11_0_0000_00000000b       ; 特权级3

DESC_S_CODE equ 0_0_0_0_0000_0_00_1_0000_00000000b      ;非系统段：代码段
DESC_S_DATA equ DESC_S_CODE                             ;非系统段：数据段
DESC_S_SYS  equ 0_0_0_0_0000_0_00_0_0000_00000000b      ;系统段

DESC_TYPE_CODE equ 0_0_0_0_0000_0_00_0_1000_00000000b   ; 可执行不可读非一致性代码(非依存)段类型
DESC_TYPE_DATA equ 0_0_0_0_0000_0_00_0_0010_00000000b   ; 不可执行可写向上拓展的数据段类型

DESC_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L_32 + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + \
                    DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00 ; 定义代码段描述符高32位

DESC_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L_32 + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + \
                    DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00 ; 定义数据段描述符高32位

DESC_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L_32 + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + \
                    DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x0b ; 定义显存段描述符高32位

RESERVE_GDT_NUM equ 60

; 选择子内RPL
RPL_0 equ 00b
RPL_1 equ 01b
RPL_2 equ 10b
RPL_3 equ 11b
; TI位选项
TI_GDT equ 000b ;段选择子请求的段描述符在GDT
TI_LDT equ 100b ;段选择子请求的段描述符在LDT

; 获取物理内存参数
ARDS_SIZE equ 20                    ; ards结构体大小
SIGNATURE_MARKUP equ 0x534d4150     ; 签名标识

; 页表属性相关参数
PAGES_DIR_TABLE_BASE_ADDR equ 0x100000 ; 页目录表的首物理地址

PG_P    equ 1b
PG_RW_R equ 00b
PG_RW_W equ 10b
PG_US_S equ 000b
PG_US_U equ 100b


; 内核相关参数
KERNEL_START_SECTOR equ 9     ;内核所在扇区
KERNEL_BASE_ADDR equ 0x70000  ;加载内核bin到此物理地址
KERNEL_SECTOR_NUM equ 200     ;内核读入扇区数
KERNEL_ENTRY_ADDR  equ 0xc0001500 ;内核执行入口地址
