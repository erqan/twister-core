#ifndef DBUS_H_INCLUDED
#define DBUS_H_INCLUDED

void DBusThread();

#include "json/json_spirit_value.h"
json_spirit::Object tw_dbus_call(const char *method, json_spirit::Array& params);

#endif // DBUS_H_INCLUDED
