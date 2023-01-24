workspace "3D"
	architecture "x64"

	configurations
	{
		"Debug",
		"Test",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["Glad"] = "3D/vendor/Glad/include"
IncludeDir["GLFW"] = "3D/vendor/GLFW/include"
IncludeDir["IMGUI"] = "3D/vendor/IMGUI"
IncludeDir["JSON"] = "3D/vendor/JSON"
IncludeDir["GLM"] = "3D/vendor/GLM"

include "3D/vendor/Glad"
include "3D/vendor/GLFW"
include "3D/vendor/IMGUI"

project "3D"
	location "3D"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{prj.name}/src/engine/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/src",
		"%{prj.name}/src/engine",
		"%{prj.name}/src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.JSON}",
		"%{IncludeDir.GLM}",
	}

	libdirs
	{
		
	}

	links
	{
		"opengl32.lib",
		"Glad",
		"GLFW",
		"IMGUI",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"_GLFW_WIN32"
		}

	filter "configurations:Debug"
		defines ""
		symbols "On"

	filter "configurations:Test"
		defines ""
		optimize "On"

	filter "configurations:Release"
		defines ""
		optimize "On"

	filter { "system:windows", "configurations:Debug" }
		runtime "Debug"

	filter { "system:windows", "configurations:Test" }
		runtime "Release"

	filter { "system:windows", "configurations:Release" }
		runtime "Release"