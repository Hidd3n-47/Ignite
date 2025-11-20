-- ---------------------------- IgniteMem Project.
project "IgniteMem"
    location "%{prj.name}"
    kind "SharedLib"
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

        "$(SolutionDir)deps/include/",
        "$(SolutionDir)deps/imgui/",
    }

    libdirs
    {
        "$(SolutionDir)deps/Lib/",
    }

    links
    {
        "SDL3.lib",
        "imgui"
    }

    postbuildcommands
    {
        ("{COPYFILE} %[" .. outputPath .. "IgniteMem/IgniteMem.dll] %[" .. outputPath .."IgniteMemTests/]"),
        ("{COPYFILE} %[" .. outputPath .. "IgniteMem/IgniteMem.dll] %[" .. outputPath .."IgniteMemBenchmark/]"),
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        defines { "MEM_MANAGER", "DEV_CONFIGURATION" }
        symbols "on"
        
    filter "configurations:Dev_LiveStats"
        runtime "Debug"
        defines { "MEM_MANAGER", "DEV_CONFIGURATION", "DEV_LIVE_STATS" }
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        defines "MEM_MANAGER"
        optimize "on"
