-- ---------------------------- ConsoleLog Project.
project "ConsoleLog"
    location "%{prj.name}"
    kind "StaticLib"
    language "C++"
    staticruntime "Off"
    cppdialect "C++latest"
	clr "On"

    targetdir(outputPath .. "%{prj.name}")
    objdir(outputIntPath .. "%{prj.name}")

    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
    }
	
	links
	{
		"DebugConsole"
	}
	
	
    buildoptions ('/AI "' ..outputPath .. 'DebugConsole/"')
	
    postbuildcommands
    {
        --("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3.dll] %[" .. outputPath .."IgniteGame/]"),
        --("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3_ttf.dll] %[" .. outputPath .."IgniteGame/]"),
        --("{COPYFILE} %[$(SolutionDir)deps/Lib/SDL3_image.dll] %[" .. outputPath .."IgniteGame/]"),
        --("{COPYFILE} " .. outputPath .. "DebugConsole/DebugConsole.dll] %[" .. outputPath .."IgniteGame/]"),
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
