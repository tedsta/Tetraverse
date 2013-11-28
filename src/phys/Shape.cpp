#include "phys/Shape.h"

#include <cassert>
#include <cfloat>

#include "phys/RigidBody.h"

namespace phys
{
    Shape::~Shape()
    {
        //dtor
    }

    // ********************************************************************************************
    // CircleShape

    CircleShape::CircleShape(float r) : radius(r)
    {
        boundingBox.left = -radius;
        boundingBox.top = -radius;
        boundingBox.width = 2*radius;
        boundingBox.height = 2*radius;
    }

    void CircleShape::computeMass(RigidBody* body, float density)
    {
        body->mass = PI * radius * radius * density;
        body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
        body->inertia = body->mass * radius * radius;
        body->inverseInertia = (body->inertia) ? 1.0f / body->inertia : 0.0f;
    }

    // ********************************************************************************************
    // PolygonShape

    void PolygonShape::computeMass(RigidBody* body, float density)
    {
        // Calculate centroid and moment of interia
        sf::Vector2f c( 0.0f, 0.0f ); // centroid
        float area = 0.0f;
        float I = 0.0f;
        const float k_inv3 = 1.0f / 3.0f;

        for(sf::Uint32 i1 = 0; i1 < vertices.size(); ++i1)
        {
            // Triangle vertices, third vertex implied as (0, 0)
            sf::Vector2f p1( vertices[i1] );
            sf::Uint32 i2 = i1 + 1 < vertices.size() ? i1 + 1 : 0;
            sf::Vector2f p2( vertices[i2] );

            float D = cross( p1, p2 );
            float triangleArea = 0.5f * D;

            area += triangleArea;

            // Use area to weight the centroid average, not just vertex position
            c += triangleArea * k_inv3 * (p1 + p2);

            float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
            float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
            I += (0.25f * k_inv3 * D) * (intx2 + inty2);
        }

        c *= 1.0f / area;

        // Translate vertices to centroid (make the centroid (0, 0)
        // for the polygon in model space)
        // Not floatly necessary, but I like doing this anyway
        for(sf::Uint32 i = 0; i < vertices.size(); ++i)
            vertices[i] -= c;

        body->mass = density * area;
        body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
        body->inertia = I * density;
        body->inverseInertia = body->inertia ? 1.0f / body->inertia : 0.0f;
    }

     // Half width and half height
    void PolygonShape::setBox(float hw, float hh)
    {
        vertices.resize(4);
        transformedVertices.resize(4);
        normals.resize(4);
        vertices[0] = sf::Vector2f(-hw, -hh);
        vertices[1] = sf::Vector2f( hw, -hh);
        vertices[2] = sf::Vector2f( hw,  hh);
        vertices[3] = sf::Vector2f(-hw,  hh);
        normals[0] = sf::Vector2f( 0.0f,  -1.0f);
        normals[1] = sf::Vector2f( 1.0f,   0.0f);
        normals[2] = sf::Vector2f( 0.0f,   1.0f);
        normals[3] = sf::Vector2f(-1.0f,   0.0f);
        boundingBox.left = -hw;
        boundingBox.width = hw*2;
        boundingBox.top = -hh;
        boundingBox.height = hh*2;
    }

    void PolygonShape::set(sf::Vector2f* verts, sf::Uint32 count)
    {
        // Find the right most point on the hull
        int rightMost = 0;
        float highestXCoord = verts[0].x;
        for(sf::Uint32 i = 1; i < count; ++i)
        {
            float x = verts[i].x;
            if(x > highestXCoord)
            {
                highestXCoord = x;
                rightMost = i;
            }
            else if(x == highestXCoord) // If matching x then take farthest negative y
                if(verts[i].y < verts[rightMost].y)
                    rightMost = i;
        }

        std::vector<int> hull(count);
        int outCount = 0;
        int indexHull = rightMost;

        for (;;)
        {
            hull[outCount] = indexHull;

            // Search for next index that wraps around the hull
            // by computing cross products to find the most counter-clockwise
            // vertex in the set, given the previos hull index
            int nextHullIndex = 0;
            for(int i = 1; i < (int)count; ++i)
            {
                // Skip if same coordinate as we need three unique
                // points in the set to perform a cross product
                if(nextHullIndex == indexHull)
                {
                    nextHullIndex = i;
                    continue;
                }

                // Cross every set of three unique vertices
                // Record each counter clockwise third vertex and add
                // to the output hull
                // See : http://www.oocities.org/pcgpe/math2d.html
                sf::Vector2f e1 = verts[nextHullIndex] - verts[hull[outCount]];
                sf::Vector2f e2 = verts[i] - verts[hull[outCount]];
                float c = cross(e1, e2);
                if(c < 0.0f)
                    nextHullIndex = i;

                // Cross product is zero then e vectors are on same line
                // therefor want to record vertex farthest along that line
                if(c == 0.0f && lengthSqr(e2) > lengthSqr(e1))
                    nextHullIndex = i;
            }

            ++outCount;
            indexHull = nextHullIndex;

            // Conclude algorithm upon wrap-around
            if (nextHullIndex == rightMost)
            {
                vertices.resize(outCount);
                transformedVertices.resize(outCount);
                normals.resize(outCount);
                break;
            }
        }

        sf::Vector2f topLeft(FLT_MAX, FLT_MAX);
        sf::Vector2f botRight(-FLT_MAX, -FLT_MAX);

        // Copy vertices into shape's vertices
        for(sf::Uint32 i = 0; i < vertices.size(); ++i)
        {
            vertices[i] = verts[hull[i]];

            topLeft.x = std::min(topLeft.x, vertices[i].x);
            topLeft.y = std::min(topLeft.y, vertices[i].y);
            botRight.x = std::max(botRight.x, vertices[i].x);
            botRight.x = std::max(botRight.y, vertices[i].y);
        }

        boundingBox.left = topLeft.x;
        boundingBox.width = botRight.x-topLeft.x;
        boundingBox.top = topLeft.y;
        boundingBox.height = botRight.y-topLeft.y;


        // Compute face normals
        for(sf::Uint32 i1 = 0; i1 < vertices.size(); ++i1)
        {
            sf::Uint32 i2 = i1 + 1 < vertices.size() ? i1 + 1 : 0;
            sf::Vector2f face = vertices[i2] - vertices[i1];

            // Ensure no zero-length edges, because that's bad
            assert(lengthSqr(face) > EPSILON * EPSILON);

            // Calculate normal with 2D cross product between vector and scalar
            normals[i1] = sf::Vector2f(face.y, -face.x);
            normals[i1] = normalize(normals[i1]);
        }
    }

    // The extreme point along a direction within a polygon
    sf::Vector2f PolygonShape::getSupport(const sf::Vector2f& dir)
    {
        float bestProjection = -FLT_MAX;
        sf::Vector2f bestVertex;

        for(sf::Uint32 i = 0; i < vertices.size(); ++i)
        {
            sf::Vector2f v = vertices[i];
            float projection = dot(v, dir);

            if(projection > bestProjection)
            {
                bestVertex = v;
                bestProjection = projection;
            }
        }

        return bestVertex;
    }
}

