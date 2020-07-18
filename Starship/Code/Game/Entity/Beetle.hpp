#pragma once

#include "Game/Entity/Entity.hpp"

class PlayerShip;

class Beetle: public Entity
{
public:
    Beetle( Game* game, const Vec3& startingPosition );
    ~Beetle();

    virtual void Create() override;
    virtual void Update( float deltaSeconds ) override;
    virtual void Render() const override;
    virtual void Die() override;
    virtual void Destroy() override;

private:
    const PlayerShip* m_TargetPlayer = nullptr;

    bool UpdateTarget();
    void MoveTowardsTarget();
};