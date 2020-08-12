#include <stdio.h>
#include <stdlib.h>
#include "kos32sys1.h"
#include <string.h>
#include <stdarg.h>
#include "algorithms/md5.h"
#include "algorithms/sha1.h"
#include "algorithms/sha256.h"



#define MAX_HASH_LEN 128

struct kolibri_system_colors sys_color_table;

char hash_str_md5[MAX_HASH_LEN];
char hash_str_sha1[MAX_HASH_LEN];
char hash_str_sha256[MAX_HASH_LEN];
char filename[FILENAME_MAX];
char title[FILENAME_MAX+10];
enum BUTTONS
{
    BTN_QUIT=1,
    BTN_MD5 = 16,
    BTN_SHA1 = 32,
    BTN_SHA256 = 64
};

void md5_hash(FILE* input, BYTE* hash )
{
    int input_size;
    BYTE *temp_buffer;
    temp_buffer=malloc(1024);
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
    buffer=malloc(1024);
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
    buffer=malloc(1024);
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
    hash = malloc(alg);
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

void sprint_hash(BYTE *hash, char* hash_str)
{
    char block[2];
    char *temp_str;
    temp_str=malloc(MAX_HASH_LEN);
    int hash_size = strlen((char*)hash);
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
    sprintf(title,"%s - checksum", filename);
    begin_draw();
    sys_create_window(10, 40, 500, 120, title, 0xFFFFFF, 0x13);
    define_button((10<<16)+80,(30<<16)+20, BTN_MD5, sys_color_table.work_button);
    define_button((10<<16)+80,(60<<16)+20, BTN_SHA1, sys_color_table.work_button);
    define_button((10<<16)+80,(90<<16)+20, BTN_SHA256, sys_color_table.work_button);

    draw_text_sys("MD5:", 15, 34, 0, 0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA1:", 15, 64, 0, 0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA256:", 15,94, 0, 0x90000000 | sys_color_table.work_button_text);

    draw_text_sys(hash_str_md5, 100, 34, 0, 0x80000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha1, 100, 64, 0, 0x80000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha256, 100, 94, 0, 0x80000000| sys_color_table.work_text);
    end_draw();
}

int main(int argc, char** argv)
{
    strcpy(filename, argv[1]);
    if(argc<2)
    {
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
                    sprint_hash(check_sum(filename, MD5_BLOCK_SIZE),hash_str_md5);
                    redraw_window();
                break;

                case BTN_SHA1:
                    sprint_hash(check_sum(filename, SHA1_BLOCK_SIZE),hash_str_sha1);
                    redraw_window();
                break;

                case BTN_SHA256:
                    sprint_hash(check_sum(filename, SHA256_BLOCK_SIZE),hash_str_sha256);
                    redraw_window();
                break;

                case BTN_QUIT:
                    exit(0);
                break;
            }
        }
    }while(1);
}


