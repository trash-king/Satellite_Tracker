#include "orbit_generator.h"
#include <SDL2/SDL.h>
#include <GL/glu.h>
#include <cmath>

//cx - center X coord
//cy - center Y coord
//rx - Horizontal radius - distance from center to edge on x-axis
//ry - Vertical radius - distance from center to edge on y-axis

OrbitGenerator::OrbitGenerator()
{

}

OrbitGenerator::drawEllipse(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = 2 * M_PI / float(num_segments);
    float c = cosf(theta);
    float s = sinf(theta);
    float t;

    float x = 1;
    float y = 0;

    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < num_segments; i++)
    {
        glVertex2f(x * rx + cx, y * ry + cy);
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

OrbitGenerator::drawEllipseAtInclination(float angle, float cx, float rx, float ry, int num_segments)
{

    
}


/*/
Earth is not at the center of an orbit ellipse. It sits at one focus. The focus is offset from the center by c = a·e, where e is the eccentricity. So if you want Earth at the origin, set cx = -a * e (or +, depending on where perigee should point) and cy = 0.
Convert orbital elements to radii. Use rx = a (the semi-major axis) and ry = a * sqrt(1 - e*e) (the semi-minor axis). A circular orbit has e = 0, which makes rx == ry.
*/

//Remember to rotate the ellipse by inclination