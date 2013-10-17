#ifndef TEXTENGINE_PROGRAM_H_
#define TEXTENGINE_PROGRAM_H_

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
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

    void Uniforms(const std::unordered_map<std::string, float> &&uniforms);

    void Uniforms(const std::unordered_map<std::string, glm::mat4> &&uniforms);

    void Use();

  private:
    void MaybeOutputLinkerError();

  private:
    std::vector<Shader *> shaders;
    GLuint handle;
  };

}  // namespace textengine

#endif  // TEXTENGINE_PROGRAM_H_
