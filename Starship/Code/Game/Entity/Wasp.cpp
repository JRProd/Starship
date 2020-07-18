#include "Wasp.hpp"

#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity/PlayerShip.hpp"

Wasp::Wasp( Game* game, const Vec3& startingPosition )
    : Entity( game, startingPosition )
{
    m_CosmeticRadius = WASP_COSMETIC_RADIUS;
    m_PhysicsRadius = WASP_PHYSICS_RADIUS;

    m_Health = WASP_HEALTH;

    m_Color = WASP_COLOR;
}

Wasp::~Wasp()
{
}

void Wasp::Create()
{
}

void Wasp::Update( float deltaSeconds )
{
    if ( m_IsDead )
    {
        m_IsGarbage = true;
        return;
    }


    if ( UpdateTarget() )
    {
        MoveTowardsTarget();
    }

    ClampVelocity();

    if ( WasJustHit() )
    {
        m_Color = Rgba8::WHITE;
    }
    else
    {
        m_Color = WASP_COLOR;
    }

    Entity::Update( deltaSeconds );
}

void Wasp::Render() const
{
    std::vector<VertexMaster> visual;
        visual.emplace_back( Vec2( -1.f, 1.f ), m_Color );
        visual.emplace_back( Vec2( 3.f, 1.f ), m_Color );
        visual.emplace_back( Vec2( 0.f, 2.f ), m_Color );
        visual.emplace_back(                            );
        visual.emplace_back( Vec2( -1.f, 1.f ), m_Color );
        visual.emplace_back( Vec2( -1.f, 0.f ), m_Color );
        visual.emplace_back( Vec2( 0.f, 1.f ), m_Color );
        visual.emplace_back(                            );
        visual.emplace_back( Vec2( -2.f, 0.f ), m_Color );
        visual.emplace_back( Vec2( -1.f, -1.f ), m_Color);
        visual.emplace_back( Vec2( -1.f, 1.f ), m_Color );
        visual.emplace_back(                            );
        visual.emplace_back( Vec2( -1.f, 0.f ), m_Color );
        visual.emplace_back( Vec2( -1.f, -1.f ), m_Color);
        visual.emplace_back( Vec2( 0.f, -1.f ), m_Color );
        visual.emplace_back(                            );
        visual.emplace_back( Vec2( -1.f, -1.f ), m_Color);
        visual.emplace_back( Vec2( 0.f, -2.f ), m_Color );
        visual.emplace_back( Vec2( 1.f, -1.f ), m_Color );

    TransformVertexArray( visual,
                          static_cast<Vec2>(m_Position),
                          m_AngleDegrees,
                          m_UniformScale );

    g_Renderer->DrawVertexArray( visual );
}

void Wasp::Die()
{
}

void Wasp::Destroy()
{
    m_Game->AddScreenShake( .1f );
    m_Game->AddControllerVibration( 0, .1f, .1f );
    m_Game->CreateDebrisClusterAt( m_Position, WASP_COLOR, 1.25f, 14 );
}

bool Wasp::UpdateTarget()
{
    if ( m_TargetPlayer == nullptr )
    {
        m_TargetPlayer = m_Game->GetAlivePlayer();
        //  If the ship is still nullptr
        if ( m_TargetPlayer == nullptr ) { return false; }
    }

    if ( m_TargetPlayer->IsDead() )
    {
        m_TargetPlayer = m_Game->GetAlivePlayer();
    }

    // If no player is alive stop
    if ( m_TargetPlayer == nullptr ) { return false; }

    return true;
}

void Wasp::MoveTowardsTarget()
{
    Vec3 displacement = m_TargetPlayer->GetPosition() - m_Position;
    float angleOfDisplacement = atan2fDegrees( displacement.y,
                                               displacement.x );

    if ( displacement.GetLength() > m_PhysicsRadius * .5f )
    {
        SetAngleDegrees( angleOfDisplacement );
        SetAcceleration( Vec3::MakeFromPolarDegreesXY( angleOfDisplacement,
                                                       WASP_ACCELERATION_PER_SECOND ) );
    }
    else
    {
        SetAcceleration( Vec3::ZERO );
        SetVelocity( Vec3::ZERO );
    }
}

void Wasp::ClampVelocity()
{
    SetVelocity( GetVelocity().GetClamped( WASP_MAX_VELOCITY ) );
}
