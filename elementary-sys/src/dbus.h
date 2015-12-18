#ifndef __keyboard_dbus__
#define __keyboard_dbus__

typedef void (dbus_cb)(void* data);

void init_dbus(dbus_cb cb, void* data);

#endif
