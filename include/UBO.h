#ifndef UBO_CLASS_H
#define UBO_CLASS_H

#include <cstring>
#include <glad/glad.h>

class UBO
{
public:

    GLuint ID;
    // Binding point for the UBO
    GLuint bindingPoint;

    // Constructor that generates a UBO ID and binds it to a binding point
    UBO();
    UBO(GLuint bindingPoint, size_t size);

    // Updates the UBO with new data
    void UpdateData(void* data, size_t size);

    // Binds the UBO to its binding point
    void Bind();

    // Unbinds the UBO
    void Unbind();

    // Deletes the UBO
    void Delete();
};

#endif