#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "phys/Mat2.h"

namespace phys
{
    class RigidBody;

    class Shape
    {
        public:
            enum Type
            {
                Polygon,
                Circle,
                Count
            };

            Shape();
            virtual ~Shape();

            virtual void computeMass(RigidBody* body, float density) = 0;
            virtual void setRotation(float rot) = 0;
            virtual int getType() const = 0;
            const sf::FloatRect& getLocalBounds(){return boundingBox;}

        protected:
            sf::FloatRect boundingBox; // local bounding box
    };

    class CircleShape : public Shape
    {
        public:
            CircleShape( float r );

            void computeMass(RigidBody* body, float density);

            void setRotation(float rot){}

            int getType() const {return Circle;}
            float getRadius() const {return radius;}

        private:
            float radius;
    };

    class PolygonShape : public Shape
    {
        public:
            PolygonShape() : u(0.f) {}

            void computeMass(RigidBody* body, float density);

            void setRotation(float rot);

            void setBox(float hw, float hh);
            void set(sf::Vector2f* vertices, sf::Uint32 count);
            sf::Vector2f getSupport(const sf::Vector2f& dir);

            int getType() const {return Polygon;}
            const std::vector<sf::Vector2f>& getVertices(){return vertices;}
            const std::vector<sf::Vector2f>& getNormals(){return normals;}
            const Mat2& getU(){return u;}

        private:
            std::vector<sf::Vector2f> vertices;
            std::vector<sf::Vector2f> normals;
            Mat2 u;
    };
}


#endif // SHAPE_H
