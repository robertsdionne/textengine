#ifndef TEXTENGINE_PROGRAM_H_
#define TEXTENGINE_PROGRAM_H_

#include <GLFW/glfw3.h>
#include <vector>

namespace textengine {

  class Shader;

  class Program {
  public:
    Program();

    virtual ~Program();

    void CompileAndLink();

    void Create(const std::vector<Shader *> &&shaders);

    GLint GetAttributeLocation(const std::string &name);

    GLuint GetHandle() const;

    GLint GetUniformLocation(const std::string &name);

    void MaybeOutputLinkerError();

    void Use();

  private:
    std::vector<Shader *> shaders;
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROGRAM_H_
