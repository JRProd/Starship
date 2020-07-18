#include "Beetle.hpp"

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity/PlayerShip.hpp"

Beetle::Beetle( Game* game, const Vec3& startingPosition )
    : Entity( game, startingPosition )
{
    m_CosmeticRadius = BEETLE_COSMETIC_RADIUS;
    m_PhysicsRadius = BEETLE_PHYSICS_RADIUS;

    m_Health = BEETLE_HEALTH;

    m_Color = BEETLE_COLOR;
}

Beetle::~Beetle()
{
}

void Beetle::Create()
{
}

void Beetle::Update( float deltaSeconds )
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

    if ( WasJustHit() )
    {
        m_Color = Rgba8::WHITE;
    }
    else
    {
        m_Color = BEETLE_COLOR;
    }

    Entity::Update( deltaSeconds );
}

void Beetle::Render() const
{
    std::vector<VertexMaster> beetleVisual;
    beetleVisual.push_back(VertexMaster( Vec2( -2.f, 0.f ), m_Color )    );
    beetleVisual.push_back(VertexMaster( Vec2( 1.f, 0.f ), m_Color )   );
    beetleVisual.push_back(VertexMaster( Vec2( 2.f, 1.f ), m_Color )    );

    beetleVisual.push_back(VertexMaster(Vec2( -2.f, 0.f ), m_Color )  );
    beetleVisual.push_back(VertexMaster( Vec2( 2.f, -1.f ), m_Color )   );
    beetleVisual.push_back(VertexMaster( Vec2( 1.f, 0.f ), m_Color )   );

    beetleVisual.push_back(VertexMaster( Vec2( -3.f, 1.5f ), m_Color )  );
    beetleVisual.push_back(VertexMaster( Vec2( -2.f, 0.f ), m_Color )  );
    beetleVisual.push_back(VertexMaster( Vec2( 2.f, 1.f ), m_Color )    );

    beetleVisual.push_back(VertexMaster( Vec2( -3.f, -1.5f ), m_Color ) );
    beetleVisual.push_back(VertexMaster( Vec2( 2.f, -1.f ), m_Color )   );
    beetleVisual.push_back(VertexMaster( Vec2( -2.f, 0.f ), m_Color )   );

    TransformVertexArray( beetleVisual,
                          static_cast<Vec2>(m_Position),
                          m_AngleDegrees,
                          m_UniformScale );
    g_Renderer->DrawVertexArray( beetleVisual );
}

void Beetle::Die()
{
    m_Game->AddScreenShake( .15f );
    m_Game->AddControllerVibration( 0, .25f, .1f );
    m_Game->CreateDebrisClusterAt( m_Position, BEETLE_COLOR, 1.25f, 20 );
}

void Beetle::Destroy()
{
}

bool Beetle::UpdateTarget()
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

void Beetle::MoveTowardsTarget()
{
    Vec3 displacement = m_TargetPlayer->GetPosition() - m_Position;
    float angleOfDisplacement = atan2fDegrees( displacement.y, displacement.x );

    if ( displacement.GetLength() > m_PhysicsRadius * .5f )
    {
        SetAngleDegrees( angleOfDisplacement );
        SetVelocity( Vec3::MakeFromPolarDegreesXY( angleOfDisplacement, BEETLE_VELOCITY ) );
    }
    else
    {
        SetVelocity( Vec3::ZERO );
    }
}
