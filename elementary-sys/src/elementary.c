#include <Elementary.h>
#include "keyboard.h"

Evas_Object* _win;

void init()
{
  elm_init(0,0);
}

void run()
{
  elm_run();
  elm_shutdown();
}

void kexit()
{
  elm_exit();
}

void reduce()
{
  elm_win_iconified_set(_win, EINA_TRUE);
}

static void
_rect_down(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
  evas_object_color_set(o, 0, 0, 255, 255);
   Evas_Event_Mouse_Down *ev = event_info;

   if (ev->button != 1) return;
   printf("MOUSE: down @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
   //evas_object_move(indicator[0], ev->canvas.x, ev->canvas.y);
   //evas_object_resize(indicator[0], 1, 1);
   //evas_object_show(indicator[0]);

   Eina_List* obs = evas_objects_at_xy_get(
         e, 
         //evas_object_evas_get(o), 
         ev->canvas.x, ev->canvas.y, EINA_TRUE, EINA_FALSE);

   printf("mouse eina list count : %d \n", eina_list_count(obs));

   Eina_List* l;
   Evas_Object* next;

   EINA_LIST_FOREACH(obs, l, next) {

     if (next != NULL) {
       printf("mouse next is %s, %d, %s \n",
             evas_object_type_get(next),
             evas_object_layer_get(next),
             evas_object_name_get(next)
             );
       evas_object_color_set(next, 0, 100, 0, 255);
     }
   }

}

static void
_multi_down(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
  printf("multi is %s, %d, %s \n",
        evas_object_type_get(o),
        evas_object_layer_get(o),
        evas_object_name_get(o)
        );


  evas_object_color_set(o,  255, 255, 0, 255);
   Evas_Event_Multi_Down *ev = event_info;
   printf("MULTI: down @ %4i %4i | dev: %i\n", ev->canvas.x, ev->canvas.y, ev->device);
   //if (ev->device >= IND_NUM) return;
   //evas_object_move(indicator[ev->device], ev->canvas.x, ev->canvas.y);
   //evas_object_resize(indicator[ev->device], 1, 1);
   //evas_object_show(indicator[ev->device]);

   Eina_List* obs = evas_objects_at_xy_get(
         //e, 
         evas_object_evas_get(o), 
         ev->canvas.x, ev->canvas.y, EINA_TRUE, EINA_TRUE);

   printf("eina list count : %d \n", eina_list_count(obs));

   Eina_List* l;
   Evas_Object* next;

   EINA_LIST_FOREACH(obs, l, next) {

     if (next != NULL) {
       printf("next is %s, %d, %s \n",
             evas_object_type_get(next),
             evas_object_layer_get(next),
             evas_object_name_get(next)
             );
       Evas_Object* below = evas_object_below_get(next);
       Evas_Object* above = evas_object_above_get(next);

       if (below) printf("below : %s \n", evas_object_name_get(below));
       if (above) {
         printf("above : %s \n", evas_object_name_get(above));
         above = evas_object_above_get(above);
         if (above) {
           printf("above, above : %s \n", evas_object_name_get(above));
         }
       }
       evas_object_color_set(next, 0, 255, 0, 255);
     }
   }
}




Evas_Object* table_new(Evas_Object* win)
{
  Evas_Object* tb = elm_table_add(win);
  evas_object_name_set(tb, "table");
  evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, tb);
  elm_table_homogeneous_set(tb, EINA_TRUE);
  elm_table_padding_set(tb, 10, 10);
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

static void
_on_pressed_fn(void *data, Evas_Object *obj, void *event_info)
{
  rust_cb *cb = evas_object_data_get(obj, "cb");
  const void* cb_data = evas_object_data_get(obj, "cb_data");
  if (cb != NULL) {
    printf("ok\n");
    cb(cb_data);
  }
  else {
    printf("not ok : %p, %p \n", cb, cb_data);
  }
}

Evas_Object* window_new()
{
  Evas_Object* win = elm_win_util_standard_add("keyboard", "keyboard");
  evas_object_name_set(win, strdup("win"));
  _win = win;
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", _window_del, NULL);

  Evas_Object* bg = elm_bg_add(win);
  evas_object_name_set(bg, strdup("bg"));
  evas_object_show(bg);
  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, bg);

  //evas_object_event_callback_add(bg, EVAS_CALLBACK_MOUSE_DOWN, _rect_down, NULL);
  //evas_object_event_callback_add(bg, EVAS_CALLBACK_MULTI_DOWN, _multi_down, NULL);

  //elm_win_keyboard_win_set(win, EINA_TRUE);
  //elm_win_prop_focus_skip_set(win, EINA_TRUE);
  //elm_win_override_set(win, EINA_TRUE);
  elm_win_screen_constrain_set(win, EINA_TRUE);
  elm_win_sticky_set(win, EINA_TRUE);
  //elm_win_borderless_set(win, EINA_TRUE);

  int dpx, dpy;
  elm_win_screen_dpi_get(win, &dpx, &dpy);
  printf("screen dpx, dpy : %d, %d \n", dpx, dpy);

  int x, y, w, h;
  elm_win_screen_size_get(win, &x, &y, &w, &h);
  printf("screen x, y, w, h : %d, %d, %d, %d \n", x, y, w, h);

  int winh = h/3;
  evas_object_resize(win, w, winh);
  elm_win_size_base_set(win, w, winh);
  evas_object_move(win, 0, h - winh);
  //test size
  //evas_object_resize(win, 206, 56);

  //Ecore_X_Window *xwin = elm_win_xwindow_get(win);
  //ecore_x_e_virtual_keyboard_set(


  evas_object_show(win);
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

static Evas_Object* _keyboard_add(Keyboard* keyboard, const char* keyname, int col, int row, int width, int height)
{
  Evas_Object* bt = elm_button_add(keyboard->win);
  elm_object_text_set(bt, keyname);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_table_pack(keyboard->table, bt, col, row, width, height);

  evas_object_size_hint_min_set(bt, 30, 30);
  evas_object_show(bt);
  
  return bt;
}

void keyboard_add(Keyboard* keyboard, const char* keyname, int col, int row, int width, int height)
{
  Evas_Object* bt = _keyboard_add(keyboard, keyname, col, row, width, height);
  evas_object_smart_callback_add(bt, "pressed", _on_pressed, NULL);
  //evas_object_event_callback_add(bt, EVAS_CALLBACK_MULTI_DOWN, _multi_down, NULL);
}

void keyboard_rect_add(Keyboard* keyboard, const char* keyname, int col, int row, int width, int height)
{
  Evas_Object* bt = evas_object_rectangle_add(evas_object_evas_get(keyboard->win));
  evas_object_name_set(bt, keyname);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_table_pack(keyboard->table, bt, col, row, width, height);

  //evas_object_layer_set(bt, 10);
  //evas_object_raise(bt);
  
  evas_object_color_set(bt, 0, 255, 255, 255);

  evas_object_size_hint_min_set(bt, 30, 30);
  evas_object_show(bt);

  //evas_object_smart_callback_add(bt, "pressed", _on_pressed, NULL);
  evas_object_event_callback_add(bt, EVAS_CALLBACK_MOUSE_DOWN, _rect_down, NULL);
  evas_object_event_callback_add(bt, EVAS_CALLBACK_MULTI_DOWN, _multi_down, NULL);
}


void keyboard_fn_add(
      Keyboard* keyboard,
      const char* name,
      rust_cb cb,
      void* cb_data,
      int col,
      int row,
      int width,
      int height)
{
  Evas_Object* bt = _keyboard_add(keyboard, name, col, row, width, height);
  evas_object_data_set(bt, "cb", cb);
  evas_object_data_set(bt, "cb_data", cb_data);
  evas_object_smart_callback_add(bt, "pressed", _on_pressed_fn, NULL);
}

