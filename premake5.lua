workspace "Ignite"
architecture "x64"

configurations
{
    "Dev",
    "Dev_LiveStats",
    "Release"
}

outputName    = "%{cfg.buildcfg}"
outputPath    = "$(SolutionDir)Scratch/Bin/" .. outputName .. "/"
outputIntPath = "$(SolutionDir)Scratch/Int/" .. outputName .. "/"

include "IgniteEngineProject/"
include "IgniteGameProject/"

include "IgniteMemProject/"
include "IgniteUtilsProject/"

include "LogProject/"

include "deps/imgui/"

-- Tests
include "Tests/TestProject/"
include "Tests/IgniteMemTests/"

-- Benchmark
include "IgniteMemBenchmark"