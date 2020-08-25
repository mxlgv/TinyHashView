#include <stdio.h>
#include <stdlib.h>
#include "libmsgbox/kos32sys.h"
#include "libmsgbox/msgbox.h"
#include <string.h>
#include <stdarg.h>
#include "algorithms/md5.h"
#include "algorithms/sha1.h"
#include "algorithms/sha256.h"

#define TRUE 1;
#define FALSE 0;
#define MAX_HASH_LEN 65 // Максимальная длина строкию

typedef unsigned char bool;
struct kolibri_system_colors sys_color_table;

char hex[]={"abcdefABCDEF1234567890"}; //Для проверки вводимых символов
char hash_str_md5[MAX_HASH_LEN]=   "Click the 'MD5:' button to show the md5-checksum!      "; //Вывод MD5
char hash_str_sha1[MAX_HASH_LEN]=  "Click the 'SHA1:' button to show the sha1-checksum!    "; //Вывод SHA1
char hash_str_sha256[MAX_HASH_LEN]="Click the 'SHA256:' button to show the sha256-checksum!"; //Вывод SHA256
char edit_box_buff[MAX_HASH_LEN]; // Буффер для ввода
char filename[FILENAME_MAX]; // Имя обрабатываемого файла
char title[FILENAME_MAX+10]; // Заголовок окна

enum MYCOLORS // Цвета
{
    GREEN = 0x00067D06,
    RED =   0x00FF0000,
    BLACK = 0x00000000
};

unsigned int str_pos=0; // Позиция курсора при пичати в строке ввода
int md5_flag=0, sha1_flag=0, sha256_flag=0; // Флаги показывающие был ли уже рассчитана котрольная сумма в функции check_sum()
int edit_box_text_color=BLACK; // Изначальный цвет текста в строке ввода

enum MYKEYS // Коды клавиш
{
    BACKSPACE=8
};

enum BUTTONS // Кнопки в интрефейсе
{
    BTN_QUIT=1,        //Выход
    BTN_MD5 = 10,      //Рассчитать md5-контрольную сумму
    BTN_SHA1 = 20,     //Рассчитать sha1-контрольную сумму
    BTN_SHA256 = 30,   //Рассчитать sha256-контрольную сумму
    BTN_COPY_MD5= 11,  //Скопировать в буффер обмена
    BTN_COPY_SHA1= 21,
    BTN_COPY_SHA256=31,
    BTN_CMP=40,        //Сравнить edit_box и контрольную сумму
    BTN_PASTE=50       //Вставить в edit_box(пока в разработке)
};
void edit_box(oskey_t key)              //Прототип ввода строки
{
    edit_box_text_color=BLACK;
    if(key.code==BACKSPACE && str_pos>0) // Если backspace то удалить последний символ
    {
        str_pos--;
        edit_box_buff[str_pos]='\0';

    }
    else if(str_pos<MAX_HASH_LEN-1) // Ограничение длины ввода
    {
        if(strchr(hex,key.code)!=NULL)
        {
           edit_box_buff[str_pos]=key.code;
           str_pos++;
        }
    }
   // printf("%d",str_pos); Для отладки
}

void* safe_malloc(size_t size) // Безопасный malloc. Вызывает окно ошибки если память не была выделена
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

/* Функции генерации контрольных сумм */
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


BYTE* check_sum(int alg) // Генерируем контрольные суммы используя один из алгоритмов
{
    FILE* input_file;
    BYTE *hash;
    input_file=fopen(filename,"rb");
    hash = safe_malloc(alg);
    switch (alg)
    {
        case MD5_BLOCK_SIZE :
            md5_hash(input_file, hash);
            md5_flag=1;
        break;

        case SHA1_BLOCK_SIZE :
            sha1_hash(input_file, hash);
            sha1_flag=1;
        break;

        case SHA256_BLOCK_SIZE :
            sha256_hash(input_file, hash);
            sha256_flag=1;
        break;
    }
    fclose(input_file);
    return hash;
}

void sprint_hash(BYTE *hash, char* hash_str, int hash_size) //Преобрауем двоичные данные из hash в строку hash_str
{
    char block[2];
    memset(hash_str, 0, MAX_HASH_LEN); // Очищаем строку для strcat
    for(int i=0; i<hash_size; i++)
    {
        sprintf(block,"%02x", hash[i]);
        strcat(hash_str,block);
    }
    free(hash);
}

void redraw_window() //Рисуем окно
{
    pos_t win_pos = get_mouse_pos(0); //Получаем позицию курсора мыши.
    sprintf(title,"%s - thashview 1.1", filename); // Устанавливаем заголовок окна
    begin_draw(); //Начинаем рисование интерфейса )
    sys_create_window(win_pos.x, win_pos.y, 665, 160, title, 0xFFFFFFFF, 0x14); // Создаём окно.

    draw_bar(10, 121, 525,20, 0x802C8C8C8); // Создаём прямоугольник для поля ввода
    draw_text_sys(edit_box_buff,15, 125, 0, 0x90000000| edit_box_text_color); // Выводим текст из буффера ввода

    define_button((10 << 16) + 60, (30 << 16) + 20, BTN_MD5, GREEN); // Определяем кнопку md5
    define_button((10 << 16) + 60, (60 << 16) + 20, BTN_SHA1, GREEN);// Определяем кнопку sha1
    define_button((10 << 16) + 60, (90 << 16) + 20, BTN_SHA256, GREEN);// Определяем кнопку sha256

    draw_text_sys("MD5:", 15, 34, 0,   0x90000000 | sys_color_table.work_button_text); // Пищем текст на кнопках
    draw_text_sys("SHA1:", 15, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("SHA256:", 15,94, 0, 0x90000000 | sys_color_table.work_button_text);

    draw_text_sys(hash_str_md5, 80, 34, 0, 0x90000000 | sys_color_table.work_text); // Выводим контрольные суммы в окно
    draw_text_sys(hash_str_sha1, 80, 64, 0, 0x90000000 | sys_color_table.work_text);
    draw_text_sys(hash_str_sha256, 80, 94, 0, 0x90000000| sys_color_table.work_text);

    define_button((610 << 16) + 42, (30 << 16) + 20, BTN_COPY_MD5, sys_color_table.work_button); // Определяем кнопки для копирования
    define_button((610<< 16) + 42, (60 << 16) + 20, BTN_COPY_SHA1, sys_color_table.work_button);
    define_button((610<< 16) + 42, (90 << 16) + 20, BTN_COPY_SHA256, sys_color_table.work_button);

    draw_text_sys("copy", 615, 34, 0,   0x90000000 | sys_color_table.work_button_text); // Пишем copy на всех кнопках для копирования
    draw_text_sys("copy", 615, 64, 0,  0x90000000 | sys_color_table.work_button_text);
    draw_text_sys("copy", 615, 94, 0, 0x90000000 | sys_color_table.work_button_text);

    define_button((592<< 16) + 60, (120 << 16) + 20, BTN_CMP, sys_color_table.work_button); // Определяем кнопку для сравнения контольных сумм
    draw_text_sys("compare", 595, 124 , 0,0x90000000 | sys_color_table.work_button_text); // Пишем текс на кнопке.

    define_button((540 << 16) + 45, (120 << 16) + 20, BTN_PASTE, sys_color_table.work_button); //Кнопка для вставки (неработает)
    draw_text_sys("paste", 542, 124 , 0,0x90000000 | sys_color_table.work_button_text); // Текст paste на кнопке
    end_draw();
}

/*
void paste_to_edit_buffer()
{
    char* temp_buff=malloc(MAX_HASH_LEN+12);
    temp_buff=kol_clip_get(MAX_HASH_LEN+12);
    if((int)*(temp_buff+4)==0 && (int)*(temp_buff+8)==1)
    {
        strcpy(edit_box_buff,temp_buff+12);
        str_pos=strlen(edit_box_buff);
    }
}
*/

void copy_to_clipboard(char *text) // Копирлвать в буффер обмена
{
    if(55!=strlen(text))
    {
        char temp_buffer[MAX_HASH_LEN+12];
        *(temp_buffer+4)=0;
        *(temp_buffer+8)=1;
        strcpy(temp_buffer+12, text);
        kol_clip_set(strlen(text)+12, temp_buffer);
    }
}

void print_pending_calc(char *str) // Выводим сообщение о том что контрольная суммма вычисляется...
{
  strcpy(str, "Please wait! Calculating checksum...                   ");
  redraw_window();
}

bool calc_and_cmp(char *hash_str_universal,int alg) // Вычисляем контрольную сумму и сравниваем с edit_box_buff(ВЫЗЫВАЕТ ПАДЕНИЕ ПРОГРАММЫ)
{
   print_pending_calc(hash_str_universal);
   sprint_hash(check_sum(alg),hash_str_universal, alg);
   return !strcmp(edit_box_buff, hash_str_universal);
}

bool hash_compare() // Главная функция для сравнения
{
   int alg=strlen(edit_box_buff)/2;

        switch (alg) // Если вычисления ещё небыло
        {
        case MD5_BLOCK_SIZE:
            if(md5_flag)
            {
                return !strcmp(edit_box_buff,hash_str_md5);
            }
            else
            {
                return calc_and_cmp(hash_str_md5,alg);
            }
        break;

        case SHA1_BLOCK_SIZE:
            if(sha1_flag)
            {
                return !strcmp(edit_box_buff,hash_str_sha1);
            }
            else
            {
                return calc_and_cmp(hash_str_sha1,alg);
            }
        break;

        case SHA256_BLOCK_SIZE:

            if(sha256_flag)
            {
                return !strcmp(edit_box_buff,hash_str_sha256);
            }
            else
            {
                return calc_and_cmp(hash_str_sha256,alg);
            }
        break;

        default:
            return FALSE;
        break;
        }
}


int main(int argc, char** argv)
{
    strcpy(filename, argv[1]); // получаем имя файла
    if(argc<2) // Если аргументов нет то сообщаем об этом
    {
        error_box("No file selected!");
        exit(0);
    }
    if(NULL==fopen(filename,"rb")) // Если файла нет или не открывается
    {
        error_box("File not found!");
        exit(0);
    }

    int gui_event; // Перемная для хранения события
    uint32_t pressed_button = 0; // Код нажатой кнопки в окне

    get_system_colors(&sys_color_table);
    set_event_mask(0xC0000027); // Устанавливаем маску событий
    do // Цикл обработки событий
    {
        gui_event = get_os_event(); // Получаем событие
        switch(gui_event) // Обрабатываем события
        {
        case KOLIBRI_EVENT_NONE:
            break;
        case KOLIBRI_EVENT_REDRAW:
            redraw_window();
            break;
        case KOLIBRI_EVENT_KEY:
            edit_box(get_key()); // Получаем нажатую клавишу и задействуем строку ввода
            redraw_window();
            break;
        case KOLIBRI_EVENT_BUTTON: // Событие обработки кнопок
            pressed_button = get_os_button(); // Получение кода нажатой кнопки.
            switch (pressed_button) // Проверка какая кнопка была нажата
            {
                case BTN_MD5:
                    print_pending_calc(hash_str_md5);
                    sprint_hash(check_sum(MD5_BLOCK_SIZE),hash_str_md5, MD5_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA1:
                    print_pending_calc(hash_str_sha1);
                    sprint_hash(check_sum(SHA1_BLOCK_SIZE),hash_str_sha1, SHA1_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_SHA256:
                    print_pending_calc(hash_str_sha256);
                    sprint_hash(check_sum(SHA256_BLOCK_SIZE),hash_str_sha256, SHA256_BLOCK_SIZE);
                    redraw_window();
                break;

                case BTN_COPY_MD5:
                    redraw_window();
                    copy_to_clipboard(hash_str_md5);
                break;

                case BTN_COPY_SHA1:
                    redraw_window();
                    copy_to_clipboard(hash_str_sha1);
                break;

                case BTN_COPY_SHA256:
                    redraw_window();
                    copy_to_clipboard(hash_str_sha256);
                break;
                /*
                case BTN_PASTE:
                    paste_to_edit_buffer();
                    redraw_window();
                break;
                 */
                case BTN_CMP:
                if(hash_compare())
                {
                    edit_box_text_color=GREEN; // Устанавливаем текст ввода зелёным если контрольная сумма совпадает
                }
                else
                {
                    edit_box_text_color=RED; // Устанавливаем текст ввода красным если контрольная суммы не совпадает
                }
                redraw_window();
                break;

                case BTN_QUIT:
                    exit(0);
                break;
            }
        }
    }while(1);
}


