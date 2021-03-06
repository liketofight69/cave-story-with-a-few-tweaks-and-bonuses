#include "player.h"

#include "damage_texts.h"
#include "first_cave_bat.h"
#include "game.h"
#include "input.h"
#include "map.h"
#include "graphics.h"
#include "particle_tools.h"
#include "projectile.h"
#include "timer.h"
#include "secondenemy.h"
#include <SFML/Audio.hpp>
#include "SDL_ttf.h"
const units::FPS kFps{60};
const auto kMaxFrameTime = std::chrono::milliseconds{5 * 1000 / 60};
units::Tile Game::kScreenWidth{20};
units::Tile Game::kScreenHeight{15};

//struct Graphics *graph;

Game::Game() :
    sdlEngine_(),
    graphics_(),
    player_{std::make_shared<Player>(graphics_,
            Vector<units::Game>{
            units::tileToGame(Game::kScreenWidth/2),
            units::tileToGame(Game::kScreenHeight/2)}
            )
    },
    player1_{std::make_shared<Player>(graphics_,
            Vector<units::Game>{
            units::tileToGame(12),//Game::kScreenWidth/4
            units::tileToGame(Game::kScreenHeight/2)}
            )
    },





    bat_{std::make_shared<FirstCaveBat>(graphics_,
            Vector<units::Game>{
            units::tileToGame(7),
            units::tileToGame(Game::kScreenHeight/2 + 1)}
            )
    },
    sec_{std::make_shared<Secondenemy>(graphics_,
            Vector<units::Game>{
            units::tileToGame(17),//x axis
            units::tileToGame(Game::kScreenHeight/2 + 3)}//Game::kScreenHeight/2 + 1 //y axis of object
            )
    },
    sec1_{std::make_shared<Secondenemy>(graphics_,
            Vector<units::Game>{
            units::tileToGame(15),//x axis
            units::tileToGame(Game::kScreenHeight/2 + 3)}//Game::kScreenHeight/2 + 1 //y axis of object
            )
    },
     bat1_{std::make_shared<FirstCaveBat>(graphics_,
            Vector<units::Game>{
            units::tileToGame(14),
            units::tileToGame(Game::kScreenHeight/2 + 1)}
            )
    },





    //test
    map_{Map::createTestMap(graphics_)},
    particle_system_{},
    damage_texts_()
{
    runEventLoop();
}

Game::~Game()
{
}




void Game::runEventLoop() {
    Input input;
    SDL_Event event;

    damage_texts_.addDamageable(player_);
    damage_texts_.addDamageable(bat_);
    damage_texts_.addDamageable(bat1_);
    damage_texts_.addDamageable(sec_);

    bool running{true};
    auto last_updated_time = std::chrono::high_resolution_clock::now();
    while (running) {
        using std::chrono::high_resolution_clock;
        using std::chrono::milliseconds;
        using std::chrono::duration_cast;

        const auto start_time = high_resolution_clock::now();
        //test
        //SDL_RenderCopy(graph->sdlRenderer, Message, NULL, &Message_rect);

        input.beginNewFrame();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                input.keyDownEvent(event);
                break;
            case SDL_KEYUP:
                input.keyUpEvent(event);
                break;
            case SDL_QUIT:
                running = false;
                break;
            default:
                break;
            }
        }
        if (input.wasKeyPressed(SDL_SCANCODE_Q)) {
            running = false;
        }

        // Player Horizontal Movement
        if (input.isKeyHeld(SDL_SCANCODE_LEFT)
                && input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
            // if both left and right are being pressed we need to stop moving
            player_->stopMoving();
        } else if (input.isKeyHeld(SDL_SCANCODE_LEFT)) {
            player_->startMovingLeft();
        } else if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
            player_->startMovingRight();
        } else {
            player_->stopMoving();
        }

        if (input.isKeyHeld(SDL_SCANCODE_UP) &&
                input.isKeyHeld(SDL_SCANCODE_DOWN)) {
            player_->lookHorizontal();
        } else if (input.isKeyHeld(SDL_SCANCODE_UP)) {
            player_->lookUp();
        } else if (input.isKeyHeld(SDL_SCANCODE_DOWN)) {
            player_->lookDown();
        } else {
            player_->lookHorizontal();
        }

        // Player Jump
        if (input.wasKeyPressed(SDL_SCANCODE_Z)) {
            player_->startJump();
        } else if (input.wasKeyReleased(SDL_SCANCODE_Z)) {
            player_->stopJump();
        }
        // Player Fire
        if (input.wasKeyPressed(SDL_SCANCODE_X)) {


            player_->startFire();




        } else if (input.wasKeyReleased(SDL_SCANCODE_X)) {
            player_->stopFire();
        }
        player1_->startFire();
        // update scene and last_updated_time
        const auto current_time = high_resolution_clock::now();
        const auto upd_elapsed_time = current_time - last_updated_time;

        update(std::min(
                    duration_cast<milliseconds>(upd_elapsed_time),
                    kMaxFrameTime),
               graphics_
              );
        last_updated_time = current_time;

        draw(graphics_);

        // calculate delay for constant fps
        const auto end_time = high_resolution_clock::now();
        const auto elapsed_time = duration_cast<milliseconds>(
                end_time - start_time);

        const auto delay_duration = milliseconds(1000) / kFps - elapsed_time;
        if (delay_duration.count() >= 0)
            SDL_Delay(delay_duration.count());
    }
}

void Game::update(const std::chrono::milliseconds elapsed_time, Graphics& graphics)
{
    Timer::updateAll(elapsed_time);
    damage_texts_.update(elapsed_time);
    particle_system_.update(elapsed_time);

    auto particle_tools = ParticleTools{ particle_system_, graphics };
    //TODO: update map when it is changed
    player_->update(elapsed_time, *map_, particle_tools);
    auto player_pos = player_->getCenterPos();
    if (bat_) {
        if (!bat_->update(elapsed_time, player_pos.x)) {
            bat_.reset();
        }
    }

    auto projectiles = player_->getProjectiles();
    for (auto projectile: projectiles) {
        auto projectile_rect = projectile->getCollisionRectangle();
        if (bat_ && bat_->getCollisionRectangle().collidesWith(projectile_rect)) {
            projectile->collideWithEnemy();
            bat_->takeDamage(projectile->getContactDamage());
        }
    }

    if (bat_) {
        const auto batRect = bat_->getDamageRectangle();
        if (batRect.collidesWith(player_->getDamageRectangle())) {
            player_->takeDamage(bat_->contactDamage());
        }
    }
    if (sec_) {//if player collides with spike take away one health box
        const auto secRect = sec_->getDamageRectangle();
        if (secRect.collidesWith(player_->getDamageRectangle())) {
            player_->takeDamage(sec_->contactDamage());
        }
    }
    if (sec1_) {//if player collides with spike take away one health box
        const auto secRect = sec_->getDamageRectangle();
        if (secRect.collidesWith(player_->getDamageRectangle())) {
            player_->takeDamage(sec_->contactDamage());
        }
    }//animate second bat
    if (bat1_) {
        if (!bat1_->update(elapsed_time, player_pos.x)) {
            bat1_.reset();
        }
    }



    //bat1_->startFire();
    if (bat1_) {
        const auto batRect = bat1_->getDamageRectangle();
        if (batRect.collidesWith(player_->getDamageRectangle())) {
            player_->takeDamage(bat1_->contactDamage());
        }
    }//if second bat is colliding with players action
    auto projectiles1 = player_->getProjectiles();
    for (auto projectile: projectiles1) {
        auto projectile_rect = projectile->getCollisionRectangle();
        if (bat1_ && bat1_->getCollisionRectangle().collidesWith(projectile_rect)) {
            projectile->collideWithEnemy();
            bat1_->takeDamage(projectile->getContactDamage());
        }
    }
     auto projectiles2 = player1_->getProjectiles();
    for (auto projectile: projectiles2) {
        auto projectile_rect = projectile->getCollisionRectangle();
        if (player_ && player_->getCollisionRectangle().collidesWith(projectile_rect)) {
            projectile->collideWithEnemy();
            player_->takeDamage(projectile->getContactDamage());
        }
    }






//test
}

void Game::draw(Graphics& graphics) const
{
    graphics.clear();

    map_->drawBackground(graphics);
    if (bat_)
        bat_->draw(graphics);
     if (bat1_)
        bat1_->draw(graphics);

    if (sec_)
       sec_->draw(graphics);//our second enemy
    //our second spike
     if (sec1_)
       sec1_->draw(graphics);
    player_->draw(graphics);
    player1_->draw(graphics);
    map_->draw(graphics);
    particle_system_.draw(graphics);

    damage_texts_.draw(graphics);
    player_->drawHUD(graphics);

    graphics.flip();





}
