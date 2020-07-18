#pragma once

#include "Game/Entity/Entity.hpp"

extern bool g_AstroidsWrapScreen;

class Asteroid: public Entity
{
public:
    Asteroid( Game* game, Vec3 startingPosition );
    ~Asteroid();

    virtual void Create() override;
    virtual void Update( float deltaSeconds ) override;
    virtual void Render() const override;
    virtual void Die() override;
    virtual void Destroy() override;

private:
    Vec2* m_TriangleCorners = nullptr;

    void WrapAstroid();
};