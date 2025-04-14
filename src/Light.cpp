#ifndef LIGHT_H
#define LIGHT_H

#include"Light.h"


lightController::lightController(Shader& program, int MaxLights)
    : shaderProgram(&program)
{
    maxLength = MaxLights;
    // Get the binding point of all three light arrays
    GLuint blockIndexPointLight = glGetUniformBlockIndex(shaderProgram->ID, "pointLightsBlock");
    GLuint blockIndexDirecLight = glGetUniformBlockIndex(shaderProgram->ID, "direcLightsBlock");
    GLuint blockIndexSpotLight = glGetUniformBlockIndex(shaderProgram->ID, "spotLightsBlock");

    glUniformBlockBinding(shaderProgram->ID, blockIndexPointLight, 1);
    glUniformBlockBinding(shaderProgram->ID, blockIndexDirecLight, 2);
    glUniformBlockBinding(shaderProgram->ID, blockIndexSpotLight, 3);

    // Initialize the UBO with the correct binding point and size
    pointLightUBO = UBO(1, sizeof(pointLightGPU) * MaxLights);  
    direcLightUBO = UBO(2, sizeof(direcLightGPU) * MaxLights);
    spotLightUBO = UBO(3, sizeof(spotLightGPU) * MaxLights);

    GLenum err = glGetError();
    std::cerr << "OpenGL Error: " << err << std::endl;
}
// {
    
//     // Get the binding point of the point light array
//     GLuint blockIndex = glGetUniformBlockIndex(shaderProgram->ID, "pointLightsBlock");
//     GLuint bindingPoint;
    
//     glGetActiveUniformBlockiv(shaderProgram->ID, blockIndex, GL_UNIFORM_BLOCK_BINDING, (GLint*)&bindingPoint);
    
//     // Initialize the UBO with the correct binding point and size
//     pointLightUBO = UBO(bindingPoint, sizeof(pointLightGPU) * 15);  // 15 is from the length of the array in the shader
// }

void lightController::updatePointUniform()
{
    //send updated amountOfLights to the GPU
    GLuint location = glGetUniformLocation(shaderProgram->ID, "amountOfLights");
	glUniform1iv(location, 3, amountOfLights);

    std::vector<pointLightGPU> lightArray;

    for (const pointLight& light : pointLights) 
    {
        lightArray.push_back(pointLightGPU{light.color, light.position, 0.0f, light.linearDropoff, light.quadraticDropoff, {0.0f, 0.0f}});
    }

    pointLightUBO.UpdateData(lightArray.data(), sizeof(pointLightGPU) * lightArray.size());
}
void lightController::updateDirecUniform()
{
    //send updated amountOfLights to the GPU
    GLuint location = glGetUniformLocation(shaderProgram->ID, "amountOfLights");
    glUniform1iv(location, 3, amountOfLights);

    std::vector<direcLightGPU> lightArray;

    for (const direcLight& light : direcLights) {
        lightArray.push_back(direcLightGPU{light.color, light.direction, 0.0f});
    }

    direcLightUBO.UpdateData(lightArray.data(), sizeof(pointLightGPU) * lightArray.size());
}
void lightController::updateSpotUniform()
{
    //send updated amountOfLights to the GPU
    GLuint location = glGetUniformLocation(shaderProgram->ID, "amountOfLights");
    glUniform1iv(location, 3, amountOfLights);

    std::vector<spotLightGPU> lightArray;

    for (const spotLight& light : spotLights) {
        lightArray.push_back(spotLightGPU{light.color, light.position, 0.0f, light.direction, 0.0f, light.outerCone, light.innerCone, {0.0f, 0.0f}});
    }

    spotLightUBO.UpdateData(lightArray.data(), sizeof(spotLightGPU) * lightArray.size());
    
}

void lightController::updateLight(pointLight& light, pointLight& newLight)
{
    std::for_each(pointLights.begin(), pointLights.end(), 
        [&light, &newLight](pointLight& currentLight) {
            if (currentLight.color == light.color &&
                currentLight.position == light.position &&
                currentLight.linearDropoff == light.linearDropoff &&
                currentLight.quadraticDropoff == light.quadraticDropoff) ;
            {
                currentLight = newLight;
            }
        });
    updatePointUniform();
}

void lightController::updateLight(direcLight& light, direcLight& newLight)
{
    std::for_each(direcLights.begin(), direcLights.end(), 
        [&light, &newLight](direcLight& currentLight) {
            if (currentLight.color == light.color &&
                currentLight.direction == light.direction) 
            {
                currentLight = newLight;
            }
        });
    updateDirecUniform();
}

void lightController::updateLight(spotLight& light, spotLight& newLight)
{
    std::for_each(spotLights.begin(), spotLights.end(), 
        [&light, &newLight](spotLight& currentLight) {
            if (currentLight.color == light.color &&
                currentLight.position == light.position &&
                currentLight.direction == light.direction &&
                currentLight.outerCone == light.outerCone &&
                currentLight.innerCone == light.innerCone) 
            {
                currentLight = newLight;
            }
        });
    updateSpotUniform();
}

pointLight* lightController::addLight(pointLight light)
{
    if(pointLights.size() < maxLength)
    {
        pointLights.push_back(light);
        amountOfLights[0] = pointLights.size();
        updatePointUniform();
        return &pointLights.back();
    }
    else
    {
        std::cerr<<"exceeded max pointLight length"<<"\n";
    }
    return nullptr;
}
direcLight* lightController::addLight(direcLight light)
{
    if(direcLights.size() < maxLength)
    {
        direcLights.push_back(light);
        amountOfLights[1] = direcLights.size();
        updateDirecUniform();
        return &direcLights.back();
    }
    else
    {
        std::cerr<<"exceeded max direcLight length"<<"\n";
    }
    return nullptr;
}
spotLight* lightController::addLight(spotLight light)
{
    if(spotLights.size() < maxLength)
    {
        spotLights.push_back(light);
        amountOfLights[2] = spotLights.size();
        updateSpotUniform();
        return &spotLights.back();
    }
    else
    {
        std::cerr<<"exceeded max spotLight length"<<"\n";
    }
    return nullptr;
}

void lightController::removeLight(pointLight& light)
{
    pointLights.remove_if([&light](const pointLight& currentLight) {
        return currentLight.color == light.color &&
               currentLight.position == light.position &&
               currentLight.linearDropoff == light.linearDropoff &&
               currentLight.quadraticDropoff == light.quadraticDropoff;
    });
    amountOfLights[0] = pointLights.size();
    updatePointUniform();
}

void lightController::removeLight(direcLight& light)
{
    direcLights.remove_if([&light](const direcLight& currentLight) {
        return currentLight.color == light.color &&
               currentLight.direction == light.direction;
    });
    amountOfLights[1] = direcLights.size();
    updateDirecUniform();
}

void lightController::removeLight(spotLight& light)
{
    spotLights.remove_if([&light](const spotLight& currentLight) {
        return currentLight.color == light.color &&
               currentLight.position == light.position &&
               currentLight.direction == light.direction &&
               currentLight.outerCone == light.outerCone &&
               currentLight.innerCone == light.innerCone;
    });
    amountOfLights[2] = spotLights.size();
    updateSpotUniform();
}

void lightController::debug(int i)
{
    std::cout << "amount of lights "
    <<amountOfLights[0] << " "
    <<amountOfLights[1] << " "
    <<amountOfLights[2] << std::endl;
     

    if (i == 0)
    {
        std::vector<pointLightGPU> debugData(15); // Assuming max 15 lights
        glBindBuffer(GL_UNIFORM_BUFFER, pointLightUBO.ID);
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(pointLightGPU) * 15, debugData.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Print the first few lights to verify values
        for (int i = 0; i < 3; ++i) {
            std::cout << "pointLight " << i << " Color: "
                    << debugData[i].color.r << ", "
                    << debugData[i].color.g << ", "
                    << debugData[i].color.b << ", "
                    << debugData[i].color.a << std::endl;
        }
    }   
    if (i == 1)
    {
        std::vector<direcLightGPU> debugData(15); // Assuming max 15 lights
        glBindBuffer(GL_UNIFORM_BUFFER, direcLightUBO.ID);
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(direcLightGPU) * 15, debugData.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Print the first few lights to verify values
        for (int i = 0; i < 3; ++i) {
            std::cout << "direcLight " << i << " Color: "
                    << debugData[i].color.r << ", "
                    << debugData[i].color.g << ", "
                    << debugData[i].color.b << ", "
                    << debugData[i].color.a << std::endl;
        }
    }   
    if (i == 2)
    {
        std::vector<spotLightGPU> debugData(15); // Assuming max 15 lights
        glBindBuffer(GL_UNIFORM_BUFFER, spotLightUBO.ID);
        glGetBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(spotLightGPU) * 15, debugData.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Print the first few lights to verify values
        for (int i = 0; i < 3; ++i) {
            std::cout << "spotLight " << i << " Color: "
                    << debugData[i].color.r << ", "
                    << debugData[i].color.g << ", "
                    << debugData[i].color.b << ", "
                    << debugData[i].color.a << std::endl;
            std::cout << "spotLight " << i << " Position: "
                    << debugData[i].position.x << ", "
                    << debugData[i].position.y << ", "
                    << debugData[i].position.z << std::endl;
            std::cout << "spotLight " << i << " Direction: "
                    << debugData[i].direction.x << ", "
                    << debugData[i].direction.y << ", "
                    << debugData[i].direction.z << std::endl ;
            std::cout << "spotLight " << i << " innerCone: "<< debugData[i].innerCone<< std::endl;
            std::cout << "spotLight " << i << " outerCone: "<< debugData[i].outerCone<< std::endl<< std::endl;
        }
    }   
}
#endif