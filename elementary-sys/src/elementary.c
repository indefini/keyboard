#include <Elementary.h>

void init()
{
  elm_init(0,0);
}

void run()
{
  elm_run();
  elm_shutdown();
}

void
create_simple_window()
{

  Evas_Object* win = elm_win_util_standard_add("chris window", "chris window");
  elm_win_autodel_set(win, EINA_TRUE);
  //evas_object_smart_callback_add(win, "delete,request", simple_window_del, NULL>

  Evas_Object* box = elm_box_add(win);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);
  evas_object_show(box);

  evas_object_resize(win, 256, 256);
  //evas_object_resize(win, 64, 64);
  evas_object_show(win);
}

