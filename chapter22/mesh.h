#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "texture.h"

#include <map>
#include <vector>
#include <GL/glew.h>

#include "util.h"
#include "math_3d.h"

#define INVALID_MATERIAL 0xFFFFFFFF

class Mesh
{
    public:
        Mesh(){};
        ~Mesh(){
            Clear();
        };
        bool LoadMesh(const std::string& Filename);
        void Render();

    private:
        bool InitFromScene(const aiScene* pScene, const std::string& Filename);
        void InitMesh(unsigned int Index, const aiMesh* paiMesh);
        bool InitMaterials(const aiScene* pScene, const std::string& Filename);
        void Clear();

        struct MeshEntry{
            MeshEntry();
            ~MeshEntry();

            bool Init(const std::vector<Vertex>& Vertices,
                      const std::vector<unsigned int>& Indices);

            GLuint VB;
            GLuint IB;

            unsigned int NumIndices;
            unsigned int MaterialIndex;
        };

        std::vector<MeshEntry> m_Entries;
        std::vector<Texture*> m_Textures;
};