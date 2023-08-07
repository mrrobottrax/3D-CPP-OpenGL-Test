workspace "3D"
	startproject "Game"

	configurations
	{
		"Debug",
		"Test",
		"Release",
	}

	platforms
	{
		"Win64",
		"Win32",
	}

	filter "platforms:Win32"
		system "Windows"
		architecture "x86"

	filter "platforms:Win64"
		system "Windows"
		architecture "x86_64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Glad"]   = "%{wks.location}/3D/vendor/Glad/include"
IncludeDir["GLFW"]   = "%{wks.location}/3D/vendor/GLFW/include"
IncludeDir["IMGUI"]  = "%{wks.location}/3D/vendor/IMGUI"
IncludeDir["JSON"]   = "%{wks.location}/3D/vendor/JSON"
IncludeDir["GLM"]    = "%{wks.location}/3D/vendor/GLM"
IncludeDir["Engine"] = "%{wks.location}/3D/src"
IncludeDir["Game"]   = "%{wks.location}/Game/src"
IncludeDir["NFD"]    = "%{wks.location}/Mallet/vendor/NativeFileDialog/include"

include "3D"
include "Game"
include "Mallet"
include "3D/vendor/Glad"
include "3D/vendor/GLFW"
include "3D/vendor/IMGUI"
include "Mallet/vendor/NativeFileDialog"