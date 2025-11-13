-- ---------------------------- IgniteMemTests project.
project "IgniteMemTests"
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
        "$(SolutionDir)Tests/%{prj.name}/%{prj.name}/",
        "$(SolutionDir)Tests/TestProject/",

        "$(SolutionDir)IgniteMemProject/",

        "$(SolutionDir)deps/Include/",
    }

    links
    {
       "TestProject",
       "IgniteMem",
    }

    postbuildcommands
    {
        ("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3.dll] %[" .. outputPath .."IgniteMemTests/]"),
        ("{COPYFILE} %[" .. outputPath .. "IgniteMem/IgniteMem.dll] %[" .. outputPath .."IgniteMemTests/]"),
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        defines "DEV_CONFIGURATION"
        symbols "on"

    filter "configurations:Dev_LiveStats"
        runtime "Debug"
        defines { "DEV_CONFIGURATION", "DEV_LIVE_STATS" }
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
