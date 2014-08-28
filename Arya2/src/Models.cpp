#include "Models.h"
#include "Files.h"
#include "Geometry.h"
#include "Locator.h"
#include "Materials.h"
#include "AnimationVertex.h"
#include "common/Logger.h"
#include <map>
#include <algorithm>

using std::map;
using std::make_pair;

//=============================================================================
// Part of the Arya model file format

typedef struct{
    int materialIndex;
    int primitiveType;
    int vertexCount; //per frame
    int hasNormals;
    int indexCount;
    int bufferOffset;
    int indexbufferOffset;
} SubmeshInfo;

typedef struct{
    int magic;
    int modeltype; //1 static, 2 vertex animated, 3 bone animated
    int submeshCount;
    int materialCount;
    int frameCount; //1 for static meshes
    SubmeshInfo submesh[0];
} AryaHeader;

#define ARYAMAGICINT (('A' << 0) | ('r' << 8) | ('M' << 16) | ('o' << 24))

//=============================================================================

namespace Arya
{

//=============================================================================
//Mesh
    Mesh::Mesh()
    {
        geometry = 0;
        material = 0;
    }

    Mesh::~Mesh()
    {
        setGeometry(0); //deletes reference
    }

    void Mesh::setGeometry(Geometry* geom)
    {
        if( geometry == geom ) return;
        if( geometry ) {
            geometry->release();
            if( geometry->getRefCount() == 0 )
                delete geometry;
        }
        geometry = geom;
        if( geometry ) geometry->addRef();
    }

    void Mesh::setMaterial(Material* mat)
    {
        material = mat;
    }

//=============================================================================
//Model

    Model::Model(ModelType type) : modelType(type)
    {
        animationData = 0;
        minX = 0.0f;
        maxX = 0.0f;
        minY = 0.0f;
        maxY = 0.0f;
        minZ = 0.0f;
        maxZ = 0.0f;
        refCount = 0;
    }

    Model::~Model()
    {
        if (animationData) delete animationData;
        for(unsigned int i = 0; i < meshes.size(); ++i)
            delete meshes[i];
    }

    AnimationState* Model::createAnimationState()
    {
        if(animationData == 0) return 0;
        //If bone, create BoneAnimationState
        //If vertex, create VertexAnimationState
        //else return 0

        if(modelType == VertexAnimated)
            return new VertexAnimationState((VertexAnimationData*)animationData);
        return 0;
    }

    vec3 Model::getBoundingBoxVertex(int vertexNumber)
    {
        switch(vertexNumber)
        {
            case 0: return vec3(minX, minY, minZ);
            case 1: return vec3(minX, minY, maxZ);
            case 2: return vec3(minX, maxY, minZ);
            case 3: return vec3(minX, maxY, maxZ);
            case 4: return vec3(maxX, minY, minZ);
            case 5: return vec3(maxX, minY, maxZ);
            case 6: return vec3(maxX, maxY, minZ);
            case 7: return vec3(maxX, maxY, maxZ);
            default: break;
        }
        return vec3(0,0,0);
    }    

    Mesh* Model::createMesh()
    {
        Mesh* mesh = new Mesh;
        meshes.push_back(mesh);
        return mesh;
    }

//=============================================================================
//ModelManager

    ModelManager::ModelManager()
    {
    }

    ModelManager::~ModelManager()
    {
        cleanup();
    }

    bool ModelManager::initialize()
    {
        //TODO: Load primitives?
        return true;
    }

    void ModelManager::cleanup()
    {
        unloadAll();
    }

    Model* ModelManager::loadResource(string filename)
    {
        File* modelfile = Locator::getFileSystem().getFile(string("models/") + filename);
        if( modelfile == 0 ) return 0;

        //Note: except for the first magic int
        //this loader does not check the integrity of the data
        //This means that it could crash on invalid files

        char* pointer = modelfile->getData();

        AryaHeader* header = (AryaHeader*)pointer;

        Model* model = 0;

        do{ //for easy break on errors

            if( (modelfile->getSize() < sizeof(AryaHeader) + sizeof(SubmeshInfo)) || header->magic != ARYAMAGICINT )
            {
                LogError << "Not a valid Arya model file: " << filename << endLog;
                break;
            }

            if( header->modeltype < 1 || header->modeltype > 2 )
            {
                LogError << "Arya model with unkown modeltype: " << header->modeltype << endLog;
                break;
            }

            if( header->frameCount < 1 )
            {
                LogError << "Arya model with invalid number of frames: " << header->frameCount << endLog;
                break;
            }

            model = new Model((ModelType)header->modeltype);

            //All materials in the file
            vector<Material*> materials;

            LogDebug << "Loading model " << filename << " with " << header->submeshCount << " meshes." << endLog;

            //Parse all materials
            pointer += sizeof(AryaHeader);
            pointer += header->submeshCount*sizeof(SubmeshInfo);

            char* nameBuf = new char[512];
            for(int m = 0; m < header->materialCount; ++m)
            {
                //Get string
                int count = 0;
                nameBuf[0] = *pointer++;
                while(nameBuf[count]){ ++count; nameBuf[count] = *pointer++; }
                nameBuf[count++] = '.';
                nameBuf[count++] = 't';
                nameBuf[count++] = 'g';
                nameBuf[count++] = 'a';
                nameBuf[count++] = 0;
                
                Material* mat = Locator::getMaterialManager().getMaterial(nameBuf);
                materials.push_back(mat);
            }

            //Parse animations
            VertexAnimationData* animData = 0;

            int animationCount = *(int*)pointer; pointer += 4;
            if(!animationCount)
            {
                model->animationData = 0;
                LogDebug << "Model has no animations" << endLog;
            }
            else
            {
                LogDebug << "Model has " << animationCount << " animations in " << header->frameCount << " frames: ";

                animData = new VertexAnimationData;
                model->animationData = animData;

                VertexAnim newAnim;
                for(int anim = 0; anim < animationCount; ++anim)
                {
                    //Get string
                    int count = 0;
                    nameBuf[0] = *pointer++;
                    while(nameBuf[count]){ ++count; nameBuf[count] = *pointer++; }

                    LogDebug << nameBuf << " ";

                    newAnim.frameTimes.clear();
                    newAnim.startFrame = *(int*)pointer; pointer += 4;
                    newAnim.endFrame = *(int*)pointer; pointer += 4;
                    for(int i = 0; i <= (newAnim.endFrame-newAnim.startFrame); ++i)
                    {
                        newAnim.frameTimes.push_back( *(float*)pointer );
                        pointer += 4;
                    }

                    //Only add the animation if there are actually enough frames
                    if( newAnim.startFrame < header->frameCount && newAnim.endFrame < header->frameCount )
                        animData->animations.insert(make_pair(nameBuf, newAnim));
                    else
                        LogDebug << "(not enough frames) ";
                }

                LogDebug << endLog;
            }

            delete[] nameBuf;

            float* boundingBoxData = (float*)pointer;
            pointer += 6*sizeof(float);
            model->minX = boundingBoxData[0];
            model->maxX = boundingBoxData[1];
            model->minY = boundingBoxData[2];
            model->maxY = boundingBoxData[3];
            model->minZ = boundingBoxData[4];
            model->maxZ = boundingBoxData[5];

            //Parse all geometries
            for(int s = 0; s < header->submeshCount; ++s)
            {
                Mesh* mesh = model->createMesh();
                Geometry* geometry = new Geometry();
                mesh->setGeometry(geometry); //adds refcount

                geometry->primitiveType = header->submesh[s].primitiveType;
                geometry->vertexCount = header->submesh[s].vertexCount;
                geometry->frameCount = header->frameCount;

                mesh->setMaterial(materials[header->submesh[s].materialIndex]);

                int floatCount = header->submesh[s].hasNormals ? 8 : 5;
                int frameBytes = geometry->vertexCount * floatCount * sizeof(GLfloat);

                glGenBuffers(1, &geometry->vertexBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER,
                        geometry->frameCount * frameBytes,
                        modelfile->getData() + header->submesh[s].bufferOffset,
                        GL_STATIC_DRAW);
                if( header->submesh[s].indexCount > 0 )
                {
                    geometry->indexCount = header->submesh[s].indexCount;
                    glGenBuffers(1, &geometry->indexBuffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                            sizeof(GLuint) * geometry->indexCount,
                            modelfile->getData() + header->submesh[s].indexbufferOffset,
                            GL_STATIC_DRAW);
                }
                else
                {
                    geometry->indexCount = 0;
                    geometry->indexBuffer = 0;
                }

                //Create a VAO for every frame
                geometry->createVAOs(geometry->frameCount);
                int stride = floatCount * sizeof(GLfloat);

                if( geometry->frameCount == 1 )
                {
                    //Not animated
                    glBindVertexArray(geometry->vaoHandles[0]);
                    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);

                    glEnableVertexAttribArray(0); //pos
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(0));
                    glEnableVertexAttribArray(1); //tex
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(12));
                    if(header->submesh[s].hasNormals)
                    {
                        glEnableVertexAttribArray(2); //norm
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(20));
                    }
                    if(geometry->indexCount > 0)
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBuffer);
                }
                else
                {
                    //Animated
                    //We actually have to parse the list of animations here
                    //because the endFrame of one animation should have startFrame as 'nextFrame'
                    for(int f = 0; f < geometry->frameCount; ++f)
                    {
                        int nextf = (f+1)%geometry->frameCount;
                        if(animData)
                        {
                            for(auto iter = animData->animations.begin(); iter != animData->animations.end(); ++iter)
                            {
                                if( iter->second.endFrame == f )
                                {
                                    nextf = iter->second.startFrame;
                                    break;
                                }
                            }
                        }

                        glBindVertexArray(geometry->vaoHandles[f]);
                        glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);

                        glEnableVertexAttribArray(0); //pos
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 0));

                        glEnableVertexAttribArray(3); //next pos
                        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(nextf*frameBytes + 0));

                        glEnableVertexAttribArray(1); //tex
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 12));

                        if(header->submesh[s].hasNormals)
                        {
                            glEnableVertexAttribArray(2); //norm
                            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 20));

                            glEnableVertexAttribArray(4); //next norm
                            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(nextf*frameBytes + 20));
                        }
                        if(geometry->indexCount > 0)
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBuffer);
                    }
                }
            }

            addResource(filename, model);
        }while(0);

        Locator::getFileSystem().releaseFile(modelfile);
        return model;
    }
}
