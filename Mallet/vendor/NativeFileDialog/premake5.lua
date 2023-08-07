project "NFD"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("obj/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"include"
	}

	links
	{
		"comctl32.lib"
	}

	files
	{
		"include/nfd.h",
		"common.h",
		"nfd_common.c",
		"nfd_common.h",
	}

	filter "system:windows"
        	staticruntime "On"
		systemversion "latest"

		files
		{
			"nfd_win.cpp",
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Test"
		runtime "Debug"
		symbols "on"
		optimize "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
