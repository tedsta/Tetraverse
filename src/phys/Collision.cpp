#include "phys/Collision.h"

#include <cfloat>
#include <cassert>

#include "phys/RigidBody.h"
#include "phys/Shape.h"
#include "phys/Manifold.h"

namespace phys
{
    void circleToCircle(Collision* c, RigidBody* a, RigidBody* b)
    {
        c->addManifold();

        CircleShape* A = reinterpret_cast<CircleShape*>(a->getShape());
        CircleShape* B = reinterpret_cast<CircleShape*>(b->getShape());

        // Calculate translational vector, which is normal
        sf::Vector2f normal = b->getPosition() - a->getPosition();

        float dist_sqr = lengthSqr(normal);
        float radius = A->getRadius() + B->getRadius();

        // Not in contact
        if(dist_sqr >= radius * radius)
        {
            c->getLastManifold()->contactCount = 0;
            return;
        }

        float distance = std::sqrt( dist_sqr );

        c->getLastManifold()->contactCount = 1;

        if(distance == 0.0f)
        {
            c->getLastManifold()->penetration = A->getRadius();
            c->getLastManifold()->normal = sf::Vector2f( 1, 0 );
            c->getLastManifold()->contacts [0] = a->getPosition();
        }
        else
        {
            c->getLastManifold()->penetration = radius - distance;
            c->getLastManifold()->normal = normal / distance; // Faster than using Normalized since we already performed sqrt
            c->getLastManifold()->contacts[0] = c->getLastManifold()->normal * A->getRadius() + a->getPosition();
        }
    }

    void circleToPolygon( Collision* c, RigidBody *a, RigidBody *b )
    {
        c->addManifold();

        CircleShape* A = reinterpret_cast<CircleShape*>(a->getShape());
        PolygonShape* B = reinterpret_cast<PolygonShape*>(b->getShape());

        c->getLastManifold()->contactCount = 0;

        // Transform circle center to Polygon model space
        sf::Vector2f center = a->getPosition();
        center = B->getU().transpose() * (center - b->getPosition());

        // Find edge with minimum penetration
        // Exact concept as using support points in Polygon vs Polygon
        float separation = -FLT_MAX;
        sf::Uint32 faceNormal = 0;
        for(sf::Uint32 i = 0; i < B->getVertices().size(); ++i)
        {
            float s = dot( B->getNormals()[i], center - B->getVertices()[i] );

            if(s > A->getRadius())
                return;

            if(s > separation)
            {
                separation = s;
                faceNormal = i;
            }
        }

        // Grab face's vertices
        sf::Vector2f v1 = B->getVertices()[faceNormal];
        sf::Uint32 i2 = faceNormal + 1 < B->getVertices().size() ? faceNormal + 1 : 0;
        sf::Vector2f v2 = B->getVertices()[i2];

        // Check to see if center is within polygon
        if(separation < EPSILON)
        {
            c->getLastManifold()->contactCount = 1;
            c->getLastManifold()->normal = -(B->getU() * B->getNormals()[faceNormal]);
            c->getLastManifold()->contacts[0] = c->getLastManifold()->normal * A->getRadius() + a->getPosition();
            c->getLastManifold()->penetration = A->getRadius();
            return;
        }

        // Determine which voronoi region of the edge center of circle lies within
        float dot1 = dot( center - v1, v2 - v1 );
        float dot2 = dot( center - v2, v1 - v2 );
        c->getLastManifold()->penetration = A->getRadius() - separation;

        // Closest to v1
        if(dot1 <= 0.0f)
        {
            if(lengthSqr( center - v1 ) > A->getRadius() * A->getRadius())
                return;

            c->getLastManifold()->contactCount = 1;
            sf::Vector2f n = v1 - center;
            n = B->getU() * n;
            n = normalize(n);
            c->getLastManifold()->normal = n;
            v1 = B->getU() * v1 + b->getPosition();
            c->getLastManifold()->contacts[0] = v1;
        }

        // Closest to v2
        else if(dot2 <= 0.0f)
        {
            if(lengthSqr( center - v2 ) > A->getRadius() * A->getRadius())
                return;

            c->getLastManifold()->contactCount = 1;
            sf::Vector2f n = v2 - center;
            v2 = B->getU() * v2 + b->getPosition();
            c->getLastManifold()->contacts[0] = v2;
            n = B->getU() * n;
            n = normalize(n);
            c->getLastManifold()->normal = n;
        }

        // Closest to face
        else
        {
            sf::Vector2f n = B->getNormals()[faceNormal];
            if(dot( center - v1, n ) > A->getRadius())
                return;

            n = B->getU() * n;
            c->getLastManifold()->normal = -n;
            c->getLastManifold()->contacts[0] = c->getLastManifold()->normal * A->getRadius() + a->getPosition();
            c->getLastManifold()->contactCount = 1;
        }
    }

    void polygonToCircle( Collision* c, RigidBody *a, RigidBody *b )
    {
      circleToPolygon( c, b, a );
      c->getLastManifold()->normal = -c->getLastManifold()->normal;
    }

    float findAxisLeastPenetration( sf::Uint32 *faceIndex, RigidBody *a, RigidBody *b )
    {
        PolygonShape *A = reinterpret_cast<PolygonShape*>(a->getShape());
        PolygonShape *B = reinterpret_cast<PolygonShape*>(b->getShape());

        float bestDistance = -FLT_MAX;
        sf::Uint32 bestIndex;

        for(sf::Uint32 i = 0; i < A->getVertices().size(); ++i)
        {
            // Retrieve a face normal from A
            sf::Vector2f n = A->getNormals()[i];
            sf::Vector2f nw = A->getU() * n;

            // Transform face normal into B's model space
            Mat2 buT = B->getU().transpose( );
            n = buT * nw;

            // Retrieve support point from B along -n
            sf::Vector2f s = B->getSupport( -n );

            // Retrieve vertex on face from A, transform into
            // B's model space
            sf::Vector2f v = A->getVertices()[i];
            v = A->getU() * v + a->getPosition();
            v -= b->getPosition();
            v = buT * v;

            // Compute penetration distance (in B's model space)
            float d = dot( n, s - v );

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

    void findIncidentFace( sf::Vector2f *v, RigidBody* Ref, RigidBody* Inc, sf::Uint32 referenceIndex )
    {
        PolygonShape *RefPoly = reinterpret_cast<PolygonShape*>(Ref->getShape());
        PolygonShape *IncPoly = reinterpret_cast<PolygonShape*>(Inc->getShape());

        sf::Vector2f referenceNormal = RefPoly->getNormals()[referenceIndex];

        // Calculate normal in incident's frame of reference
        referenceNormal = RefPoly->getU() * referenceNormal; // To world space
        referenceNormal = IncPoly->getU().transpose( ) * referenceNormal; // To incident's model space

        // Find most anti-normal face on incident polygon
        int incidentFace = 0;
        float minDot = FLT_MAX;
        for(sf::Uint32 i = 0; i < IncPoly->getVertices().size(); ++i)
        {
            float _dot = dot( referenceNormal, IncPoly->getNormals()[i] );
            if(_dot < minDot)
            {
                minDot = _dot;
                incidentFace = i;
            }
        }

        // Assign face vertices for incidentFace
        v[0] = IncPoly->getU() * IncPoly->getVertices()[incidentFace] + Inc->getPosition();
        incidentFace = incidentFace + 1 >= (int)IncPoly->getVertices().size() ? 0 : incidentFace + 1;
        v[1] = IncPoly->getU() * IncPoly->getVertices()[incidentFace] + Inc->getPosition();
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
        float d1 = dot( n, face[0] ) - c;
        float d2 = dot( n, face[1] ) - c;

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

    void polygonToPolygon( Collision* c, RigidBody *a, RigidBody *b )
    {
        c->addManifold();

        PolygonShape *A = reinterpret_cast<PolygonShape *>(a->getShape());
        PolygonShape *B = reinterpret_cast<PolygonShape *>(b->getShape());
        c->getLastManifold()->contactCount = 0;

        // Check for a separating axis with A's face planes
        sf::Uint32 faceA;
        float penetrationA = findAxisLeastPenetration( &faceA, a, b );
        if(penetrationA >= 0.0f)
            return;

        // Check for a separating axis with B's face planes
        sf::Uint32 faceB;
        float penetrationB = findAxisLeastPenetration( &faceB, b, a );
        if(penetrationB >= 0.0f)
            return;

        sf::Uint32 referenceIndex;
        bool flip; // Always point from a to b

        RigidBody* Ref;
        RigidBody* Inc;
        PolygonShape *RefPoly; // Reference
        PolygonShape *IncPoly; // Incident

        // Determine which shape contains reference face
        if(biasGreaterThan( penetrationA, penetrationB ))
        {
            Ref = a;
            Inc = b;
            RefPoly = A;
            IncPoly = B;
            referenceIndex = faceA;
            flip = false;
        }

        else
        {
            Ref = b;
            Inc = a;
            RefPoly = B;
            IncPoly = A;
            referenceIndex = faceB;
            flip = true;
        }

        // World space incident face
        sf::Vector2f incidentFace[2];
        findIncidentFace( incidentFace, Ref, Inc, referenceIndex );

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
        sf::Vector2f v1 = RefPoly->getVertices()[referenceIndex];
        referenceIndex = referenceIndex + 1 == RefPoly->getVertices().size() ? 0 : referenceIndex + 1;
        sf::Vector2f v2 = RefPoly->getVertices()[referenceIndex];

        // Transform vertices to world space
        v1 = RefPoly->getU() * v1 + Ref->getPosition();
        v2 = RefPoly->getU() * v2 + Ref->getPosition();

        // Calculate reference face side normal in world space
        sf::Vector2f sidePlaneNormal = (v2 - v1);
        sidePlaneNormal = normalize(sidePlaneNormal);

        // Orthogonalize
        sf::Vector2f refFaceNormal( sidePlaneNormal.y, -sidePlaneNormal.x );

        // ax + by = c
        // c is distance from origin
        float refC = dot( refFaceNormal, v1 );
        float negSide = -dot( sidePlaneNormal, v1 );
        float posSide =  dot( sidePlaneNormal, v2 );

        // Clip incident face to reference face side planes
        if(clip( -sidePlaneNormal, negSide, incidentFace ) < 2)
            return; // Due to floating point error, possible to not have required points

        if(clip(  sidePlaneNormal, posSide, incidentFace ) < 2)
            return; // Due to floating point error, possible to not have required points

        // Flip
        c->getLastManifold()->normal = flip ? -refFaceNormal : refFaceNormal;

        // Keep points behind reference face
        sf::Uint32 cp = 0; // clipped points behind reference face
        float separation = dot( refFaceNormal, incidentFace[0] ) - refC;
        if(separation <= 0.0f)
        {
            c->getLastManifold()->contacts[cp] = incidentFace[0];
            c->getLastManifold()->penetration = -separation;
            ++cp;
        }
        else
            c->getLastManifold()->penetration = 0;

        separation = dot( refFaceNormal, incidentFace[1] ) - refC;
        if(separation <= 0.0f)
        {
            c->getLastManifold()->contacts[cp] = incidentFace[1];

            c->getLastManifold()->penetration += -separation;
            ++cp;

            // Average penetration
            c->getLastManifold()->penetration /= (float)cp;
        }

        c->getLastManifold()->contactCount = cp;
    }
}
