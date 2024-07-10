''' custom script for platformio '''

from os.path import join
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

print ("Custom extra_script launching...")
#
#1. warning 126 : unreachable code.
#2. warning 59 : conditional flow changed by optimizer.
#3. linker flag : --xram-loc
env.Append(
    CFLAGS = [
        "--disable-warning", 154,
        "--disable-warning", 126,
        "--disable-warning", 59
        ],
    LINKFLAGS = [
        "--xram-size", 0x0300,
        "--xram-loc", 0x0100
        ]
    )
print("LINKFLAGS:"+env["LINKFLAGS"])
