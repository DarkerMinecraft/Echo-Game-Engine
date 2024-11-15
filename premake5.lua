workspace "Echo"
architecture "x64"
systemversion "latest"
configurations { "Debug", "Release", "Dist" }

startproject "Echo Editor"

include "Dependencies.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "Echo/vendor/GLFW"
include "Echo/vendor/imgui"
group ""

include "Echo"
include "Echo Editor"