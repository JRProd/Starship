#include "Asteroid.hpp"

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

constexpr int ASTEROID_TRIANGLES = 16;
constexpr int ASTEROID_VERTEXES = ASTEROID_TRIANGLES * 3;

bool g_AstroidsWrapScreen = true;

Asteroid::Asteroid( Game* game, Vec3 startingPosition )
    : Entity( game, startingPosition )
{
    m_PhysicsRadius = ASTEROID_PHYSICS_RADIUS;
    m_CosmeticRadius = ASTEROID_COSMETIC_RADIUS;

    m_Health = 3;

    m_Color = ASTEROID_COLOR;
}

Asteroid::~Asteroid()
{
}

void Asteroid::Create()
{
    // Define the outer vertex based on n number of equal degree triangles
    m_TriangleCorners = new Vec2[ ASTEROID_TRIANGLES ];
    constexpr float degreesPerCorner = 360.f / ( float) ASTEROID_TRIANGLES;
    for ( int cornerIndex = 0; cornerIndex < ASTEROID_TRIANGLES; ++cornerIndex )
    {
        // Get a Vec2 based on rng length and the current degree
        float currentDegree = degreesPerCorner * cornerIndex;
        float length = m_Game->GetRng()->FloatInRange( m_PhysicsRadius, m_CosmeticRadius );
        m_TriangleCorners[ cornerIndex ] = Vec2::MakeFromPolarDegrees( currentDegree, 
                                                                       length );
    }
}

void Asteroid::Update( float deltaSeconds )
{
    if ( m_IsDead )
    {
        m_IsGarbage = true;
        return;
    }

    if ( IsOffscreen() )
    {
        if ( g_AstroidsWrapScreen )
        {
            WrapAstroid();
        }
        else
        {
            m_IsGarbage = true;
        }
        return;
    }

    if ( WasJustHit() )
    {
        m_Color = Rgba8::WHITE;
    }
    else
    {
        m_Color = ASTEROID_COLOR;
    }

    Entity::Update( deltaSeconds );
}

void Asteroid::Render() const
{
    std::vector<VertexMaster> visual;

    // Defines the n * 3 vertexes for the n triangles
    for ( int triangleIndex = 0; triangleIndex < ASTEROID_TRIANGLES; ++triangleIndex )
    {
        // First vertex is always the center
        visual.push_back(  VertexMaster( Vec3( 0.f, 0.f, 0.f ), 
                                                   m_Color ) );
        // Second vertex (counter-clockwise) is always the current triangle point in the TriangleCorners array
        visual.push_back( VertexMaster( m_TriangleCorners[ triangleIndex ],
                                                       m_Color ) );
        // If not the last triangle, use the next element in the TriangleCorners array
        if ( triangleIndex + 1 < ASTEROID_TRIANGLES )
        {
            visual.push_back( VertexMaster( m_TriangleCorners[ triangleIndex + 1 ],
                                                           m_Color ) );
        }
        // If is the last triangle, use the first TriangleCorners element to connect back to the start
        else
        {
            visual.push_back( VertexMaster( m_TriangleCorners[ 0 ],
                                                           m_Color ) );
        }
    }

    TransformVertexArray( visual, static_cast<Vec2>(m_Position), m_AngleDegrees, m_UniformScale );
    g_Renderer->DrawVertexArray( visual );
}

void Asteroid::Die()
{
    m_Game->AddScreenShake( .25f );
    m_Game->AddControllerVibration( 0, .35f, .1f );

    m_Game->CreateDebrisClusterAt( m_Position, ASTEROID_COLOR, 1.5f, 30 );
}

void Asteroid::Destroy()
{
    delete[] m_TriangleCorners;
}

void Asteroid::WrapAstroid()
{
    if ( m_Position.x < -MAX_SCREEN_SHAKE - m_CosmeticRadius )
    {
        m_Position.x = WORLD_SIZE_X + MAX_SCREEN_SHAKE + m_CosmeticRadius;
    }
    if ( m_Position.x > WORLD_SIZE_X + MAX_SCREEN_SHAKE + m_CosmeticRadius )
    {
        m_Position.x = -MAX_SCREEN_SHAKE - m_CosmeticRadius;
    }
    if ( m_Position.y < -MAX_SCREEN_SHAKE - m_CosmeticRadius )
    {
        m_Position.y = WORLD_SIZE_Y + MAX_SCREEN_SHAKE + m_CosmeticRadius;
    }
    if ( m_Position.y > WORLD_SIZE_Y + MAX_SCREEN_SHAKE + m_CosmeticRadius )
    {
        m_Position.y = -MAX_SCREEN_SHAKE - m_CosmeticRadius;
    }
}
