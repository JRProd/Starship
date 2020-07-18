#pragma once

#include "Game/Entity/Entity.hpp"

class PlayerShip: public Entity
{
public:
    PlayerShip( Game* game, const Vec3& startingPosition );
    virtual ~PlayerShip() override;

    virtual void Update( float deltaSeconds ) override;
    virtual void Render() const override;
    virtual void Die() override;

    bool IsThrusting() const;
    void SetThrusting( bool newThrusting );
    void TurnLeft( float deltaSeconds );
    void TurnRight( float deltaSeconds );

    void ShootBullet();

    void RespawnShip();

private:
    Vec2 m_RandomThurstOffset = Vec2( 0.f, 0.f );
    bool m_Thrusting = false;

    Vec3 GetNoseSpawn();

    void ProcessInput();
    void BounceOffSides();
};