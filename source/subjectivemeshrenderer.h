#ifndef TEXTENGINE_SUBJECTIVEMESHRENDERER_H_
#define TEXTENGINE_SUBJECTIVEMESHRENDERER_H_

#include <glm/glm.hpp>
#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "vertexarray.h"
#include "vertexformat.h"

namespace textengine {

  class Mesh;
  class MeshEditor;
  class Updater;

  class SubjectiveMeshRenderer : public Renderer {
  public:
    SubjectiveMeshRenderer(Mesh &mesh);

    virtual ~SubjectiveMeshRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;

    void SetPerspective(glm::vec2 perspective, glm::vec2 camera_position);
    
  private:
    Mesh &mesh;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, vertex_shader;
    Program edge_program, face_program;
    VertexFormat vertex_format;
    VertexArray mesh_array, mesh_edge_array;
    Buffer mesh_buffer, mesh_edge_buffer;

    glm::mat4 model_view, projection;
    glm::vec2 perspective;
  };

}  // namespace textengine

#endif  // TEXTENGINE_SUBJECTIVEMESHRENDERER_H_
