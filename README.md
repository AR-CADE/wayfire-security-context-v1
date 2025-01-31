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

or for example:
blacklist_0 = zwlr_screencopy_manager_v1
blacklist_1 = zwlr_layer_shell_v1

blacklist_[A_UNIQUE_IDENTIFIER] = [THE_PROTOCOL_INTERFACE_NAME]
```

to set an application to manage:
```
app_id_waycheck = dev.serebit.Waycheck
sandbox_engine_waycheck = org.flatpak (optional)
app_id_mpv = io.mpv.Mpv
sandbox_engine_mpv = org.flatpak (optional)

or for example:
app_id_0 = dev.serebit.Waycheck
sandbox_engine_0 = org.flatpak (optional)
app_id_1 = io.mpv.Mpv
sandbox_engine_1 = org.flatpak (optional)

app_id_[A_UNIQUE_IDENTIFIER] = [THE_APPLICATION_ID]
sandbox_engine_[A_UNIQUE_IDENTIFIER] = [THE_SANDBOX_ENGINE] (optional)
```

to set a deny / allow list of protocols for a specific application:
```
deny_waycheck_screencopy = zwlr_screencopy_manager_v1
deny_waycheck_security_context = wp_security_context_manager_v1
deny_mpv_dmabuf = zwlr_export_dmabuf_manager_v1

or for example:
deny_0_0 = zwlr_screencopy_manager_v1
deny_0_1 = wp_security_context_manager_v1
deny_1_0 = zwlr_export_dmabuf_manager_v1

deny_[A_UNIQUE_IDENTIFIER]_[THE_ITERATION_UNIQUE_IDENTIFIER] = [THE_PROTOCOL_INTERFACE_NAME]

allow_waycheck_screencopy = zwlr_screencopy_manager_v1
allow_waycheck_security_context = wp_security_context_manager_v1
allow_mpv_dmabuf = zwlr_export_dmabuf_manager_v1

or for example:
allow_0_0 = zwlr_screencopy_manager_v1
allow_0_1 = wp_security_context_manager_v1
allow_1_0 = zwlr_export_dmabuf_manager_v1

allow_[A_UNIQUE_IDENTIFIER]_[THE_ITERATION_UNIQUE_IDENTIFIER] = [THE_PROTOCOL_INTERFACE_NAME]
 
```


# Note
- This implementation is inspired from sway (see [server.c](https://github.com/swaywm/sway/blob/master/sway/server.c#L123)) and should work nearly the same. 
- I am currently working on adding preferences to the plugin to let users configure a global blacklist and a whitelist per application. After I’m done, I will submit a PR.
- The file `wlr_security_context_v1.c` has been added to the project as a workaround, and just only because Wayfire do not currently support this protocol, otherwise it would not be needed.
- The plugin should be compatible with any version of Wayfire

# Contact
arm-cade@proton.me
