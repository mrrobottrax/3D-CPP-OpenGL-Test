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

include "3D/vendor/Glad"

project "3D"
	location "3D"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/include",
		"%{prj.name}/vendor/src",
		"%{prj.name}/src",
		"%{IncludeDir.Glad}"
	}

	libdirs
	{
		"%{prj.name}/lib"
	}

	links
	{
		"opengl32.lib",
		"glew32s.lib",
		"glfw3.lib",
		"Glad"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{

		}

		postbuildcommands
		{

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
		buildoptions "/MDd"

	filter { "system:windows", "configurations:Test" }
		buildoptions "/MD"

	filter { "system:windows", "configurations:Release" }
		buildoptions "/MD"