project "Game"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{wks.location}/Game/src/pch.cpp"

	files
	{
		"%{wks.location}/Game/src/**.h",
		"%{wks.location}/Game/src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Game/src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.JSON}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.Engine}",
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
		"3D",
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