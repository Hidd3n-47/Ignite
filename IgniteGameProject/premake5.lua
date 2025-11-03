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
        "$(SolutionDir)LogProject/",

        "$(SolutionDir)IgniteEngineProject/",
    }

    libdirs
    {
    }

    links
    {
        "IgniteEngine",
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
