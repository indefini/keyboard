#include "keyboard.h"
#include "Evas.h"

typedef struct _Key Key;
struct _Key
{
	float width_factor;
	Evas_Object* object;
	Evas_Object* text;
};

typedef struct _Popup Popup;
struct _Popup
{
	Evas_Object* object;
	Evas_Object* text;
};


Key* key_new(float width_factor, Evas_Object* o, Evas_Object* txt)
{
	Key* k = calloc(1, sizeof *k);
	k->width_factor = width_factor;
	k->object = o;
	k->text = txt;

	return k;
}

Eo* text_new(Evas* e, int r, int g, int b, int size)
{
  Evas_Object* t = evas_object_text_add(e);
  evas_object_text_style_set(t, EVAS_TEXT_STYLE_PLAIN);
  evas_object_color_set(t, r, g, b, 255);
  evas_object_text_font_set(t, "Ubuntu", size);
  return t;
}

static const Evas_Smart_Cb_Description _smart_callbacks[] =
{
   {"christest", "i"},
   {NULL, NULL}
};

typedef struct _Smart_Keyboard Smart_Keyboard;
/*
 * This structure augments clipped smart object's instance data,
 * providing extra members required by our example smart object's
 * implementation.
 */
struct _Smart_Keyboard
{
   Evas_Object_Smart_Clipped_Data base;
   Evas_Object *border;
   Evas_Object *win;

   unsigned int key_width;
   unsigned int key_height;

   unsigned int key_space_minx;
   unsigned int key_space_miny;
   unsigned int key_space_maxx;
   unsigned int key_space_maxy;

   unsigned int paddingx;
   unsigned int paddingy;

   unsigned int ideal_width;
   unsigned int ideal_index;
   float ideal_factor_width;
   int more;

   Eina_Array* rows;
   Popup* popup;
};

static float get_ideal_height(Smart_Keyboard* k)
{
	int count =  eina_array_count(k->rows);
	return count * k->key_height + (count -1) * k->key_space_maxy;
}

static float get_ideal_width(Smart_Keyboard* k)
{
   Eina_Array *row;
   Eina_Array_Iterator iterator;
   unsigned int i;
   float maxx = 0;

   EINA_ARRAY_ITER_NEXT(k->rows, i, row, iterator) {

   		Key* col;
   		Eina_Array_Iterator iterator_col;
   		unsigned int j;
		float row_width = 0;

   		EINA_ARRAY_ITER_NEXT(row, j, col, iterator_col) {
			row_width += k->key_width*col->width_factor + k->key_space_maxx ;
		}

		row_width -= k->key_space_maxx;

		if (row_width > maxx) {
			maxx = row_width;
		}
   }

   return maxx;

   /*
   if (maxx <= width) {
	   // use max key and max space
   }
   else {
	   float ratio = width / maxx;
   }
   */
}

EVAS_SMART_SUBCLASS_NEW("Smart_Keyboard", _smart_keyboard,
                        Evas_Smart_Class, Evas_Smart_Class,
                        evas_object_smart_clipped_class_get, _smart_callbacks);

static void
_smart_keyboard_add(Evas_Object *o)
{
   EVAS_SMART_DATA_ALLOC(o, Smart_Keyboard);

   /* call parent_sc->add() before member_adding the children, otherwise the
    * smart object's clipper won't be created yet, and the children won't be
    * clipped to it */
   _smart_keyboard_parent_sc->add(o);

   Evas* e = evas_object_evas_get(o);

   /* this is a border around the smart object's area, delimiting it */
   priv->border = evas_object_image_filled_add(e);
   evas_object_image_file_set(priv->border, "red.png", NULL);
   evas_object_image_border_set(priv->border, 3, 3, 3, 3);
   evas_object_image_border_center_fill_set(
     priv->border, EVAS_BORDER_FILL_NONE);
   //evas_object_show(priv->border);
   evas_object_smart_member_add(priv->border, o);

   priv->rows = eina_array_new(6);

   //Eo* winpop = elm_win_add(NULL, "keyboard_popup", ELM_WIN_TOOLTIP);
   Eo* winpop = elm_win_add(NULL, "keyboard_popup", ELM_WIN_DOCK);
  elm_win_override_set(winpop, EINA_TRUE);
   evas_object_name_set(winpop, strdup("keypop"));
   elm_win_raise(winpop);
  //elm_win_autodel_set(winpop, EINA_TRUE);
  //evas_object_smart_callback_add(win, "delete,request", _window_del, NULL);

   /*
  Evas_Object* bg = elm_bg_add(winpop);
  evas_object_name_set(bg, strdup("bg"));
  evas_object_show(bg);
  evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(winpop, bg);
  */

  //elm_win_keyboard_win_set(win, EINA_TRUE);
  elm_win_prop_focus_skip_set(winpop, EINA_TRUE);
  //elm_win_override_set(win, EINA_TRUE);
  elm_win_screen_constrain_set(winpop, EINA_TRUE);
  //elm_win_sticky_set(win, EINA_TRUE);
  elm_win_borderless_set(winpop, EINA_TRUE);

  //Ecore_X_Window *xwin = elm_win_xwindow_get(winpop);
  ////ecore_x_e_virtual_keyboard_set(
  //ecore_x_window_cursor_show(xwin, EINA_FALSE);


  //evas_object_show(win);








   Evas* ew = evas_object_evas_get(winpop);
   Eo* rect = evas_object_rectangle_add(ew);
   evas_object_show(rect);
   evas_object_color_set(rect, 180, 180, 180, 255);
   Eo* t = text_new(ew, 10,10, 10, 30);
   evas_object_raise(t);
   evas_object_show(t);
   evas_object_text_text_set(t, "test");
  evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(winpop, rect);

  //evas_object_size_hint_weight_set(t, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(t, 0.5, 0.5);
  //elm_win_resize_object_add(winpop, t);

   Popup* pp = calloc(1, sizeof *pp);
   pp->object = winpop;//rect;
   pp->text = t;
   priv->popup = pp;
}

static void
_smart_keyboard_del(Evas_Object *o)
{
  Smart_Keyboard * priv = evas_object_smart_data_get(o);

   _smart_keyboard_parent_sc->del(o);
}

static void
_smart_keyboard_resize(Evas_Object *o,
                                 Evas_Coord w,
                                 Evas_Coord h)
{
   Evas_Coord ow, oh;
   evas_object_geometry_get(o, NULL, NULL, &ow, &oh);
   printf("resize to %d, %d \n", ow, oh);
   if ((ow == w) && (oh == h)) return;

   /* this will trigger recalculation */
   evas_object_smart_changed(o);
}

static void
_smart_keyboard_calculate(Evas_Object *o)
{
   Evas_Coord x, y, w, h;

   Smart_Keyboard * priv = evas_object_smart_data_get(o);
   if (!priv) return;

   evas_object_geometry_get(o, &x, &y, &w, &h);

   printf("recalculate to %d, %d, %d, %d \n", x, y, w, h);

   evas_object_resize(priv->border, w, h);
   evas_object_move(priv->border, x, y);

   int pxo = priv->paddingx;
   int pyo = priv->paddingy;;
   int px = pxo;
   printf("px, pxo ------------ : %d,%d \n", px, pxo);
   int py = pyo;

   unsigned int mx = priv->key_space_maxx;
   unsigned int my = priv->key_space_maxy;

   //float ix = get_ideal_width(priv);
   float ix = priv->ideal_width;

   //float width = 20.0f;
   int width = 20;
   float height = 15.0f;

   if (w > 2* priv->paddingx) {
   	w -= 2* priv->paddingx;
   }

   printf("w is : %d \n", w);

   Eina_Array* longrow = eina_array_data_get(priv->rows,priv->ideal_index);
   if (ix <= w) {
	   width = priv->key_width;
	   mx = priv->key_space_maxx;
   }
   else {
	   float ratio = ((float)w)/ix;
	   printf("ratio : %f \n", ratio);
	   mx = priv->key_space_maxx * ratio;
	   if (mx < priv->key_space_minx) {
		   mx = priv->key_space_minx;
	   	printf("minx is  : %d, %d \n", priv->key_space_minx, mx);
	   }
	   int keys_count = eina_array_count(longrow);
	   int left_for_keys = w - (priv->more + keys_count -1) * mx;
	   ////float yop = (float) left_for_keys/ (float)keys_count*priv->ideal_factor_width;
	   ////width = ((float) left_for_keys)/priv->ideal_factor_width;
	   width = left_for_keys/priv->ideal_factor_width;
	   //int multiplier = left_for_keys/priv->ideal_factor_width/mx;
	   //width = mx * multiplier;

   }

   /*
   float iy = get_ideal_height(priv);
   if (iy <= h) {
	   height = priv->key_height;
	   my = priv->key_space_maxy;
	   printf("height, my : %f , %d\n", height,my);
   }
   else
   {
	   //float ratio = ((float)h)/iy;
	   printf("ratio : %f \n", ratio);
	   my = priv->key_space_maxy * ratio;
	   if (my < priv->key_space_miny) {
		   my = priv->key_space_miny;
	   	printf("miny is  : %d, %d \n", priv->key_space_miny, my);
	   }
	   int rows_count = eina_array_count(priv->rows);
	   int left_for_keys = h - (rows_count -1) * my;
	   height = ((float) left_for_keys)/rows_count;
	   int hh = ((int)height) * rows_count + (rowsj_count-1) * my;;
   	   py = ((int) priv->paddingy) + ((int) ( ((int)h - (int) hh)/2));
   }
   */

   float ratio = ((float) width) / ((float) priv->key_width);
   height = priv->key_height *ratio;
   my = priv->key_space_maxy *ratio;
   if (my < priv->key_space_miny) {
		   my = priv->key_space_miny;
   }
	   int rows_count = eina_array_count(priv->rows);
   float iy = height*rows_count + my * (rows_count -1);

   if (iy > h) {
       //TODO
       py = 0;
   }
   else {
       py = (h-iy)/2.0f;
   }




   	Key* col;
   	Eina_Array_Iterator iterator_col;
   	unsigned int j;
   	unsigned int test = 0;
   	EINA_ARRAY_ITER_NEXT(longrow, j, col, iterator_col) {
		int ww = width* col->width_factor;// + 0.5f;
		test += ww + mx;
	}
   	test -= mx;

   pxo = ((int) priv->paddingx) + ((int) ( ((int)w - (int) test)/2));



   Eina_Array *row;
   Eina_Array_Iterator iterator;
   unsigned int i;

   EINA_ARRAY_ITER_NEXT(priv->rows, i, row, iterator) {

   		Key* col;
   		Eina_Array_Iterator iterator_col;
   		unsigned int j;
		px = pxo;
   		EINA_ARRAY_ITER_NEXT(row, j, col, iterator_col) {
            if (col->text) {
        	evas_object_move(col->text, x + px, y + py);
            }
            int more_space = ((int) col->width_factor) -1;
            //int more_space = 0;
            if (col->width_factor - ((int) col->width_factor)  > 0) {
                more_space += 1;
            }
            if (j== 0 ) {
                more_space = 0;
            }

            //if (j== 0 || more_space < 0) {
            if (more_space < 0) {
                more_space = 0;
            }
            printf("more space for %f, %d ¥n", col->width_factor, more_space);

			//int ww = width* col->width_factor + ( ((int) col->width_factor) -1)*mx;;
			int ww = width* col->width_factor + more_space*mx;
            if (col->object) {
        	evas_object_move(col->object, x + px, y + py);
        	evas_object_resize(col->object, ww, height);
            }

			printf("%d,,,px before : %d \n", j, px);
			px += ww + (int) mx;
		}
		printf("px after row is : %d \n", px);
		py += height + my;
   }
}

static void
_smart_keyboard_smart_set_user(Evas_Smart_Class *sc)
{
   /* specializing these two */
   sc->add = _smart_keyboard_add;
   sc->del = _smart_keyboard_del;;

   /* clipped smart object has no hook on resizes or calculations */
   sc->resize = _smart_keyboard_resize;
   sc->calculate = _smart_keyboard_calculate;
}

// smart keyboard interface
//
//
//
//
Evas_Object *
smart_keyboard_add(Evas *evas)
{
   return evas_object_smart_add(evas, _smart_keyboard_smart_class_new());
}

void
smart_keyboard_keys_padding_set(Evas_Object *keyboard)
{
	//TODO
}

Evas_Object*
smart_keyboard_key_add(
		Evas_Object *keyboard,
		const char* keyname,
		int row,
		float wf,
		float hf)
{
	//TODO
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);

  while (row >= eina_array_count(priv->rows)) {
	  eina_array_push(priv->rows, eina_array_new(10));
  }

  Eina_Array *row_cur = eina_array_data_get(priv->rows, row);

  Evas* e = evas_object_evas_get(keyboard);

  Evas_Object *rect = NULL;

  if (strcmp(keyname, "__empty")) {
  rect = evas_object_rectangle_add(e);
  //evas_object_color_set(rect, rand() % 255, rand() % 255, rand() % 255, 255);
  evas_object_color_set(rect, 80, 80, 80, 255);
  evas_object_show(rect);
  evas_object_smart_member_add(rect, keyboard);

  Eo* t = text_new(e, 200, 200, 200, 10);
  evas_object_text_text_set(t, keyname);
  evas_object_raise(t);
  evas_object_show(t);

  Key* k = key_new(wf, rect, t);

  eina_array_push(row_cur, k);
  }
  else {
  Key* k = key_new(wf, NULL, NULL);

  eina_array_push(row_cur, k);
  }

   Key *key;
   Eina_Array_Iterator iterator;
   unsigned int i;

   unsigned int roww = 0;
   float f = 0;
   int more = 0;
   EINA_ARRAY_ITER_NEXT(row_cur, i, key, iterator) {

            int more_space = ((int) key->width_factor);
            if (more_space < 0) {
                more_space = 0;
            }
            if (i== 0 && more_space > 1 ) {
                //more_space = 1;
            }
            //printf("more space for %f, %d ¥n", col->width_factor, more_space);

			//int ww = key->width* key->width_factor + more_space*mx;;
            if (more_space > 1) more += more_space -1;
            int ww = key->width_factor*priv->key_width + priv->key_space_maxx*more_space;

       //roww += key->width_factor*priv->key_width + priv->key_space_maxx;;
       roww += ww;
	   f += key->width_factor;
   }
   roww -= priv->key_space_maxx;

   if (roww > priv->ideal_width){
       printf("longuest is  : %d ¥n \n", row);
       priv->ideal_width = roww;
       priv->ideal_index = row;
	   priv->ideal_factor_width = f;
       priv->more = more;
   }

  return rect;
}


void smart_keyboard_key_max_set(
		Evas_Object *keyboard,
		Evas_Coord x, Evas_Coord y)
{
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);

  if (x != priv->key_width || y != priv->key_height) {
  	priv->key_width = x;
  	priv->key_height = y;
  	evas_object_smart_changed(keyboard);
  }
}

void smart_keyboard_padding_set(
		Evas_Object *keyboard,
		Evas_Coord x, Evas_Coord y)
{
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);

  if (x != priv->paddingx || y != priv->paddingy) {
  	priv->paddingx = x;
  	priv->paddingy = y;
  	evas_object_smart_changed(keyboard);
  }
}

void smart_keyboard_key_space_set(
		Evas_Object *keyboard,
		Evas_Coord minx, Evas_Coord miny,
		Evas_Coord maxx, Evas_Coord maxy
		)
{
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);

  if (minx != priv->key_space_minx ||
	  miny != priv->key_space_miny||
	  maxx != priv->key_space_maxx||
	  maxy != priv->key_space_maxy
	  ) {
  	priv->key_space_minx = minx;
  	priv->key_space_miny = miny;
  	priv->key_space_maxx = maxx;
  	priv->key_space_maxy = maxy;
  	evas_object_smart_changed(keyboard);
  }
}

void smart_keyboard_size_get(
		Evas_Object* keyboard,
		Evas_Coord* x,
		Evas_Coord* y)
{
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);

}


void smart_keyboard_show_popup(
      Evas_Object* k,
      Evas_Object* o,
      const char* name)
{
	printf("todo \n");
  Smart_Keyboard* priv = evas_object_smart_data_get(k);
  Eo* rect = priv->popup->object;
  Eo* text = priv->popup->text;

  Evas_Coord kx, ky, kw, kh;
  evas_object_geometry_get(priv->win, &kx, &ky, &kw, &kh);

  Evas_Coord x, y, w, h;
  evas_object_geometry_get(o, &x, &y, &w, &h);

  printf("ky : %d \n", ky);
  evas_object_move(rect, x, ky + y - priv->key_height*1.5f);
  evas_object_resize(rect, w, h);
  evas_object_show(rect);

  evas_object_text_text_set(text, name);
  //evas_object_move(text, kx + x, ky + y - 100);
  evas_object_move(text, w/2, h/2);
  //evas_object_move(text, 50, 40);
  evas_object_show(text);
}

void smart_keyboard_hide_popup(
      Evas_Object* k)
{
  Smart_Keyboard* priv = evas_object_smart_data_get(k);
  Eo* rect = priv->popup->object;
  evas_object_hide(rect);
  Eo* text = priv->popup->text;
  evas_object_hide(text);
}

/*
Eo* keyboard_create(Evas_Object* win)
{
  Evas* e = evas_object_evas_get(win);
  Evas_Object* smart = smart_keyboard_add(e);
  smart_keyboard_key_max_set(smart, kx, ky);
  smart_keyboard_key_space_set(smart, 4, 2, ksx, ksy);
  smart_keyboard_padding_set(smart, 0, 2);
  Smart_Keyboard* priv = evas_object_smart_data_get(keyboard);
  priv->win = win;

  return smart;

}
*/

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

Evas_Object* edje_test(Evas* e, int width, int height)
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
   edje_edit_group_max_w_set(edje, width);
   edje_edit_group_max_h_set(edje, height);

   edje_object_size_max_get(edje, &maxw, &maxh);
   edje_object_size_min_get(edje, &minw, &minh);
   printf("edje max %d, %d \n", maxw, maxh);
   if ((minw <= 0) && (minh <= 0))
   edje_object_size_min_calc(edje, &minw, &minh);

   evas_object_size_hint_max_set(edje, maxw, maxh);
   evas_object_size_hint_min_set(edje, minw, minh);

   evas_object_size_hint_align_set(edje, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(edje, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   evas_object_show(edje);
   return edje;
}




Keyboard* keyboard_new(Evas_Object* win, int px, int py, int kx, int ky, int ksx, int ksy)
{
  int x, y, w, h;
  elm_win_screen_size_get(win, &x, &y, &w, &h);
  //printf("screen x, y, w, h : %d, %d, %d, %d \n", x, y, w, h);

  //int winh = h/2.5;
  evas_object_resize(win, w, py);
  //elm_win_size_base_set(win, , winh);
  evas_object_move(win, 0, h - py);
  //test size
  //evas_object_resize(win, 206, 156);

  //printf("screen dpx, dpy : %d, %d \n", dpx, dpy);

  Evas* e = evas_object_evas_get(win);
  Evas_Object* edje = edje_test(e, px, py);
  //edje_edit_group_max_w_set(edje, 721);
  //edje_edit_group_max_h_set(edje, py);

  Evas_Object* stack = _create_stack(e);
  evas_object_box_append(stack, edje);
  evas_object_event_callback_add(stack, EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                                  _reset_size_hints, edje);

  elm_win_resize_object_add(win, stack);

  //TODO
  Evas_Object* smart = smart_keyboard_add(e);
  smart_keyboard_key_max_set(smart, kx, ky);
  smart_keyboard_key_space_set(smart, 4, 2, ksx, ksy);
  smart_keyboard_padding_set(smart, 0, 2);
  Smart_Keyboard* priv = evas_object_smart_data_get(smart);
  priv->win = win;

  edje_object_part_swallow(edje, "rect", smart);
  evas_object_show(smart);


  Keyboard* k = calloc(1, sizeof *k);
  k->win = win;
  k->smart = smart;

  return k;
}

