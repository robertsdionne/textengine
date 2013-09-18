#ifndef TEXTENGINE_BUFFER_H_
#define TEXTENGINE_BUFFER_H_

#include <GLFW/glfw3.h>

namespace textengine {

  class Buffer {
  public:
    Buffer();

    virtual ~Buffer();

    void Bind();

    void Create(GLenum target);

    void Data(GLsizeiptr size, const GLvoid *data, GLenum usage);

    void SubData(GLintptr offset, GLsizeiptr size, const GLvoid * data);

  private:
    GLenum target;
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_BUFFER_H_
