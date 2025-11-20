-- ---------------------------- IgniteMemBenchmark project.
project "IgniteMemBenchmark"
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
        "$(SolutionDir)IgniteMemProject/",

        "$(SolutionDir)deps/imgui/",
        "$(SolutionDir)deps/Include/",
    }

    libdirs
    {
        "$(SolutionDir)deps/Lib/",
    }

    links
    {
       "IgniteMem",
       "imgui",

       "SDL3.lib"
    }

    postbuildcommands
    {
        ("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3.dll] %[" .. outputPath .."IgniteMemBenchmark/]")
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
