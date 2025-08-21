#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>
#include <cmath>

// planet class

const int screenWidth = 1920;
const int screenHeight = 1080;

const float G = 0.7;
const int starAmount = 5000;

class Planet
{
public:
    Vector2 m_pos{};
    Vector2 m_vel{};
    float m_mass{};

    float m_size{};
    Color m_color{};

    Planet() = default;
    Planet(Vector2 pos, float mass, float size, Color color) : m_pos(pos), m_mass(mass), m_size(size), m_color(color) {}
    Planet(Vector2 pos, Vector2 vel, float mass, float size, Color color) : m_pos(pos), m_vel(vel), m_mass(mass), m_size(size), m_color(color) {}

    ~Planet() = default;

    void draw()
    {
        DrawCircleV(m_pos, m_size, m_color);
    }

    void update(const float dt)
    {
        m_pos += m_vel * dt;
    }
};

// create star background

std::vector<Vector2> initStars(int starAmount)
{
    std::vector<Vector2> stars;

    for (int i = 0; i < starAmount; ++i)
    {
        float x = GetRandomValue(0, screenWidth - 1);
        float y = GetRandomValue(0, screenHeight - 1);
        Vector2 star = {(float)x, (float)y};
        stars.push_back(star); 
    }
    return stars;
}

void renderStars(std::vector<Vector2> stars, std::vector<Planet> *planets)
{     
    for (auto star : stars)
    {
        Vector2 gravitationOffset{0, 0};
        for (auto planet : *planets)
        {
            Vector2 dir = {planet.m_pos.x- star.x, planet.m_pos.y - star.y};
            float dist = sqrtf(dir.x * dir.x + dir.y * dir.y + 300.f);
            dir = Vector2Normalize(dir);

            float Fg = 0.01 * G * planet.m_mass / (dist * dist);

            Vector2 delta = {dir.x * Fg, dir.y * Fg};

            float d2 = delta.x*delta.x + delta.y*delta.y;
            if (d2 > (6 * 6))
            {
                float d = sqrtf(d2);
                delta.x *= (6 / d);
                delta.y *= (6 / d);
            }
            gravitationOffset.x += delta.x;
            gravitationOffset.y += delta.y;
        }
        const float l2 = gravitationOffset.x*gravitationOffset.x + gravitationOffset.y*gravitationOffset.y;
        if (l2 > (15 * 15)) 
        {
            const float l = sqrtf(l2);
            gravitationOffset.x *= 15 / l;
            gravitationOffset.y *= 15 / l;
        }
        DrawPixel(star.x + gravitationOffset.x, star.y + gravitationOffset.y, WHITE);
    }
}

// create and update planets

Planet createPlanet()
{
    Vector2 planetPos = GetMousePosition();
    float planetMass = GetRandomValue(80000, 400000);
    float planetSize = roundf(planetMass/80000);
    Color planetColor = {(unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255};

    return Planet(planetPos, planetMass, planetSize, planetColor);
}

Planet createSun()
{
    Vector2 planetPos = GetMousePosition();
    float planetMass = 15000000;
    float planetSize = 10;
    Color planetColor = {255, 255, 0, 255};

    return Planet(planetPos, planetMass, planetSize, planetColor);
}

void updatePlanets(std::vector<Planet> *planets, const float dt)
{
    for (size_t i = 0; i < planets->size(); ++i)
    {
        for (size_t j = 0; j < planets->size(); ++j)
        {
            if (i == j) continue;

            Planet& planet = (*planets)[i];
            Planet& other = (*planets)[j];

            Vector2 dir = {other.m_pos.x - planet.m_pos.x, other.m_pos.y - planet.m_pos.y};
            double dist = sqrtf(dir.x * dir.x + dir.y * dir.y + 300.f);

            dir = Vector2Normalize(dir);

            double Fg = G * (planet.m_mass * other.m_mass) / (dist * dist);

            planet.m_vel.x += (dir.x * (Fg / planet.m_mass)) * dt;
            planet.m_vel.y += (dir.y * (Fg / planet.m_mass)) * dt;
        }
    }

    for (auto& planet : *planets)
    {
        Vector2 mouseDelta = Vector2Subtract(planet.m_pos, GetMousePosition());

        if ((sqrtf(mouseDelta.x * mouseDelta.x + mouseDelta.y * mouseDelta.y)) < planet.m_size)
        {
            if (IsKeyDown(KEY_A)) planet.m_vel.x -= 100 * dt;
            if (IsKeyDown(KEY_D)) planet.m_vel.x += 100 * dt;
            if (IsKeyDown(KEY_W)) planet.m_vel.y -= 100 * dt;
            if (IsKeyDown(KEY_S)) planet.m_vel.y += 100 * dt;
        }
        planet.update(dt);
    }
}

// game init and loop

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "PlanetSim2D");
    SetTargetFPS(240);
    std::vector<Vector2> stars = initStars(starAmount);    
    std::vector<Planet> planets;

    while (!WindowShouldClose())
    {   
        // handle Input

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) planets.push_back(createPlanet());
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) planets.push_back(createSun());
        // update

        updatePlanets(&planets, GetFrameTime());

        // render

        BeginDrawing();
        ClearBackground(BLACK);

        renderStars(stars, &planets);
        for (auto planet : planets) planet.draw();

        EndDrawing();
    }
    CloseWindow();
}
