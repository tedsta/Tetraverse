#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Network/Packet.hpp>

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
                Custom
            };

            Shape() : u(0.f) {}
            virtual ~Shape();

            virtual void serialize(sf::Packet &packet) = 0;
            virtual void deserialize(sf::Packet &packet) = 0;

            virtual void computeMass(RigidBody* body, float density) = 0;
            virtual void setRotation(float rot){u.set(rot);}
            virtual int getType() const = 0;
            const sf::FloatRect& getLocalBounds(){return boundingBox;}
            const Mat2& getU(){return u;}

        protected:
            sf::FloatRect boundingBox; // local bounding box
            Mat2 u;
    };

    class CircleShape : public Shape
    {
        public:
            CircleShape( float r );

            void serialize(sf::Packet &packet);
            void deserialize(sf::Packet &packet);

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
            PolygonShape(){}

            void serialize(sf::Packet &packet);
            void deserialize(sf::Packet &packet);

            void computeMass(RigidBody* body, float density);

            void setRotation(float rot);
            void setBox(float hw, float hh);
            void set(sf::Vector2f* vertices, sf::Uint32 count);
            sf::Vector2f getSupport(const sf::Vector2f& dir);

            int getType() const {return Polygon;}
            const std::vector<sf::Vector2f>& getVertices(){return vertices;}
            const std::vector<sf::Vector2f>& getNormals(){return normals;}

            std::vector<sf::Vector2f> transformedVertices;
            std::vector<sf::Vector2f> transformedNormals;

        private:
            std::vector<sf::Vector2f> vertices;
            std::vector<sf::Vector2f> normals;
    };
}


#endif // SHAPE_H
