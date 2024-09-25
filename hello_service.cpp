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
#include <string.h>
#include <unistd.h>  // Include this header for usleep

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
        //dbus_connection_read_write(connection, 1000);// return true/false; can set timeout (ms)
        // Blocking way
        // useful to look at spec: https://dbus.freedesktop.org/doc/api/html/group__DBusConnection.html#ga580d8766c23fe5f49418bc7d87b67dc6
        dbus_connection_read_write_dispatch(connection, -1);
        std::cout << "read write:" << std::endl;
        
        DBusMessage* message = dbus_connection_pop_message(connection);
        std::cout << "pop message:" << std::endl;
        if (message) {
            if (dbus_message_is_method_call(message,
                    "com.example.HelloInterface",
                    "Hello")
                ) {
                // Get input
                const char* inputName;
                dbus_message_get_args(message, nullptr, DBUS_TYPE_STRING, &inputName, DBUS_TYPE_INVALID);
                
                // Service
                std::string greeting = Hello(inputName);
                
                // Create a reply message
                DBusMessage* reply = dbus_message_new_method_return(message);
                dbus_message_append_args(reply, DBUS_TYPE_STRING, &greeting, DBUS_TYPE_INVALID);
                
                // Send the reply
                dbus_connection_send(connection, reply, nullptr);
                dbus_message_unref(reply);
            }
            dbus_message_unref(message);
        }
        
        // Sleep briefly to avoid busy waiting
        //usleep(100000); // 100 ms
    }

    dbus_connection_unref(connection);
    return 0;
}