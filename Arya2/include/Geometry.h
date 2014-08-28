//See Models.h for explanation of Mesh,Material,Geometry system
#pragma once

#include <GL/glew.h>

namespace Arya
{
    //TODO: Make this class more general
    class Geometry
    {
        public:
            void addRef(){ refCount++; }
            void release(){ refCount--; }
            int getRefCount() const { return refCount; }

            bool isAnimated() const { return frameCount > 1; }

            void createVAOs(int frameCount);

            //GLuint getVAO() { return vaoHandle; }
            //GLuint getVertexBuffer() { return vertexBuffer; }
            //GLsizei getVertexCount() { return vertexCount; }
            //GLenum getPrimitiveType() { return primitiveType; }

            int frameCount; //1 for static models
            GLuint* vaoHandles; //a list of framecount handles
            GLuint vertexBuffer;
            GLsizei vertexCount; //PER FRAME
            GLuint indexBuffer;
            GLsizei indexCount;
            GLenum primitiveType;

        private:
            int refCount;

            //Only Model and ModelManger can create meshes
            friend class ModelManager;
            friend class Model;
            friend class Mesh;
            Geometry();
            ~Geometry();
    };
}
