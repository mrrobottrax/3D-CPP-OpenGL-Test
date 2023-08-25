project "Glad"
    kind "StaticLib"
    language "C"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir    ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"
        staticruntime "On"

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