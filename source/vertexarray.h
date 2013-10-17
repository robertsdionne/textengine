#ifndef TEXTENGINE_VERTEXARRAY_H_
#define TEXTENGINE_VERTEXARRAY_H_

#include <GLFW/glfw3.h>

namespace textengine {

  class VertexArray {
  public:
    VertexArray();
    
    virtual ~VertexArray();

    void Bind();

    void Create();

    void EnableVertexAttribArray(GLuint index);

    void VertexAttribPointer(GLuint index, GLint size, GLenum type,
                             GLboolean normalized, GLsizei stride, const GLvoid *pointer);

  private:
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_VERTEXARRAY_H_
