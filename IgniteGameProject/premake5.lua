-- ---------------------------- IgniteGame Project.
project "IgniteGame"
    location "%{prj.name}"
    kind "ConsoleApp"
    language "C++"
    staticruntime "Off"
    cppdialect "C++latest"

    targetdir(outputPath .. "%{prj.name}")
    objdir(outputIntPath .. "%{prj.name}")

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
    }

    includedirs
    {
        "$(SolutionDir)ConsoleLogProject/",

        "$(SolutionDir)IgniteEngineProject/",
        "$(SolutionDir)IgniteGameProject/IgniteGame/",

        "$(SolutionDir)IgniteMemProject/",
        "$(SolutionDir)IgniteUtilsProject/",
    }

    libdirs
    {
    }

    links
    {
        "IgniteEngine",

        "IgniteMem",
        "IgniteUtils",
    }

    postbuildcommands
    {
        "call \"$(SolutionDir)copyDeps.bat\""
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        defines "DEV_CONFIGURATION"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
