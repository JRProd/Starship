#pragma once

#include "Engine/Core/Rgba8.hpp"

class Camera;
struct Vec3;

#include "Game/GameCommon.hpp"


class Entity;
class PlayerShip;
class Asteroid;
class Bullet;
class Debris;

class Game
{
public:
    Game();
    ~Game();

    void Startup();
    void Update( float deltaSeconds );
    void Render() const;
    void Shutdown();

    
    RandomNumberGenerator* GetRng();

    bool RequestSpawnAstroid();
    bool RequestSpawnBeetle();
    bool RequestSpawnWasp();
    bool RequestSpawnBullet( const Vec3& position, float degrees );

    void AddScreenShake( float screenShakePercentage );
    void AddControllerVibration( int contollerId,
                                 float leftVibrationPercent,
                                 float rightVibrationPercent );

    const PlayerShip* GetAlivePlayer() const;

    void CreateDebrisClusterAt( const Vec3& position,
                                const Rgba8& color,
                                float scale,
                                int number,
                                float lifeSpan = MAX_DEBRIS_LIFESPAN );

private:
    Camera* m_GameCamera = nullptr;
    Camera* m_UICamera = nullptr;
    RandomNumberGenerator* m_Rng = nullptr;

    PlayerShip* m_PlayerShip = nullptr;
    Entity* m_Bullets[ MAX_BULLETS ] = { nullptr };
    Entity* m_Asteroids[ MAX_ASTEROIDS ] = { nullptr };
    Entity* m_Debris[ MAX_DEBRIS ] = { nullptr };
    Entity* m_Beetles[ MAX_BEETLES ] = { nullptr };
    Entity* m_Wasps[ MAX_WASPS ] = { nullptr };

    float m_GameTime = 0.f;

    Rgba8 m_TitleColor = Rgba8::RED;
    float m_TitleRotaiton = 0.f;
    float m_TitleScale = 3.f;
    float m_TitleTime = 0.f;
    float m_LastColorChangeTime = 0.f;

    int m_PlayerShipCurrentLife = 0;
    double m_PlayerShipDestroyedTime = 0.f;

    bool m_IsDebug = false;

    bool m_SpawnNextWave = false;
    int m_WaveNumber = 0;

    bool m_IsAttractMode = false;
    bool m_WasJustAttractMode = false;
    bool m_IsPaused = false;
    bool m_WasJustPaused = false;
    bool m_IsSlowMo = false;
    float m_SlowMoPercentage = .1f;

    float m_CurrentScreenShakePercentage = 0.f;
    float m_CurrentControllerLeftVibration = 0.f;
    float m_CurrentControllerRightVibration = 0.f;

    void DebugRender() const;

    void HandleUserInput();

    void AttractionMode( float deltaSeconds );
    void PostAttractionExplosion();

    void AttractModeDefaults();
    void RenderAttractMode() const;

    float UpdateDeltaSecondsBasedOnState( float deltaSeconds );

    void SpawnWave();
    bool CheckWaveComplete();

    void ScreenShakeAblation( float deltaSeconds );
    void ControllerVibrationAblation( float deltaSeconds );

    void UpdateEntities( float deltaSeconds,
                         Entity** const
                         entities,
                         int entitiesSize );
    void RenderEntities( const Entity* const* entities,
                         int entitiesSize ) const;
    void DebugRenderEntities( const Entity* const* entities,
                              int entitiesSize ) const;

    void RequestShipRespawn();
    void RenderLives() const;

    void CreateAstroidInArray( int x );

    void PhysicsCollisions();
    void DeleteGarbageEntities();
    void DeleteAllEntities();

    void ErrorRecoverable( const char* message );
};