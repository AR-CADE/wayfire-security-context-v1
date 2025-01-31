# wayfire-security-context-v1
A plugin for wayfire that implement the wlr-security-context-v1 protocol.


## Building & install

```
$ git clone https://github.com/AR-CADE/wayfire-security-context-v1.git
$ cd wayfire-security-context-v1
$ meson setup build --prefix=/usr --buildtype=release
$ ninja -C build 
$ sudo ninja -C build install
```

# Basic usage

in ~/.config/wayfire.ini:

- Add `security-context-v1` to the plugins list
- Add a `[security-context-v1]` section

in the [security-context-v1] section:

to blacklist a protocol for all applications:
```
blacklist_screencopy = zwlr_screencopy_manager_v1
blacklist_layer_shell = zwlr_layer_shell_v1
blacklist_xxx = [THE_PROTOCOL_INTERFACE_NAME]
```

to set an application to manage:
```
app_id_waycheck = dev.serebit.Waycheck
sandbox_engine_waycheck = org.flatpak

app_id_xxx = [THE_APPLICATION_ID]
sandbox_engine_xxx = [THE_SANDBOX_ENGINE]
```

to set a deny / allow list of protocols for a specific application:
```
deny_waycheck_screencopy = zwlr_screencopy_manager_v1
deny_waycheck_security_context = wp_security_context_manager_v1

deny_xxx_yyy = [THE_PROTOCOL_INTERFACE_NAME]


allow_waycheck_screencopy = zwlr_screencopy_manager_v1
allow_waycheck_security_context = wp_security_context_manager_v1

allox_xxx_yyy = [THE_PROTOCOL_INTERFACE_NAME]
 
```


# Note
- This implementation is inspired from sway (see [server.c](https://github.com/swaywm/sway/blob/master/sway/server.c#L123)) and should work nearly the same. 
- I am currently working on adding preferences to the plugin to let users configure a global blacklist and a whitelist per application. After I’m done, I will submit a PR.
- The file `wlr_security_context_v1.c` has been added to the project as a workaround, and just only because Wayfire do not currently support this protocol, otherwise it would not be needed.
- The plugin should be compatible with any version of Wayfire

# Contact
arm-cade@proton.me