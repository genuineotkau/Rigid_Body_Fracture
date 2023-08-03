/**
    Reference Material: https://blog.csdn.net/manipu1a/article/details/85340045
**/

#ifndef MESH_H
#define MESH_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Material {
    //ambient
    glm::vec4 Ka;
    //diffuse
    glm::vec4 Kd;
    //specular
    glm::vec4 Ks;
    //specular coff
    float Ns;
    //optical density
    float Ni;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

// Triangle struct to store vertices and compute normal, texCoords, tangent, and bitangent
struct Triangle {
    Vertex v0, v1, v2;
    glm::vec3 center;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
        : v0(v0), v1(v1), v2(v2) {
        // Compute center
        center = (v0.Position + v1.Position + v2.Position) / 3.0f;

        // Compute normal
        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;
        normal = glm::normalize(glm::cross(edge1, edge2));

        // Compute texCoords, tangent, and bitangent at the center using barycentric interpolation
        // For the center of the triangle, each vertex contributes equally, so the weights are all 1/3
        float weight0 = 1.0f / 3.0f;
        float weight1 = 1.0f / 3.0f;
        float weight2 = 1.0f / 3.0f;

        texCoords = weight0 * v0.TexCoords + weight1 * v1.TexCoords + weight2 * v2.TexCoords;
        tangent = glm::normalize(weight0 * v0.Tangent + weight1 * v1.Tangent + weight2 * v2.Tangent);
        bitangent = glm::normalize(weight0 * v0.Bitangent + weight1 * v1.Bitangent + weight2 * v2.Bitangent);
    }
};


class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    Material             material;
    vector<glm::vec3>    verticePositions;
    vector<Triangle> triangles;
    unsigned int VAO;
    unsigned int uniformBlockIndex; 

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material mat)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->material = mat;
        for (int i = 0; i < vertices.size(); ++i) {
            verticePositions.push_back(vertices[i].Position);
        }

        if (indices.size() % 3 != 0) {
			cout << "Error: indices number is not a multiple of 3" << endl;
		}

        for (int i = 0; i < indices.size(); i += 3) {
            Vertex v0 = vertices[indices[i]];
            Vertex v1 = vertices[indices[i + 1]];
            Vertex v2 = vertices[indices[i + 2]];
            triangles.push_back(Triangle(v0, v1, v2));
        }

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if (name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if (name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // draw mesh
        glBindVertexArray(VAO);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockIndex, 0, sizeof(Material));
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void DrawBoundingVolume()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
        //glEnable(GL_DEPTH_TEST);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &uniformBlockIndex);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + sizeof(material), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(material), (void*)(&material), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif