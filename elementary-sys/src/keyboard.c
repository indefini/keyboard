#include "keyboard.h"
#include "Evas.h"

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
   Evas_Object *children[2], *border;
   int child_count;
};

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

   /* this is a border around the smart object's area, delimiting it */
   priv->border = evas_object_image_filled_add(evas_object_evas_get(o));
   //evas_object_image_file_set(priv->border, border_img_path, NULL);
   evas_object_image_border_set(priv->border, 3, 3, 3, 3);
   evas_object_image_border_center_fill_set(
     priv->border, EVAS_BORDER_FILL_NONE);
   evas_object_show(priv->border);
   evas_object_smart_member_add(priv->border, o);
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

   evas_object_resize(priv->border, w, h);
   evas_object_move(priv->border, x, y);

   if (priv->children[0])
     {
        evas_object_move(priv->children[0], x + 3, y + 3);
        evas_object_resize(priv->children[0], (w / 2) - 3, (h / 2) - 3);
     }

   if (priv->children[1])
     {
        evas_object_move(priv->children[1], x + (w / 2), y + (h / 2));
        evas_object_resize(priv->children[1], (w / 2) - 3, (h / 2) - 3);
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



