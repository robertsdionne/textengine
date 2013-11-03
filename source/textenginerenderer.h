#ifndef TEXTENGINE_TEXTENGINERENDERER_H_
#define TEXTENGINE_TEXTENGINERENDERER_H_

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

    virtual void Change(int width, int height);

    virtual void Create();

    virtual void Render();

	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color);

	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf);
    
  private:
    Updater &updater;
    MeshRenderer &mesh_renderer;
    float inverse_aspect_ratio;

    Shader edge_geometry_shader, fragment_shader, point_geometry_shader, vertex_shader;
    Program edge_program, face_program, point_program;
    VertexFormat vertex_format;
    VertexArray player_array, player_edge_array, npc_array, npc_edge_array, item_array,
        item_edge_array;
    Buffer player_buffer, player_edge_buffer, npc_buffer, npc_edge_buffer, item_buffer,
        item_edge_buffer;

    glm::mat4 model_view, projection;
  };

}  // namespace textengine

#endif  // TEXTENGINE_TEXTENGINERENDERER_H_
