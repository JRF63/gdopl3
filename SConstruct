#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

opts = Variables([], ARGUMENTS)

opts.Add("target_name", "Name of the library to be built by SCons", "libgdopl3")
opts.Update(env)

if env["platform"] == "windows" and env["target"] == "template_release":
    env.Append(CCFLAGS=["/GL"])
    env.Append(LINKFLAGS=["/LTCG"])

env.Append(CPPPATH=["src/", "ymfmidi/src", "ymfmidi/ymfm/src"])
sources = Glob("src/*.cpp")
sources += Glob("ymfmidi/src/*.cpp", exclude=["ymfmidi/src/console.cpp", "ymfmidi/src/main.cpp"])
sources += Glob("ymfmidi/ymfm/src/*.cpp")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "project/bin/{}.{}.{}.framework/{}.{}.{}".format(
            env["target_name"], env["platform"], env["target"], env["target_name"], env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        "project/bin/{}{}{}".format(env["target_name"], env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
