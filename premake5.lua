workspace "Echo"
architecture "x64"
systemversion "latest"
configurations { "Debug", "Release", "Dist" }

startproject "Minecraft Clone"

include "Dependencies.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "Echo/vendor/GLFW"
include "Echo/vendor/imgui"
include "Echo/vendor/vk-bootstrap"
group ""

include "Echo"
include "Echo Editor"
include "Minecraft Clone"