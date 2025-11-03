workspace "Ignite"
architecture "x64"

configurations
{
    "Dev",
    "Release"
}

outputName    = "%{cfg.buildcfg}"
outputPath    = "$(SolutionDir)Scratch/Bin/" .. outputName .. "/"
outputIntPath = "$(SolutionDir)Scratch/Int/" .. outputName .. "/"

include "IgniteEngineProject/"
include "IgniteGameProject/"

include "LogProject/"
