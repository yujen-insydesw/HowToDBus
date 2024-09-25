/*

busctl call --system com.example.HelloService /com/example/HelloService com.example.HelloInterface Hello s world

*/

#include <iostream>
#include <string.h>
#include <unistd.h>

#include <dbus/dbus.h>

int main()
{
    DBusError dbus_error;
    DBusConnection * dbus_conn = nullptr;
    DBusMessage * dbus_msg = nullptr;
    DBusMessage * dbus_reply = nullptr;
    const char * dbus_result = nullptr;

    const char* name = "Alice";

    // Initialize D-Bus error
    ::dbus_error_init(&dbus_error);

    // Connect to D-Bus
    if ( nullptr == (dbus_conn = ::dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error)) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
    }
    // Compose remote procedure call
    else if ( nullptr == (dbus_msg = ::dbus_message_new_method_call(
                                "com.example.HelloService",
                                "/org/example/HelloService",
                                "com.example.HelloInterface",
                                "Hello"
                            )
                        )
            ) {
        ::perror("ERROR: ::dbus_message_new_method_call - Unable to allocate memory for the message!");
        goto UNREF_CONNECTION;
    }
    // Append argument
    else if ( ! ::dbus_message_append_args(dbus_msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
        goto UNREF_REPLY;
    }
    // Invoke remote procedure call, block for response
    else if ( nullptr == (dbus_reply = ::dbus_connection_send_with_reply_and_block(dbus_conn, dbus_msg, DBUS_TIMEOUT_USE_DEFAULT, &dbus_error)) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
        goto UNREF_MESSAGE;
    }
    // Parse response
    else if ( ! ::dbus_message_get_args(dbus_reply, &dbus_error, DBUS_TYPE_STRING, &dbus_result, DBUS_TYPE_INVALID) ) {
        ::perror(dbus_error.name);
        ::perror(dbus_error.message);
        goto UNREF_REPLY;
    }
    // Work with the results of the remote procedure call
    else {
        std::cout << "Connected to D-Bus as \"" << ::dbus_bus_get_unique_name(dbus_conn) << "\"." << std::endl;
        std::cout << "Introspection Result:" << std::endl << std::endl;
        std::cout << dbus_result << std::endl;
        goto UNREF_REPLY;
    }

/*
* Release message
*/
UNREF_REPLY:
    ::dbus_message_unref(dbus_reply);
UNREF_MESSAGE:
    ::dbus_message_unref(dbus_msg);

/*
* Applications must not close shared connections -
* see dbus_connection_close() docs. This is a bug in the application.
*/
//CLOSE_CONNECTION:
//    ::dbus_connection_close(dbus_conn);

/*
* When using the System Bus, unreference
* the connection instead of closing it
*/
UNREF_CONNECTION:
    ::dbus_connection_unref(dbus_conn);

    return 0;
}