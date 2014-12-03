#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include "GL/glew.h"

class ResourceLoader
{
public:
    ResourceLoader();
    static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
};

#endif // RESOURCELOADER_H
