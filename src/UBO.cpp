#include "UBO.h"

#include<iostream>

// Constructor: generates a UBO ID and binds it to the specified binding point
UBO::UBO() {}
UBO::UBO(GLuint bindingPoint, size_t size)
    : bindingPoint(bindingPoint)
{
    
    glGenBuffers(1, &ID);  // Generate the buffer ID
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ID);  // Bind the buffer to the specified binding point
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);  // Allocate memory for the buffer
    
}

// Method to update the UBO data
void UBO::UpdateData(void* data, size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
    void* bufferData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);  // Map the buffer for writing    
    std::memcpy(bufferData, data, size);  // Copy data into the UBO
    glUnmapBuffer(GL_UNIFORM_BUFFER);  // Unmap the buffer after writing

}

void UBO::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, ID);
}

void UBO::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::Delete()
{
    glDeleteBuffers(1, &ID);
}