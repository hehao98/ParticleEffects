/*
This is a utility program that helps OpenGL programmers check their program for errors.
The following functions are provided.

// Print the content of an aiScene object.
// Call this function after Assimp::Importer.ReadFile().
void printAiSceneInfo(const aiScene* scene, AiScenePrintOption option);

Written by Ying Zhu
Department of Computer Science
Georgia State University

2014

Some Portion modified by He Hao.

*/

#ifndef ASSIMP_UTILITIES_H
#define ASSIMP_UTILITIES_H

#include <assimp/scene.h>

#include <iostream>
#include <fstream>

enum AiScenePrintOption { PRINT_AISCENE_SUMMARY, PRINT_AISCENE_DETAIL };

using namespace std;

void printVector3D(string name, aiVector3D vector);

void printColor3D(string name, aiColor3D color);

void indent(unsigned int layer);

void printMatrix4x4(aiMatrix4x4 matrix, unsigned int layer = 0);

void printNodeTree(const aiNode* node, unsigned int layer);

void printAiSceneInfo(const aiScene* scene, AiScenePrintOption option = PRINT_AISCENE_SUMMARY);


#endif

