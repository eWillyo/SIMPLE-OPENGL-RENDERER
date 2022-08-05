#include "model.h"
#include <vector>


const char* vertex_shader =
"#version 420\n"
"layout (location = 0) in vec3 position;"
"layout (location = 1) in vec2 uvs;"
"layout (location = 0) out vec2 UVs;"
"uniform mat4 MVP;"
"void main() {"
"  UVs = uvs;"
"  gl_Position = MVP * vec4(position, 1.0);"
"}";

const char* fragment_shader =
"#version 420\n"
"layout (location = 0) in vec2 UVs;"
"layout (location = 0) out vec4 frag_color;"
"layout (binding = 0) uniform sampler2D tex1;"
"void main() {"
//"  frag_color = vec4(1.0, 0.0, 0.0, 1.0);"
"  frag_color = texture(tex1, UVs);"
"}";

float points[] = {
    0.0f,  0.5f,  0.0f,
    0.5f, -0.5f,  0.0f,
    -0.5f, -0.5f,  0.0f
};

float uvs[] = {
    0.5f, 1.2f,
    1.2f, 0.4f,
    -0.2f, 0.4f
};

unsigned int indices[] = { 0, 1, 2 };


GLuint load_shader(GLenum type, const char* shaderSrc)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    if (shader == 0)
    {
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool create_model(  GLuint* vao, 
                    GLuint* vbo, 
                    GLuint* vs, 
                    GLuint* fs,     
                    GLuint* shader_programme)
{
    // BUFFERS
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(NUM_VBs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);


    /*
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[INDEX_BUFFER]);
    // Specify the data format of attribute channel 0
    glBindBuffer(GL_ARRAY_BUFFER, vbo[POS_VB]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[TEXCOORD_VB]);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // end of storing vertex binding information into VertexArray
    glBindVertexArray(0);

    glDisableVertexAttribArray(0);*/
    // VAO
    // BUFFERS

    // SHADER
    *vs = load_shader(GL_VERTEX_SHADER, vertex_shader);
    *fs = load_shader(GL_FRAGMENT_SHADER, fragment_shader);

    if (*vs == 0 || *fs == 0)
    {
        fprintf(stderr, "Can't compile shader(s).");
        return false;
    }

    *shader_programme = glCreateProgram();
    glAttachShader(*shader_programme, *fs);
    glAttachShader(*shader_programme, *vs);
    
    // Bind "position" to attribute 0	
    glBindAttribLocation(*shader_programme, 0, "position");

    glLinkProgram(*shader_programme);

    GLint isLinked = 0;
    glGetProgramiv(*shader_programme, GL_LINK_STATUS, &isLinked);
    if (!isLinked)
    {
        GLint infoLen = 0;
        glGetProgramiv(*shader_programme, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(*shader_programme, infoLen, NULL, infoLog);
            fprintf(stderr, "Error linking program:\n%s\n", infoLog);

            free(infoLog);
        }
        glDeleteProgram(*shader_programme);
        return false;
    }
    // SHADER

    return true;
}

void destroy_model( GLuint* vao, 
                    GLuint* vbo, 
                    GLuint* vs, 
                    GLuint* fs, 
                    GLuint* shader_programme)
{
    glUseProgram(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDetachShader(*shader_programme, *vs);
    glDetachShader(*shader_programme, *fs);
    glDeleteProgram(*shader_programme);
    glDeleteShader(*vs);
    glDeleteShader(*fs);
    glDeleteBuffers(NUM_VBs, vbo);
    glDeleteVertexArrays(1, vao);

    //free(vertexsource);
    //free(fragmentsource);
}