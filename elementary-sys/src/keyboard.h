#ifndef __keyboard__
#define __keyboard__

typedef struct _Keyboard Keyboard;

struct _Keyboard
{
  Evas_Object* win;
  Evas_Object* table;
};

typedef void (rust_cb)(void* data);

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

#endif 
