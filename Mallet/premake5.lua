project "Mallet"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	pchheader "malletpch.h"
	pchsource "%{wks.location}/Mallet/src/malletpch.cpp"

	files
	{
		"%{wks.location}/Mallet/src/**.h",
		"%{wks.location}/Mallet/src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Mallet/src",
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
		defines "DEBUG"
		symbols "On"

	filter "configurations:Test"
		defines "DEBUG"
		symbols "On"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"

	filter { "system:windows", "configurations:Debug" }
		runtime "Debug"

	filter { "system:windows", "configurations:Test" }
		runtime "Release"

	filter { "system:windows", "configurations:Release" }
		runtime "Release"