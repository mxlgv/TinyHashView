#include <stdio.h>
#include <stdlib.h>
#include "apis/kos32sys.h"
#include "apis/msgbox.h"
#include <string.h>
#include <stdarg.h>
#include "algorithms/md5.h"
#include "algorithms/sha1.h"
#include "algorithms/sha256.h"

#define GREEN 0x00067D06
#define MAX_HASH_LEN 128

struct kolibri_system_colors sys_color_table;

char hash_str_md5[MAX_HASH_LEN]=   "Click the 'MD5:' button to show the md5-checksum!      ";
char hash_str_sha1[MAX_HASH_LEN]=  "Click the 'SHA1:' button to show the sha1-checksum!    ";
char hash_str_sha256[MAX_HASH_LEN]="Click the 'SHA256:' button to show the sha256-checksum!";
char filename[FILENAME_MAX];
char title[FILENAME_MAX+10];
enum BUTTONS
{
    BTN_QUIT=1,
    BTN_MD5 = 10,
    BTN_SHA1 = 20,
    BTN_SHA256 = 30,
    BTN_COPY_MD5= 11,
    BTN_COPY_SHA1= 21,
    BTN_COPY_SHA256=31
};

void* safe_malloc(size_t size)
{
    void *p=malloc(size);
    if(p==NULL)
    {
        error_box("Memory allocation error!");
        exit(0);
    }
    else
    {
        return p;
    }
}


void md5_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *temp_buffer;
    temp_buffer=safe_malloc(1024);
    MD5_CTX ctx;
    md5_init(&ctx);
    while((input_size = fread(temp_buffer, 1, 1024, input)) > 0){
                md5_update(&ctx, temp_buffer, input_size);
    }
    md5_final(&ctx, hash);
    free(temp_buffer);
}

void sha1_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *buffer;
    buffer=safe_malloc(1024);
    SHA1_CTX ctx;
    sha1_init(&ctx);
    while((input_size = fread(buffer, 1, 1024, input)) > 0){
                sha1_update(&ctx, buffer, input_size);
    }
    sha1_final(&ctx, hash);
    free(buffer);
}

void sha256_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *buffer;
    buffer=safe_malloc(1024);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    while((input_size = fread(buffer, 1, 1024, input)) > 0){
                sha256_update(&ctx, buffer, input_size);
    }
    sha256_final(&ctx, hash);
    free(buffer);
}


BYTE* check_sum(char *in_file_name, int alg)
{
    FILE* input_file;
    BYTE *hash;
    input_file=fopen(in_file_name,"rb");
    hash = safe_malloc(alg);
    switch (alg)
    {
        case MD5_BLOCK_SIZE :
            md5_hash(input_file, hash);
        break;

        case SHA1_BLOCK_SIZE :
            sha1_hash(input_file, hash);
        break;

        case SHA256_BLOCK_SIZE :
            sha256_hash(input_file, hash);
        break;
    }
    return hash;
}

void sprint_hash(BYTE *hash, char* hash_str, int hash_size)
{
    char block[2];
    char *temp_str;
    temp_str=safe_malloc(MAX_HASH_LEN);
    for(int i=0; i<hash_size; i++)
    {
        sprintf(block,"%02x", hash[i]);
        strcat(temp_str,block);
    }
    strcpy(hash_str, temp_str);
    free(temp_str);
    free(hash);
}


void redraw_window()
{
    pos_t win_pos = get_mouse_pos(0);
    sprintf(title,"%s - thashview 1.1", filename);
    begin_draw();
    sys_create_window(win_pos.x, win_pos.y, 665, 120, title, 0xFFFFFFFF, 0x14);

    define_button((10 << 16) + 60, (30 << 16) + 20, BTN_MD5, GREEN);
    define_button((10 << 16) + 60, (60 << 16) + 20, BTN_SHA1, GREEN);
    define_button((10 << 16) + 60, (90 << 16) + 20, BTN_SHA256, GREEN);

    draw_text_sys("MD5:", 15, 34, 0,   0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA1:", 15, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA256:", 15,94, 0, 0x90000000 | sys_color_table.work_button_text);

    draw_text_sys(hash_str_md5, 80, 34, 0, 0x90000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha1, 80, 64, 0, 0x90000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha256, 80, 94, 0, 0x90000000| sys_color_table.work_text);

    define_button((610 << 16) + 42, (30 << 16) + 20, BTN_COPY_MD5, sys_color_table.work_button);
    define_button((610<< 16) + 42, (60 << 16) + 20, BTN_COPY_SHA1, sys_color_table.work_button);
    define_button((610<< 16) + 42, (90 << 16) + 20, BTN_COPY_SHA256, sys_color_table.work_button);
    draw_text_sys("copy", 615, 34, 0,   0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("copy", 615, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("copy", 615, 94, 0, 0x90000000 | sys_color_table.work_button_text);
    end_draw();
}

void copy_to_buffer(char *text)
{
    if(55!=strlen(text))
    {
    char temp_buffer[MAX_HASH_LEN];
    *(temp_buffer+4)=0;
    *(temp_buffer+8)=1;
    strcpy(temp_buffer+12, text);
    kol_clip_set(strlen(text)+12, temp_buffer);
    }
}

void print_pending_calc(char *line)
{
  strcpy(line, "Please wait! Calculating checksum...                   ");
  redraw_window();
}

int main(int argc, char** argv)
{
    strcpy(filename, argv[1]);
    if(argc<2)
    {
        error_box("No file selected!");
        exit(0);
    }
    if(NULL==fopen(argv[1],"rb"))
    {
        error_box("File not found!");
        exit(0);
    }

    int gui_event;
    uint32_t pressed_button = 0;
    get_system_colors(&sys_color_table);
    set_event_mask(0xC0000027);
    do
    {
        gui_event = get_os_event();
        switch(gui_event)
        {
        case KOLIBRI_EVENT_NONE:
            break;
        case KOLIBRI_EVENT_REDRAW:
            redraw_window();
            break;
        case KOLIBRI_EVENT_KEY:
            break;
        case KOLIBRI_EVENT_BUTTON:
            pressed_button = get_os_button();
            switch (pressed_button)
            {
                case BTN_MD5:
                    print_pending_calc(hash_str_md5);
                    sprint_hash(check_sum(filename, MD5_BLOCK_SIZE),hash_str_md5, MD5_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA1:
                    print_pending_calc(hash_str_sha1);
                    sprint_hash(check_sum(filename, SHA1_BLOCK_SIZE),hash_str_sha1, SHA1_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA256:
                    print_pending_calc(hash_str_sha256);
                    sprint_hash(check_sum(filename, SHA256_BLOCK_SIZE),hash_str_sha256, SHA256_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_COPY_MD5:
                     redraw_window();
                     copy_to_buffer(hash_str_md5);
                break;

                case BTN_COPY_SHA1:
                     redraw_window();
                     copy_to_buffer(hash_str_sha1);
                break;

                case BTN_COPY_SHA256:
                     redraw_window();
                     copy_to_buffer(hash_str_sha256);
                break;

                case BTN_QUIT:
                    exit(0);
                break;
            }
        }
    }while(1);
}


