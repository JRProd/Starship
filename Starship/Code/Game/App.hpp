#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Event/EventSystem.hpp"

class Window;
extern Window* g_Window;

class Game;
class Camera;

class App
{
public:
    App();
    ~App();
    void Startup();
    void Shutdown();
    void RunFrame();

    bool IsQuitting() const { return m_isQuitting; }
    bool HandleKeyPressed( unsigned char keyCode );
    bool HandleKeyReleased( unsigned char keyCode );
    bool HandleQuitRequested( EventArgs* );

private:
    bool m_isQuitting = false;
    bool m_isPaused = false;
    bool m_isSlowMo = false;
    float m_SlowMoPercentage = .1f;
    Vec2 m_shipPos;

    Game* m_GameInstance = nullptr;
    Camera* m_Camera = nullptr;

    float m_shipSpeedSeconds = 10.f;
    float m_shipWidth = 6.f;
    float m_shipHeight = 2.5f;

    void BeginFrame();
    void Update( float deltaSeconds );
    void Render() const;
    void EndFrame();

    void HandleUserInput();

    void RestartGame();
};