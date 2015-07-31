#ifndef __keyboard__
#define __keyboard__

typedef struct _Keyboard Keyboard;

struct _Keyboard
{
  Evas_Object* win;
  Evas_Object* table;
};

typedef void (rust_cb)(void* data);
typedef void (pressed_cb)(void* data, int device, int x, int y);

Keyboard* keyboard_new();
void keyboard_add(Keyboard* keyboard, const char* keyname, int col, int row, int width, int height);

void keyboard_fn_add(
      Keyboard* keyboard,
      const char* name,
      rust_cb cb,
      void* cb_data,
      int col,
      int row,
      int width,
      int height);

void keyboard_bg_add(
      pressed_cb cb,
      pressed_cb cb_up,
      pressed_cb cb_move,
      void* cb_data);

Eina_Bool is_point_inside(Evas_Object* o, int x, int y);

#endif 
