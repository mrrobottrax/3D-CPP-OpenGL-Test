project "IMGUI"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir    ("obj/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
	}

	files
	{
		"imconfig.h",
		"imgui.cpp",
		"imgui.h",
		"imgui_demo.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_impl_glfw.cpp",
		"imgui_impl_glfw.h",
		"imgui_impl_opengl3.cpp",
		"imgui_impl_opengl3.h",
		"imgui_impl_opengl3_loader.h",
	}

	filter "system:windows"
        	staticruntime "On"
		systemversion "latest"

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
