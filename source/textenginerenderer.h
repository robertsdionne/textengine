#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>
#include <iostream>

#include "buffer.h"
#include "program.h"
#include "renderer.h"
#include "shader.h"
#include "shaders.h"
#include "vertexarray.h"
#include "vertexformat.h"

namespace textengine {

  class MeshRenderer;
  class Updater;

  class TextEngineRenderer : public Renderer, public b2Draw {
  public:
    TextEngineRenderer(Updater &updater, MeshRenderer &mesh_renderer);

    virtual ~TextEngineRenderer() = default;

    virtual void Change(int width, int height) override;

    virtual void Create() override;

    virtual void Render() override;

	virtual void DrawPolygon(const b2Vec2* vertices,
                             int32 vertex_count, const b2Color& color) override;

	virtual void DrawSolidPolygon(const b2Vec2* vertices,
                                  int32 vertex_count, const b2Color& color) override;

	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

	virtual void DrawSolidCircle(const b2Vec2& center,
                                 float32 radius, const b2Vec2& axis, const b2Color& color) override;

	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

	virtual void DrawTransform(const b2Transform& xf) override;
    
  private:
    Updater &updater;
    MeshRenderer &mesh_renderer;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray player_array, player_edge_array, npc_array, npc_edge_array, item_array,
        item_edge_array, player_view_array;
    Buffer player_buffer, player_edge_buffer, npc_buffer, npc_edge_buffer, item_buffer,
        item_edge_buffer, player_view_buffer;

    glm::mat4 model_view, projection;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
