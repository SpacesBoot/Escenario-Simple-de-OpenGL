#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ahmmm una sola palabra pereza, no quiero hacer esto en medio codigo de renderizado

std::vector<float> generateGridVertices(int slices, float size) {
    std::vector<float> vertices;
    float halfSize = size / 2.0f;
    float step = size / slices;

    for (int i = 0; i <= slices; ++i) {
        float pos = -halfSize + i * step;

        vertices.push_back(pos);      vertices.push_back(0.0f); vertices.push_back(-halfSize);
        vertices.push_back(pos);      vertices.push_back(0.0f); vertices.push_back(halfSize);

        vertices.push_back(-halfSize); vertices.push_back(0.0f); vertices.push_back(pos);
        vertices.push_back(halfSize);  vertices.push_back(0.0f); vertices.push_back(pos);
    }
    return vertices;
}

// para tener orden y legibilidad xD

unsigned int loadShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

// preparamos esto pa despues

struct Cam {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creamos pantalla 

    GLFWwindow* window = glfwCreateWindow(800, 600, "Un escenario simple xD", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // todos estos modelos osea los vertices los hizo gemini pq no mamen no voy a ponerme e escribir -0.5 durante media hora

    float SphereModel[] = {
        0.0f, 0.5f, 0.0f,   -0.35f, 0.35f, 0.35f,   0.35f, 0.35f, 0.35f,
        0.0f, 0.5f, 0.0f,    0.35f, 0.35f, 0.35f,   0.35f, 0.35f,-0.35f,
        0.0f, 0.5f, 0.0f,    0.35f, 0.35f,-0.35f,  -0.35f, 0.35f,-0.35f,
        0.0f, 0.5f, 0.0f,   -0.35f, 0.35f,-0.35f,  -0.35f, 0.35f, 0.35f,

        -0.35f, 0.35f, 0.35f,  -0.5f, 0.0f, 0.0f,   0.0f, 0.0f, 0.5f,
         0.35f, 0.35f, 0.35f,   0.0f, 0.0f, 0.5f,   0.5f, 0.0f, 0.0f,
         0.35f, 0.35f,-0.35f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,-0.5f,
        -0.35f, 0.35f,-0.35f,   0.0f, 0.0f,-0.5f,  -0.5f, 0.0f, 0.0f,

        -0.5f, 0.0f, 0.0f,   -0.35f,-0.35f, 0.35f,   0.0f, 0.0f, 0.5f,
         0.5f, 0.0f, 0.0f,    0.0f, 0.0f, 0.5f,      0.35f,-0.35f, 0.35f,
         0.5f, 0.0f, 0.0f,    0.35f,-0.35f,-0.35f,   0.0f, 0.0f,-0.5f,
        -0.5f, 0.0f, 0.0f,    0.0f, 0.0f,-0.5f,     -0.35f,-0.35f,-0.35f,

        0.0f,-0.5f, 0.0f,   -0.35f,-0.35f, 0.35f,   0.35f,-0.35f, 0.35f,
        0.0f,-0.5f, 0.0f,    0.35f,-0.35f, 0.35f,   0.35f,-0.35f,-0.35f,
        0.0f,-0.5f, 0.0f,    0.35f,-0.35f,-0.35f,  -0.35f,-0.35f,-0.35f,
        0.0f,-0.5f, 0.0f,   -0.35f,-0.35f,-0.35f,  -0.35f,-0.35f, 0.35f
    };

    float CubeModel[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,

        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,

         0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,

        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
         0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,

        -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
    };

    float PyramidModel[] = {
         0.0f,  0.5f,  0.0f,   -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f,
         0.0f,  0.5f,  0.0f,    0.5f, -0.5f,  0.5f,    0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,    0.5f, -0.5f, -0.5f,   -0.5f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.0f,   -0.5f, -0.5f, -0.5f,   -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f,    0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,   -0.5f, -0.5f,  0.5f,   -0.5f, -0.5f, -0.5f
    };

    float HouseModel[] = {
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.0f,-0.5f,
     0.5f, 0.0f,-0.5f, -0.5f, 0.0f,-0.5f, -0.5f,-0.5f,-0.5f,

    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.0f, 0.5f,
     0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, -0.5f,-0.5f, 0.5f,

    -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.0f, 0.5f,

     0.5f, 0.0f, 0.5f,  0.5f, 0.0f,-0.5f,  0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.0f, 0.5f,

    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,

    -0.5f, 0.0f,-0.5f,  0.5f, 0.0f,-0.5f,  0.5f, 0.0f, 0.5f,
     0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,-0.5f,

    -0.5f, 0.0f,-0.5f,  0.5f, 0.0f,-0.5f,  0.0f, 0.5f, 0.0f,
     0.5f, 0.0f,-0.5f,  0.5f, 0.0f, 0.5f,  0.0f, 0.5f, 0.0f,
     0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f,  0.0f, 0.5f, 0.0f,
    -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,-0.5f,  0.0f, 0.5f, 0.0f
    };

    std::vector<float> gridVertices = generateGridVertices(20, 20.0f);

    unsigned int sphereVAO, sphereVBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SphereModel), SphereModel, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeModel), CubeModel, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidModel), PyramidModel, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int houseVAO, houseVBO;
    glGenVertexArrays(1, &houseVAO);
    glGenBuffers(1, &houseVBO);
    glBindVertexArray(houseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HouseModel), HouseModel, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // cargamos shaders

    unsigned int shaderProgram = loadShaders("shader.vs", "shader.fs");

    glEnable(GL_DEPTH_TEST);

    // esto es pa poder hacer el movimiento w/a/s/d de la camara

    Cam cam = { 0, 0, 0 };

    // bucle de juego

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 3.0f, 6.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, cam.z));
        view = glm::rotate(view, (float)cam.x, glm::vec3(0.0f, 0.1f, 0.0f));
        view = glm::rotate(view, (float)cam.y, glm::vec3(0.1f, 0.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Controles de la camara
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam.z += 0.001f;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.z -= 0.001f;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cam.x += 0.001f;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cam.x -= 0.001f;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            cam.y += 0.0005f;

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            cam.y -= 0.0005f;

        // estos son esas cosas llamadas uniforms que son como variables que usamos en los shaders xD

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // el grid esa madre gris que usa unity, blender etc que parece una malla

        glm::mat4 modelGrid = glm::mat4(1.0f);
        modelGrid = glm::translate(modelGrid, glm::vec3(0.0f, -0.2f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelGrid));
        glUniform4f(colorLoc, 0.4f, 0.4f, 0.4f, 1.0f);

        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertices.size() / 3));

        // Los Primitivos

        glm::mat4 modelSphere = glm::mat4(1.0f);
        modelSphere = glm::translate(modelSphere, glm::vec3(-2.0f, 0.0f, 0.0f));
        modelSphere = glm::rotate(modelSphere, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));
        glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f);

        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, 1028);

        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::translate(modelCube, glm::vec3(-0.6f, 0.0f, 0.0f));
        modelCube = glm::rotate(modelCube, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));
        glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 modelPyramid = glm::mat4(1.0f);
        modelPyramid = glm::translate(modelPyramid, glm::vec3(1.5f, 0.0f, 0.0f));
        modelPyramid = glm::rotate(modelPyramid, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPyramid));
        glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // esta la añadi porque si pero esta compuesta de primitivos mas no es uno

        glm::mat4 modelHouse = glm::mat4(1.0f);
        modelHouse = glm::translate(modelHouse, glm::vec3(3.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelHouse));
        glUniform4f(colorLoc, 0.8f, 0.8f, 0.0f, 1.0f);

        glBindVertexArray(houseVAO);
        glDrawArrays(GL_TRIANGLES, 0, 64);

        // los poll events o esta cosa detecta cada que haces algo y lo registra por ejemplo cuando presionas una tecla

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberamos espacio, esto es importante porque (es anecdota) tu .exe o ejecutable se va a comer la ram peor que windows o simplemente por usar mas ram de lo normal crashea

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &houseVAO);
    glDeleteBuffers(1, &houseVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}