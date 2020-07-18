#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Entity/Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexTypes/VertexMaster.hpp"

struct Vertex_PCU;

class Debris: public Entity
{
public:
    Debris(Game* game, const Vec3& startingPosition);
    Debris( Game* game, const Vec3& startingPosition, const Rgba8& color );
    Debris( Game* game, const Vec3& startingPosition, 
            const Rgba8& color, 
            float lifeSpan);
    ~Debris();

    virtual void Create() override;
    virtual void Update( float deltaSeconds ) override;
    virtual void Render() const override;
    virtual void Die() override;
    virtual void Destroy() override;

private:
    Vec2* m_TriangleCorners = nullptr;
    float m_DebrisTime = MAX_DEBRIS_LIFESPAN;

    Rgba8 m_DebrisColor = Rgba8::MAGENTA;

    std::vector<VertexMaster> m_LocalVisual;
    void GenerateVertexPCU();
};