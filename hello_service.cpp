/*

Configuration Location:
/etc/dbus-1/system.d/ for system services
/etc/dbus-1/session.d/ for session services
Configuration File:
sudo vim org.freedesktop.PackageKit.conf




com.example.HelloService.conf

<!DOCTYPE busconfig PUBLIC "-//freedesktop//DTD D-Bus Bus Configuration 1.0//EN"
    "http://www.freedesktop.org/standards/dbus/1.0/busconfig.dtd">
<busconfig>
    <!-- Allow any user to talk to our service -->
    <policy user="*">
        <allow own="com.example.HelloService"/>
        <allow send_destination="com.example.HelloService"/>
        <allow receive_sender="com.example.HelloService"/>
    </policy>
</busconfig>


*/

#include <iostream>
#include <cstddef>
#include <cstdio>
#include <string.h>
#include <unistd.h>  // Include this header for usleep

//#include <thread>
#include <future>     // async

#include <dbus/dbus.h>

// Define the method implementation
std::string Hello(const std::string& name) {
    return "Hello, " + name + "!";
}

int main() {
    DBusError error;
    dbus_error_init(&error);

    // Connect to the session bus
    DBusConnection* connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Connection Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return 1;
    }

    // Request a name on the bus
    int ret = dbus_bus_request_name(connection, "com.example.HelloService", DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Name Error: " << error.message << std::endl;
        dbus_error_free(&error);
        return 1;
    }
    
    // Main loop
    while (true) {

        // Handle incoming messages
        // Sleep briefly to avoid busy waiting usleep(1000);
        //dbus_connection_read_write(connection, 1000);// return true/false; can set timeout (ms)

        // Blocking way
        // useful to look at spec: https://dbus.freedesktop.org/doc/api/html/group__DBusConnection.html#ga580d8766c23fe5f49418bc7d87b67dc6
        dbus_connection_read_write_dispatch(connection, -1);
        

        // Perform the task asynchronously
        std::async(std::launch::async, 
            [] () {
                //
                std::cerr << "async: " << std::endl;
            });

        // avoid crosses initialization (related with goto)
        DBusMessage* message = nullptr;
        const char* inputName = nullptr;
        std::string greeting;
        DBusMessage* reply = nullptr;


        //

        if ( nullptr == (message = dbus_connection_pop_message(connection)) ) {
//            ::perror(dbus_error.name);
//            ::perror(dbus_error.message);
            goto UNREF_MESSAGE;
        }

        //

        if ( false == dbus_message_is_method_call(message, "com.example.HelloInterface", "Hello") ) {
            //continue;
            goto UNREF_MESSAGE;
        }


        // Get input
        dbus_message_get_args(message, nullptr, DBUS_TYPE_STRING, &inputName, DBUS_TYPE_INVALID);
        
        // Service
        greeting = Hello(inputName);
        
        // Create a reply message
        reply = dbus_message_new_method_return(message);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &greeting, DBUS_TYPE_INVALID);
        
        // Send the reply
        dbus_connection_send(connection, reply, nullptr);

UNREF_REPLY:
        dbus_message_unref(reply);

UNREF_MESSAGE:
        dbus_message_unref(message);
        
    }

/*
* Applications must not close shared connections -
* see dbus_connection_close() docs. This is a bug in the application.
*/
//CLOSE_CONNECTION:
//    ::dbus_connection_close(connection);

/*
* When using the System Bus, unreference
* the connection instead of closing it
*/
UNREF_CONNECTION:
    dbus_connection_unref(connection);
    return 0;
}