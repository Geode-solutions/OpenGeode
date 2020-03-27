

#include <geode/geometry/perpendicular.h>

namespace geode
{
    Vector2D perpendicular( const Vector2D& v )
    {
        return Vector2D{ { v.value( 1 ), -v.value( 0 ) } };
    }

    double dot_perpendicular( const Vector2D& v0, const Vector2D& v1 )
    {
        return v0.dot( perpendicular( v1 ) );
    }
} // namespace geode
