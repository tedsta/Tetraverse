#include "TetraCollision.h"

#include <cfloat>
#include <cstring> // for memcpy
#include <cassert>

#include "phys/Shape.h"

sf::Vector2f calcSupportPoint(sf::Vector2f* vertices, int vertexCount, sf::Vector2f dir)
{
    float bestProjection = -FLT_MAX;
    sf::Vector2f bestVertex;

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        sf::Vector2f v = vertices[i];
        float projection = phys::dot(v, dir);

        if(projection > bestProjection)
        {
            bestVertex = v;
            bestProjection = projection;
        }
    }

    return bestVertex;
}

float findAxisLeastPenetration(int* faceIndex, sf::Vector2f* a, sf::Vector2f* an, int aCount, sf::Vector2f* b, int bCount)
{
    float bestDistance = -FLT_MAX;
    sf::Uint32 bestIndex;

    for (sf::Uint32 i = 0; i < aCount; ++i)
    {
        // Retrieve a face normal from A
        sf::Vector2f n = an[i];

        // Retrieve support point from B along -n
        sf::Vector2f s = calcSupportPoint(b, bCount, -n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        sf::Vector2f v = a[i];

        // Compute penetration distance (in B's model space)
        float d = phys::dot( n, s - v );

        // Store greatest distance
        if(d > bestDistance)
        {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIndex = bestIndex;
    return bestDistance;
}

void findIncidentFace(sf::Vector2f* v, sf::Vector2f* ref, sf::Vector2f* refNormals, int refCount,
                      sf::Vector2f* inc, sf::Vector2f* incNormals, int incCount, int referenceIndex)
{
    sf::Vector2f referenceNormal = refNormals[referenceIndex];

    // Find most anti-normal face on incident polygon
    int incidentFace = 0;
    float minDot = FLT_MAX;
    for(int i = 0; i < incCount; ++i)
    {
        float _dot = phys::dot( referenceNormal, incNormals[i] );
        if(_dot < minDot)
        {
            minDot = _dot;
            incidentFace = i;
        }
    }

    // Assign face vertices for incidentFace
    v[0] = inc[incidentFace];
    incidentFace = incidentFace + 1 >= incCount ? 0 : incidentFace + 1;
    v[1] = inc[incidentFace];
}

int clip( sf::Vector2f n, float c, sf::Vector2f *face )
{
    sf::Uint32 sp = 0;
    sf::Vector2f out[2] = {
        face[0],
        face[1]
    };

    // Retrieve distances from each endpoint to the line
    // d = ax + by - c
    float d1 = phys::dot( n, face[0] ) - c;
    float d2 = phys::dot( n, face[1] ) - c;

    // If negative (behind plane) clip
    if(d1 <= 0.0f) out[sp++] = face[0];
    if(d2 <= 0.0f) out[sp++] = face[1];

    // If the points are on different sides of the plane
    if(d1 * d2 < 0.0f) // less than to ignore -0.0f
    {
        // Push interesection point
        float alpha = d1 / (d1 - d2);
        out[sp] = face[0] + alpha * (face[1] - face[0]);
        ++sp;
    }

    // Assign our new converted values
    face[0] = out[0];
    face[1] = out[1];

    assert( sp != 3 );

    return sp;
}

void gridToPolygon(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b)
{
    GridShape* grid = reinterpret_cast<GridShape*>(a->getShape());
    phys::PolygonShape* poly = reinterpret_cast<phys::PolygonShape*>(b->getShape());

    m->contactCount = 0;

    sf::Vector2f topLeft(FLT_MAX, FLT_MAX);
    sf::Vector2f botRight(-FLT_MAX, -FLT_MAX);

    for (unsigned int i = 0; i < poly->getVertices().size(); i++)
    {
        // Retrieve a face normal from A
        sf::Vector2f n = poly->getNormals()[i];
        sf::Vector2f nw = poly->getU() * n;

        // Transform face normal into B's model space
        phys::Mat2 buT = grid->getU().transpose( );
        n = buT * nw;

        // Retrieve vertex on face from A, transform into
        // B's model space
        sf::Vector2f v = poly->getVertices()[i];
        v = poly->getU() * v + b->getPosition();
        v -= a->getPosition();
        v = buT * v;

        poly->transformedVertices[i] = v;
        poly->transformedNormals[i] = n;

        if (v.x < topLeft.x)
            topLeft.x = v.x;
        if (v.y < topLeft.y)
            topLeft.y = v.y;
        if (v.x > botRight.x)
            botRight.x = v.x;
        if (v.y > botRight.y)
            botRight.y = v.y;
    }

    int left = floor(topLeft.x/(PTU/TILE_SIZE))-1;
    int top = floor(topLeft.y/(PTU/TILE_SIZE))-1;
    int right = ceil(botRight.x/(PTU/TILE_SIZE))+1;
    int bot = ceil(botRight.y/(PTU/TILE_SIZE))+1;

    if (!grid->getGrid()->mWrapX)
    {
        left = std::max(left, 0);
        right = std::min(right, grid->getGrid()->mSizeX-1);
    }
	top = std::max(top, 0);
    bot = std::min(bot, grid->getGrid()->mSizeY-1);

    // Find the tile with the least overlap in the collision
    float smallestPenetration = FLT_MAX;
    int smallestFace;

    sf::Vector2f tileVerts[4];
    float tilePenetration = 0.f;
    float polyPenetration = 0.f;
    int tileFace;
    int polyFace;

    sf::Vector2f tileNormals[4];
    tileNormals[0] = sf::Vector2f(-1, 0);
    tileNormals[1] = sf::Vector2f(0, 1);
    tileNormals[2] = sf::Vector2f(1, 0);
    tileNormals[3] = sf::Vector2f(0, -1);

    for (int y = top; y <= bot; y++)
    {
        for (int _x = left; _x <= right; _x++)
        {
            int x = _x;
            if (grid->getGrid()->mWrapX)
                x = grid->getGrid()->wrapX(x);

            if (grid->getGrid()->mTiles[y][x].mMat == 0)
                continue;

            sf::Vector2f tVerts[4];
            tVerts[0] = sf::Vector2f(_x*(PTU/TILE_SIZE), y*(PTU/TILE_SIZE));
            tVerts[1] = sf::Vector2f(_x*(PTU/TILE_SIZE), (y+1)*(PTU/TILE_SIZE));
            tVerts[2] = sf::Vector2f((_x+1)*(PTU/TILE_SIZE), (y+1)*(PTU/TILE_SIZE));
            tVerts[3] = sf::Vector2f((_x+1)*(PTU/TILE_SIZE), y*(PTU/TILE_SIZE));

            // Check for a separating axis with A's face planes
            int faceA;
            float penetrationA = findAxisLeastPenetration(&faceA, poly->transformedVertices.data(),
                                                          poly->transformedNormals.data(), poly->transformedVertices.size(), tVerts, 4);
            if(penetrationA >= 0.0f || phys::equal(penetrationA, 0.f))
                continue;

            // Check for a separating axis with B's face planes
            int faceB;
            float penetrationB = findAxisLeastPenetration(&faceB, tVerts, tileNormals, 4, poly->transformedVertices.data(), poly->transformedVertices.size());
            if(penetrationB >= 0.0f || phys::equal(penetrationB, 0.f))
                continue;

            if (penetrationA < smallestPenetration || penetrationB < smallestPenetration)
            {
                tilePenetration = penetrationB;
                polyPenetration = penetrationA;
                tileFace = faceB;
                polyFace = faceA;
                memcpy(tileVerts, tVerts, sizeof(sf::Vector2f)*4);
            }

            if (penetrationA < smallestPenetration)
            {
                smallestPenetration = penetrationA;
                smallestFace = faceA;
            }
            if (penetrationB < smallestPenetration)
            {
                smallestPenetration = penetrationB;
                smallestFace = faceB;
            }
        }
    }

    if (smallestPenetration == FLT_MAX)
        return;

    sf::Uint32 referenceIndex;
    bool flip; // Always point from a to b

    phys::RigidBody* ref;
    sf::Vector2f* refVerts;
    sf::Vector2f* refNormals;
    int refCount;
    phys::RigidBody* inc;
    sf::Vector2f* incVerts;
    sf::Vector2f* incNormals;
    int incCount;

    // Determine which shape contains reference face
    if(phys::biasGreaterThan( tilePenetration, polyPenetration ))
    {
        ref = a;
        refVerts = tileVerts;
        refNormals = tileNormals;
        refCount = 4;

        inc = b;
        incVerts = poly->transformedVertices.data();
        incNormals = poly->transformedNormals.data();
        incCount = poly->transformedVertices.size();

        referenceIndex = tileFace;
        flip = false;
    }
    else
    {
        ref = b;
        refVerts = poly->transformedVertices.data();
        refNormals = poly->transformedNormals.data();
        refCount = poly->transformedVertices.size();

        inc = a;
        incVerts = tileVerts;
        incNormals = tileNormals;
        incCount = 4;

        referenceIndex = polyFace;
        flip = true;
    }

    // World space incident face
    sf::Vector2f incidentFace[2];
    findIncidentFace(incidentFace, refVerts, refNormals, refCount, incVerts, incNormals, incCount, referenceIndex);

    //        y
    //        ^  ->n       ^
    //      +---c ------posPlane--
    //  x < | i |\
    //      +---+ c-----negPlane--
    //             \       v
    //              r
    //
    //  r : reference face
    //  i : incident poly
    //  c : clipped point
    //  n : incident normal

    // Setup reference face vertices
    sf::Vector2f v1 = refVerts[referenceIndex];
    referenceIndex = referenceIndex + 1 == refCount ? 0 : referenceIndex + 1;
    sf::Vector2f v2 = refVerts[referenceIndex];

    // Transform vertices to world space
    v1 = a->getShape()->getU() * v1 + a->getPosition();
    v2 = a->getShape()->getU() * v2 + a->getPosition();

    // Calculate reference face side normal in world space
    sf::Vector2f sidePlaneNormal = (v2 - v1);
    sidePlaneNormal = phys::normalize(sidePlaneNormal);

    // Orthogonalize
    sf::Vector2f refFaceNormal( sidePlaneNormal.y, -sidePlaneNormal.x );

    // ax + by = c
    // c is distance from origin
    float refC = phys::dot( refFaceNormal, v1 );
    float negSide = -phys::dot( sidePlaneNormal, v1 );
    float posSide =  phys::dot( sidePlaneNormal, v2 );

    // Clip incident face to reference face side planes
    if(clip( -sidePlaneNormal, negSide, incidentFace ) < 2)
        return; // Due to floating point error, possible to not have required points

    if(clip(  sidePlaneNormal, posSide, incidentFace ) < 2)
        return; // Due to floating point error, possible to not have required points

    // Flip
    m->normal = flip ? -refFaceNormal : refFaceNormal;

    // Keep points behind reference face
    sf::Uint32 cp = 0; // clipped points behind reference face
    float separation = phys::dot( refFaceNormal, incidentFace[0] ) - refC;
    if(separation <= 0.0f)
    {
        m->contacts[cp] = incidentFace[0];
        m->penetration = -separation;
        ++cp;
    }
    else
        m->penetration = 0;

    separation = phys::dot( refFaceNormal, incidentFace[1] ) - refC;
    if(separation <= 0.0f)
    {
        m->contacts[cp] = incidentFace[1];

        m->penetration += -separation;
        ++cp;

        // Average penetration
        m->penetration /= (float)cp;
    }

    m->contactCount = cp;
}

void polygonToGrid(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b)
{
    gridToPolygon( m, b, a );
    m->normal = -m->normal;
}
