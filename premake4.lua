solution "GEA2"
	configurations { "Debug", "Release" }
	platforms { "x32" }
	location "build"
	
	project "GEA2"
		kind "ConsoleApp"
		language "C++"
		files { "GEA2/**.h", "GEA2/**.cpp" }
		links { "opengl32", "AntTweakBar/lib/AntTweakBar", "glew/lib/glew32", "SDL/lib/x86/SDL2" }
		includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/" }
	project "ResourceManager"
		kind "ConsoleApp"
		language "C++"
		files { "ResourceManager/**.h", "ResourceManager/**.cpp" }