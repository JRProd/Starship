#include "GameCommon.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//-------------------------------------------------------------------------------
void DrawDebugLine( const Vec2& start,
                    const Vec2& end,
                    const Rgba8& color,
                    float thickness )
{
    Vec2 displacement = end - start;
    float lineRadius = thickness * .5f;

    Vec2 forward = lineRadius * displacement.GetNormalized();
    Vec2 left = forward.GetRotated90Degrees();

    std::vector<VertexMaster> visual;
    visual.push_back( VertexMaster( start - forward - left, color ) );
    visual.push_back( VertexMaster( end + forward - left, color ) );
    visual.push_back( VertexMaster( end + forward + left, color ) );

    visual.push_back( VertexMaster( end + forward + left, color ) );
    visual.push_back( VertexMaster( start - forward + left, color ) );
    visual.push_back( VertexMaster( start - forward - left, color ) );

    g_Renderer->DrawVertexArray( visual );
}

//-------------------------------------------------------------------------------
void DrawDebugCircle( const Vec2& center,
                      float radius,
                      const Rgba8& color,
                      float thickness )
{
    float degreesPerRadius = 360.f / DEBUG_CIRCLE_RADIUSES;
    float lineRadius = thickness * .5f;
    Vec2 currShort = Vec2::MakeFromPolarDegrees( 0.f,
                                                 radius - lineRadius
                                               ) + center;
    Vec2 currLong = Vec2::MakeFromPolarDegrees( 0.f,
                                                radius + lineRadius
                                              ) + center;

    std::vector<VertexMaster> debugCircle;
    // Start at the second radius because current is defined above.
    //  Loop up to <= DEBUG_CIRCLE_RADIUSES because 360 = 0 degrees
    //  This finishes the last group of triangles
    for( int currentRadius = 1; currentRadius <= DEBUG_CIRCLE_RADIUSES; ++currentRadius )
    {
        float currentDegrees = degreesPerRadius * currentRadius;
        Vec2 nextShort = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                     radius - lineRadius
                                                   ) + center;
        Vec2 nextLong = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                    radius + lineRadius
                                                  ) + center;

        // Draw two circles per radius so 6 vertexes
        //  Because of the first definition of currShort is above the for
        //  minus one offset here
        // First triangle
        debugCircle.push_back( VertexMaster( currShort, color ) );
        debugCircle.push_back( VertexMaster( currLong, color ) );
        debugCircle.push_back( VertexMaster( nextShort, color ) );
        // Second Triangle
        debugCircle.push_back( VertexMaster( nextShort, color ));
        debugCircle.push_back( VertexMaster( currLong, color ));
        debugCircle.push_back( VertexMaster( nextLong, color ));

        currShort = nextShort;
        currLong = nextLong;
    }

    g_Renderer->DrawVertexArray( debugCircle );
}

Vec2 PointJustOffScreen( float furthestBound )
{
    RandomNumberGenerator rng = RandomNumberGenerator();

    float xLocation = 0.f;
    if( rng.FiftyFifty() )
    {
        xLocation = rng.FloatInRange( -MAX_SCREEN_SHAKE - furthestBound,
                                      -MAX_SCREEN_SHAKE
                                    );
    }
    else
    {
        xLocation = rng.FloatInRange( WORLD_SIZE_X + MAX_SCREEN_SHAKE,
                                      WORLD_SIZE_X + MAX_SCREEN_SHAKE + furthestBound
                                    );
    }

    float yLocation = 0.f;
    if( rng.FiftyFifty() )
    {
        yLocation = rng.FloatInRange( -MAX_SCREEN_SHAKE - furthestBound,
                                      -MAX_SCREEN_SHAKE
                                    );
    }
    else
    {
        yLocation = rng.FloatInRange( WORLD_SIZE_Y + MAX_SCREEN_SHAKE,
                                      WORLD_SIZE_Y + MAX_SCREEN_SHAKE + furthestBound
                                    );
    }

    return Vec2( xLocation, yLocation );
}

Rgba8 PLAYER_SHIP_COLOR_1 = Rgba8( 132, 156, 165 );
Rgba8 PLAYER_SHIP_COLOR_2 = Rgba8( 52, 76, 85 );
Rgba8 PLAYER_SHIP_EXHAUST_1 = Rgba8( 255, 0, 0 );
Rgba8 PLAYER_SHIP_EXHAUST_2 = Rgba8( 255, 255, 0 );
Rgba8 ASTEROID_COLOR = Rgba8( 100, 100, 100 );
Rgba8 BULLET_HEAD_COLOR = Rgba8( 255, 255, 0 );
Rgba8 BULLET_TAIL_COLOR_START = Rgba8( 255, 0, 0 );
Rgba8 BULLET_TAIL_COLOR_END = Rgba8( 255, 0, 0, 0 );
Rgba8 BEETLE_COLOR = Rgba8( 0, 125, 0 );
Rgba8 WASP_COLOR = Rgba8( 200, 200, 0 );

Rgba8 DEBUG_FORWARD_VECTOR_COLOR = Rgba8( 255, 255, 0 );
Rgba8 DEBUG_POSITOIN_VECTOR_COLOR = Rgba8( 50, 50, 50 );
Rgba8 DEBUG_COSMETIC_CIRCLE = Rgba8( 255, 0, 255 );
Rgba8 DEBUG_PHYSICS_CIRCLE = Rgba8( 0, 255, 255 );
