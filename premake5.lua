workspace "Magia"
    architecture "x86"
    startproject "Breakout"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    
    flags
	{
		"MultiProcessorCompile"
	}
    
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    project "Breakout"
        location "Breakout"
        kind "ConsoleApp"
        language "C++"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
        libdirs { "%{prj.name}/libs" }
        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.c",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/includes",
            "%{prj.name}/src"
        }

        links
        {
            "assimp.lib",
            "freetype.lib",
            "glfw3.lib",
            "irrKlang.lib"
        }

        filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"

        filter "configurations:Debug"
            defines "MAGIA_DEBUG"
            buildoptions "/MDd"
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines "MAGIA_RELEASE"
            runtime "Release"
            optimize "On"

        filter "configurations:Dist"
            defines "MAGIA_DIST"
            runtime "Release"
            optimize "On"