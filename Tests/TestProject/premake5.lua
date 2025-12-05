-- ---------------------------- TestProject project.
project "TestProject"
    location "%{prj.name}"
    kind "StaticLib"
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
        "$(SolutionDir)Tests/%{prj.name}/",

        "$(SolutionDir)LogProject/",
    }

    links
    {
        "Logger"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        symbols "on"
        runtime "Debug"

    filter "configurations:Dev_LiveStats"
        symbols "on"
        runtime "Debug"

    filter "configurations:Release"
        optimize "on"
        runtime "Release"