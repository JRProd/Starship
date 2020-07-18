#include "Bullet.hpp"

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Renderer/RenderContext.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

bool g_BulletWrapAround = false;

//-------------------------------------------------------------------------------
Bullet::Bullet(Game* game, const Vec3& startingPosition)
    : Entity::Entity( game, startingPosition )
{
    m_UniformScale = 1.25f;

    m_CosmeticRadius = BULLET_COSMETIC_RADIUS * m_UniformScale;
    m_PhysicsRadius = BULLET_PHYSICS_RADIUS * m_UniformScale;

}

//-------------------------------------------------------------------------------
Bullet::~Bullet()
{
}

//-------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
    if ( m_IsDead )
    {
        m_IsGarbage = true;
        return;
    }

    if ( IsOffscreen() )
    {
        if( g_BulletWrapAround )
        {
            WrapAround();
        }
        else
        {
            m_IsGarbage = true;
            return;
        }
    }

    Entity::Update( deltaSeconds );
}

//-------------------------------------------------------------------------------
void Bullet::Render() const
{
    std::vector<VertexMaster> visual;
    visual.emplace_back( Vec2( 0.f, -.5f ), BULLET_HEAD_COLOR );
    visual.emplace_back( Vec2( .5f, 0.f ), BULLET_HEAD_COLOR );
    visual.emplace_back( Vec2( 0.f, .5f ), BULLET_HEAD_COLOR );

    visual.emplace_back( Vec2( -2.f, 0.f ), BULLET_TAIL_COLOR_END );
    visual.emplace_back( Vec2( 0.f, -.5f ), BULLET_TAIL_COLOR_START );
    visual.emplace_back( Vec2( 0.f, .5f ), BULLET_TAIL_COLOR_START );

    TransformVertexArray( visual,
                          static_cast<Vec2>(m_Position),
                          m_AngleDegrees,
                          m_UniformScale );

    g_Renderer->DrawVertexArray( visual );
}

//-------------------------------------------------------------------------------
void Bullet::Die()
{
    m_Game->CreateDebrisClusterAt( m_Position, BULLET_HEAD_COLOR, .5f, 3 );
}

void Bullet::WrapAround()
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
