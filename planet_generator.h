#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>
#include <glad/glad.h>

#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       800

/*
This class generates the sphere which will serve as Earth for a more interactive satellite tracking model. It will also map the texture,and contain a vector-only
mode at some point.
*/

class PlanetGenerator
{
   public:
    PlanetGenerator(float radius, unsigned int rings, unsigned int sectors);
    void draw(GLfloat x, GLfloat y, GLfloat z);
    void outputAsText();
    protected:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLushort> indices;
    float rads;
    unsigned int ri;
    unsigned int sec;
    private:
};

/*
How do spheres work
For the sake of convienience, Earth will be a sphere instead of an oblate spheroid
Calculate circles-
Circumference = 2(pi)*r

Radius = 3959   miles
Diameter = 7,926 miles
Circumference at equator = 24,859.73 miles

Surface Area Formula = 4(pi)r^2 = (C^2)/pi

Surface Area = 4 * pi * 15,673,681
Surface Area = 196,961,284.33 miles

*/