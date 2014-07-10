#ifndef RSD_VERTEXARRAY_H_
#define RSD_VERTEXARRAY_H_

#include <GLFW/glfw3.h>

namespace rsd {

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

}  // namespace rsd

#endif  // RSD_VERTEXARRAY_H_
