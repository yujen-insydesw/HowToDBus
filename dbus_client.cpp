/*

Check D-Bus Policy Configuration: 

Ensure that the D-Bus configuration allows your user to call the method on the specified interface. The configuration files are usually located in /etc/dbus-1/system.d/ or /etc/dbus-1/session.d/. You might need to add or modify a policy rule to allow your method call. Example policy rule:

XML

<policy user="your-username">
    <allow send_destination="org.freedesktop.DBus" send_interface="org.freedesktop.DBus.Debug.Stats" />
</policy>

*/

#include <dbus/dbus.h>
#include <iostream>
#include <string>
#include <map>

int main() {
    DBusError error;
    dbus_error_init(&error);

    DBusConnection* connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Connection Error: " << error.message << std::endl;
        dbus_error_free(&error);
    }
    if (!connection) {
        return 1;
    }

    DBusMessage* msg = dbus_message_new_method_call(
        "org.freedesktop.DBus", // target service name
        "/org/freedesktop/DBus", // object path
        "org.freedesktop.DBus.Debug.Stats", // interface name
        "GetStats" // method name
    );
    if (!msg) {
        std::cerr << "Message Null" << std::endl;
        return 1;
    }

    // Send the message and get a handle for a reply
    DBusMessage* reply = dbus_connection_send_with_reply_and_block(connection, msg, -1, &error);
    dbus_message_unref(msg);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Error in send_with_reply_and_block: " << error.message << std::endl;
        dbus_error_free(&error);
    }
    if (!reply) {
        return 1;
    }

    DBusMessageIter iter;
    dbus_message_iter_init(reply, &iter);

    if (DBUS_TYPE_ARRAY == dbus_message_iter_get_arg_type(&iter)) {
        DBusMessageIter subIter;
        dbus_message_iter_recurse(&iter, &subIter);
std::cout << "enter: " << std::endl;
        while (dbus_message_iter_get_arg_type(&subIter) != DBUS_TYPE_INVALID) {
            DBusMessageIter dictEntry;
            dbus_message_iter_recurse(&subIter, &dictEntry);

            char* key;
            dbus_message_iter_get_basic(&dictEntry, &key);
            dbus_message_iter_next(&dictEntry);
            
            DBusMessageIter variant;
            dbus_message_iter_recurse(&dictEntry, &variant);

            //int valueType = dbus_message_iter_get_arg_type(&variant);
            //if (valueType == DBUS_TYPE_UINT32) {//DBUS_TYPE_STRING
                unsigned int value;
                dbus_message_iter_get_basic(&variant, &value);
                std::cout << "Key: " << key << ", Value: " << value << std::endl;
            //}
            // Handle other types as needed
            
            dbus_message_iter_next(&subIter);
        }
    }
    dbus_message_unref(reply);

    return 0;
}
