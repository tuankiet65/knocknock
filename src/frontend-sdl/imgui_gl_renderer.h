#pragma once

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <imgui.h>

using namespace gl;

class ImGuiGLRenderer {
public:
    class RGBATexture {
    public:
        void update(const void *texture);

        [[nodiscard]] GLuint id() const { return id_; }

        ~RGBATexture() {
            ::glDeleteTextures(1, &id_);
            id_ = 0;
        }

        // Textures are a pain to copy so might as well disallow it.
        RGBATexture(const RGBATexture &) = delete;
        RGBATexture &operator=(const RGBATexture &) = delete;

    private:
        explicit RGBATexture(GLuint id, GLsizei width, GLsizei height)
            : id_(id), width_(width), height_(height) {}

        GLuint id_;
        GLsizei width_, height_;

        friend ImGuiGLRenderer;
    };

    explicit ImGuiGLRenderer(glbinding::GetProcAddress get_proc_addr_func,
                             ImGuiIO &io);
    ~ImGuiGLRenderer();

    void render(ImDrawData *data);
    void clear(ImVec4 color);

    RGBATexture create_texture(int width, int height);

    [[nodiscard]] bool initialized() const { return initialized_; }

    ImGuiGLRenderer(const ImGuiGLRenderer &) = delete;
    ImGuiGLRenderer &operator=(const ImGuiGLRenderer &) = delete;

private:
    void setup_render_state(float framebuffer_width,
                            float framebuffer_height,
                            ImVec2 display_pos,
                            ImVec2 display_size) const;

    ImGuiIO &io_;
    bool initialized_;

    bool init_shaders();
    GLuint shader_program_;
    GLuint projection_matrix_uniform_;
    GLuint vertex_attribute_;
    GLuint texture_coord_attribute_;
    GLuint color_attribute_;

    void init_vertex_buffers();
    GLuint vertex_buffer_;
    GLuint element_buffer_;
    GLuint vao_;
};
