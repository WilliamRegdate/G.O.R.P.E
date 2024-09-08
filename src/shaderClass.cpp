#include "shaderClass.h"



std::string get_file_contents(const char *filename)
{
  std::FILE *fp = std::fopen(filename, "rb");
  if (fp)
  {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return(contents);
  }
  throw(errno);
}
bool checkProgramStatus(GLuint programID)
{
  GLint linkStatus;
  glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
  if(linkStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* buffer = new GLchar[infoLogLength];
    
    GLsizei bufferSize;
    glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
    std::cout << buffer << std::endl;
    delete [] buffer;
    return false;
  }
  return true;
}
bool checkcompileStatus(GLuint shaderID)
{
  GLint compileStatus;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
  if(compileStatus != GL_TRUE)
  {
    GLint infoLogLength;
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar* buffer = new GLchar[infoLogLength];
    
    GLsizei bufferSize;
    glGetShaderInfoLog(shaderID, infoLogLength, &bufferSize, buffer);
    std::cout << buffer << std::endl;
    delete [] buffer;
    return false;
  }
  return true;
}
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
  std::string vertexCode = get_file_contents(vertexFile);
  std::string fragmentCode = get_file_contents(fragmentFile);
  const char* vertexSource = vertexCode.c_str();
  const char* fragmentSource = fragmentCode.c_str();
  /*
  defines shaders and sets their source
  then compiles them into machine code
  */ 
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  std::cout <<"compile status: "<<checkcompileStatus(vertexShader)<< std::endl;

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  std::cout <<"frag shader compile status: "<< checkcompileStatus(fragmentShader)<< std::endl;

  //creates the shader program and attaches the shaders
  ID = glCreateProgram();
  glAttachShader(ID, vertexShader);
  glAttachShader(ID, fragmentShader);
  glLinkProgram(ID);
  //deletes individual shaders as they are not needed
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  std::cout <<"shader link status: "<<checkProgramStatus(ID)<< std::endl;

}
void Shader::Activate()
{
  glUseProgram(ID);
}

void Shader::Delete()
{
  glUseProgram(ID);
}
