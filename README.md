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

# Note
 - This implementation is inspired from sway (see [server.c](https://github.com/swaywm/sway/blob/master/sway/server.c#L123)) and should work nearly the same. 
 - I am currently working on adding preferences to the plugin to let users configure a global blacklist and a whitelist per application. After I’m done, I will submit a PR.
 - The file `wlr_security_context_v1.c` has been added to the project as a workaround, and just only because Wayfire do not currently support this protocol, otherwise it would not be needed.
  - The plugin should be compatible with any version of Wayfire

# Contact
arm-cade@proton.me