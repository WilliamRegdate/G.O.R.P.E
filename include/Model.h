#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include<memory>
#include<json/json.h>
#include"Mesh.h"

using json = nlohmann::json;

class Model
{

    public:
        //NOTE: Model textures have to have "baseColor" in the name for a base color or  "metallicRoughness" for specular texture
        Model(const char* file);
        void updateTransform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale = glm::vec3(1.0f));
        void Draw(Shader& shader, Camera& camera);
        std::unique_ptr<Mesh> mesh;
        glm::vec3 translation = glm::vec3(0.0f);
    private:
        const char* file;
        std::vector<unsigned char> data;
        json JSON;

        
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        std::vector<std::string> loadedTexName;
        std::vector<Texture> loadedTex;

        void loadMesh(unsigned int indMesh, std::vector<Vertex>& vertices,std::vector<GLuint>& indices);

        void traverseNode(unsigned int nextNode, std::vector<Vertex>& vertices,std::vector<GLuint>& indices, glm::mat4 matrix = glm::mat4(1.0f));


        std::vector <unsigned char> getData();
        std::vector<float> getFloats(json accessor);
        std::vector<GLuint> getIndices(json accessor);
        std::vector<Texture> getTextures();

        std::vector<Vertex> assembleVertices
        (
            std::vector<glm::vec3> positions,
            std::vector<glm::vec3> normals,
            std::vector<glm::vec2> texUVs
        );

        std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
        std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
        std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);

        bool fileExists(const std::string& fileName);
};

#endif