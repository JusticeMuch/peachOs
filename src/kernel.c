#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour){
    return (colour << 8)| c;
}

void terminal_putchar(int x, int y, char c, char color){
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color){
    if (c == '\n'){
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH){
        terminal_col = 0;
        terminal_row += 1;
    }
}

void terminal_initialize(){
    video_mem = (uint16_t*)(0xB8000);
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
        
    }
    
}

size_t str_len (const char* str){
    size_t len = 0;
    while(str[len]){
        len++;
    }

    return len;
}

void print(const char* str){
    size_t len = str_len(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);//writing is default colour is white
    }
    
}

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main()
{
    terminal_initialize();
    print("Hello World\nTest");

    //initialise the heap
    kheap_init();

    //Search and initialize disk
    disk_search_and_init();

    //initialize idit table
    idt_init();

    //paging new 4gb
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE|PAGING_ACCESS_FROM_ALL|PAGING_IS_PRESENT);

    //switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    char* ptr = kzalloc(4096);
    paging_set(paging_4gb_chunk_get_directory(kernel_chunk),(void*)0x1000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL| PAGING_IS_PRESENT|PAGING_IS_WRITABLE);

    //enable pagung
    enable_paging();

    //enable interrupts
    enable_interrupts();
}