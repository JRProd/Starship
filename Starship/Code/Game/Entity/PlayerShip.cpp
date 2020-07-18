#include "PlayerShip.hpp"

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

//-------------------------------------------------------------------------------
PlayerShip::PlayerShip( Game* game, const Vec3& startingPosition )
    : Entity( game, startingPosition )
{
    m_PhysicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
    m_CosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
}

//-------------------------------------------------------------------------------
PlayerShip::~PlayerShip()
{
    // DO NOTHING
}

//-------------------------------------------------------------------------------
void PlayerShip::Update( float deltaSeconds )
{
    if ( m_IsDead )
    {
        return;
    }

    ProcessInput();

    BounceOffSides();

    float accelLength = RangeMapFloat( 0.f,
                                       PLAYER_SHIP_ACCELERATION,
                                       0.f,
                                       1.f,
                                       GetAcceleration().GetLength() );
    m_RandomThurstOffset.x = m_Game->GetRng()->FloatInRange( -1.5f,
                                                             .5f ) * accelLength;
    m_RandomThurstOffset.y = m_Game->GetRng()->FloatInRange( -.5f,
                                                             .5f ) * accelLength;

    Entity::Update( deltaSeconds );
}

//-------------------------------------------------------------------------------
void PlayerShip::Render() const
{
    if ( IsDead() )
    {
        return;
    }

    float joystickMag = g_InputSystem->GetXboxController( 0 ).GetLeftJoystick().GetMagnitude();
    Vec2 exhaust = Vec2( -2.f - 4.f * joystickMag, 0 ) + m_RandomThurstOffset;

    std::vector<VertexMaster> visual;
        visual.push_back(VertexMaster( Vec2( -2.5f, 2.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( -1.5f, 1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( -1.5f, 2.f ), PLAYER_SHIP_COLOR_1 ) );

        visual.push_back(VertexMaster( Vec2( -1.5f, 2.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back(VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back(VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_2 )  );

        visual.push_back(VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_1 ));
        visual.push_back(VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_1 )  );

        visual.push_back(VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back(VertexMaster( Vec2( -1.5f, -2.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back(VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_2 ) );

        visual.push_back(VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_1 )  );
        visual.push_back(VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( 2.5f, 0.f ), PLAYER_SHIP_COLOR_1 )  );

        visual.push_back(VertexMaster( Vec2( -1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( -2.5f, -2.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back(VertexMaster( Vec2( -1.5f, -2.f ), PLAYER_SHIP_COLOR_1 ) );

        visual.push_back(VertexMaster( Vec2( -2.f, 0.f ), PLAYER_SHIP_EXHAUST_1 ) );
        visual.push_back(VertexMaster( Vec2( -1.7f, 1.f ), PLAYER_SHIP_EXHAUST_1  ) );
        visual.push_back(VertexMaster( exhaust, PLAYER_SHIP_EXHAUST_2 )         );

        visual.push_back(VertexMaster( Vec2( -1.7f, -1.f ), PLAYER_SHIP_EXHAUST_1 ));
        visual.push_back(VertexMaster( Vec2( -2.f, 0.f ), PLAYER_SHIP_EXHAUST_1 ) );
        visual.push_back( VertexMaster( exhaust, PLAYER_SHIP_EXHAUST_2 ) );

    TransformVertexArray( visual,
                          static_cast<Vec2>(m_Position),
                          m_AngleDegrees,
                          m_UniformScale );
    g_Renderer->DrawVertexArray( visual );
}

//-------------------------------------------------------------------------------
void PlayerShip::Die()
{
    m_Game->AddScreenShake( 1.f );
    m_Game->AddControllerVibration( 0, .75f, .45f );

    m_Game->CreateDebrisClusterAt( m_Position,
                                   PLAYER_SHIP_COLOR_1,
                                   2.f,
                                   45,
                                   3.5f );
}

//-------------------------------------------------------------------------------
bool PlayerShip::IsThrusting() const
{
    return m_Thrusting;
}

//-------------------------------------------------------------------------------
void PlayerShip::SetThrusting( bool newThrusting )
{
    if ( m_IsDead )
    {
        return;
    }
    m_Thrusting = newThrusting;
}

//-------------------------------------------------------------------------------
void PlayerShip::TurnLeft( float deltaSeconds )
{
    AddAngleDegrees( PLAYER_SHIP_TURN_SPEED * deltaSeconds );
}

//-------------------------------------------------------------------------------
void PlayerShip::TurnRight( float deltaSeconds )
{
    if ( m_IsDead )
    {
        return;
    }
    AddAngleDegrees( -PLAYER_SHIP_TURN_SPEED * deltaSeconds );
}

//-------------------------------------------------------------------------------
void PlayerShip::ShootBullet()
{
    if ( m_IsDead )
    {
        return;
    }
    m_Game->RequestSpawnBullet( GetNoseSpawn(), m_AngleDegrees );
}

//-------------------------------------------------------------------------------
void PlayerShip::RespawnShip()
{
    if ( !m_IsDead )
    {
        return;
    }

    m_Position = Vec3( WORLD_CENTER_X, WORLD_CENTER_Y, 0.f );
    m_Velocity = Vec3( 0.f, 0.f, 0.f );
    m_Acceleration = Vec3( 0.f, 0.f, 0.f );
    m_AngleDegrees = 0.f;
    m_Health = 1;
    m_IsDead = false;
}

//-------------------------------------------------------------------------------
Vec3 PlayerShip::GetNoseSpawn()
{
    return m_Position + Vec3::MakeFromPolarDegreesXY( m_AngleDegrees,
                                                      2.75f * m_UniformScale );
}

void PlayerShip::ProcessInput()
{

    if ( g_InputSystem->IsKeyPressed( 'W' ) )
    {
        SetAcceleration( Vec3::MakeFromPolarDegreesXY( m_AngleDegrees,
                                                       PLAYER_SHIP_ACCELERATION ) );
    }
    if ( g_InputSystem->IsKeyPressed( UP_ARROW ) )
    {
        SetAcceleration( Vec3::MakeFromPolarDegreesXY( m_AngleDegrees,
                                                       PLAYER_SHIP_ACCELERATION ) );
    }
    if ( !g_InputSystem->IsKeyPressed( 'W' ) &&
         !g_InputSystem->IsKeyPressed( UP_ARROW ) )
    {
        SetAcceleration( Vec3( 0.f, 0.f, 0.f ) );
    }

    if ( g_InputSystem->IsKeyPressed( 'A' ) )
    {
        SetAngularVelocity( PLAYER_SHIP_TURN_SPEED );
    }
    if ( g_InputSystem->IsKeyPressed( 'D' ) )
    {
        SetAngularVelocity( -PLAYER_SHIP_TURN_SPEED );
    }
    if ( g_InputSystem->IsKeyPressed( LEFT_ARROW ) )
    {
        SetAngularVelocity( PLAYER_SHIP_TURN_SPEED );
    }
    if ( g_InputSystem->IsKeyPressed( RIGHT_ARROW ) )
    {
        SetAngularVelocity( -PLAYER_SHIP_TURN_SPEED );
    }

    if ( g_InputSystem->IsKeyPressed( 'A' ) &&
         g_InputSystem->IsKeyPressed( 'D' ) )
    {
        SetAngularVelocity( 0.f );
    }

    if ( g_InputSystem->IsKeyPressed( LEFT_ARROW ) &&
         g_InputSystem->IsKeyPressed( RIGHT_ARROW ) )
    {
        SetAngularVelocity( 0.f );
    }

    if ( !g_InputSystem->IsKeyPressed( 'A' ) &&
         !g_InputSystem->IsKeyPressed( 'D' ) &&
         !g_InputSystem->IsKeyPressed( LEFT_ARROW ) &&
         !g_InputSystem->IsKeyPressed( RIGHT_ARROW ) )
    {
        SetAngularVelocity( 0.f );
    }

    if ( g_InputSystem->WasKeyJustPressed( SPACE ) && m_Age > 0.f )
    {
        ShootBullet();
    }

    const XboxController& gamepad = g_InputSystem->GetXboxController( 0 );
    if ( !gamepad.IsConnected() ) { return; }

    const AnalogJoystick& joystick = gamepad.GetLeftJoystick();
    if ( joystick.GetMagnitude() > 0 )
    {
        SetAngleDegrees( joystick.GetAngleDegrees() );
        SetAcceleration( Vec3::MakeFromPolarDegreesXY( GetAngleDegrees(),
                                                       PLAYER_SHIP_ACCELERATION * joystick.GetMagnitude() ) );
    }

    if ( gamepad.IsButtonJustPressed( XboxButtonID::XBOX_BUTTON_A ) )
    {
        ShootBullet();
    }
}

//-------------------------------------------------------------------------------
void PlayerShip::BounceOffSides()
{
    // Bounce off left side
    if ( m_Position.x - m_CosmeticRadius < 0 )
    {
        m_Position.x = m_CosmeticRadius;
        m_Velocity.x = -m_Velocity.x;
    }
    // Bounce off right side
    if ( m_Position.x + m_CosmeticRadius > WORLD_SIZE_X )
    {
        m_Position.x = WORLD_SIZE_X - m_CosmeticRadius;
        m_Velocity.x = -m_Velocity.x;
    }
    // Bounce of bottom side
    if ( m_Position.y - m_CosmeticRadius < 0 )
    {
        m_Position.y = m_CosmeticRadius;
        m_Velocity.y = -m_Velocity.y;
    }
    // Bounce of top side
    if ( m_Position.y + m_CosmeticRadius > WORLD_SIZE_Y )
    {
        m_Position.y = WORLD_SIZE_Y - m_CosmeticRadius;
        m_Velocity.y = -m_Velocity.y;
    }
}
