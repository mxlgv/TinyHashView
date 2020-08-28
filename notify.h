#include "kos32sys.h"
void notify_show(const char *text)
{
   start_app("/sys/@notify", text);
}