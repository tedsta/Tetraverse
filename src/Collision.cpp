#include "Collision.h"

#include <cfloat>

#include "phys/Shape.h"

void gridToPolygon(phys::Manifold *m, phys::RigidBody *a, phys::RigidBody *b)
{
    GridShape* grid = reinterpret_cast<GridShape*>(a->getShape());
    phys::PolygonShape* poly = reinterpret_cast<phys::PolygonShape*>(b->getShape());

    sf::Vector2f topLeft(FLT_MAX, FLT_MAX);
    sf::Vector2f botRight(-FLT_MAX, -FLT_MAX);

    for (unsigned int i = 0; i < poly->getVertices().size(); i++)
    {
        // Transform face normal into B's model space
        phys::Mat2 buT = grid->getU().transpose( );

        // Retrieve vertex on face from A, transform into
        // B's model space
        sf::Vector2f v = poly->getVertices()[i];
        v = poly->getU() * v + b->getPosition();
        v -= a->getPosition();
        v = buT * v;
        poly->transformedVertices[i] = v;

        if (v.x < topLeft.x)
            topLeft.x = v.x;
        if (v.y < topLeft.y)
            topLeft.y = v.y;
        if (v.x > botRight.x)
            botRight.x = v.x;
        if (v.y > botRight.y)
            botRight.y = v.y;
    }

    topLeft.x = floor(topLeft.x/(PTU/TILE_SIZE));
    topLeft.y = floor(topLeft.y/(PTU/TILE_SIZE));
    botRight.x = ceil(botRight.x/(PTU/TILE_SIZE));
    botRight.y = ceil(botRight.y/(PTU/TILE_SIZE));

    // Since we use the bounding box, every tile in this for loop is guaranteed to be colliding
    // on the x and y axes, so we don't need to check them. Only check the faces of the polygon.
    for (int y = topLeft.y; y < botRight.y; y++)
    {
        for (int x = topLeft.x; x < botRight.x; x++)
        {
            if (grid->getGrid()->mTiles[y][x].mMat == 0)
                continue;

            sf::Vector2f tileVerts[4];
            //tileVerts[0].x =

            for (unsigned int i = 0; i < poly->getVertices().size(); i++)
            {
                // Retrieve a face normal from A
                sf::Vector2f n = poly->getNormals()[i];
                sf::Vector2f nw = poly->getU() * n;

                // Transform face normal into B's model space
                phys::Mat2 buT = grid->getU().transpose( );
                n = buT * nw;
            }
        }
    }
}
