#include <Elementary.h>
#include "keyboard.h"

Evas_Object* _win;
Evas_Object* _table;
Eina_List* _btns = NULL;

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

   int x, y, w, h;
   evas_object_geometry_get(_table, &x, &y, &w, &h);
   printf("table geom : %d, %d, %d, %d\n", x, y, w, h);

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
  _table = tb;
  evas_object_name_set(tb, "table");
  evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //evas_object_size_hint_min_set(tb, 100, 100);
  //evas_object_size_hint_max_set(tb, 700, 500);
  //elm_win_resize_object_add(win, tb);
  elm_table_homogeneous_set(tb, EINA_TRUE);
  elm_table_padding_set(tb, 14, 14);
  //elm_table_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //elm_table_align_set(tb, 0, 0);
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

static Evas_Object *
_create_stack(Evas *evas)//, const struct opts *opts)
{
   Evas_Object *stack = evas_object_box_add(evas);
   if (!stack)
     {
        fputs("ERROR: could not create object stack (box).\n", stderr);
        return NULL;
     }
   //evas_object_box_layout_set(stack, evas_object_box_layout_stack, NULL, NULL);
   evas_object_box_layout_set(stack, evas_object_box_layout_vertical, NULL, NULL);
   //evas_object_resize(stack, opts->size.w, opts->size.h);
   //evas_object_resize(stack, 300, 300);
   evas_object_size_hint_weight_set(stack, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(stack, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(stack);
   return stack;
}

static void
_reset_size_hints(void *data, Evas *e EINA_UNUSED, Evas_Object *stack, void *event_info EINA_UNUSED)
{
   Evas_Coord minw, minh;
   Evas_Object *edje = data;

   edje_object_size_min_get(edje, &minw, &minh);
   if ((minw <= 0) && (minh <= 0))
     edje_object_size_min_calc(edje, &minw, &minh);

   evas_object_size_hint_min_set(stack, minw, minh);
}

Evas_Object* edje_test(Evas* e)
{
   Evas_Object *edje;
   edje = edje_object_add(e);
   if (!edje){
     EINA_LOG_CRIT("could not create edje object!");
     return NULL;
   }
   if (!edje_object_file_set(edje, "layout.edj", "main")) {
        int err = edje_object_load_error_get(edje);
        const char *errmsg = edje_load_error_str(err);
        EINA_LOG_ERR("could not load 'my_group' from .edj file: %s",
                     errmsg);
        evas_object_del(edje);
        return NULL;
   }
   /*
   if (!edje_object_part_text_set(edje, "text", text)) {
     EINA_LOG_WARN("could not set the text. "
           "Maybe part 'text' does not exist?");
   }
   */

   //evas_object_move(edje, 0, 0);
   //evas_object_resize(edje, 100, 100);

   Evas_Coord minw, minh, maxw, maxh;

   edje_object_size_max_get(edje, &maxw, &maxh);
   edje_object_size_min_get(edje, &minw, &minh);
   if ((minw <= 0) && (minh <= 0))
   edje_object_size_min_calc(edje, &minw, &minh);

   evas_object_size_hint_max_set(edje, maxw, maxh);
   evas_object_size_hint_min_set(edje, minw, minh);

   evas_object_size_hint_align_set(edje, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(edje, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   evas_object_show(edje);
   return edje;
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
  elm_win_prop_focus_skip_set(win, EINA_TRUE);
  elm_win_override_set(win, EINA_TRUE);
  elm_win_screen_constrain_set(win, EINA_TRUE);
  elm_win_sticky_set(win, EINA_TRUE);
  //elm_win_borderless_set(win, EINA_TRUE);

  //Ecore_X_Window *xwin = elm_win_xwindow_get(win);
  //ecore_x_e_virtual_keyboard_set(


  evas_object_show(win);
  return win;
}

Keyboard* keyboard_new(Evas_Object* win)
{
  //Evas_Object* win = window_new();
  int dpx, dpy;
  elm_win_screen_dpi_get(win, &dpx, &dpy);
  int x, y, w, h;
  elm_win_screen_size_get(win, &x, &y, &w, &h);
  printf("screen x, y, w, h : %d, %d, %d, %d \n", x, y, w, h);

  int winh = h/2.5;
  evas_object_resize(win, w, winh);
  elm_win_size_base_set(win, w, winh);
  evas_object_move(win, 0, h - winh);
  //test size
  //evas_object_resize(win, 206, 156);


  printf("screen dpx, dpy : %d, %d \n", dpx, dpy);

  Evas_Object* table = table_new(win);

  //chris
  /*
  Evas* e = evas_object_evas_get(win);
  Evas_Object*box = evas_object_box_add(e);
  evas_object_resize(box, 1900, 200);
  evas_object_size_hint_min_set(box, 25, 37);
  evas_object_size_hint_max_set(box, 500, 200);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_aspect_set(box, EVAS_ASPECT_CONTROL_BOTH, 1, 1);
  //elm_win_resize_object_add(win, box);

  evas_object_show(box);
  */
  /*
  Evas_Object* r = evas_object_rectangle_add(e);
  evas_object_resize(r, 50, 75);
  evas_object_size_hint_min_set(r, 25, 37);
  evas_object_size_hint_max_set(r, 500, 750);

  //evas_object_size_hint_weight_set(r, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, r);
  
  evas_object_size_hint_weight_set(r, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(r, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_color_set(r, 100, 200, 20, 255);
  evas_object_show(r);
  */
  //evas_object_box_append(box, table);

  //elm_win_resize_object_add(win, table);


  Evas* e = evas_object_evas_get(win);
  Evas_Object* edje = edje_test(e);

  Evas_Object* stack = _create_stack(e);
  evas_object_box_append(stack, edje);
  evas_object_event_callback_add(stack, EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                                  _reset_size_hints, edje);

  elm_win_resize_object_add(win, stack);

  edje_object_part_swallow(edje, "rect", table);


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

int mm_to_px(float mm, int dpi)
{
    float f = ((float)dpi )/ 25.4f * mm;
    return (int) f;
}

Evas_Object* keyboard_rect_add(Keyboard* keyboard, const char* keyname, int col, int row, int width, int height)
{
  Evas* e = evas_object_evas_get(keyboard->win);

  Evas_Object* bt = evas_object_rectangle_add(e);
  _btns = eina_list_append(_btns, bt);
  evas_object_name_set(bt, keyname);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  elm_table_pack(keyboard->table, bt, col, row, width, height);

  //evas_object_layer_set(bt, 10);
  //evas_object_raise(bt);
  
  evas_object_color_set(bt, 80, 80, 80, 255);


  int dpx, dpy;
  elm_win_screen_dpi_get(keyboard->win, &dpx, &dpy);
  //evas_object_size_hint_min_set(bt, mm_to_px(5, dpx) , mm_to_px(5, dpy));
  //evas_object_size_hint_max_set(bt, mm_to_px(15, dpx) , mm_to_px(15, dpy));
  evas_object_show(bt);

  Evas_Object* t = evas_object_text_add(e);
  evas_object_text_style_set(t, EVAS_TEXT_STYLE_PLAIN);
  evas_object_color_set(t, 200, 200, 200, 255);
  evas_object_text_font_set(t, "Ubuntu", 10);
  evas_object_size_hint_padding_set(t, 15, 10, 0, 0);
  evas_object_size_hint_weight_set(t, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(t, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //evas_object_size_hint_align_set(t, 0.2, 0.1);
  elm_table_pack(keyboard->table, t, col, row, width, height);
  evas_object_text_text_set(t, keyname);
  evas_object_raise(t);
  evas_object_show(t);

  /*
  int size;
  const char *font;
  evas_object_text_font_get(t, &font, &size);
  fprintf(stdout, "Adding text object with font %s, size %d\n", font, size);
  */

  return bt;
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

static void
_call_pressed_cb(Evas_Object* o, int device, int x, int y)
{
   pressed_cb* cb = evas_object_data_get(o, "cb");

   if (cb) {
     void* cb_data = evas_object_data_get(o, "cb_data");
     cb(cb_data, device, x, y);
   }
}

static void
_call_up_cb(Evas_Object* o, int device, int x, int y)
{
   pressed_cb* cb_up = evas_object_data_get(o, "cb_up");

   if (cb_up) {
     void* cb_data = evas_object_data_get(o, "cb_data");
     cb_up(cb_data, device, x, y);
   }
}

static void
_call_move_cb(Evas_Object* o, int device, int x, int y)
{
   pressed_cb* cb_move = evas_object_data_get(o, "cb_move");

   if (cb_move) {
     void* cb_data = evas_object_data_get(o, "cb_data");
     cb_move(cb_data, device, x, y);
   }
}

static void
_rect_mouse_down(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   if (ev->button != 1) return;
   _call_pressed_cb(o, 0, ev->canvas.x, ev->canvas.y);
}

static void
_rect_mouse_up(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Mouse_Up *ev = event_info;
   if (ev->button != 1) return;
   _call_up_cb(o, 0, ev->canvas.x, ev->canvas.y);
}

static void
_rect_mouse_move(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Mouse_Move *ev = event_info;
   _call_move_cb(o, 0, ev->cur.canvas.x, ev->cur.canvas.y);
}

static void
_rect_multi_down(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Multi_Down *ev = event_info;
   _call_pressed_cb(o, ev->device, ev->canvas.x, ev->canvas.y);
}

static void
_rect_multi_up(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Multi_Up *ev = event_info;
   _call_up_cb(o, ev->device, ev->canvas.x, ev->canvas.y);
}

static void
_rect_multi_move(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Multi_Move *ev = event_info;
   _call_move_cb(o, ev->device, ev->cur.canvas.x, ev->cur.canvas.y);
}


void keyboard_bg_add(
      pressed_cb cb,
      pressed_cb cb_up,
      pressed_cb cb_move,
      void* cb_data)
{
  Evas_Object* r = evas_object_rectangle_add(evas_object_evas_get(_win));
  evas_object_size_hint_weight_set(r, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_color_set(r, 0, 0, 0, 0);
  elm_win_resize_object_add(_win, r);
  evas_object_show(r);

  evas_object_data_set(r, "cb", cb);
  evas_object_data_set(r, "cb_up", cb_up);
  evas_object_data_set(r, "cb_data", cb_data);
  evas_object_data_set(r, "cb_move", cb_move);

  evas_object_event_callback_add(r, EVAS_CALLBACK_MOUSE_DOWN, _rect_mouse_down, NULL);
  evas_object_event_callback_add(r, EVAS_CALLBACK_MOUSE_UP, _rect_mouse_up, NULL);
  evas_object_event_callback_add(r, EVAS_CALLBACK_MOUSE_MOVE, _rect_mouse_move, NULL);
  evas_object_event_callback_add(r, EVAS_CALLBACK_MULTI_DOWN, _rect_multi_down, NULL);
  evas_object_event_callback_add(r, EVAS_CALLBACK_MULTI_UP, _rect_multi_up, NULL);
  evas_object_event_callback_add(r, EVAS_CALLBACK_MULTI_MOVE, _rect_multi_move, NULL);

}

Eina_Bool
is_point_inside(Evas_Object* o, int x, int y)
{
  int ox, oy, ow, oh;
  evas_object_geometry_get(o, &ox, &oy, &ow, &oh);

  return 
   x >= ox && x <= ox + ow &&
   y >= oy && y <= oy + oh;
}

void elm_dpi_get(const Keyboard* k, int* dpix, int *dpiy)
{
  elm_win_screen_dpi_get(k->win, dpix, dpiy);
}
