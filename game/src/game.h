#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <memory>
#include "damage_texts.h"
#include "graphics.h"
#include "particle_system.h"
#include "sdlengine.h"
#include "units.h"

struct Map;
struct Player;
struct FirstCaveBat;
struct Secondenemy;

struct Game {
    Game();
    ~Game();

    static units::Tile kScreenWidth;
    static units::Tile kScreenHeight;




private:
    void runEventLoop();
    void update(const std::chrono::milliseconds elapsed_time, Graphics& graphics);
    void draw(Graphics& graphics) const;

    const SDLEngine sdlEngine_;
    Graphics graphics_;
    std::shared_ptr<Player> player_;
    std::shared_ptr<Player> player1_;
    std::shared_ptr<FirstCaveBat> bat_;
    std::shared_ptr<FirstCaveBat> bat1_;
    std::shared_ptr<Secondenemy> sec_;
    std::shared_ptr<Secondenemy> sec1_;
    std::unique_ptr<Map> map_;
    ParticleSystem particle_system_;
    DamageTexts damage_texts_;
};

#endif /* GAME_H */
