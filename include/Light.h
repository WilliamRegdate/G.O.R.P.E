#include<glm/glm.hpp>
#include<glad/glad.h>
#include<list>
#include<algorithm>
#include<vector>

#include"UBO.h"
#include"shaderClass.h"


struct pointLight
{
    glm::vec4 color;
    glm::vec3 position;
    float linearDropoff;
	float quadraticDropoff;
};
struct direcLight
{
    glm::vec4 color;
    glm::vec3 direction;
};
struct spotLight
{
    glm::vec4 color;
    glm::vec3 position;
    glm::vec3 direction;
	float outerCone;
	float innerCone; 
};

class lightController
{
    
    private:
        UBO pointLightUBO;
        UBO direcLightUBO;
        UBO spotLightUBO;

        int amountOfLights[3] = {0, 0, 0};  // point, dir, spot
        int maxLength = 15;
        std::list<pointLight> pointLights;
        std::list<spotLight> spotLights;
        std::list<direcLight> direcLights;
        
        void updatePointUniform();
        void updateSpotUniform();
        void updateDirecUniform();

    public:
        lightController(Shader& program, int MaxLights = 15);

        Shader* shaderProgram;

        void updateLight(pointLight& light, pointLight& newLight);
        void updateLight(direcLight& light, direcLight& newLight);
        void updateLight(spotLight& light, spotLight& newLight);

        pointLight* addLight(pointLight light);
        direcLight* addLight(direcLight light);
        spotLight* addLight(spotLight light);

        void removeLight(pointLight& light);
        void removeLight(direcLight& light);
        void removeLight(spotLight& light);

        void debug(int i);


};

struct pointLightGPU 
{
    glm::vec4 color;          // 16 bytes
    glm::vec3 position;       // 12 bytes
    float padding1;           // 4 bytes
    float linearDropoff;      // 4 bytes
    float quadraticDropoff;   // 4 bytes
    float padding2[2];        // 8 bytes (align struct to 16-byte multiple) 48 bytes total
};
struct direcLightGPU 
{
    glm::vec4 color;         // 16 bytes
    glm::vec3 direction;     // 12 bytes
    float padding;           // 4 bytes (32 bytes total)
};
struct spotLightGPU 
{
    glm::vec4 color;          // 16 bytes
    glm::vec3 position;       // 12 bytes
    float padding1;           // 4 bytes
    glm::vec3 direction;      // 12 bytes  
    float padding2;           // 4 bytes
	float outerCone;          // 4 bytes
	float innerCone;          // 4 bytes
    float padding3[2];        // 8 bytes (64 bytes total)
};