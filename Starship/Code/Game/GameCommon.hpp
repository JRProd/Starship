#pragma once

class App;
class InputSystem;
class RenderContext;
class RandomNumberGenerator;

struct Vec2;
struct Rgba8;
//-------------------------------------------------------------------------------
// Global advertisement of the App  & RenderContext singletons
extern App* g_App;
extern InputSystem* g_InputSystem;
extern RenderContext* g_Renderer;

//-------------------------------------------------------------------------------
// Constants for the Game
constexpr float CLIENT_ASPECT = 2.f;                    // We are requesting a 1:1 aspect (square) window area
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float SAFEZONE = 10.f;                        // Asteroids can not spawn this close to the world edge
constexpr float MAX_SCREEN_SHAKE = 10.f;
constexpr float SCREEN_SHAKE_ABLATION_PER_SECOND = 1.f;
constexpr float CONTROLLER_VIBRATION_ABLATION_PER_SECOND = .5f;



//-------------------------------------------------------------------------------
// Game Rules
constexpr int MAX_ASTEROIDS = 120;
constexpr float CLOSEST_ASTEROID_SPAWN_TO_SHIP = 15.f;
constexpr int MAX_BULLETS = 20000;
constexpr int MAX_DEBRIS = 500;
constexpr int MAX_BEETLES = 25;
constexpr int MAX_WASPS = 10;
constexpr int MAX_NUMBER_OF_WAVES = 5;
constexpr int MAX_NUMBER_OF_LIVES = 4;
constexpr double TIME_AFTER_DEATH_BEFORE_ATTRACT = 3.f;

//-----------------------------------------------------------------------------
// Title Rules
constexpr float STARTING_TITLE_SCALE = 4.f;
constexpr float STARTING_TITLE_ROTAITON = 0.f;
constexpr float MAX_TITLE_ROTATION = 15.f;
constexpr float MAX_TITLE_SCALE_INCREASE = 2.f;
constexpr float TITLE_COLER_CHANGE_TIME = .25f;

//-----------------------------------------------------------------------------
// Entity Rules
constexpr float HIT_TIME = .2f;

//-------------------------------------------------------------------------------
// Asteroid Rules
constexpr float ASTEROID_SPEED = 15.f;
constexpr float ASTEROID_MAX_ROTATION = 200.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 2.0f;
constexpr float ASTEROID_COSMETIC_RADIUS = 3.0f;

//-------------------------------------------------------------------------------
// Bullet Rules
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 130.f;
constexpr float BULLET_PHYSICS_RADIUS = .5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;

//-------------------------------------------------------------------------------
// Ship Rules
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 2.f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.5f;
constexpr int PLAYER_BULLETS_PER_SHOT = 3;

//-------------------------------------------------------------------------------
// Beetle Rules
constexpr float BEETLE_VELOCITY = 20.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.6f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr int BEETLE_HEALTH = 3;

//-------------------------------------------------------------------------------
// Beetle Rules
constexpr float WASP_MAX_VELOCITY = 30.f;
constexpr float WASP_ACCELERATION_PER_SECOND = 65.f;
constexpr float WASP_PHYSICS_RADIUS = 1.8f;
constexpr float WASP_COSMETIC_RADIUS = 2.3f;
constexpr int WASP_HEALTH = 2;

//-------------------------------------------------------------------------------
// Debris Rules
constexpr float MAX_DEBRIS_LIFESPAN = 2.f;
constexpr float DEBRIS_COSMETIC_RADIUS = 1.f;
constexpr float DEBRIS_MIN_SPEED = 2.f;
constexpr float DEBRIS_MAX_SPEED = 55.f;


//-------------------------------------------------------------------------------
extern Rgba8 PLAYER_SHIP_COLOR_1;
extern Rgba8 PLAYER_SHIP_COLOR_2;
extern Rgba8 PLAYER_SHIP_EXHAUST_1;
extern Rgba8 PLAYER_SHIP_EXHAUST_2;
extern Rgba8 ASTEROID_COLOR;
extern Rgba8 BULLET_HEAD_COLOR;
extern Rgba8 BULLET_TAIL_COLOR_START;
extern Rgba8 BULLET_TAIL_COLOR_END;
extern Rgba8 BEETLE_COLOR;
extern Rgba8 WASP_COLOR;

extern Rgba8 DEBUG_FORWARD_VECTOR_COLOR;
extern Rgba8 DEBUG_POSITOIN_VECTOR_COLOR;
extern Rgba8 DEBUG_COSMETIC_CIRCLE;
extern Rgba8 DEBUG_PHYSICS_CIRCLE;

//-------------------------------------------------------------------------------
// Debug drawing utility functions
constexpr int DEBUG_CIRCLE_RADIUSES = 64;
constexpr int DEBUG_CIRCLE_VERTEXES = DEBUG_CIRCLE_RADIUSES * 6;
void DrawDebugLine( const Vec2& start, 
                    const Vec2& end, 
                    const Rgba8& color, 
                    float thickness );
void DrawDebugCircle( const Vec2& center, 
                      float radius, 
                      const Rgba8& color, 
                      float thickness );

//-------------------------------------------------------------------------------
// Gameplay Utility Functions
Vec2 PointJustOffScreen(float furthestBound);
