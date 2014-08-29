#include "Geometry.h"

namespace Arya
{
    Geometry::Geometry()
    {
        frameCount = 0;
        vaoHandles = 0;
        vertexBuffer = 0;
        vertexCount = 0;
        indexBuffer = 0;
        indexCount = 0;
        primitiveType = 0;
        refCount = 0;
    }

    Geometry::~Geometry()
    {
        if(vaoHandles)
        {
            glDeleteVertexArrays(frameCount, vaoHandles);
            delete[] vaoHandles;
        }
        if(indexBuffer)
            glDeleteBuffers(1, &indexBuffer);
        if(vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);
    }

    void Geometry::createVAOs(int count)
    {
        //Delete old handles if they existed
        if(vaoHandles)
        {
            glDeleteVertexArrays(frameCount, vaoHandles);
            delete[] vaoHandles;
        }
        frameCount = count;
        vaoHandles = new GLuint[frameCount];

        glGenVertexArrays(frameCount, vaoHandles);
    }
}
