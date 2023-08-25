project "3D"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{wks.location}/3D/src/pch.cpp"

	files
	{
		"%{wks.location}/3D/src/**.h",
		"%{wks.location}/3D/src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/3D/vendor/src",
		"%{wks.location}/3D/src",
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
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Test"
		defines "DEBUG"
		runtime "Release"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"