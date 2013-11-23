#ifndef MAT2_H
#define MAT2_H

#include <Fission/Core/Math.h>

namespace phys
{
    struct Mat2
    {
      union
      {
        struct
        {
          float m00, m01;
          float m10, m11;
        };

        float m[2][2];
        float v[4];
      };

      Mat2( ) {}
      Mat2( float radians )
      {
        float c = std::cos( radians );
        float s = std::sin( radians );

        m00 = c; m01 = -s;
        m10 = s; m11 =  c;
      }

      Mat2( float a, float b, float c, float d )
        : m00( a ), m01( b )
        , m10( c ), m11( d )
      {
      }

      void set( float radians )
      {
        float c = std::cos( radians );
        float s = std::sin( radians );

        m00 = c; m01 = -s;
        m10 = s; m11 =  c;
      }

      Mat2 abs( void ) const
      {
        return Mat2( std::abs( m00 ), std::abs( m01 ), std::abs( m10 ), std::abs( m11 ) );
      }

      sf::Vector2f axisX( void ) const
      {
        return sf::Vector2f( m00, m10 );
      }

      sf::Vector2f axisY( void ) const
      {
        return sf::Vector2f( m01, m11 );
      }

      Mat2 transpose( void ) const
      {
        return Mat2( m00, m10, m01, m11 );
      }

      const sf::Vector2f operator*( const sf::Vector2f& rhs ) const
      {
        return sf::Vector2f( m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y );
      }

      const Mat2 operator*( const Mat2& rhs ) const
      {
        // [00 01]  [00 01]
        // [10 11]  [10 11]

        return Mat2(
          m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
          m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
          m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
          m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
        );
      }
    };
}


#endif // MAT2_H
