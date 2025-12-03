-- ---------------------------- IgniteSampler project.
project "IgniteSampler"
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
    }

    libdirs
    {
        "$(SolutionDir)deps/Lib/",
    }

    links
    {
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        defines "DEV_CONFIGURATION"
        symbols "on"

    filter "configurations:Dev_LiveStats"
        runtime "Debug"
        defines "DEV_CONFIGURATION"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
