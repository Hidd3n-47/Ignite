-- ---------------------------- DebugConsole Project.
project "DebugConsole"
    location "%{prj.name}"
    kind "SharedLib"
    language "C#"
    dotnetframework "4.8"
	clr "On"

    targetdir(outputPath .. "%{prj.name}")
    objdir(outputIntPath .. "%{prj.name}")

    files
    {
        "%{prj.name}/**.cs",
        "%{prj.name}/**.xaml",
    }
	
	links
	{
        "System",
        "System.Core",
        "System.Data",
        "System.Xml",
        "System.Net.Http",
        "PresentationCore",
        "PresentationFramework",
        "WindowsBase",
        "System.Xaml"
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
