#pragma once

#include "Game/Entity/Entity.hpp"

extern bool g_BulletWrapAround;

class Bullet: public Entity
{
public:
    Bullet( Game* game, const Vec3& startingPosition );
    virtual ~Bullet();

    virtual void Update( float deltaSeconds ) override;
    virtual void Render() const override;
    virtual void Die() override;

private:
    void WrapAround();
};
