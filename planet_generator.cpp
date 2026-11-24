#include <cstdio>
#include "planet_generator.h"
#include "orbital_math.h"


PlanetGenerator::PlanetGenerator(float radius, unsigned int rings, unsigned int sectors)
{
    this->rads = radius;
    this->ri    = rings;
    this->sec  = sectors;

    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    int r, s;

    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    texcoords.resize(rings * sectors * 2);
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator n = normals.begin();
    std::vector<GLfloat>::iterator t = texcoords.begin();

    for(r = 0; r < rings; r++)
    {
        for(s = 0; s < sectors; s++)
        {
            float const y = sin(-M_PI_2 + M_PI * r * R);
            float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
            float const z = cos(2*M_PI * s * S) * sin(M_PI * r * R);
            
            *t++ = s*S;
            *t++ = r*R;

            *v++ = x * radius;
            *v++ = y * radius;
            *v++ = z * radius;

            *n++ = x;
            *n++ = y;
            *n++ = z;

        }
        indices.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings; r++)
        {
            for(s = 0; s < sectors; s++)
            {
                *i++ = r * sectors + s;
                *i++ = r *sectors + (s+1);
                *i++ = (r+1) * sectors + (s+1);
                *i++ = (r+1) * sectors + s;
            }
        }
    }
}

void PlanetGenerator::draw(GLfloat x, GLfloat y, GLfloat z)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
    glNormalPointer(GL_FLOAT,0, &normals[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
    glPopMatrix();
}

void PlanetGenerator::outputAsText()
{
    std::printf("rings=%u sectors=%u radius=%f\n", this->ri, this->sec, this->rads);
    std::printf("vertices.size()=%zu indices.size()=%zu\n", vertices.size(), indices.size());
    for (int k = 0; k < 12 && k < (int)vertices.size(); k += 3)
        std::printf("v: %f %f %f\n", vertices[k], vertices[k+1], vertices[k+2]);
}
