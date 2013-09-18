#ifndef TEXTENGINE_SHADER_H_
#define TEXTENGINE_SHADER_H_

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace textengine {

  class Shader {
  public:
    Shader();

    virtual ~Shader();

    void Create(GLenum type, const std::vector<std::string> &&sources);

    void Compile();

    GLuint GetHandle() const;

    void MaybeOutputCompilerError();

    static constexpr GLsizei kMaxInfoLogLength = 4 * 1024;

  private:
    GLenum type;
    std::vector<std::string> sources;
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SHADER_H_
