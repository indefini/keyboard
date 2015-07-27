#include <Elementary.h>
#include "keyboard.h"

void init()
{
  elm_init(0,0);
}

void run()
{
  elm_run();
  elm_shutdown();
}

Evas_Object* table_new(Evas_Object* win)
{
  Evas_Object* tb = elm_table_add(win);
  evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, tb);
  evas_object_show(tb);

  return tb;
}

static void
_window_del(void *data, Evas_Object *obj, void *event_info)
{
  elm_exit();
}

static void
_on_pressed(void *data, Evas_Object *obj, void *event_info)
{
  const char* key = elm_object_text_get(obj);
  //printf("pressed: %s \n", key);
  ecore_x_test_fake_key_press(key);
}


Evas_Object* window_new()
{
  Evas_Object* win = elm_win_util_standard_add("keyboard", "keyboard");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", _window_del, NULL);

  Evas_Object* box = elm_box_add(win);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);
  evas_object_show(box);

  evas_object_resize(win, 256, 56);
  evas_object_show(win);

  //elm_win_keyboard_win_set(win, EINA_TRUE);
  elm_win_prop_focus_skip_set(win, EINA_TRUE);

  Ecore_X_Window *xwin = elm_win_xwindow_get(win);
  //ecore_x_e_virtual_keyboard_set(

  return win;
}

Keyboard* keyboard_new()
{
  Evas_Object* win = window_new();
  Evas_Object* table = table_new(win);

  Keyboard* k = calloc(1, sizeof *k);
  k->win = win;
  k->table = table;

  return k;
}

void keyboard_add(Keyboard* keyboard, const char* keyname, int col, int row)
{
  Evas_Object* bt = elm_button_add(keyboard->win);
  elm_object_text_set(bt, keyname);
  evas_object_smart_callback_add(bt, "pressed", _on_pressed, NULL);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_table_pack(keyboard->table, bt, col, row, 1, 1);
  evas_object_show(bt);
}
