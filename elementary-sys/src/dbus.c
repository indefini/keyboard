#include "Eldbus.h"
#include <Ecore.h>
#define BUS "org.Enlightenment"
#define PATH "/org/enlightenment"
#define PATH_TEST_SON "/org/enlightenment/son"
#define INTERFACE "org.enlightenment.Test"

#include "dbus.h"

static Eldbus_Connection *conn;
static dbus_cb* _cb;
static void* _cb_data;

enum
{
   TEST_SIGNAL_ALIVE = 0,
   TEST_SIGNAL_HELLO
};

static const Eldbus_Signal signals[] = {
   [TEST_SIGNAL_ALIVE] = {"Alive", NULL, 0},
   [TEST_SIGNAL_HELLO] = {"Hello", ELDBUS_ARGS({ "s", "message" }), 0},
   { }
};

static Eldbus_Message *
_hello(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *message)
{
   Eldbus_Message *reply = eldbus_message_method_return_new(message);
   eldbus_message_arguments_append(reply, "s", "Hello World");
   printf("Hello\n");
   return reply;
}


static Eldbus_Message *
_quit(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *message)
{
   printf("Quit\n");
   ecore_main_loop_quit();
   return eldbus_message_method_return_new(message);
}


static Eina_Bool
send_signal_alive(void *data)
{
   Eldbus_Service_Interface *iface = data;
   eldbus_service_signal_emit(iface, TEST_SIGNAL_ALIVE);
   return ECORE_CALLBACK_RENEW;
}
static Eina_Bool
send_signal_hello(void *data)
{
   Eldbus_Service_Interface *iface = data;
   eldbus_service_signal_emit(iface, TEST_SIGNAL_HELLO, "Hello World");
   return ECORE_CALLBACK_RENEW;
}


static Eldbus_Message *
_send_bool(const Eldbus_Service_Interface *iface EINA_UNUSED, const Eldbus_Message *msg)
{
   Eldbus_Message *reply = eldbus_message_method_return_new(msg);
   Eina_Bool bool;
   if (!eldbus_message_arguments_get(msg, "b", &bool))
     printf("eldbus_message_arguments_get() error\n");
   eldbus_message_arguments_append(reply, "b", bool);

   printf("send bool from server!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   _cb(_cb_data);
   return reply;
}


static const Eldbus_Method methods[] = {
      {
        "Hello", NULL, ELDBUS_ARGS({"s", "message"}),
        _hello
      },
      {
        "Quit", NULL, NULL,
        _quit, ELDBUS_METHOD_FLAG_DEPRECATED
      },
      { "SendBool", ELDBUS_ARGS({"b", "bool"}), ELDBUS_ARGS({"b", "bool"}),
        _send_bool
      },
      /*
      { "SendByte", ELDBUS_ARGS({"y", "byte"}), ELDBUS_ARGS({"y", "byte"}),
        _send_byte
      },
      { "SendUint32", ELDBUS_ARGS({"u", "uint32"}), ELDBUS_ARGS({"u", "uint32"}),
        _send_uint32
      },
      { "SendInt32", ELDBUS_ARGS({"i", "int32"}), ELDBUS_ARGS({"i", "int32"}),
        _send_int32
      },
      { "SendInt16", ELDBUS_ARGS({"n", "int16"}), ELDBUS_ARGS({"n", "int16"}),
        _send_int16
      },
      { "SendDouble", ELDBUS_ARGS({"d", "double"}), ELDBUS_ARGS({"d", "double"}),
        _send_double
      },
      { "SendString", ELDBUS_ARGS({"s", "string"}), ELDBUS_ARGS({"s", "string"}),
        _send_string
      },
      { "AsyncTest", NULL, ELDBUS_ARGS({"s", "text"}),
        _async_test
      },
      */
      { }
};
static const Eldbus_Service_Interface_Desc iface_desc = {
   INTERFACE, methods, signals
};

static void
on_name_request(void *data, const Eldbus_Message *msg, Eldbus_Pending *pending EINA_UNUSED)
{
   Eldbus_Service_Interface *iface;
   unsigned int reply;
   iface = data;
   if (eldbus_message_error_get(msg, NULL, NULL))
     {
        printf("error on on_name_request\n");
        return;
     }
   if (!eldbus_message_arguments_get(msg, "u", &reply))
    {
       printf("error geting arguments on on_name_request\n");
       return;
    }
   if (reply != ELDBUS_NAME_REQUEST_REPLY_PRIMARY_OWNER)
     {
        printf("error name already in use\n");
        return;
     }
   ecore_timer_add(5, send_signal_alive, iface);
   ecore_timer_add(6, send_signal_hello, iface);
}

void init_dbus(dbus_cb cb, void *data)
{
  _cb = cb;
  _cb_data = data;
  Eldbus_Service_Interface *iface;
  ecore_init();
  eldbus_init();
  conn = eldbus_connection_get(ELDBUS_CONNECTION_TYPE_SESSION);
  iface = eldbus_service_interface_register(conn, PATH, &iface_desc);
  eldbus_name_request(conn, BUS, ELDBUS_NAME_REQUEST_FLAG_DO_NOT_QUEUE,
        on_name_request, iface);

}
