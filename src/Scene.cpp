
//
// Created by 何昊 on 2018/4/28.
//

#include "Scene.h"

#include <iostream>
#include <string>

#include <assimp/postprocess.h>
#include <json.hpp>

using json = nlohmann::json;

Model::Model(const char *jsonFile)
    : PbrGameObject(jsonFile)
{
    json j;
    std::ifstream inFile(jsonFile);
    if (inFile.good()) {
        inFile >> j;
    } else {
        std::cerr << "Failed to load model file " << jsonFile << std::endl;
    }

    // Load Files according to json data
    loadModelFromAssimp(j["model_file_path"].get<std::string>().c_str());
}

void Model::loadModelFromAssimp(const char *file)
{
    std::ifstream fileIn(file);

    // Check if the file exists.
    if (fileIn.good()) {
        fileIn.close();  // The file exists.
    } else {
        fileIn.close();
        std::cerr << "Unable to open the 3D file " << file << std::endl;
        scene = nullptr;
    }

    // Load scene
    scene = importer.ReadFile(file, aiProcessPreset_TargetRealtime_Quality);

    // Check if the file is loaded successfully.
    if (!scene) {
        cout << importer.GetErrorString() << endl;
    } else {
        cout << "3D file " << file << " loaded." << endl;
    }

    // Retrieve vertex arrays from the aiScene object and bind them with VBOs and VAOs.
    unsigned int buffer;

    vaoArray.resize(scene->mNumMeshes);

    // Go through each mesh stored in the aiScene object, bind it with a VAO,
    // and save the VAO index in the vaoArray.
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* currentMesh = scene->mMeshes[i];

        glGenVertexArrays(1, &vaoArray[i]);
        glBindVertexArray(vaoArray[i]);

        if (currentMesh->HasPositions()) {
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * currentMesh->mNumVertices,
                         currentMesh->mVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(VertexAttribLocations::vPos);
            glVertexAttribPointer(VertexAttribLocations::vPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        }

        if (currentMesh->HasFaces()) {
            unsigned int *indices = new unsigned int[currentMesh->mNumFaces * currentMesh->mFaces[0].mNumIndices];

            // copy the face indices from aiScene into a 1D indices array.
            int index = 0;
            for (unsigned int j = 0; j < currentMesh->mNumFaces; j++) {
                for (unsigned int k = 0; k < currentMesh->mFaces[j].mNumIndices; k++) {
                    indices[index++] = currentMesh->mFaces[j].mIndices[k];
                }
            }

            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(unsigned int) * currentMesh->mNumFaces * currentMesh->mFaces[0].mNumIndices,
                         indices, GL_STATIC_DRAW);

            delete[] indices;
        }

        if (currentMesh->HasNormals()) {
            // Create an empty Vertex Buffer Object (VBO)
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);

            // Bind (transfer) the vertex normal array (stored in aiMesh's member variable mNormals)
            // to the VBO.
            // Note that the vertex normals are stored in a 1D array (i.e. mVertices)
            // in the aiScene object.
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * currentMesh->mNumVertices,
                         currentMesh->mNormals, GL_STATIC_DRAW);

            // Associate this VBO with an the vPos variable in the vertex shader.
            // The vertex data and the vertex shader must be connected.
            glEnableVertexAttribArray(VertexAttribLocations::vNormal);
            glVertexAttribPointer(VertexAttribLocations::vNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        }

        //**************************************
        // Set up texture mapping data

        // Each mesh may have multiple UV(texture) channels (multi-texture). Here we only use
        // the first channel. Call currentMesh->GetNumUVChannels() to get the number of UV channels
        // for this mesh.
        if (currentMesh->HasTextureCoords(0)) {
            // Create an empty Vertex Buffer Object (VBO)
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);

            // mTextureCoords is different from mVertices or mNormals. It is a 2D array, not a 1D array.
            // So we need to copy it to a 1D texture coordinate array.
            // The first dimension of this array is the texture channel for this mesh.
            // The second dimension is the vertex index number.
            // The number of texture coordinates is always the same as the number of vertices.
            float *texCoords = new float[2 * currentMesh->mNumVertices];
            unsigned int k = 0;
            for (unsigned int j = 0; j < currentMesh->mNumVertices; j++) {
                texCoords[k] = currentMesh->mTextureCoords[0][j].x;
                k++;
                texCoords[k] = currentMesh->mTextureCoords[0][j].y;
                k++;
            }

            // Bind (transfer) the texture coordinate array to the VBO.
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * currentMesh->mNumVertices,
                         texCoords, GL_STATIC_DRAW);

            // Associate this VBO with the vTextureCoord variable in the vertex shader.
            // The vertex data and the vertex shader must be connected.
            glVertexAttribPointer(VertexAttribLocations::vTexCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(VertexAttribLocations::vTexCoord);
            delete[] texCoords;
        }

        // Set Up Tangent Vector Data
        if (currentMesh->HasTangentsAndBitangents()) {
            // Tangents
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * currentMesh->mNumVertices,
                         currentMesh->mTangents, GL_STATIC_DRAW);
            glEnableVertexAttribArray(VertexAttribLocations::vTangent);
            glVertexAttribPointer(VertexAttribLocations::vTangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            // Bitangents
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * currentMesh->mNumVertices,
                         currentMesh->mBitangents, GL_STATIC_DRAW);
            glEnableVertexAttribArray(VertexAttribLocations::vBitangent);
            glVertexAttribPointer(VertexAttribLocations::vBitangent, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        }

        //Close the VAOs and VBOs for later use.
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } // end for
}

void Model::render(const glm::mat4 &vp, Camera &camera)
{
    shader.use();
    traverseRender(scene->mRootNode, vp, transform, camera);
}

void Model::traverseRender(const aiNode* node, const glm::mat4 &vp, glm::mat4 &m,
                           Camera &camera)
{
    if (!node) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = row(model, 0, glm::vec4(node->mTransformation.a1,
                                    node->mTransformation.a2,
                                    node->mTransformation.a3,
                                    node->mTransformation.a4));
    model = row(model, 1, glm::vec4(node->mTransformation.b1,
                                    node->mTransformation.b2,
                                    node->mTransformation.b3,
                                    node->mTransformation.b4));
    model = row(model, 2, glm::vec4(node->mTransformation.c1,
                                    node->mTransformation.c2,
                                    node->mTransformation.c3,
                                    node->mTransformation.c4));
    model = row(model, 3, glm::vec4(node->mTransformation.d1,
                                    node->mTransformation.d2,
                                    node->mTransformation.d3,
                                    node->mTransformation.d4));

    // Multiply the parent's node's model matrix with this node's model matrix.
    // To get the model matrix of this mesh in world coordinate
    glm::mat4 currentTransform = m * model;

    if (node->mNumMeshes > 0) {

        // Create a normal matrix to transform normals.
        // We don't need to include the view matrix here because the lighting is done
        // in world space.
        glm::mat3 normalMatrix = glm::mat3(inverseTranspose(currentTransform));

        // Draw all the meshes associated with the current node.
        // Certain node may have multiple meshes associated with it.
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // This is the index of the mesh associated with this node.
            int meshIndex = node->mMeshes[i];

            const aiMesh* currentMesh = scene->mMeshes[meshIndex];

            // Vertex shader data
            shader.setMat4("vp", vp);
            shader.setMat4("model", currentTransform);
            shader.setMat3("normalMatrix", normalMatrix);

            // Fragment shader data
            shader.setInt("albedoIsSRGB", albedoIsSRGB);
            shader.setInt("albedoMap", TextureChannel::albedo);
            albedo.useTextureUnit(TextureChannel::albedo);

            shader.setInt("normalIsSRGB", normalIsSRGB);
            shader.setInt("normalMap", TextureChannel::normal);
            normal.useTextureUnit(TextureChannel::normal);

            shader.setInt("metallicSmoothnessIsSRGB", metallicSmoothnessIsSRGB);
            shader.setInt("metallicSmoothnessMap", TextureChannel::metallicSmoothness);
            metallicSmoothness.useTextureUnit(TextureChannel::metallicSmoothness);
            shader.setFloat("smoothnessFactor", smoothnessFactor);

            shader.setInt("aoMap", TextureChannel::ao);
            ao.useTextureUnit(TextureChannel::ao);
            shader.setBool("hasAO", hasAO);

            shader.setBool("hasHeightMap", hasHeightMap);
            shader.setInt("heightMap", TextureChannel::height);
            heightMap.useTextureUnit(TextureChannel::height);

            shader.setInt("irradianceMap", TextureChannel::irradiance);
            glActiveTexture(GL_TEXTURE0 + TextureChannel::irradiance);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance);

            shader.setInt("prefilterMap", TextureChannel::prefilter);
            glActiveTexture(GL_TEXTURE0 + TextureChannel::prefilter);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter);

            shader.setInt("brdfLUT", TextureChannel::brdfLUT);
            glActiveTexture(GL_TEXTURE0 + TextureChannel::brdfLUT);
            glBindTexture(GL_TEXTURE_2D, brdfLUT);

            shader.setVec3("camPos", camera.Position);

            // The 0th component is directional light
            shader.setInt("lightCount", lightCount);
            for (int i = 0; i < lightCount; ++i) {
                shader.setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
                shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            }

            // This mesh should have already been associated with a VAO in a previous function.
            // Note that mMeshes[] array and the vaoArray[] array are in sync.
            // That is, for mesh #0, the corresponding VAO index is stored in vaoArray[0], and so on.
            // Bind the corresponding VAO for this mesh.
            glBindVertexArray(vaoArray[meshIndex]);

            unsigned int numFaces = currentMesh->mNumFaces;
            unsigned int numIndicesPerFace = currentMesh->mFaces[0].mNumIndices;

            glDrawElements(GL_TRIANGLES, (numFaces * numIndicesPerFace), GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
        }

    } // end if (node->mNumMeshes > 0)

    // Recursively visit and draw all the child nodes. This is a depth-first traversal.
    // Even if this node does not contain mesh, we still need to pass down the transformation matrix.
    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        traverseRender(node->mChildren[j], vp, currentTransform, camera);
    }
    return;
}