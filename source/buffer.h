#ifndef __textengine__buffer__
#define __textengine__buffer__

#include <GLFW/glfw3.h>

namespace textengine {

  class Buffer {
  public:
    Buffer();

    virtual ~Buffer();

    GLuint get_handle() const;

    void Bind();

    void Create(GLenum target);

    void Data(GLsizeiptr size, const GLvoid *data, GLenum usage);

    void SubData(GLintptr offset, GLsizeiptr size, const GLvoid * data);

  private:
    GLenum target;
    GLuint handle;
  };

}  // namespace textengine

#endif /* defined(__textengine__buffer__) */
