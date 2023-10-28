#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include <iostream>
#include <vector>


void renderMesh(const aiMesh *mesh) {
    std::vector<float> vertices;
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
    }

    // Create OpenGL buffers and copy data
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw the mesh
    glDrawArrays(GL_TRIANGLES, 0, mesh->mNumVertices);

    // Cleanup
    glDeleteBuffers(1, &VBO);
}


int main()
{
    // Create a window with SFML
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::Window window(sf::VideoMode(800, 600), "OpenGL with SFML", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // Initialize OpenGL states
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glEnable(GL_DEPTH_TEST); // Enable depth tests

    // Initialize Assimp and load the model
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("./Package/RedFighter.obj", aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return -1;
    }
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Main loop
    while (window.isOpen()) {
        // Your existing event handling and buffer clearing code...

        // Render the first mesh in the scene
        if (scene->mNumMeshes > 0) {
            renderMesh(scene->mMeshes[0]);
        }

        // Swap buffers to display what was drawn
        window.display();
    }

    return 0;
}
