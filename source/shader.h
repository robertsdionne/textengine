#ifndef RSD_SHADER_H_
#define RSD_SHADER_H_

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace rsd {

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

}  // namespace rsd

#endif  // RSD_SHADER_H_
