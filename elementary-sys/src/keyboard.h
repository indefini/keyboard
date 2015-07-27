#ifndef __keyboard__
#define __keyboard__

typedef struct _Keyboard Keyboard;

struct _Keyboard
{
  Evas_Object* win;
  Evas_Object* table;
};

Keyboard* keyboard_new();
void keyboard_add(Keyboard* keyboard, const char* keyname, int col, int row);

#endif 
