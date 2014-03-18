#ifndef __textengine__shader__
#define __textengine__shader__

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace textengine {

  class Shader {
  public:
    Shader();

    virtual ~Shader();

    GLuint get_handle() const;

    void Create(GLenum type, const std::vector<std::string> &&sources);

    void Compile();

    static constexpr GLsizei kMaxInfoLogLength = 4 * 1024;

  private:
    void MaybeOutputCompilerError();

  private:
    GLenum type;
    std::vector<std::string> sources;
    GLuint handle;
  };

}  // namespace textengine

#endif /* defined(__textengine__shader__) */
