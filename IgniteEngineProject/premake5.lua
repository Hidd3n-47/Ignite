-- ---------------------------- IgniteEngine Project.
project "IgniteEngine"
    location "%{prj.name}"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"
    cppdialect "C++latest"

    targetdir(outputPath .. "%{prj.name}")
    objdir(outputIntPath .. "%{prj.name}")

    pchheader "IgnitePch.h"
    pchsource "%{prj.name}/Src/IgnitePch.cpp"

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
    }

    includedirs
    {
        "$(SolutionDir)IgniteEngineProject/",

        "$(SolutionDir)LogProject/",

        "$(SolutionDir)IgniteEngineProject/IgniteEngine/",
        "$(SolutionDir)IgniteEngineProject/IgniteEngine/Src/",

        "$(SolutionDir)IgniteMemProject/",
        "$(SolutionDir)IgniteUtilsProject/",

        "$(SolutionDir)deps/Include/",
    }

    libdirs
    {
        "$(SolutionDir)deps/Lib/",
    }

    links
    {
        "Log",
        "IgniteMem",
        "IgniteUtils",

        "SDL3.lib",
        "SDL3_ttf.lib",
        "SDL3_image.lib",
    }

    postbuildcommands
    {
        ("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3.dll] %[" .. outputPath .."IgniteGame/]"),
        ("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3_ttf.dll] %[" .. outputPath .."IgniteGame/]"),
        ("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3_image.dll] %[" .. outputPath .."IgniteGame/]"),
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        defines { "ENGINE_DEBUG", "DEV_CONFIGURATION" }
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
