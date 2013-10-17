#ifndef TEXTENGINE_PROGRAM_H_
#define TEXTENGINE_PROGRAM_H_

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

namespace textengine {

  class Shader;

  class Program {
  public:
    Program();

    virtual ~Program();

    GLuint get_handle() const;

    void CompileAndLink();

    void Create(const std::vector<Shader *> &&shaders);

    GLint GetAttributeLocation(const std::string &name);

    GLint GetUniformLocation(const std::string &name);

    void Uniform(std::string name, float value);

    void Uniform(std::string name, const glm::mat4 &value);

    void Use();

  private:
    void MaybeOutputLinkerError();

  private:
    std::vector<Shader *> shaders;
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROGRAM_H_
