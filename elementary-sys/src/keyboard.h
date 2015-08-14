#ifndef __keyboard__
#define __keyboard__
#include <Elementary.h>

typedef struct _Keyboard Keyboard;

struct _Keyboard
{
  Evas_Object* win;
  Evas_Object* table;
};

typedef void (rust_cb)(void* data);
typedef void (pressed_cb)(void* data, int device, int x, int y);

Keyboard* keyboard_new(Evas_Object* win, int px, int py, int kx, int ky, int ksx, int ksy);
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
Evas_Object* smart_keyboard_add(Evas *evas);
Evas_Object* smart_keyboard_key_add(
		Evas_Object *keyboard,
		const char* keyname,
		int row,
		float wf,
		float hf);
void smart_keyboard_key_max_set(
		Evas_Object *keyboard,
		Evas_Coord x, Evas_Coord y);

void smart_keyboard_key_space_set(
		Evas_Object *keyboard,
		Evas_Coord minx, Evas_Coord miny,
		Evas_Coord maxx, Evas_Coord maxy);

void smart_keyboard_padding_set(
		Evas_Object *keyboard,
		Evas_Coord x, Evas_Coord y);


#endif
