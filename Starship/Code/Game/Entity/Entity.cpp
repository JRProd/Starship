#include "Entity.hpp"

#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

//-------------------------------------------------------------------------------
Entity::Entity( Game* game, const Vec3& startingPositon )
    : m_Game( game )
    , m_Position( startingPositon )
{
    m_LastHitTime = -HIT_TIME;
    m_LenghtHitTime = HIT_TIME;
}

//-------------------------------------------------------------------------------
Entity::~Entity()
{
    // DO NOTHING
}

void Entity::Create()
{
}

//-------------------------------------------------------------------------------
void Entity::Update( float deltaSeconds )
{
    m_Age += deltaSeconds;

    m_Position += m_Velocity * deltaSeconds;
    m_Velocity += m_Acceleration * deltaSeconds;

    m_AngleDegrees += m_AngularVelocity * deltaSeconds;
    m_AngularVelocity += m_AngularAcceleration * deltaSeconds;
}

//-------------------------------------------------------------------------------
void Entity::DebugRender() const
{
    // Draw debug velocity
    DrawDebugLine( Vec2( m_Position.x, m_Position.y ),
                   Vec2( m_Position.x, m_Position.y ) +
                   Vec2( m_Velocity.x, m_Velocity.y ),
                   Rgba8( 255, 255, 0 ), .2f );
    // Draw Physics circle
    DrawDebugCircle( Vec2( m_Position.x, m_Position.y ),
                     m_PhysicsRadius,
                     DEBUG_PHYSICS_CIRCLE,
                     .1f );
    // Draw Cosmetic circle
    DrawDebugCircle( Vec2( m_Position.x, m_Position.y ),
                     m_CosmeticRadius,
                     DEBUG_COSMETIC_CIRCLE,
                     .1f );
}

void Entity::Destroy()
{
}

//-------------------------------------------------------------------------------
const Vec3 Entity::GetPosition() const
{
    return m_Position;
}

//-------------------------------------------------------------------------------
const Vec3 Entity::GetVelocity() const
{
    return m_Velocity;
}

//-------------------------------------------------------------------------------
const Vec3 Entity::GetAcceleration() const
{
    return m_Acceleration;
}

//-------------------------------------------------------------------------------
const Vec3 Entity::GetForwardVector() const
{
    return Vec3::MakeFromPolarDegreesXY( m_AngleDegrees );
}

float Entity::GetUniformScale() const
{
    return m_UniformScale;
}

//-------------------------------------------------------------------------------
float Entity::GetAngleDegrees() const
{
    return m_AngleDegrees;
}

//-------------------------------------------------------------------------------
float Entity::GetAngularVelocity() const
{
    return m_AngularVelocity;
}

//-------------------------------------------------------------------------------
float Entity::GetAngularAcceleration() const
{
    return m_AngularAcceleration;
}

//-------------------------------------------------------------------------------
int Entity::GetHealth() const
{
    return m_Health;
}

//-------------------------------------------------------------------------------
bool Entity::IsDead() const
{
    return m_IsDead;
}

//-------------------------------------------------------------------------------
bool Entity::IsOffscreen() const
{
    if ( m_Position.x < -MAX_SCREEN_SHAKE - m_CosmeticRadius )
    {
        return true;
    }
    if ( m_Position.x > WORLD_SIZE_X + MAX_SCREEN_SHAKE + m_CosmeticRadius )
    {
        return true;
    }
    if ( m_Position.y < -MAX_SCREEN_SHAKE - m_CosmeticRadius )
    {
        return true;
    }
    if ( m_Position.y > WORLD_SIZE_Y + MAX_SCREEN_SHAKE + m_CosmeticRadius )
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------
bool Entity::IsGarbage() const
{
    return m_IsGarbage;
}

//-------------------------------------------------------------------------------
void Entity::SetPosition( const Vec3& newPosition )
{
    m_Position = newPosition;
}

//-------------------------------------------------------------------------------
void Entity::AddPosition( const Vec3& deltaPosition )
{
    m_Position += deltaPosition;
}

//-------------------------------------------------------------------------------
void Entity::SetVelocity( const Vec3& newVelocity )
{
    m_Velocity = newVelocity;
}

//-------------------------------------------------------------------------------
void Entity::AddVelocity( const Vec3& deltaVelocity )
{
    m_Velocity += deltaVelocity;
}

//-------------------------------------------------------------------------------
void Entity::SetAcceleration( const Vec3& newAcceleration )
{
    m_Acceleration = newAcceleration;
}

//-------------------------------------------------------------------------------
void Entity::AddAcceleration( const Vec3& deltaAcceleration )
{
    m_Acceleration += deltaAcceleration;
}

void Entity::SetUniformScale( float newScale )
{
    m_UniformScale = newScale;
}

void Entity::AddUniformScale( float deltaScale )
{
    m_UniformScale += deltaScale;
}

//-------------------------------------------------------------------------------
void Entity::SetAngleDegrees( float newRotationDegrees )
{
    m_AngleDegrees = newRotationDegrees;
}

//-------------------------------------------------------------------------------
void Entity::AddAngleDegrees( float deltaDegrees )
{
    m_AngleDegrees += deltaDegrees;
}

//-------------------------------------------------------------------------------
void Entity::SetAngularVelocity( float newAngularVelocity )
{
    m_AngularVelocity = newAngularVelocity;
}

//-------------------------------------------------------------------------------
void Entity::AddAngularVelocity( float deltaAngularVelocity )
{
    m_AngularVelocity += deltaAngularVelocity;
}

//-------------------------------------------------------------------------------
void Entity::SetAngularAcceleration( float newAngularAcceleration )
{
    m_AngularAcceleration = newAngularAcceleration;
}

//-------------------------------------------------------------------------------
void Entity::AddAngularAcceleration( float deltaAngularAcceleration )
{
    m_AngularAcceleration += deltaAngularAcceleration;
}

//-------------------------------------------------------------------------------
bool Entity::OverlapsEntity( const Entity& otherEntity )
{
    Disc playerDisc = Disc( static_cast<Vec2>(m_Position), m_PhysicsRadius );
    return DoDiscsOverlap( playerDisc, Disc( static_cast<Vec2>(otherEntity.m_Position),
                                             otherEntity.m_PhysicsRadius ) );
}

//-------------------------------------------------------------------------------
void Entity::DamageEntity( int damage )
{
    m_Health -= damage;

    m_LastHitTime = m_Age;

    if ( m_Health <= 0 )
    {
        m_IsDead = true;
        Die();
    }
}

bool Entity::WasJustHit()
{
    return m_LastHitTime + m_LenghtHitTime > m_Age;
}

//-------------------------------------------------------------------------------
void Entity::SetDead( bool newDead )
{
    m_IsDead = newDead;
}
