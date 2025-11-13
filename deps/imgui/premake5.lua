project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"
    cppdialect "C++17"
    
    targetdir(outputPath .. "%{prj.name}")
    objdir(outputIntPath .. "%{prj.name}")

    files
    {
        "imgui/*.h",
        "imgui/*.cpp",
    }

    includedirs
    {
        "$(SolutionDir)deps/include/"
    }

    removefiles
    {
        "%{prj.name}/backends/**.h",
        "%{prj.name}/backends/**.cpp"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Dev"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
