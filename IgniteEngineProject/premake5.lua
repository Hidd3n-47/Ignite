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
    }

    libdirs
    {
    }

    links
    {
        "Log",
        "IgniteMem",
        "IgniteUtils",
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
