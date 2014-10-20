solution "GEA2"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	
	location "Build/"
	debugdir "Bin/"
	buildoptions { "-D_VARIADIC_MAX=10" }
	
	configuration { "x32", "Debug" }
		flags { "Symbols" }
		targetsuffix "d"
		targetdir "Bin/x86/"
	configuration { "x32", "Release" }
		flags { "Optimize", "Symbols" }
		targetdir "Bin/x86/"
	configuration { "x64", "Debug" }
		defines { "SIXTYFOUR_BIT" }
		flags { "Symbols" }
		targetsuffix "d"
		targetdir "Bin/x64/"
	configuration { "x64", "Release" }
		defines { "SIXTYFOUR_BIT" }
		flags { "Optimize", "Symbols" }
		targetdir "Bin/x64/"
	configuration {}
	
	includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/", "gtest/include/", "zlibwapi/include/", "ResourceManager/" }
	
	project "GEA2"
		kind "ConsoleApp"
		language "C++"
		files { "GEA2/**.h", "GEA2/**.cpp" }
		
		configuration { "x32" }
			links { "opengl32", "AntTweakBar/lib/AntTweakBar", "glew/lib/x86/glew32", "SDL/lib/x86/SDL2", "SDL/lib/x86/SDL2_image", "zlibwapi/lib/x86/zlibwapi", "ResourceManager" }
		configuration { "x64" }
			links { "opengl32", "glew/lib/x64/glew32", "SDL/lib/x64/SDL2", "SDL/lib/x64/SDL2_image", "zlibwapi/lib/x64/zlibwapi", "ResourceManager" }
		configuration { }
		
		--includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/", "gtest/include/", "zlibwapi/include/", "ResourceManager/" }
	project "ResourceManager"
		kind "StaticLib"
		language "C++"
		files { "ResourceManager/**.h", "ResourceManager/**.cpp" }
		links { "zlibwapi/lib/zlibwapi" }
		
		--includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/", "gtest/include/", "zlibwapi/include/", "ResourceManager/" }
	project "ResourceManagerTests"
		kind "ConsoleApp"
		language "C++"
		files { "ResourceManagerTests/**.h", "ResourceManagerTests/**.cpp" }
		
		configuration { "x32" }
			links { "gtest/lib/x86/gtest", "ResourceManager", "zlibwapi/lib/x86/zlibwapi", "SDL/lib/x86/SDL2", "SDL/lib/x86/SDL2_image", "opengl32", "glew/lib/x86/glew32" }
		configuration { "x64" }
			links { "gtest/lib/x64/gtest", "ResourceManager", "zlibwapi/lib/x64/zlibwapi", "SDL/lib/x64/SDL2", "SDL/lib/x64/SDL2_image", "opengl32", "glew/lib/x64/glew32" }
		configuration { }
		
		--includedirs { "AntTweakBar/include/", "glew/include/GL/", "glm/glm/", "SDL/include/", "gtest/include/", "zlibwapi/include/", "ResourceManager/" }
	project "WorldGenerator"
		kind "ConsoleApp"
		language "C++"
		files { "WorldGenerator/**.h", "WorldGenerator/**.cpp" }
		links { }
		
		--includedirs {"glm/glm/"}