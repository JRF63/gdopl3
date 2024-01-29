# GDOPL3

GDExtension for playing MIDI files with an OPL3 emulator.

## Building

Compilation is done purely through SCons.

```sh
git submodule update --init --recursive
scons target=template_debug && scons target=template_release
```