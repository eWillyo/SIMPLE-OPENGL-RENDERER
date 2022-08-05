#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <vector>

#include "model.h"
#include "image.h"


GLuint vbo[NUM_VBs] = { 0, 0, 0 };
GLuint vao = 0;
GLuint shader_programme = 0;
GLuint vs, fs;

int w = 800, h = 600;

GLuint my_texture = 0;


void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW ERROR: code %i msg: %s\n", error, description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    w = width;
    h = height;

    glViewport(0, 0, w, h);
}

int main() {

    glfwSetErrorCallback(glfw_error_callback);

    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(w, h, "Simple OpenGL renderer", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glClearColor(0.5f, 0.5f, .0f, 1.f);

    // -------------------------------------------------------------------------

    if (!create_model(&vao, vbo, &vs, &fs, &shader_programme))
    {
        destroy_model(&vao, vbo, &vs, &fs, &shader_programme);
        glfwTerminate();
        return 1;
    }

    // load texture
    my_texture = load_2D_texture("C:\\programming\\SIMPLE OPENGL RENDERER\\me.jpg");
    if (my_texture == 0) {
        return false;
    }

    // -------------------------------------------------------------------------

    //glUseProgram(shader_programme);
    GLint mvploc = glGetUniformLocation(shader_programme, "MVP");
    GLint tex1loc = glGetUniformLocation(shader_programme, "tex1");
    //glUseProgram(0);

    float angle = 0.0f;
    double time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //PROJECTION
        glm::mat4 Projection = glm::perspective(65.0f, (float)w / (float)h, 0.1f, 100.0f);
        //VIEW
        glm::mat4 View = glm::mat4(1.);
        View = glm::translate(View, glm::vec3(0.0f, 0.0f, -1.8f));
        //MODEL
        glm::mat4 Model = glm::mat4(1.0);
        //Rotate by Y axis
        Model = glm::rotate(Model, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
        angle += (float)(glfwGetTime() - time) * 40.f;
        time = glfwGetTime();
        //Scale by factor 0.5
        Model = glm::scale(Model, glm::vec3(2.2f));
        glm::mat4 mvp = Projection * View * Model;

        glUseProgram(shader_programme);
       
        glUniformMatrix4fv(mvploc, 1, GL_FALSE, &mvp[0][0]);
        glUniform1i(tex1loc, 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, my_texture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    // close GL context and any other GLFW resources
    destroy_model(&vao, vbo, &vs, &fs, &shader_programme);
    glDeleteTextures(1, &my_texture);
    glfwTerminate();
    return 0;
}