solution "GEA2"
	configurations { "Debug", "Release" }
	platforms { "x32" }
	
	location "Build"
	targetdir "Bin"
	debugdir "Bin"
	buildoptions { "-D_VARIADIC_MAX=10" }
	
	configuration "Debug"
		flags { "Symbols" }
		targetsuffix "d"
	configuration "Release"
		flags { "Optimize" }
	
	project "GEA2"
		kind "ConsoleApp"
		language "C++"
		files { "GEA2/**.h", "GEA2/**.cpp" }
		links { "opengl32", "AntTweakBar/lib/AntTweakBar", "glew/lib/glew32", "SDL/lib/x86/SDL2" }
		includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/" }
	project "ResourceManager"
		kind "StaticLib"
		language "C++"
		files { "ResourceManager/**.h", "ResourceManager/**.cpp" }
	project "ResourceManagerTests"
		kind "ConsoleApp"
		language "C++"
		files { "ResourceManagerTests/**.h", "ResourceManagerTests/**.cpp" }
		links { "gtest/lib/gtest", "gtest/lib/gtest_main", "ResourceManager" }
		includedirs { "gtest/include/", "ResourceManager/" }