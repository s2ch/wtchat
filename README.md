# WTF
Anonymous webchat written in C++/Wt. Can work without JS support at all. Current address:
http://s2chperdovidos2g.onion (tor2web addres is, for example, https://s2chperdovidos2g.onion.sh
but better use a dedicated tor browser for true anonymity).

# Why C++?
Because we can! Also the server memory requirements are pretty low this way (about 20 Mb) and it's quite fast. (and I hate JS)

# Compiling
The build scripts expect Wt4 installed to `/opt/build/wt4`, if that's not the case please change the paths.

For a standalone deployment you might want to follow a "single directory" approach to not pollute the target system with libraries.
Create a `lib` directory in the app root (where `build_release.sh` is located) and put the dependencies there. You can check them
with `ldd build/release/wtchat`, it's usually `libwt.so`, `libwthttp.so`, maybe `libstdc++` (if the one on your server is too old)
and a bunch of boost libraries. Copy them from your system to `lib`, install `meson` and `patchelf` then do `./build_release.sh`.
The libraries will be patched to be able to load well from that directory.

Now put `build/release/wtchat`, `lib`, `templates` to the same directory on your server, create a `docroot`
directory there and put `css` and `/opt/build/wt4/share/Wt/resources` to `docroot`.

# Running
As easy as `./wtchat --docroot docroot --http-address 127.0.0.1 --http-port 8080`. Your server should be available at http://127.0.0.1:8080
