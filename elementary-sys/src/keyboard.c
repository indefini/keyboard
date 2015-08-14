#include "keyboard.h"
#include "Evas.h"

typedef struct _Key Key;
struct _Key
{
	float width_factor;
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
   Evas_Object *children[2];

   unsigned int key_width;
   unsigned int key_height;

   unsigned int key_space_minx;
   unsigned int key_space_miny;
   unsigned int key_space_maxx;
   unsigned int key_space_maxy;

   unsigned int paddingx;
   unsigned int paddingy;

   Eina_Array* rows;
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
   evas_object_show(priv->border);
   evas_object_smart_member_add(priv->border, o);

   /*
   Evas_Object *rect = evas_object_rectangle_add(e);
   evas_object_color_set(rect, rand() % 255, rand() % 255, rand() % 255, 255);
   evas_object_show(rect);
   priv->children[0] = rect;
   evas_object_smart_member_add(rect, o);

   rect = evas_object_rectangle_add(e);
   evas_object_color_set(rect, rand() % 255, rand() % 255, rand() % 255, 255);
   evas_object_show(rect);
   priv->children[1] = rect;
   evas_object_smart_member_add(rect, o);
   */

   priv->rows = eina_array_new(6);
}

static void
_smart_keyboard_del(Evas_Object *o)
{
  Smart_Keyboard * priv = evas_object_smart_data_get(o);

   if (priv->children[0])
     {
       // _evas_smart_example_child_callbacks_unregister(priv->children[0]);
        priv->children[0] = NULL;
     }

   if (priv->children[1])
     {
        //_evas_smart_example_child_callbacks_unregister(priv->children[1]);
        priv->children[1] = NULL;
     }

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
   int py = pyo;

   int mx = priv->key_space_maxx;
   int my = priv->key_space_maxy;

   if (priv->children[0])
     {
        evas_object_move(priv->children[0], x + px, y + py);
        evas_object_resize(priv->children[0], (w / 2) - px -mx, (h / 2) - py -my);
     }

   if (priv->children[1])
     {
        evas_object_move(priv->children[1], x + (w / 2) + mx, y + (h / 2) + my);
        evas_object_resize(priv->children[1], (w / 2) - px - mx, (h / 2) - px -my);
     }

   float ix = get_ideal_width(priv);
   float iy = get_ideal_height(priv);

   float width = 20.0f;
   float height = 15.0f;

   if (w > 2* priv->paddingx) {
   	w -= 2* priv->paddingx;
   }

   printf("w is : %d \n", w);

   if (ix <= w) {
	   width = priv->key_width;
	   mx = priv->key_space_maxx;
   }
   else {
	   float ratio = ((float)w)/ix;
	   printf("ratio : %f \n", ratio);
	   width = priv->key_width * ratio;
	   mx = priv->key_space_maxx * ratio;
   }

   Eina_Array *row;
   Eina_Array_Iterator iterator;
   unsigned int i;

   EINA_ARRAY_ITER_NEXT(priv->rows, i, row, iterator) {

   		Key* col;
   		Eina_Array_Iterator iterator_col;
   		unsigned int j;
		px = pxo;
   		EINA_ARRAY_ITER_NEXT(row, j, col, iterator_col) {
        	evas_object_move(col->text, x + px, y + py);
        	evas_object_move(col->object, x + px, y + py);
			int ww = width* col->width_factor + 0.5f;
        	evas_object_resize(col->object, ww, height);
			px += ww + mx;
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

  Evas_Object *rect = evas_object_rectangle_add(e);
  evas_object_color_set(rect, rand() % 255, rand() % 255, rand() % 255, 255);
  evas_object_show(rect);
  evas_object_smart_member_add(rect, keyboard);

  Evas_Object* t = evas_object_text_add(e);
  evas_object_text_style_set(t, EVAS_TEXT_STYLE_PLAIN);
  evas_object_color_set(t, 200, 200, 200, 255);
  evas_object_text_font_set(t, "Ubuntu", 10);
  evas_object_text_text_set(t, keyname);
  evas_object_raise(t);
  evas_object_show(t);

  Key* k = key_new(wf, rect, t);

  eina_array_push(row_cur, k);

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




