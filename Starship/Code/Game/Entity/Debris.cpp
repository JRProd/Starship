#include "Debris.hpp"

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

constexpr int DEBRIS_TRIANGLES = 5;
constexpr int DEBRIS_VERTEXES = DEBRIS_TRIANGLES * 3;

Debris::Debris( Game* game, const Vec3& startingPosition )
    : Entity( game, startingPosition )
{
    m_CosmeticRadius = DEBRIS_COSMETIC_RADIUS;
}

Debris::Debris( Game* game, const Vec3& startingPosition, const Rgba8& color )
    : Entity(game, startingPosition)
    , m_DebrisColor(color)
{
    m_CosmeticRadius = DEBRIS_COSMETIC_RADIUS;
}

Debris::Debris( Game* game, 
                const Vec3& startingPosition, 
                const Rgba8& color, 
                float lifeSpan )
    : Entity( game, startingPosition )
    , m_DebrisColor( color )
    , m_DebrisTime( lifeSpan )
{
    m_CosmeticRadius = DEBRIS_COSMETIC_RADIUS;
}

Debris::~Debris()
{
}

void Debris::Create()
{
    // Define the outer vertex based on n number of equal degree triangles
    m_TriangleCorners = new Vec2[ DEBRIS_TRIANGLES ];
    constexpr float degreesPerCorner = 360.f / ( float) DEBRIS_TRIANGLES;
    for ( int cornerIndex = 0; cornerIndex < DEBRIS_TRIANGLES; ++cornerIndex )
    {
        // Get a Vec2 based on rng length and the current degree
        float currentDegree = degreesPerCorner * cornerIndex;
        float length = m_Game->GetRng()->FloatInRange( m_CosmeticRadius * .25f, 
                                                       m_CosmeticRadius );
        m_TriangleCorners[ cornerIndex ] = Vec2::MakeFromPolarDegrees( currentDegree, 
                                                                       length );
    }

    SetAngularVelocity( m_Game->GetRng()->FloatInRange( -500.f, 500.f ) );
    SetAngleDegrees( m_Game->GetRng()->FloatLessThan( 360.f ) );
    SetVelocity( Vec3::MakeFromPolarDegreesXY( GetAngleDegrees(), 
                                               m_Game->GetRng()->FloatInRange( DEBRIS_MIN_SPEED, 
                                                                               DEBRIS_MAX_SPEED ) ) );

    GenerateVertexPCU();
}

void Debris::Update( float deltaSeconds )
{
    if ( m_Age > m_DebrisTime )
    {
        Die();
    }

    // Update the color of the debris
     float alphaModifier = RangeMapFloat( 0.f, m_DebrisTime, .5f, 0.f, m_Age );
     m_DebrisColor.SetAlphaAsPercent( alphaModifier );

    Entity::Update( deltaSeconds );
}

void Debris::Render() const
{
    Transform transform;
    transform.position = m_Position;
    transform.rotationAroundAxis.z = m_AngleDegrees;

    g_Renderer->SetModelUBO( transform.GetAsMatrix() );
    g_Renderer->DrawVertexArray( m_LocalVisual );
    g_Renderer->SetModelUBO();
}

void Debris::Die()
{
    m_IsDead = true;
    m_IsGarbage = true;
}

void Debris::Destroy()
{
    delete[] m_TriangleCorners;
    m_TriangleCorners = nullptr;
}

void Debris::GenerateVertexPCU()
{
    // Defines the n * 3 vertexes for the n triangles
    m_LocalVisual.reserve( DEBRIS_TRIANGLES * 3 );
    for ( int triangleIndex = 0; triangleIndex < DEBRIS_TRIANGLES; ++triangleIndex )
    {
        // First vertex is always the center
        m_LocalVisual.emplace_back(  Vec3( 0.f, 0.f, 0.f ), m_DebrisColor );
        // Second vertex (counter-clockwise) is always the current triangle point in the TriangleCorners array
        m_LocalVisual.emplace_back( m_TriangleCorners[ triangleIndex ], m_DebrisColor );
        // If not the last triangle, use the next element in the TriangleCorners array
        if ( triangleIndex + 1 < DEBRIS_TRIANGLES )
        {
            m_LocalVisual.emplace_back( m_TriangleCorners[ triangleIndex + 1 ],
                                                          m_DebrisColor );
        }
        // If is the last triangle, use the first TriangleCorners element to connect back to the start
        else
        {
            m_LocalVisual.emplace_back( m_TriangleCorners[ 0 ],
                                                          m_DebrisColor );
        }
    }
}
