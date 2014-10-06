#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

ShaderProgram::ShaderProgram(const char* shaderProgramPath)
{
	CreateProgram(shaderProgramPath);
}

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::~ShaderProgram()
{
	if( programHandle != 0 )
		glDeleteShader( programHandle);
}

GLuint ShaderProgram::GetHandle()
{
	return programHandle;
}

bool ShaderProgram::CreateProgram(const char* shaderProgramPath)
{
	//Create file stream and open
	std::string tPath = shaderProgramPath;
	std::string folderPath = tPath.substr(0, tPath.find_last_of("/") + 1);
	tPath.append(".sp");
	std::ifstream shaderProgramFile(tPath);
	if(!shaderProgramFile.is_open())
	{
		std::cout << "Failed to to open shader program file!" << "[" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		return false;
	}

	//Read .sp file to find out what shaders should be created. Create them and store all shader handles in vector for later attachment and linking
	std::string shaderProgSource;
	std::vector<GLuint> shaderHandles;
	while (getline(shaderProgramFile, shaderProgSource))
	{
		std::string prefix;
		std::stringstream stream;

		stream << shaderProgSource;
		stream >> prefix;

		if(prefix == "VERTEX")
		{
			std::string shaderName;
			std::string shaderPath = folderPath;;
			stream >> shaderName;
			shaderName.append(".vert");
			shaderPath.append(shaderName);

			GLuint shader = CreateShader(GL_VERTEX_SHADER, shaderPath.c_str());
			if(shader != GL_FALSE)
				shaderHandles.push_back(shader);
		}
		else if(prefix == "FRAGMENT")
		{
			std::string shaderName;
			std::string shaderPath = folderPath;
			stream >> shaderName;
			shaderName.append(".frag");
			shaderPath.append(shaderName);

			GLuint shader = CreateShader(GL_FRAGMENT_SHADER, shaderPath.c_str());
			if(shader != GL_FALSE)
				shaderHandles.push_back(shader);
		}
	}

	//Shaders are created and now it's time to create program and attach!
	programHandle = glCreateProgram();
	if(!programHandle)
	{
		std::cout << "Failed to create program handle!" << "[" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		return false;
	}

	//Go through all compiled shaders and attach them to program
	for(GLuint handle : shaderHandles)
	{
		glAttachShader( programHandle, handle);
	}

	glUseProgram( 0 );

	//Link program
	glLinkProgram(programHandle);

	//Verify link status
	GLint status;
	glGetProgramiv( programHandle, GL_LINK_STATUS, &status );

	if( status != GL_TRUE )
	{
		std::cout << "Failed to link shader program!" << "[" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		int length = 1024;
		if( length > 0 )
		{
			//Create a log of error messages
			char* errorLog = new char[ length ];
			int written = 0;
			glGetProgramInfoLog(programHandle, length, &written, errorLog );
			std::cout << errorLog << "[" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
			delete[] errorLog;
		}
		return false;
	}

	//All done, return good times
	return true;
}

GLuint ShaderProgram::CreateShader(GLenum shaderType, const char* shaderPath)
{
	//Create shader and check for errors
	GLuint shaderHandle = glCreateShader (shaderType);
	if(!shaderHandle)
	{
		std::cout << "Failed to create shader: " << shaderPath <<" [" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		return GL_FALSE;
	}
	//Create stream, check errors
	std::ifstream shaderFile(shaderPath);
	if(!shaderFile.is_open())
	{
		std::cout << "Failed to open shader file: " << shaderPath <<" [" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		return GL_FALSE;
	}

	//Get the size of the file
	shaderFile.seekg(0,std::ios::end);
	std::streampos length = shaderFile.tellg();
	shaderFile.seekg(0,std::ios::beg);

	//Resize buffer and read shader source
	std::string buffer;
	buffer.resize((unsigned int)length);
	shaderFile.read(&buffer[0], length);

	//Load source into char array
	const char* source = buffer.c_str();

	//Set shader source and compile
	glShaderSource (shaderHandle, 1, &source, NULL);
	glCompileShader (shaderHandle);

	//Compilation error checking
	int result = 0;
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &result );
	if( result != GL_TRUE )
	{
		std::cout << "Failed to compile shader: " << shaderPath <<" [" <<__FUNCTION__ << ", line " << __LINE__ << "]" << std::endl;
		int length = 0;
		glGetShaderiv( shaderHandle, GL_INFO_LOG_LENGTH, &length );
		if( length > 0 )
		{
			//Create a log of error messages
			char* errorLog = new char[ length ];
			int written = 0;
			glGetShaderInfoLog( shaderHandle, length, &written, errorLog );
			std::cout << errorLog << std::endl;
			delete[] errorLog;
		}

		return result;
	}

	return shaderHandle;
}

void ShaderProgram::Use()
{
	glUseProgram(programHandle);
}

