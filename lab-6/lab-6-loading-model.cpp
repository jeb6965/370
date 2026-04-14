#include <gl/glew.h> // include GLEW and new version of GL on Windows
#include <glfw/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int main() {
    return 0;
}

int countLabel(std::string modelName, char label[]){
    int numLab = 0;
    FILE *objFile;
    objFile = fopen(modelName.c_str(),"r");
    char buf[128];
    while (fscanf(objFile, "%s", &buf) != EOF){
        //cout << buf << endl;
        if (strcmp(buf,label) == 0)
        numLab++;
        }
    std::cout << "Model has " << numLab << " " << label << "\n";
    fclose(objFile);
    return numLab;
}

void loadVertices(std::string modelName, GLfloat verts[]){
    std::cout << "Loading vertices\n";
    int numVert = 0;
    FILE *objFile;
    objFile = fopen(modelName.c_str(),"r");
    float maxX = -100000000.0;
    float minX = 100000000.0;
    float maxY = -100000000.0;
    float minY = 100000000.0;
    float maxZ = -100000000.0;
    float minZ = 100000000.0;
    char buf[128];
    char label[] = "v";
    float a, b, c;
    while (fscanf(objFile, "%s", &buf) != EOF){
        if (strcmp(buf,label) == 0){
            //cout << "vertex" << endl;
            fscanf(objFile, "%f %f %f\n", &a, &b, &c);
            //cout << "a,b,c: " << a << ", " << b << ", " << c << endl;
            if (a > maxX)
                maxX = a;
            if (a < minX)
                minX = a;
            if (b > maxY)
                maxY = b;
            if (b < minY)
                minY = b;
            if (c > maxZ)
                maxZ = c;
            if (c < minZ)
                minZ = c;
        verts[3*numVert + 0] = 1.0*a;
        verts[3*numVert + 1] = 1.0*b;
        verts[3*numVert + 2] = 1.0*c;
        numVert++;
        }
    }
    float scaleX = maxX-minX;
    float scaleY = maxY-minY;
    float scaleZ = maxZ-minZ;
    float transX = 0.5*(maxX+minX);
    float transY = 0.5*(maxY+minY);
    float transZ = 0.5*(maxZ+minZ);
    std::cout << "scales: " << scaleX << ", " << scaleY << ", " << scaleZ <<
    std::endl;
    float uniScale = std::max(std::max(scaleX, scaleY), scaleZ);
    for (int i = 0; i < numVert; i++){
        verts[3*i+0] = (verts[3*i+0] - transX)/uniScale;
        verts[3*i+1] = (verts[3*i+1] - transY)/uniScale;
        verts[3*i+2] = (verts[3*i+2] - transZ)/uniScale;
    }
    fclose(objFile);
    std::cout << "Done loading vertices\n";
}
void loadFaces(std::string modelName, GLint faces[]){
    std::cout << "Loading faces\n";
    int numFaces = 0;
    FILE *objFile;
    objFile = fopen(modelName.c_str(),"r");
    char buf[128];
    char label[] = "f";
    GLfloat a, b, c;
    while (fscanf(objFile, "%s", &buf) != EOF){
        if (strcmp(buf,label) == 0){
            fscanf(objFile, "%f %f %f\n", &a, &b, &c);
            faces[3*numFaces + 0] = a-1;
            faces[3*numFaces + 1] = b-1;
            faces[3*numFaces + 2] = c-1;
            numFaces++;
        }
    }
    fclose(objFile);
    std::cout << "Done loading faces\n";
}