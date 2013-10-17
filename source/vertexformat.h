#ifndef TEXTENGINE_VERTEXFORMAT_H_
#define TEXTENGINE_VERTEXFORMAT_H_

#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace textengine {

  class Program;
  class VertexArray;

  class VertexFormat {
  public:
    struct Attribute {
      std::string name;
      GLenum type;
      GLint size;
    };

    VertexFormat() = default;

    virtual ~VertexFormat() = default;

    void Apply(VertexArray &array, Program &program) const;

    void Create(std::vector<Attribute> &&attributes);

  private:
    static const std::unordered_map<GLenum, GLint> kTypeSizes;

    std::vector<Attribute> attributes;
  };

}  // namespace textengine

#endif  // TEXTENGINE_VERTEXFORMAT_H_
