#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include "GL/glew.h"

class ShaderLoader
{
public:
    ShaderLoader();
    static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
};

#endif // SHADERLOADER_H
