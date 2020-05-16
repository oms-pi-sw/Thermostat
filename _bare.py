Import("env", "projenv")

from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

env.Append(
    ASFLAGS=["-x", "assembler-with-cpp"],

    CCFLAGS=[
        "-Os",  # optimize for size
        "-ffunction-sections",  # place each function in its own section
        "-fdata-sections",
        "-Wall",
        "-mthumb",
        "-nostdlib"
    ],

    CXXFLAGS=[
        "-fno-rtti",
        "-fno-exceptions"
    ],

    CPPDEFINES=[
        ("F_CPU", "$BOARD_F_CPU")
    ],

    LINKFLAGS=[
        "-Os",
        "-Wl,--gc-sections,--relax",
        "-mthumb",
        "-nostartfiles",
        "-nostdlib"
    ],

    LIBS=["c", "gcc", "m", "stdc++", "nosys"]
)

if "BOARD" in env:
    env.Append(
        CCFLAGS=[
            "-mcpu=%s" % env.BoardConfig().get("build.cpu")
        ],
        CPPDEFINES=[
            env.BoardConfig().get("build.variant", "").upper()
        ],
        LINKFLAGS=[
            "-mcpu=%s" % env.BoardConfig().get("build.cpu")
        ]
    )

# copy CCFLAGS to ASFLAGS (-x assembler-with-cpp mode)
env.Append(ASFLAGS=env.get("CCFLAGS", [])[:]) 

for e in [ env, projenv ]:
    #
    # Fix options after "_bare.py"
    #
    e.Replace(LINKFLAGS = [i for i in e['LINKFLAGS'] if i not in [ '-nostartfiles', '-nostdlib' ]])
    e.Append(LINKFLAGS = [ "--specs=nano.specs", "--specs=nosys.specs" ])
    e.Replace(AS = '$CC', ASCOM = '$ASPPCOM')

    #
    # .ld script should be passed to linker directly as "-T<path>"
    # instead of "-Wl,-T<path>", to avoid 1K RAM loss for ".data.impure_data"
    #
    e.Replace(BUILD_FLAGS = [i for i in e['BUILD_FLAGS'] if not i.startswith('-Wl,-T')])
    e.Replace(LINKFLAGS = [(i[4:] if i.startswith('-Wl,-T') else i) for i in e['LINKFLAGS']])