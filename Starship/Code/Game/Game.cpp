#include "Game.hpp"


#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Entity/PlayerShip.hpp"
#include "Game/Entity/Asteroid.hpp"
#include "Game/Entity/Bullet.hpp"
#include "Game/Entity/Debris.hpp"
#include "Game/Entity/Beetle.hpp"
#include "Game/Entity/Wasp.hpp"

#include <vector>


//-----------------------------------------------------------------------------
Game::Game()
{
}

//-----------------------------------------------------------------------------
Game::~Game()
{
}

//-----------------------------------------------------------------------------
void Game::Startup()
{
    m_Rng = new RandomNumberGenerator();
    m_PlayerShip = new PlayerShip( this,
                                   Vec3( WORLD_CENTER_X, WORLD_CENTER_Y, 0.f )
                                 );
    m_PlayerShipCurrentLife = 1;

    m_GameCamera = new Camera( g_Renderer );
    m_UICamera = new Camera( g_Renderer );

    const AABB2 screenSize( Vec2::ZERO, Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
    m_GameCamera->SetProjectionOrthographic( screenSize );
    m_GameCamera->SetClearMode( CLEAR_COLOR_BIT | CLEAR_DEPTH_BIT, Rgba8::BLACK );
    m_GameCamera->SetColorTarget( g_Renderer->GetBackBuffer() );


    m_UICamera->SetProjectionOrthographic( screenSize );
    m_UICamera->SetColorTarget( g_Renderer->GetBackBuffer() );

    m_IsAttractMode = true;

    for( int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex )
    {
        m_Asteroids[ astroidIndex ] = nullptr;
    }

    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        m_Bullets[ bulletIndex ] = nullptr;
    }

    for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
    {
        m_Debris[ debrisIndex ] = nullptr;
    }

    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        m_Beetles[ beetleIndex ] = nullptr;
    }

    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        m_Wasps[ waspIndex ] = nullptr;
    }

    m_SpawnNextWave = true;
}

//-----------------------------------------------------------------------------
void Game::Shutdown()
{
    for( int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex )
    {
        if( m_Asteroids[ astroidIndex ] == nullptr )
        {
            delete m_Asteroids[ astroidIndex ];
            m_Asteroids[ astroidIndex ] = nullptr;
        }
    }

    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        if( m_Bullets[ bulletIndex ] == nullptr )
        {
            delete m_Bullets[ bulletIndex ];
            m_Bullets[ bulletIndex ] = nullptr;
        }
    }

    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        if( m_Beetles[ beetleIndex ] == nullptr )
        {
            delete m_Beetles[ beetleIndex ];
            m_Beetles[ beetleIndex ] = nullptr;
        }
    }

    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        if( m_Wasps[ waspIndex ] == nullptr )
        {
            delete m_Wasps[ waspIndex ];
            m_Wasps[ waspIndex ] = nullptr;
        }
    }

    for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
    {
        if( m_Debris[ debrisIndex ] == nullptr )
        {
            delete m_Debris[ debrisIndex ];
            m_Debris[ debrisIndex ] = nullptr;
        }
    }

    delete m_PlayerShip;
    m_PlayerShip = nullptr;

    delete m_Rng;
    m_Rng = nullptr;

    delete m_GameCamera;
    m_GameCamera = nullptr;

    delete m_UICamera;
    m_UICamera = nullptr;
}

//-----------------------------------------------------------------------------
RandomNumberGenerator* Game::GetRng()
{
    return m_Rng;
}

//-----------------------------------------------------------------------------
bool Game::RequestSpawnAstroid()
{
    for( int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex )
    {
        if( m_Asteroids[ astroidIndex ] == nullptr )
        {
            CreateAstroidInArray( astroidIndex );
            return true;
        }
    }
    return false;
}

bool Game::RequestSpawnBeetle()
{
    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        if( m_Beetles[ beetleIndex ] == nullptr )
        {
            Entity*& currentBeetle = m_Beetles[ beetleIndex ];
            Vec2 outOfBounds = PointJustOffScreen( 25.f );
            Vec3 startingPos = Vec3( outOfBounds.x, outOfBounds.y, 0.f );
            currentBeetle = new Beetle( this, startingPos );
            currentBeetle->Create();
            return true;
        }
    }
    return false;
}

bool Game::RequestSpawnWasp()
{
    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        if( m_Wasps[ waspIndex ] == nullptr )
        {
            Entity*& currentWasp = m_Wasps[ waspIndex ];
            Vec2 outOfBounds = PointJustOffScreen( 25.f );
            Vec3 startingPos = Vec3( outOfBounds.x, outOfBounds.y, 0.f );
            currentWasp = new Wasp( this, startingPos );
            currentWasp->Create();
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
bool Game::RequestSpawnBullet( const Vec3& position, float degrees )
{
    bool anyBulletSpawned = false;
    for( int bulletNumber = 0; bulletNumber < PLAYER_BULLETS_PER_SHOT; ++bulletNumber )
    {
        for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
        {
            if( m_Bullets[ bulletIndex ] == nullptr )
            {
                // More bullets spawned less accurate they are
                degrees = degrees + m_Rng->FloatInRange( -5.f, 5.f ) * bulletNumber;
                m_Bullets[ bulletIndex ] = new Bullet( this, position );
                m_Bullets[ bulletIndex ]->SetAngleDegrees( degrees );
                m_Bullets[ bulletIndex ]->SetVelocity(
                                                      Vec3::MakeFromPolarDegreesXY( degrees, BULLET_SPEED )
                                                     );

                AddControllerVibration( 0, .0f, .1f );
                anyBulletSpawned = true;
                break;
            }
        }
    }
    if( anyBulletSpawned )
    {
        return true;
    }
    ErrorRecoverable( "Maximum of 20 Bullets alive at once; attempting to spawn an 21st Bullet" );
    return false;
}

void Game::AddScreenShake( float screenShakePercentage )
{
    m_CurrentScreenShakePercentage += screenShakePercentage;
    m_CurrentScreenShakePercentage = Clamp( m_CurrentScreenShakePercentage,
                                            0.f,
                                            1.f
                                          );
}

void Game::AddControllerVibration( int contollerId,
                                   float leftVibrationPercent,
                                   float rightVibrationPercent )
{
    UNUSED( contollerId );

    m_CurrentControllerLeftVibration += leftVibrationPercent;
    m_CurrentControllerRightVibration += rightVibrationPercent;
}

const PlayerShip* Game::GetAlivePlayer() const
{
    if( m_PlayerShip == nullptr )
    {
        return nullptr;
    }
    if( m_PlayerShip->IsDead() )
    {
        return nullptr;
    }

    return m_PlayerShip;
}

//-----------------------------------------------------------------------------
void Game::Update( float deltaSeconds )
{
    m_GameTime += deltaSeconds;

    m_GameCamera->SetCameraPosition( Vec3::ZERO );

    HandleUserInput();

    ScreenShakeAblation( deltaSeconds );
    ControllerVibrationAblation( deltaSeconds );

    if( g_InputSystem->GetXboxController( 0 )
                     .IsButtonJustPressed( XBOX_BUTTON_START ) )
    {
        m_IsAttractMode = false;
        if( m_PlayerShip->IsDead() )
        {
            RequestShipRespawn();
        }
    }

    if( m_IsAttractMode )
    {
        AttractionMode( deltaSeconds );
    }

    if( !m_IsAttractMode && m_WasJustAttractMode )
    {
        PostAttractionExplosion();
        m_WasJustAttractMode = m_IsAttractMode;
    }

    if( m_PlayerShipCurrentLife >= MAX_NUMBER_OF_LIVES && m_PlayerShip->IsDead() )
    {
        if( m_PlayerShipDestroyedTime == 0 )
        {
            m_PlayerShipDestroyedTime = m_GameTime;
        }
        if( m_PlayerShipDestroyedTime + TIME_AFTER_DEATH_BEFORE_ATTRACT < m_GameTime )
        {
            m_IsAttractMode = true;
            m_PlayerShipDestroyedTime = 0.f;
            DeleteAllEntities();
        }
    }

    deltaSeconds = UpdateDeltaSecondsBasedOnState( deltaSeconds );

    if( m_SpawnNextWave )
    {
        SpawnWave();
    }

    m_PlayerShip->Update( deltaSeconds );
    UpdateEntities( deltaSeconds, m_Asteroids, MAX_ASTEROIDS );
    UpdateEntities( deltaSeconds, m_Bullets, MAX_BULLETS );
    UpdateEntities( deltaSeconds, m_Debris, MAX_DEBRIS );
    UpdateEntities( deltaSeconds, m_Beetles, MAX_BEETLES );
    UpdateEntities( deltaSeconds, m_Wasps, MAX_WASPS );

    PhysicsCollisions();

    m_SpawnNextWave = CheckWaveComplete();

    DeleteGarbageEntities();
}

//-----------------------------------------------------------------------------
void Game::Render() const
{
    g_Renderer->ClearColor( *m_GameCamera );

    // Render Game
    g_Renderer->BeginCamera( *m_GameCamera );

    if( m_IsAttractMode )
    {
        RenderAttractMode();
    }
    else
    {
        m_PlayerShip->Render();

        RenderEntities( m_Asteroids, MAX_ASTEROIDS );
        RenderEntities( m_Bullets, MAX_BULLETS );
        RenderEntities( m_Debris, MAX_DEBRIS );
        RenderEntities( m_Beetles, MAX_BEETLES );
        RenderEntities( m_Wasps, MAX_WASPS );
    }


    if( m_IsDebug )
    {
        DebugRender();
    }

    g_Renderer->EndCamera( *m_GameCamera );

    // Render UI
    g_Renderer->BeginCamera( *m_UICamera );

    RenderLives();

    g_Renderer->EndCamera( *m_UICamera );
}

//-----------------------------------------------------------------------------
void Game::DebugRender() const
{
    bool isShip = false;
    Vec2 shipPosition = Vec2( 0.f, 0.f );
    if( m_PlayerShip != nullptr )
    {
        m_PlayerShip->DebugRender();
        isShip = true;
        shipPosition = Vec2( m_PlayerShip->GetPosition().x,
                             m_PlayerShip->GetPosition().y
                           );
    }

    DebugRenderEntities( m_Asteroids, MAX_ASTEROIDS );
    DebugRenderEntities( m_Bullets, MAX_BULLETS );
    DebugRenderEntities( m_Debris, MAX_DEBRIS );
    DebugRenderEntities( m_Beetles, MAX_BEETLES );
    DebugRenderEntities( m_Wasps, MAX_WASPS );


    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        Entity* const& currentBullet = m_Bullets[ bulletIndex ];
        if( currentBullet != nullptr )
        {
            currentBullet->DebugRender();
            if( isShip )
            {
                DrawDebugLine( shipPosition,
                               Vec2( currentBullet->GetPosition().x,
                                     currentBullet->GetPosition().y
                                   ),
                               Rgba8::DARK_GRAY,
                               .1f
                             );
            }
        }
    }

    if( g_InputSystem->GetXboxController( 0 ).IsConnected() )
    {
        XboxController const& gamepad = g_InputSystem->GetXboxController( 0 );
        Vec2 centerLeft = Vec2( 25.f, 25.f );
        Vec2 centerRight = Vec2( WORLD_SIZE_X - 25.f, 25.f );
        float circleRadius = 20.f;
        DrawDebugCircle( centerLeft, circleRadius, Rgba8::WHITE, .1f );
        DrawDebugCircle( centerRight, circleRadius, Rgba8::WHITE, .1f );

        // Draw Left Joystick Debug
        DrawDebugCircle( centerLeft + gamepad.GetLeftJoystick().GetPosition() * circleRadius,
                         1.f,
                         Rgba8::GREEN,
                         .1f
                       );
        DrawDebugCircle( centerLeft + gamepad.GetLeftJoystick().GetRawPosition() * circleRadius,
                         1.f,
                         Rgba8::RED,
                         .1f
                       );
        DrawDebugLine( centerLeft,
                       centerLeft + gamepad.GetLeftJoystick().GetPosition() * circleRadius,
                       Rgba8::GREEN,
                       .1f
                     );
        DrawDebugLine( centerLeft,
                       centerLeft + gamepad.GetLeftJoystick().GetRawPosition() * circleRadius,
                       Rgba8::RED,
                       .1f
                     );

        // Draw Right Joystick Debug
        DrawDebugCircle( centerRight + gamepad.GetRightJoystick().GetPosition() * circleRadius,
                         1.f,
                         Rgba8::GREEN,
                         .1f
                       );
        DrawDebugCircle( centerRight + gamepad.GetRightJoystick().GetRawPosition() * circleRadius,
                         1.f,
                         Rgba8::RED,
                         .1f
                       );
        DrawDebugLine( centerRight,
                       centerRight + gamepad.GetRightJoystick().GetPosition() * circleRadius,
                       Rgba8::GREEN,
                       .1f
                     );
        DrawDebugLine( centerRight,
                       centerRight + gamepad.GetRightJoystick().GetRawPosition() * circleRadius,
                       Rgba8::RED,
                       .1f
                     );

        Vec2 leftTriggerStart = centerLeft + Vec2( 25.f, -20.f );
        Vec2 rightTriggerStart = centerRight + Vec2( -25.f, -20.f );
        float triggerDistance = circleRadius * 2.f;
        // Draw Left Trigger Debug
        DrawDebugLine( leftTriggerStart,
                       leftTriggerStart + Vec2( 0.f, 1.f ) * triggerDistance * gamepad.GetLeftTrigger(),
                       Rgba8::CYAN,
                       .5f
                     );
        DrawDebugLine( rightTriggerStart,
                       rightTriggerStart + Vec2( 0.f, 1.f ) * triggerDistance * gamepad.GetRightTrigger(),
                       Rgba8::MAGENTA,
                       .5f
                     );
    }
}

void Game::HandleUserInput()
{
    if( g_InputSystem->WasKeyJustPressed( F1 ) )
    {
        m_IsDebug = !m_IsDebug;
    }

    if( g_InputSystem->IsKeyPressed( SPACE ) )
    {
        if( m_IsAttractMode )
        {
            m_IsAttractMode = false;
        }
    }

    if( g_InputSystem->WasKeyJustPressed( 'N' ) )
    {
        if( m_IsAttractMode )
        {
            m_IsAttractMode = false;
        }
        else
        {
            RequestShipRespawn();
        }
    }

    if( g_InputSystem->WasKeyJustPressed( 'O' ) )
    {
        RequestSpawnAstroid();
    }

    if( g_InputSystem->WasKeyJustPressed( 'P' ) )
    {
        m_IsPaused = !m_IsPaused;
    }

    if( g_InputSystem->WasKeyJustPressed( 'T' ) )
    {
        m_IsSlowMo = true;
    }

    if( g_InputSystem->WasKeyJustReleased( 'T' ) )
    {
        m_IsSlowMo = false;
    }
}

void Game::AttractionMode( float deltaSeconds )
{
    if( !m_IsAttractMode && m_WasJustAttractMode )
    {
        PostAttractionExplosion();
    }
    m_WasJustAttractMode = m_IsAttractMode;

    AttractModeDefaults();

    float noise = m_Rng->FloatInRange( -.03f, .03f );
    float rotationResult = sinf( m_TitleTime + noise );
    float scaleResult = sinf( (m_TitleTime + noise) * 2 );
    m_TitleRotaiton = STARTING_TITLE_ROTAITON + MAX_TITLE_ROTATION * rotationResult;
    m_TitleScale = STARTING_TITLE_SCALE + MAX_TITLE_SCALE_INCREASE * scaleResult;

    if( m_LastColorChangeTime + TITLE_COLER_CHANGE_TIME < m_TitleTime )
    {
        m_LastColorChangeTime = m_TitleTime;

        m_TitleColor = Rgba8( (unsigned char)m_Rng->IntLessThan( 256 ),
                              (unsigned char)m_Rng->IntLessThan( 256 ),
                              (unsigned char)m_Rng->IntLessThan( 256 )
                            );
    }

    m_TitleTime += deltaSeconds;
    return;
}

void Game::PostAttractionExplosion()
{
    int randomExplosion = m_Rng->IntInRange( 3, 7 );
    Vec3 worldCenter = Vec3( WORLD_CENTER_X, WORLD_CENTER_Y, 0.f );

    for( int explosion = 0; explosion < randomExplosion; ++explosion )
    {
        Vec3 offset = Vec3( m_Rng->FloatInRange( -40.f, 40.f ),
                            m_Rng->FloatInRange( -10.f, 10.f ),
                            0.f
                          );
        CreateDebrisClusterAt( worldCenter + offset,
                               m_TitleColor,
                               m_Rng->FloatInRange( 1.5f, 3.5f ),
                               m_Rng->IntInRange( 35, 100 ),
                               m_Rng->FloatInRange( .5f, 3.f )
                             );
    }

    AddControllerVibration( 0, .4f, .6f );
    AddScreenShake( 1.f );
}

void Game::AttractModeDefaults()
{
    AddScreenShake( -1.f );
    AddControllerVibration( 0, -1.f, -1.f );

    m_PlayerShip->SetAngleDegrees( 0.f );
    m_PlayerShip->SetPosition( Vec3( WORLD_CENTER_X, WORLD_CENTER_Y, 0.f ) );
    m_PlayerShip->SetVelocity( Vec3::ZERO );
    m_PlayerShip->SetDead( false );

    m_PlayerShipCurrentLife = 1;
    m_WaveNumber = 0;
}

void Game::RenderAttractMode() const
{
    Vec3 center = Vec3( WORLD_CENTER_X, WORLD_CENTER_Y + 10.f, 0.f );

    std::vector<VertexMaster> visual;
    // S
    visual.emplace_back( Vec2( -11.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -9.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -9.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -11.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -10.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -10.5f, .5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -10.5f, .5f ), m_TitleColor );
    visual.emplace_back( Vec2( -10.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -9.5f, -1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -11.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -11.5f, -1.5f ), m_TitleColor);
    visual.emplace_back( Vec2( -9.5f, -1.5f ), m_TitleColor);

    // T
    visual.emplace_back( Vec2( -8.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -8.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -7.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -6.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -6.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -7.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -8.f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -7.f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -7.5f, 1.5f ), m_TitleColor );

    // A
    visual.emplace_back( Vec2( -5.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -5.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -3.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -5.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -3.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -3.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -5.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -5.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -4.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -4.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -3.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -3.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -5.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -4.5f, -1.f ), m_TitleColor );
    visual.emplace_back( Vec2( -4.5f, 0.f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( -3.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -4.5f, 0.f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -4.5f, -1.f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( -2.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -2.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -1.5f, 1.5f ), m_TitleColor );

    // R
    visual.emplace_back( Vec2( -2.5f, 1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -.5f, 1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( -2.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -.5f, .5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( -2.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( -2.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( -.5f, -1.5f ), m_TitleColor );
                                                            
    // S
    visual.emplace_back( Vec2( .5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 2.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 2.5f, 1.5f ), m_TitleColor ) ;

    visual.emplace_back( Vec2( .5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 1.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 1.5f, .5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 1.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 1.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 2.5f, -1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( 2.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( .5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( .5f, -1.5f ), m_TitleColor ) ;
                                                            
    // H
    visual.emplace_back( Vec2( 3.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 3.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 4.5f, 1.5f ), m_TitleColor ) ;
    
    visual.emplace_back( Vec2( 4.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 5.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 5.5f, 1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 3.5f, 0.f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 4.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 4.5f, .5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 4.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 4.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 5.5f, 0.f ), m_TitleColor ) ;

    // I
    visual.emplace_back( Vec2( 6.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 6.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 7.5f, 1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 7.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 8.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 8.5f, 1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 7.f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 7.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 8.f, 1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 7.f, -1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 7.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 8.f, -1.5f ), m_TitleColor ) ;
                                                            
    visual.emplace_back( Vec2( 6.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 6.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 7.5f, -1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( 7.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 8.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 8.5f, -.5f ), m_TitleColor ) ;
                                                            
    // P                                                    
    visual.emplace_back( Vec2( 9.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 9.5f, -1.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 10.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( 9.5f, 1.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 11.5f, .5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 11.5f, 1.5f ), m_TitleColor );
                                                            
    visual.emplace_back( Vec2( 9.5f, -.5f ), m_TitleColor ) ;
    visual.emplace_back( Vec2( 11.5f, -.5f ), m_TitleColor );
    visual.emplace_back( Vec2( 11.5f, .5f ), m_TitleColor ) ;

    TransformVertexArray( visual, Vec2(100,50) , m_TitleRotaiton, m_TitleScale );

    g_Renderer->DrawVertexArray( visual );
}

float Game::UpdateDeltaSecondsBasedOnState( float deltaSeconds )
{
    if( m_IsSlowMo ) deltaSeconds = deltaSeconds * m_SlowMoPercentage;
    if( deltaSeconds > 1.f / 30.f )
    {
        deltaSeconds = 1.f / 30.f;
    }
    if( m_IsPaused ) deltaSeconds = 0;

    return deltaSeconds;
}

void Game::SpawnWave()
{
    static int asteroidsWaveNumbers[ ] = {4, 4, 4, 8, 16};
    static int beetleWaveNumbers[ ] = {0, 2, 2, 4, 8};
    static int waspWaveNumbers[ ] = {0, 0, 2, 4, 4};

    if( m_WaveNumber >= MAX_NUMBER_OF_WAVES )
    {
        m_IsAttractMode = true;
        DeleteAllEntities();
    }

    int asteroidsThisWave = asteroidsWaveNumbers[ m_WaveNumber ];
    int beetlesThisWave = beetleWaveNumbers[ m_WaveNumber ];
    int waspsThisWave = waspWaveNumbers[ m_WaveNumber ];

    for( int astroidIndex = 0; astroidIndex < asteroidsThisWave; ++astroidIndex )
    {
        RequestSpawnAstroid();
    }

    for( int beetleIndex = 0; beetleIndex < beetlesThisWave; ++beetleIndex )
    {
        RequestSpawnBeetle();
    }

    for( int waspIndex = 0; waspIndex < waspsThisWave; ++waspIndex )
    {
        RequestSpawnWasp();
    }

    m_SpawnNextWave = false;
    m_WaveNumber++;
}

bool Game::CheckWaveComplete()
{
    for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
    {
        if( m_Asteroids[ asteroidIndex ] != nullptr )
        {
            return false;
        }
    }

    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        if( m_Beetles[ beetleIndex ] != nullptr )
        {
            return false;
        }
    }

    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        if( m_Wasps[ waspIndex ] != nullptr )
        {
            return false;
        }
    }

    return true;
}

void Game::ScreenShakeAblation( float deltaSeconds )
{
    if( m_CurrentScreenShakePercentage > 0 )
    {
        Vec2 shake = Vec2(
                          m_Rng->FloatInRange( -MAX_SCREEN_SHAKE, MAX_SCREEN_SHAKE ),
                          m_Rng->FloatInRange( -MAX_SCREEN_SHAKE, MAX_SCREEN_SHAKE )
                         );
        shake *= m_CurrentScreenShakePercentage * m_CurrentScreenShakePercentage;
        m_GameCamera->SetCameraPosition( m_GameCamera->GetPosition() + static_cast<Vec3>(shake) );
        m_CurrentScreenShakePercentage -= SCREEN_SHAKE_ABLATION_PER_SECOND * deltaSeconds;
    }
}

void Game::ControllerVibrationAblation( float deltaSeconds )
{
    if( m_CurrentControllerLeftVibration < 0 )
    {
        m_CurrentControllerLeftVibration = 0;
    }
    if( m_CurrentControllerRightVibration < 0 )
    {
        m_CurrentControllerRightVibration = 0;
    }
    if( m_CurrentControllerLeftVibration >= 0 ||
        m_CurrentControllerRightVibration >= 0 )
    {
        g_InputSystem->SetXboxControllerVibration( 0,
                                                   m_CurrentControllerLeftVibration,
                                                   m_CurrentControllerRightVibration
                                                 );
        m_CurrentControllerLeftVibration -= CONTROLLER_VIBRATION_ABLATION_PER_SECOND * deltaSeconds;
        m_CurrentControllerRightVibration -= CONTROLLER_VIBRATION_ABLATION_PER_SECOND * deltaSeconds;

        if( m_CurrentControllerLeftVibration < 0 )
        {
            m_CurrentControllerLeftVibration = 0;
        }
        if( m_CurrentControllerRightVibration < 0 )
        {
            m_CurrentControllerRightVibration = 0;
        }
    }
}

//-----------------------------------------------------------------------------
void Game::UpdateEntities( float deltaSeconds,
                           Entity** const entities,
                           int entitiesSize )
{
    for( int entitiyIndex = 0; entitiyIndex < entitiesSize; ++entitiyIndex )
    {
        Entity*& currentEntity = entities[ entitiyIndex ];
        if( currentEntity != nullptr )
        {
            currentEntity->Update( deltaSeconds );
        }
    }
}

//-----------------------------------------------------------------------------
void Game::RenderEntities( const Entity* const* entities,
                           int entitiesSize ) const
{
    for( int entityIndex = 0; entityIndex < entitiesSize; ++entityIndex )
    {
        const Entity* const& currentEntity = entities[ entityIndex ];
        if( currentEntity != nullptr )
        {
            currentEntity->Render();
        }
    }
}

void Game::DebugRenderEntities( const Entity* const* entities,
                                int entitiesSize ) const
{
    bool isShip = false;
    Vec2 shipPosition = Vec2( 0.f, 0.f );
    if( m_PlayerShip != nullptr )
    {
        isShip = true;
        shipPosition = Vec2( m_PlayerShip->GetPosition().x,
                             m_PlayerShip->GetPosition().y
                           );
    }

    for( int entityIndex = 0; entityIndex < entitiesSize; ++entityIndex )
    {
        const Entity* const& currentEntity = entities[ entityIndex ];
        if( currentEntity != nullptr )
        {
            if( isShip && dynamic_cast<const Debris*>(currentEntity) == nullptr )
            {
                currentEntity->DebugRender();
                DrawDebugLine( shipPosition,
                               Vec2( currentEntity->GetPosition().x,
                                     currentEntity->GetPosition().y
                                   ),
                               Rgba8::DARK_GRAY,
                               .1f
                             );
            }
        }
    }
}

void Game::RequestShipRespawn()
{
    if( m_PlayerShip->IsDead() )
    {
        if( m_PlayerShipCurrentLife >= MAX_NUMBER_OF_LIVES )
        {
            return;
        }
        m_PlayerShipCurrentLife += 1;

        m_PlayerShip->RespawnShip();
    }

}

void Game::RenderLives() const
{
    for( int liveIndex = 0; liveIndex < MAX_NUMBER_OF_LIVES - m_PlayerShipCurrentLife; ++liveIndex )
    {
        std::vector<VertexMaster> visual;
        // Life 1
        visual.push_back(VertexMaster( Vec2( -2.5f, 2.f ), PLAYER_SHIP_COLOR_1 ));
        visual.push_back(VertexMaster( Vec2( -1.5f, 1.f ), PLAYER_SHIP_COLOR_1 ));
        visual.push_back(VertexMaster( Vec2( -1.5f, 2.f ), PLAYER_SHIP_COLOR_1 ));

        visual.push_back( VertexMaster( Vec2( -1.5f, 2.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back( VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back( VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_2 ));

        visual.push_back( VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_1 ));

        visual.push_back( VertexMaster( Vec2( -1.5f, 0.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back( VertexMaster( Vec2( -1.5f, -2.f ), PLAYER_SHIP_COLOR_2 ) );
        visual.push_back( VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_2 ));

        visual.push_back( VertexMaster( Vec2( 1.5f, 1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( 1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( 2.5f, 0.f ), PLAYER_SHIP_COLOR_1 ));

        visual.push_back( VertexMaster( Vec2( -1.5f, -1.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( -2.5f, -2.f ), PLAYER_SHIP_COLOR_1 ) );
        visual.push_back( VertexMaster( Vec2( -1.5f, -2.f ), PLAYER_SHIP_COLOR_1 ));

        Vec2 liveDisplayPosition = Vec2( 5.f, WORLD_SIZE_Y - 5.f );
        float scale = .5f;
        Vec2 displacement = Vec2( 7.f * scale, 0.f );
        Vec2 livePosition = liveDisplayPosition + displacement * (float)liveIndex;
        TransformVertexArray( visual, livePosition, 90.f, scale );

        g_Renderer->DrawVertexArray( visual );
    }
}

//-------------------------------------------------------------------------------
void Game::CreateAstroidInArray( int x )
{
    Vec3 startingPoint = Vec3::ZERO;
    do
    {
        startingPoint.x = m_Rng->FloatInRange( SAFEZONE, WORLD_SIZE_X - SAFEZONE );
        startingPoint.y = m_Rng->FloatInRange( SAFEZONE, WORLD_SIZE_Y - SAFEZONE );
    }
    while( Vec3::GetDistance( startingPoint, m_PlayerShip->GetPosition() ) < CLOSEST_ASTEROID_SPAWN_TO_SHIP );

    m_Asteroids[ x ] = new Asteroid( this, startingPoint );

    Entity* thisAstroid = m_Asteroids[ x ];
    thisAstroid->Create();
    float degree = m_Rng->FloatInRange( 0.f, 360.f );
    float angularVelocity = m_Rng->FloatInRange( -200.f, 200.f );
    thisAstroid->AddAngularVelocity( angularVelocity );
    thisAstroid->SetVelocity( Vec3::MakeFromPolarDegreesXY( degree, ASTEROID_SPEED ) );
}

void Game::CreateDebrisClusterAt( const Vec3& position,
                                  const Rgba8& color,
                                  float scale, int number,
                                  float lifeSpan )
{
    for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
    {
        if( number <= 0 ) { return; }

        Entity*& currentDebris = m_Debris[ debrisIndex ];
        if( currentDebris == nullptr )
        {
            currentDebris = new Debris( this, position, color, lifeSpan );
            currentDebris->Create();
            // currentDebris->SetUniformScale( scale );
            number--;
        }
    }
}

//-------------------------------------------------------------------------------
void Game::PhysicsCollisions()
{
    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        Entity*& currentBullet = m_Bullets[ bulletIndex ];
        if( currentBullet != nullptr )
        {
            for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
            {
                Entity*& currentAsteroid = m_Asteroids[ asteroidIndex ];
                if( currentAsteroid != nullptr )
                {
                    if( currentAsteroid->OverlapsEntity( *currentBullet ) )
                    {
                        currentAsteroid->DamageEntity( 1 );
                        currentBullet->DamageEntity( 1 );

                        CreateDebrisClusterAt( currentAsteroid->GetPosition(),
                                               ASTEROID_COLOR,
                                               1.5f,
                                               2,
                                               .5f
                                             );
                    }
                }
            }

            for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
            {
                Entity*& currentBeetle = m_Beetles[ beetleIndex ];
                if( currentBeetle != nullptr )
                {
                    if( currentBeetle->OverlapsEntity( *currentBullet ) )
                    {
                        currentBeetle->DamageEntity( 1 );
                        currentBullet->DamageEntity( 1 );

                        CreateDebrisClusterAt( currentBeetle->GetPosition(),
                                               BEETLE_COLOR,
                                               1.f,
                                               4,
                                               .5f
                                             );
                    }
                }
            }

            for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
            {
                Entity*& currentWasp = m_Wasps[ waspIndex ];
                if( currentWasp != nullptr )
                {
                    if( currentWasp->OverlapsEntity( *currentBullet ) )
                    {
                        currentWasp->DamageEntity( 1 );
                        currentBullet->DamageEntity( 1 );

                        CreateDebrisClusterAt( currentWasp->GetPosition(),
                                               WASP_COLOR,
                                               1.f,
                                               4,
                                               .5f
                                             );
                    }
                }
            }
        }
    }

    if( m_PlayerShip != nullptr && !m_PlayerShip->IsDead() )
    {
        // Player Asteroid Collision
        for( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; ++asteroidIndex )
        {
            Entity*& currentAsteroid = m_Asteroids[ asteroidIndex ];
            if( currentAsteroid != nullptr && currentAsteroid->OverlapsEntity( *m_PlayerShip ) )
            {
                currentAsteroid->DamageEntity( 1 );
                m_PlayerShip->DamageEntity( 1 );

                CreateDebrisClusterAt( currentAsteroid->GetPosition(),
                                       ASTEROID_COLOR,
                                       1.5f,
                                       2,
                                       .5f
                                     );
            }
        }

        // Player Beetle Collision
        for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
        {
            Entity*& currentBeetle = m_Beetles[ beetleIndex ];
            if( currentBeetle != nullptr && currentBeetle->OverlapsEntity( *m_PlayerShip ) )
            {
                currentBeetle->DamageEntity( 1 );
                m_PlayerShip->DamageEntity( 1 );

                CreateDebrisClusterAt( currentBeetle->GetPosition(),
                                       BEETLE_COLOR,
                                       1.f,
                                       4,
                                       .5f
                                     );
            }
        }

        // Player Wasp Collision
        for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
        {
            Entity*& currentWasp = m_Wasps[ waspIndex ];
            if( currentWasp != nullptr && currentWasp->OverlapsEntity( *m_PlayerShip ) )
            {
                currentWasp->DamageEntity( 1 );
                m_PlayerShip->DamageEntity( 1 );

                CreateDebrisClusterAt( currentWasp->GetPosition(),
                                       WASP_COLOR,
                                       1.f,
                                       4,
                                       .5f
                                     );
            }
        }
    }
}


//-----------------------------------------------------------------------------
void Game::DeleteGarbageEntities()
{
    for( int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex )
    {
        Entity*& currentAsteroid = m_Asteroids[ astroidIndex ];
        if( currentAsteroid != nullptr )
        {
            if( currentAsteroid->IsGarbage() )
            {
                currentAsteroid->Destroy();
                delete currentAsteroid;
                currentAsteroid = nullptr;
            }
        }
    }

    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        Entity*& currentBullet = m_Bullets[ bulletIndex ];
        if( currentBullet != nullptr )
        {
            if( currentBullet->IsGarbage() )
            {
                currentBullet->Destroy();
                delete currentBullet;
                currentBullet = nullptr;
            }
        }
    }

    for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
    {
        Entity*& currentDebris = m_Debris[ debrisIndex ];
        if( currentDebris != nullptr )
        {
            if( currentDebris->IsGarbage() )
            {
                currentDebris->Destroy();
                delete currentDebris;
                currentDebris = nullptr;
            }
        }
    }

    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        Entity*& currentBeetle = m_Beetles[ beetleIndex ];
        if( currentBeetle != nullptr )
        {
            if( currentBeetle->IsGarbage() )
            {
                currentBeetle->Destroy();
                delete currentBeetle;
                currentBeetle = nullptr;
            }
        }
    }

    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        Entity*& currentWasp = m_Wasps[ waspIndex ];
        if( currentWasp != nullptr )
        {
            if( currentWasp->IsGarbage() )
            {
                currentWasp->Destroy();
                delete currentWasp;
                currentWasp = nullptr;
            }
        }
    }
}

void Game::DeleteAllEntities()
{
    for( int astroidIndex = 0; astroidIndex < MAX_ASTEROIDS; ++astroidIndex )
    {
        Entity*& currentAsteroid = m_Asteroids[ astroidIndex ];
        if( currentAsteroid != nullptr )
        {
            currentAsteroid->Destroy();
            delete currentAsteroid;
            currentAsteroid = nullptr;
        }
    }

    for( int bulletIndex = 0; bulletIndex < MAX_BULLETS; ++bulletIndex )
    {
        Entity*& currentBullet = m_Bullets[ bulletIndex ];
        if( currentBullet != nullptr )
        {
            currentBullet->Destroy();
            delete currentBullet;
            currentBullet = nullptr;
        }
    }

    for( int debrisIndex = 0; debrisIndex < MAX_DEBRIS; ++debrisIndex )
    {
        Entity*& currentDebris = m_Debris[ debrisIndex ];
        if( currentDebris != nullptr )
        {
            currentDebris->Destroy();
            delete currentDebris;
            currentDebris = nullptr;
        }
    }

    for( int beetleIndex = 0; beetleIndex < MAX_BEETLES; ++beetleIndex )
    {
        Entity*& currentBeetle = m_Beetles[ beetleIndex ];
        if( currentBeetle != nullptr )
        {
            currentBeetle->Destroy();
            delete currentBeetle;
            currentBeetle = nullptr;
        }
    }

    for( int waspIndex = 0; waspIndex < MAX_WASPS; ++waspIndex )
    {
        Entity*& currentWasp = m_Wasps[ waspIndex ];
        if( currentWasp != nullptr )
        {
            currentWasp->Destroy();
            delete currentWasp;
            currentWasp = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------
void Game::ErrorRecoverable( const char* message )
{
    if( m_IsSlowMo )
    {
        m_IsSlowMo = false;
    }
    ERROR_RECOVERABLE( message );
    m_WasJustPaused = true;
}
