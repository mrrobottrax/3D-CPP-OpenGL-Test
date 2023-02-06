workspace "3D"
	architecture "x64"
	startproject "Game"

	configurations
	{
		"Debug",
		"Test",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Glad"]   = "%{wks.location}/3D/vendor/Glad/include"
IncludeDir["GLFW"]   = "%{wks.location}/3D/vendor/GLFW/include"
IncludeDir["IMGUI"]  = "%{wks.location}/3D/vendor/IMGUI"
IncludeDir["JSON"]   = "%{wks.location}/3D/vendor/JSON"
IncludeDir["GLM"]    = "%{wks.location}/3D/vendor/GLM"
IncludeDir["Engine"] = "%{wks.location}/3D/src"
IncludeDir["Game"]   = "%{wks.location}/Game/src"

include "3D"
include "Game"
include "Mallet"
include "3D/vendor/Glad"
include "3D/vendor/GLFW"
include "3D/vendor/IMGUI"