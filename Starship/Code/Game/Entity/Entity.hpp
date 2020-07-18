#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

#include "Game/GameCommon.hpp"

class Game;

class Entity
{
public:
    Entity( Game* game, const Vec3& startingPositon );
    virtual ~Entity();

    virtual void Create();
    virtual void Update( float deltaSeconds );
    virtual void Render() const = 0;
    virtual void DebugRender() const;
    virtual void Die() = 0;
    virtual void Destroy();

    const Vec3 GetPosition() const;
    const Vec3 GetVelocity() const;
    const Vec3 GetAcceleration() const;
    const Vec3 GetForwardVector() const;

    float GetUniformScale() const;

    float GetAngleDegrees() const;
    float GetAngularVelocity() const;
    float GetAngularAcceleration() const;

    int GetHealth() const;

    bool IsDead() const;
    bool IsOffscreen() const;
    bool IsGarbage() const;

    void SetPosition( const Vec3& newPosition );
    void AddPosition( const Vec3& deltaPosition );
    void SetVelocity( const Vec3& newVelocity );
    void AddVelocity( const Vec3& deltaVelocity );
    void SetAcceleration( const Vec3& newAcceleration );
    void AddAcceleration( const Vec3& deltaAcceleration );

    void SetUniformScale( float newScale );
    void AddUniformScale( float deltaScale );

    void SetAngleDegrees( float newAngleDegrees );
    void AddAngleDegrees( float deltaDegrees );
    void SetAngularVelocity( float newAngularVelocity );
    void AddAngularVelocity( float deltaAngularVelocity );
    void SetAngularAcceleration( float newAngularAcceleration );
    void AddAngularAcceleration( float deltaAngularAcceleration );

    bool OverlapsEntity( const Entity& otherEntity );
    void DamageEntity( int damage );
    bool WasJustHit();
    void SetDead( bool newDead );

protected:
    Vec3 m_Position = Vec3::ZERO;           // Position of the Entity units
    Vec3 m_Velocity = Vec3::ZERO;           // Velocity of the Entity u/s
    Vec3 m_Acceleration = Vec3::ZERO;       // Acceleration of the Entity u/s/s

    Rgba8 m_Color = Rgba8::MAGENTA;         // The main color of the entity

    float m_UniformScale = 1.f;             // Uniform scale

    float m_AngleDegrees = 0.f;             // Angular Orientation of the Entity ( 0 is East )
    float m_AngularVelocity = 0.f;          // Angular Velocity of the Entity deg/s
    float m_AngularAcceleration = 0.f;      // Angular Acceleration of the Entity deg/s/s
    float m_PhysicsRadius = 10.f;           // Collision Radius
    float m_CosmeticRadius = 20.f;          // Cosmetic Radius ( no geometry outside this radius)

    float m_Age = 0.f;                      // Age of the entity from spawn time
    float m_LenghtHitTime = HIT_TIME;       // How many seconds the entity should be hit;
    float m_LastHitTime = -HIT_TIME;        // Last time the entity was hit
    int m_Health = 1;                       // Health of the Entity

    bool m_IsDead = false;                  // Is the Entity Dead
    bool m_IsGarbage = false;               // Will the Entity be Garbage Collected next Update

    Game* m_Game = nullptr;                 // Reference to the Game where Entity Lives
};