#include <GLES2/gl2.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#define BUFFER_LEN 2048
#define MAX_QUADS 5

static uint8_t BUFFER[BUFFER_LEN] = {0};
static uint16_t INDEX_BUFFER[MAX_QUADS * 6];
static uint8_t WHITE_TEXTURE[] = {0xff, 0xff, 0xff, 0xff};


static constexpr const char *VERTEX_SHADER = ""
                                             "#version 100\n"

                                             "precision mediump float;\n"

                                             "attribute vec2 aPosition;\n"
                                             "attribute vec4 aColor;\n"
                                             "attribute vec2 aTexCoord;\n"

                                             "varying vec4 vColor;\n"
                                             "varying vec2 vTexCoord;\n"

                                             "void main() {\n"
                                             "    gl_Position = vec4(aPosition, 0, 1);\n"

                                             "    vColor = aColor;\n"
                                             "    vTexCoord = aTexCoord;\n"
                                             "}";

static constexpr const char *FRAGMENT_SHADER = ""
                                               "#version 100\n"
                                               "precision mediump float;\n"

                                               "varying vec4 vColor;\n"
                                               "varying vec2 vTexCoord;\n"

                                               "uniform sampler2D uTexture;\n"

                                               "void main() {\n"
                                               "    gl_FragColor = vColor * texture2D(uTexture, vTexCoord);\n"
                                               "}\n";

struct Vertex
{
    glm::vec2 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
};

struct Context
{
    unsigned int VertexBufferId;
    unsigned int IndexBufferId;
    unsigned int WhiteTextureId;
    unsigned int ShaderProgramId;
    Vertex QuadVertexBufferBase[MAX_QUADS * 4];
    Vertex *QuadVertexBufferPtr;
    uint32_t QuadIndexCount;
};

static Context s_Context{};

static inline void FillIndexBuffer() {
    uint32_t offset = 0;

    for (uint32_t i = 0; i < MAX_QUADS * 6; i += 6) {
        INDEX_BUFFER[i + 0] = offset + 0;
        INDEX_BUFFER[i + 1] = offset + 1;
        INDEX_BUFFER[i + 2] = offset + 2;

        INDEX_BUFFER[i + 3] = offset + 2;
        INDEX_BUFFER[i + 4] = offset + 3;
        INDEX_BUFFER[i + 5] = offset + 0;

        offset += 4;
    }
}

static inline unsigned int CreateTexture(int width, int height, uint8_t *data) {
    unsigned int id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return id;
}

static inline void SetVertexBufferLayout() {
    size_t offset = 0;
    size_t stride = sizeof(float) * 8;

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    offset += sizeof(float) * 2;

    // Color / Tint
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    offset += sizeof(float) * 4;

    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    offset += sizeof(float) * 2;
}

static inline void CompileShaders() {
    s_Context.ShaderProgramId = glCreateProgram();

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &VERTEX_SHADER, 0);
    glCompileShader(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER, 0);
    glCompileShader(fragment_shader);

    glAttachShader(s_Context.ShaderProgramId, vertex_shader);
    glAttachShader(s_Context.ShaderProgramId, fragment_shader);
    glLinkProgram(s_Context.ShaderProgramId);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

// Accessible functions
void Cleanup() {
    glDeleteTextures(1, &s_Context.WhiteTextureId);

    glDeleteBuffers(1, &s_Context.VertexBufferId);
    glDeleteBuffers(1, &s_Context.IndexBufferId);

    glDeleteProgram(s_Context.ShaderProgramId);
}

void SetupScene() {
    // Open GL base stuff
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1, 0.1, 0.1, 1);

    CompileShaders();
    glUseProgram(s_Context.ShaderProgramId);

    // Create vertex buffer
    glGenBuffers(1, &s_Context.VertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, s_Context.VertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, BUFFER_LEN, nullptr, GL_DYNAMIC_DRAW);
    SetVertexBufferLayout();

    // Create index buffer
    FillIndexBuffer();
    glGenBuffers(1, &s_Context.IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Context.IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_QUADS * 6 * sizeof(uint16_t), (void *)INDEX_BUFFER, GL_STATIC_DRAW);

    // Generate a white 1x1 texture
    s_Context.WhiteTextureId = CreateTexture(1, 1, WHITE_TEXTURE);

    s_Context.QuadVertexBufferPtr = s_Context.QuadVertexBufferBase;

    // Add a quad to our buffer
    s_Context.QuadVertexBufferPtr->Position = glm::vec2{-0.5f, -0.5f};
    s_Context.QuadVertexBufferPtr->Color = glm::vec4{1.f, 1.f, 1.f, 1.f};
    s_Context.QuadVertexBufferPtr->TexCoord = glm::vec2{0.f, 0.f};
    s_Context.QuadVertexBufferPtr++;

    s_Context.QuadVertexBufferPtr->Position = glm::vec2{-0.5f, 0.5f};
    s_Context.QuadVertexBufferPtr->Color = glm::vec4{1.f, 1.f, 1.f, 1.f};
    s_Context.QuadVertexBufferPtr->TexCoord = glm::vec2{0.f, 1.f};
    s_Context.QuadVertexBufferPtr++;

    s_Context.QuadVertexBufferPtr->Position = glm::vec2{0.5f, 0.5f};
    s_Context.QuadVertexBufferPtr->Color = glm::vec4{1.f, 1.f, 1.f, 1.f};
    s_Context.QuadVertexBufferPtr->TexCoord = glm::vec2{1.f, 1.f};
    s_Context.QuadVertexBufferPtr++;

    s_Context.QuadVertexBufferPtr->Position = glm::vec2{0.5f, -0.5f};
    s_Context.QuadVertexBufferPtr->Color = glm::vec4{1.f, 1.f, 1.f, 1.f};
    s_Context.QuadVertexBufferPtr->TexCoord = glm::vec2{1.f, 0.f};
    s_Context.QuadVertexBufferPtr++;

    s_Context.QuadIndexCount += 6;
}

void RenderScene() {
    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    // Bind everything
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_Context.WhiteTextureId);

    size_t data_size = (uint8_t *)s_Context.QuadVertexBufferPtr - (uint8_t *)s_Context.QuadVertexBufferBase;
    glBindBuffer(GL_ARRAY_BUFFER, s_Context.VertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, data_size, s_Context.QuadVertexBufferBase, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Context.IndexBufferId);
    glDrawElements(GL_TRIANGLES, s_Context.QuadIndexCount, GL_UNSIGNED_SHORT, nullptr);
}
