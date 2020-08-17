#include "kos32sys.h"
#include <stdlib.h>


enum BUTTONS2
{
    BTN_QUIT_ERR_BOX=1
};

void draw_error_box(char *msg)
{
  begin_draw();
  sys_create_window(50, 50, 200, 60, "Error!", 0xFFFFFFFF, 0x14);
  draw_text_sys(msg, 20, 34, 0,   0x90000000);
  end_draw();
}


void error_box(char *msg)
{
   draw_error_box(msg);
   int gui_event;
   int pressed_button = 0;
   set_event_mask(0xC0000027);

   draw_error_box(msg);
   do
   {
        gui_event = get_os_event();

        switch(gui_event)
        {
        case KOLIBRI_EVENT_NONE:
            break;
        case KOLIBRI_EVENT_REDRAW:
            draw_error_box(msg);
            break;
        case KOLIBRI_EVENT_KEY:
            break;
        case KOLIBRI_EVENT_BUTTON:
            pressed_button = get_os_button();

            switch (pressed_button)
            {
                case BTN_QUIT_ERR_BOX:
                exit(0);
                break;
            }
        break;
        }
    }while(1);
}