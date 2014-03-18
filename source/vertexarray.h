#ifndef __textengine__vertexarray__
#define __textengine__vertexarray__

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

#endif /* defined(__textengine__vertexarray__) */
