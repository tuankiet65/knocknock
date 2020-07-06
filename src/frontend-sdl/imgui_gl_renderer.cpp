#include "imgui_gl_renderer.h"

#include <fmt/format.h>
#include <glbinding/glbinding.h>
#include <glog/logging.h>

#include <utility>

using namespace gl;

namespace {

std::string decode_source(GLenum source) {
    switch (source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        default: return fmt::format("(unknown: {})", source);
    }
}

std::string decode_type(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECARED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        default: return fmt::format("(unknown: {})", type);
    }
}

std::string decode_severity(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        default: return fmt::format("(unknown: {})", severity);
    }
}

// As defined in
// https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_debug.txt
void gl_debug_output_callback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei /* length*/,
                              const char *message,
                              const void * /* user_param */) {
    std::string output =
        fmt::format("[OpenGL] {} [source={} type={} id={} severity={}]",
                    message, decode_source(source), decode_type(type), id,
                    decode_severity(severity));

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        LOG(INFO) << output;
    } else {
        LOG(ERROR) << output;
    }
}

struct CompilationStatus {
    bool success = false;
    std::string message;
};

CompilationStatus get_shader_status(GLuint shader) {
    GLboolean success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) {
        return {true, ""};
    }

    GLsizei log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<GLchar> log(log_length);
    glGetShaderInfoLog(shader, log_length, nullptr, log.data());

    return {false, log.data()};
}

CompilationStatus get_program_status(GLuint program) {
    GLboolean success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_TRUE) {
        return {true, ""};
    }

    GLsizei log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<GLchar> log(log_length);
    glGetProgramInfoLog(program, log_length, nullptr, log.data());

    return {false, log.data()};
}

inline GLuint VOIDP_TO_GLUINT(void *voidp) {
    return (GLuint)(intptr_t)(voidp);
}

bool extension_available(const std::string &requested_extension) {
    GLint extension_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);

    for (int i = 0; i < extension_count; ++i) {
        const GLubyte *extension = glGetStringi(GL_EXTENSIONS, i);
        if (requested_extension == (const char *)(extension)) {
            return true;
        }
    }

    return false;
}

bool gl_context_is_debug() {
    GLint context_flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

    return (context_flags & (int)GL_CONTEXT_FLAG_DEBUG_BIT);
}

int get_gl_version() {
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    return (major * 100) + (minor * 10);
}

void init_debug_output_if_available() {
    if (!gl_context_is_debug()) {
        LOG(INFO) << "Not a OpenGL debug context, won't enable debug output";
        return;
    }

    LOG(INFO) << "Is a OpenGL debug context, OpenGL errors will be reported.";

    if (get_gl_version() >= 430 || extension_available("GL_KHR_debug")) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(gl_debug_output_callback, nullptr);
    } else if (extension_available("GL_ARB_debug_output")) {
        // Debug Output is also supported if the ARB_debug_output extension is
        // available, however the enum and function names are different from
        // core Debug Output.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

        glDebugMessageCallbackARB(gl_debug_output_callback, nullptr);
    } else {
        LOG(WARNING) << "OpenGL version < 4.3, and ARB_debug_output not "
                        "available, OpenGL errors won't be reported.";
    }
}

const GLchar *VERTEX_SHADER_SOURCE =
    "#version 130\n"
    "in vec2 vertex;\n"
    "in vec2 in_texture_coord;\n"
    "in vec4 in_color;\n"
    "out vec2 texture_coord;\n"
    "out vec4 color;\n"
    "uniform mat4 projection_matrix;\n"
    "void main() {\n"
    "   gl_Position = projection_matrix * vec4(vertex.xy, 0.0, 1.0);\n"
    "   texture_coord = in_texture_coord;\n"
    "   color = in_color;\n"
    "}\n";

const GLchar *FRAGMENT_SHADER_SOURCE =
    "#version 130\n"
    "uniform sampler2D Texture;\n"
    "in vec2 texture_coord;\n"
    "in vec4 color;\n"
    "out vec4 out_color;\n"
    "void main() {\n"
    "    out_color = color * texture(Texture, texture_coord.st);\n"
    "}\n";

}  // namespace

void ImGuiGLRenderer::RGBATexture::update(const void *texture) {
    GLint prev_bound_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_bound_texture);

    glBindTexture(GL_TEXTURE_2D, id_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA,
                    GL_UNSIGNED_BYTE, texture);

    glBindTexture(GL_TEXTURE_2D, prev_bound_texture);
}

ImGuiGLRenderer::ImGuiGLRenderer(glbinding::GetProcAddress get_proc_addr_func,
                                 ImGuiIO &io)
    : io_(io),
      initialized_(false),
      shader_program_(0),
      projection_matrix_uniform_(0),
      vertex_attribute_(0),
      texture_coord_attribute_(0),
      color_attribute_(0),
      vertex_buffer_(0),
      element_buffer_(0),
      vao_(0) {
    io_.BackendRendererName = "ImGuiGLRenderer";
    glbinding::initialize(std::move(get_proc_addr_func));

    LOG(INFO) << fmt::format("OpenGL version {}", get_gl_version());

    init_debug_output_if_available();

    initialized_ = init_shaders();
    if (!initialized_) {
        return;
    }

    init_vertex_buffers();
}

bool ImGuiGLRenderer::init_shaders() {
    CompilationStatus status;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertex_shader);
    status = get_shader_status(vertex_shader);
    if (!status.success) {
        glDeleteShader(vertex_shader);
        LOG(ERROR) << "[OpenGL] Vertex shader compilation failure: "
                   << status.message;
        return false;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragment_shader);
    status = get_shader_status(fragment_shader);
    if (!status.success) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        LOG(ERROR) << "[OpenGL] Fragment shader compilation failure: "
                   << status.message;
        return false;
    }

    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    status = get_program_status(shader_program_);
    if (!status.success) {
        // Oops, can't link the program. But before we can delete the program,
        // we have to detach and delete shaders attached to it (explanation
        // below).
        glDetachShader(shader_program_, vertex_shader);
        glDeleteShader(vertex_shader);
        glDetachShader(shader_program_, fragment_shader);
        glDeleteShader(fragment_shader);

        // Now then can we delete the program.
        glDeleteProgram(shader_program_);
        shader_program_ = 0;

        LOG(ERROR) << "[OpenGL] Shader program linkage failure: "
                   << status.message;
        return false;
    }

    // Program is linked so the compiled shaders are no longer needed, detach
    // and delete them. The detach is necessary because a shader object is only
    // deleted when it's marked for deletion (using glDeleteShader), and it is
    // not attached to any programs.
    glDetachShader(shader_program_, vertex_shader);
    glDeleteShader(vertex_shader);
    glDetachShader(shader_program_, fragment_shader);
    glDeleteShader(fragment_shader);

    // Grab the location of uniforms and attributes.
    projection_matrix_uniform_ =
        glGetUniformLocation(shader_program_, "projection_matrix");
    vertex_attribute_ = glGetAttribLocation(shader_program_, "vertex");
    texture_coord_attribute_ =
        glGetAttribLocation(shader_program_, "in_texture_coord");
    color_attribute_ = glGetAttribLocation(shader_program_, "in_color");

    return true;
}

void ImGuiGLRenderer::init_vertex_buffers() {
    GLint prev_vertex_buffer, prev_element_buffer, prev_vao;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vertex_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &prev_element_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);

    // Configure VAO.
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);

    // ImDrawVert:
    // |         pos         |         uv          |       col      |
    // | x: float | y: float | u: float | v: float | r, g, b, a: u8 |
    // First attribute: vertex coordinates
    glVertexAttribPointer(
        vertex_attribute_,
        2,  // ImDrawVert.pos contains two float
        GL_FLOAT,
        GL_FALSE,  // will be normalized by the perspective matrix
        sizeof(ImDrawVert), (const void *)IM_OFFSETOF(ImDrawVert, pos));
    glEnableVertexAttribArray(vertex_attribute_);
    // Second attribute: texture coordinates
    glVertexAttribPointer(texture_coord_attribute_, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ImDrawVert),
                          (const void *)IM_OFFSETOF(ImDrawVert, uv));
    glEnableVertexAttribArray(texture_coord_attribute_);
    // Third attribute: color
    glVertexAttribPointer(color_attribute_, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                          sizeof(ImDrawVert),
                          (const void *)IM_OFFSETOF(ImDrawVert, col));
    glEnableVertexAttribArray(color_attribute_);

    glBindVertexArray(prev_vao);
    glBindBuffer(GL_ARRAY_BUFFER, prev_vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prev_element_buffer);
}

ImGuiGLRenderer::~ImGuiGLRenderer() {
    glDeleteProgram(shader_program_);
    shader_program_ = 0;
    projection_matrix_uniform_ = 0;
    vertex_attribute_ = 0;
    texture_coord_attribute_ = 0;
    color_attribute_ = 0;

    glDeleteBuffers(1, &vertex_buffer_);
    vertex_buffer_ = 0;

    glDeleteBuffers(1, &element_buffer_);
    element_buffer_ = 0;

    glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
}

ImGuiGLRenderer::RGBATexture ImGuiGLRenderer::create_texture(int width,
                                                             int height) {
    GLint prev_bound_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_bound_texture);

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Apparently SDL is messing with this parameter.
    // https://github.com/ocornut/imgui/pull/752
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, prev_bound_texture);

    return RGBATexture(texture_id, width, height);
}

void ImGuiGLRenderer::setup_render_state(float framebuffer_width,
                                         float framebuffer_height,
                                         ImVec2 display_pos,
                                         ImVec2 display_size) const {
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glViewport(0, 0, framebuffer_width, framebuffer_height);

    glUseProgram(shader_program_);

    // Orthographic projection matrix.
    float left = display_pos.x, right = display_pos.x + display_size.x,
          top = display_pos.y, bottom = display_pos.y + display_size.y;
    float projection_matrix[4][4] = {
        {2 / (right - left), 0.0, 0.0, (right + left) / (left - right)},
        {0.0, 2 / (top - bottom), 0.0, (top + bottom) / (bottom - top)},
        {0.0, 0.0, -1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}};
    glUniformMatrix4fv(projection_matrix_uniform_, 1, GL_TRUE,
                       &projection_matrix[0][0]);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
}

void ImGuiGLRenderer::render(ImDrawData *data) {
    DCHECK(initialized_);

    DCHECK(data->Valid);

    float framebuffer_width = io_.DisplaySize.x * io_.DisplayFramebufferScale.x;
    float framebuffer_height =
        io_.DisplaySize.y * io_.DisplayFramebufferScale.y;

    if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        // Window is minimized, don't render.
        return;
    }

    setup_render_state(framebuffer_width, framebuffer_height, data->DisplayPos,
                       data->DisplaySize);

    for (int draw_list_idx = 0; draw_list_idx < data->CmdListsCount;
         ++draw_list_idx) {
        ImDrawList *draw_list = data->CmdLists[draw_list_idx];

        // Upload vertex and index buffers.
        glBufferData(GL_ARRAY_BUFFER, draw_list->VtxBuffer.size_in_bytes(),
                     draw_list->VtxBuffer.begin(), GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     draw_list->IdxBuffer.size_in_bytes(),
                     draw_list->IdxBuffer.begin(), GL_STREAM_DRAW);

        for (ImDrawCmd &draw_cmd : draw_list->CmdBuffer) {
            if (draw_cmd.UserCallback == nullptr) {
                // Is a render command.

                // Configure scissor area.
                // ImGui's coordinate system is (0, 0) at the top-left corner.
                // display_pos: top-left corner of the framebuffer.
                // (area.x, area.y): top-left corner of the scissor area.
                // (area.z, area.w): bottom-right corner of the scissor area.
                ImVec2 display_pos = data->DisplayPos;
                ImVec4 area = draw_cmd.ClipRect;
                ImVec2 framebuffer_scale = io_.DisplayFramebufferScale;

                // Width and height of the scissor area.
                GLsizei width = (area.z - area.x) * framebuffer_scale.x,
                        height = (area.w - area.y) * framebuffer_scale.y;

                // (x, y): lower-left corner of the scissor area.
                float x = (area.x - display_pos.x) * framebuffer_scale.x;
                float y = (area.w - display_pos.y) * framebuffer_scale.y;

                // OpenGL's coordinate system is (0, 0) at the bottom-left
                // corner, so y has to be flipped.
                glScissor(x, framebuffer_height - y, width, height);

                // Load the specified texture.
                glBindTexture(
                    GL_TEXTURE_2D,
                    reinterpret_cast<RGBATexture *>(draw_cmd.TextureId)->id());

                glDrawElements(
                    GL_TRIANGLES, draw_cmd.ElemCount, GL_UNSIGNED_SHORT,
                    (void *)(draw_cmd.IdxOffset * sizeof(ImDrawIdx)));
            } else if (draw_cmd.UserCallback ==
                       ImDrawCallback_ResetRenderState) {
                setup_render_state(framebuffer_width, framebuffer_height,
                                   data->DisplayPos, data->DisplaySize);
            } else {
                draw_cmd.UserCallback(draw_list, &draw_cmd);
            }
        }
    }
}

void ImGuiGLRenderer::clear(ImVec4 color) {
    DCHECK(initialized_);

    GLboolean prev_scissor_enabled = glIsEnabled(GL_SCISSOR_TEST);

    glDisable(GL_SCISSOR_TEST);
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    if (prev_scissor_enabled) {
        glEnable(GL_SCISSOR_TEST);
    }
}
