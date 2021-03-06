#include <cassert>
#include <cmath>
#include <stdio.h>
#include "merc.h"
#include "animated_sprite.h"
#include "number_sprite.h"
#include "graphics.h"
#include "game.h"
#include "head_bump_particle.h"
#include "map.h"
#include "particle_tools.h"
#include "rectangle.h"

#define SDL_MESSAGEBOX_DEBUG 16


// Walk Motion
const units::Acceleration kWalkingAcceleration{0.00083007812};
const units::Acceleration kFriction{0.00049804687};
const units::Velocity kMaxSpeedX{0.15859375};
// Fall Motion
const units::Acceleration kGravity{0.00078125};
const units::Velocity kMaxSpeedY{0.2998046875};
// Jump Motion
const units::Acceleration kAirAcceleration{0.0003125};
const units::Acceleration kJumpGravity{0.0003125};
const units::Velocity kJumpSpeed{0.25};
const units::Velocity kShortJumpSpeed{kJumpSpeed / 1.5};
// Sprites
const std::string kPlayerSpriteFilePath{"MyChar"};
// Sprite Frames
const units::Frame kCharacterFrame{0};

const units::Frame kWalkFrame{0};
const units::Frame kStandFrame{0};
const units::Frame kJumpFrame{1};
const units::Frame kFallFrame{2};

const units::Frame kUpFrameOffset{3};
const units::Frame kLeftFrameOffset{1};
const units::Frame kRightFrameOffset{2};
const units::Frame kDownFrame{6};
const units::Frame kBackFrame{7};

//Collision rectangles
const Rectangle kCollisionX{ 6, 10, 20, 12 };

const units::Game kCollisionYTop{2};
const units::Game kCollisionYHeight{30};
const units::Game kCollisionYBottom{kCollisionYTop+ kCollisionYHeight};

const units::Game kCollisionTopWidth{18};
const units::Game kCollisionBottomWidth{10};
const units::Game kCollisionTopLeft{
    (units::tileToGame(1) - kCollisionTopWidth) / 2
};
const units::Game kCollisionBottomLeft{
    (units::tileToGame(1) - kCollisionBottomWidth) / 2
};

const std::chrono::milliseconds kInvincibleFlashTime{50};
const std::chrono::milliseconds kInvincibleTime{3000};

namespace {





struct CollisionInfo {
    bool collided;
    units::Tile row;
    units::Tile col;
};
CollisionInfo getWallCollisionInfo(const Map& map, const Rectangle& rect) {
    CollisionInfo info{false, 0, 0};
    //container that encapsulates dynamic size arrays
    std::vector<Map::CollisionTile> tiles(map.getCollidingTiles(rect));
    for (auto &tile : tiles) {
        if (tile.tile_type == Map::TileType::WALL) {
            info = {true, tile.row, tile.col};
            break;
        }
    }
    return info;
}

} // anonymous namespace

Merc::Merc(Graphics& graphics, Vector<units::Game> pos) :
    pos_(std::move(pos)),
    velocity_{0.0, 0.0},
    acceleration_x_direction_{0},
    horizontal_facing_{HorizontalFacing::LEFT},
    intended_vertical_facing_{VerticalFacing::HORIZONTAL},
    is_on_ground_{false},
    is_jump_active_{false},
    is_interacting_{false},
    health_(graphics),
    invincible_timer_{kInvincibleTime},
    damage_text_(std::make_shared<DamageText>()),
    walking_animation_(),

    polar_star_(graphics),
     sprites_()
{
    initializeSprites(graphics);
}

Merc::~Merc() {}





/*void Player::render( int camX, int camY )
{
    //Show the dot relative to the camera
    //gDotTexture.render( mPosX - camX, mPosY - camY );
}*/


void Merc::update(const std::chrono::milliseconds elapsed_time,
                    const Map& map,
                    ParticleTools& particle_tools)
{
    sprites_[getSpriteState()]->update();

    health_.update();
    walking_animation_.update();

    polar_star_.updateProjectiles(elapsed_time, map);

    updateX(elapsed_time, map, particle_tools);
    updateY(elapsed_time, map, particle_tools);
}

void Merc::draw(Graphics& graphics) const
{

        polar_star_.draw(
                graphics,
                horizontal_facing_,
                vertical_facing(),
                is_gun_up(),
                pos_
                );
        sprites_.at(getSpriteState())->draw(graphics, pos_);

}

void Merc::drawHUD(Graphics& graphics) const
{
    health_.draw(graphics);

}

void Merc::startMovingLeft()
{

    horizontal_facing_ = HorizontalFacing::LEFT;
    acceleration_x_direction_ = -1;

}

void Merc::startMovingRight()
{
    horizontal_facing_ = HorizontalFacing::RIGHT;
    acceleration_x_direction_ = 5;//1

}

void Merc::stopMoving()
{
    acceleration_x_direction_ = 0;
}

void Merc::lookUp()
{
    intended_vertical_facing_ = VerticalFacing::UP;
    is_interacting_ = false;
}

void Merc::lookDown()
{
    if (intended_vertical_facing_ == VerticalFacing::DOWN) return;
    intended_vertical_facing_ = VerticalFacing::DOWN;
    is_interacting_ = is_on_ground();
}

void Merc::lookHorizontal()
{
    intended_vertical_facing_ = VerticalFacing::HORIZONTAL;
}

void Merc::startJump()
{
    is_interacting_ = false;
    is_jump_active_ = true;
    if (is_on_ground()) {
        velocity_.y = -kJumpSpeed;
    }
}

void Merc::stopJump()
{
    is_jump_active_ = false;
}

void Merc::startFire()
{
    polar_star_.startFire(
            pos_,
            horizontal_facing_,
            intended_vertical_facing_,
            is_gun_up()
            );
}

void Merc::stopFire()
{
    polar_star_.stopFire();
}

void Merc::takeDamage(units::HP damage) {
    if (invincible_timer_.is_active()) return;

    velocity_.y = std::min(velocity_.y, -kShortJumpSpeed);
    invincible_timer_.reset();
    health_.takeDamage(damage);
    damage_text_->setDamage(damage);
}

const Rectangle Merc::getDamageRectangle() const
{
    return Rectangle(pos_.x + kCollisionX.getLeft(),
            pos_.y + kCollisionYTop,
            kCollisionX.getWidth(),
            kCollisionYHeight);
}

const Vector<units::Game> Merc::getCenterPos() const
{
    return Vector<units::Game> {
        pos_.x + units::kHalfTile,
        pos_.y + units::kHalfTile
    };
}

const std::shared_ptr<DamageText> Merc::getDamageText() const
{
    return damage_text_;
}

std::vector<std::shared_ptr<GenericProjectile> > Merc::getProjectiles()
{
    return polar_star_.getProjectiles();
}

bool Merc::is_gun_up() const
{
    return (getMotionType() == MotionType::WALKING)
        && (walking_animation_.stride() != StrideType::MIDDLE);
}

units::Tile Merc::getFrameY(const SpriteState& s) const
{
    units::Tile tile_y = (s.horizontal_facing == HorizontalFacing::LEFT)
        ? 2 * kCharacterFrame
        : 1 + 2 * kCharacterFrame;
    return tile_y;
}

units::Tile Merc::getFrameX(const SpriteState& s) const
{
    units::Tile tile_x{0};
    switch (s.motion_type) {
    case MotionType::WALKING:
        tile_x = kWalkFrame;

        switch (s.stride_type) {
        case StrideType::MIDDLE:
            break;
        case StrideType::LEFT:
            tile_x += kLeftFrameOffset;
            break;
        case StrideType::RIGHT:
            //tile_x += kRightFrameOffset;
            break;
        default:
            break;
        }

        break;
    case MotionType::STANDING:
        tile_x = kStandFrame;
        break;
    case MotionType::INTERACTING:
        tile_x = kBackFrame;
        break;
    case MotionType::JUMPING:
        tile_x = kJumpFrame;
        break;
    case MotionType::FALLING:
        tile_x = kFallFrame;
        break;
    case MotionType::LAST:
        break;
    }
    switch (s.vertical_facing) {
    case VerticalFacing::HORIZONTAL:
        break;
    case VerticalFacing::UP:
        tile_x += kUpFrameOffset;
        break;
    case VerticalFacing::DOWN:
        tile_x = kDownFrame;
        break;
    default:
        break;
    }

    return tile_x;
}

void Merc::initializeSprite(Graphics& graphics,
        const SpriteState& sprite_state)
{
    units::Tile tile_y = getFrameY(sprite_state);
    units::Tile tile_x = getFrameX(sprite_state);

    sprites_[sprite_state] = std::make_unique<Sprite>(
                graphics,
                kPlayerSpriteFilePath,
                units::tileToPixel(tile_x), units::tileToPixel(tile_y),
                units::tileToPixel(1), units::tileToPixel(1)
    );
}

void Merc::initializeSprites(Graphics& graphics)
{
    for (auto mt = MotionType::FIRST; mt != MotionType::LAST; ++mt) {
        for (auto hf = HorizontalFacing::FIRST; hf != HorizontalFacing::LAST; ++hf) {
            for (auto vf = VerticalFacing::FIRST; vf != VerticalFacing::LAST; ++vf) {
                for (auto st = StrideType::FIRST; st != StrideType::LAST; ++st) {
                    auto sprite = SpriteState(mt, hf, vf, st);
                    initializeSprite(graphics, sprite);
                }
            }
        }
    }
}

MotionType Merc::getMotionType() const
{
    MotionType motion;
    if (is_interacting_) {
        motion = MotionType::INTERACTING;
    } else if (is_on_ground()) {
        motion = acceleration_x_direction_ == 0
            ? MotionType::STANDING
            : MotionType::WALKING;
    } else {
        motion = velocity_.y < 0.0
            ? MotionType::JUMPING
            : MotionType::FALLING;
    }
    return motion;
}

bool Merc::is_on_ground() const
{
    return is_on_ground_;
}

VerticalFacing Merc::vertical_facing() const
{
    if (is_on_ground() && intended_vertical_facing_ == VerticalFacing::DOWN) {
        return VerticalFacing::HORIZONTAL;
    } else {
        return intended_vertical_facing_;
    }
}

auto Merc::SpriteState::rank() const
{
    return std::tie(motion_type, horizontal_facing, vertical_facing, stride_type);
}

bool operator<(const Merc::SpriteState& a, const Merc::SpriteState& b)
{
    return a.rank() < b.rank();
}

const Merc::SpriteState Merc::getSpriteState() const
{
    return SpriteState(
            getMotionType(),
            horizontal_facing_,
            vertical_facing(),
            walking_animation_.stride()
            );
}

const Rectangle Merc::leftCollision(units::Game delta) const
{

    assert(delta <= 0 && "Wrong delta value");
    /*SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_DEBUG,
                "Collision Detected",
                "DEBUG:Collision has been detected!",
                NULL);*/

    return Rectangle(
            pos_.x + kCollisionX.getLeft() + delta,
            pos_.y + kCollisionX.getTop(),
            kCollisionX.getWidth() / 2 - delta,
            kCollisionX.getHeight()
            );




}

const Rectangle Merc::rightCollision(units::Game delta) const
{
    assert(delta >= 0 && "Wrong delta value");

    return Rectangle(
            pos_.x + kCollisionX.getLeft() + kCollisionX.getWidth() / 2,
            pos_.y + kCollisionX.getTop(),
            kCollisionX.getWidth() / 2 + delta,
            kCollisionX.getHeight()
            );
}

const Rectangle Merc::topCollision(units::Game delta) const
{
    assert(delta <= 0 && "Wrong delta value");
    return Rectangle(
            pos_.x + kCollisionTopLeft,
            pos_.y + kCollisionYTop + delta,
            kCollisionTopWidth,
            kCollisionYHeight / 2 - delta
            );
}

const Rectangle Merc::bottomCollision(units::Game delta) const
{
    assert(delta >= 0 && "Wrong delta value");
    return Rectangle(
            pos_.x + kCollisionBottomLeft,
            pos_.y + kCollisionYTop + kCollisionYHeight / 2,
            kCollisionBottomWidth,
            kCollisionYHeight / 2 + delta
            );
}

void Merc::updateX(const std::chrono::milliseconds elapsed_time,
                     const Map& map,
                     ParticleTools&)
{
    // Update velocity
    units::Acceleration acceleration_x{0.0};
    if (acceleration_x_direction_ < 0) {
        acceleration_x = is_on_ground()
            ? -kWalkingAcceleration
            : -kAirAcceleration;
    } else if (acceleration_x_direction_ > 0) {
        acceleration_x = is_on_ground()
            ? kWalkingAcceleration
            : kAirAcceleration;
    }

    velocity_.x += acceleration_x * elapsed_time.count();

    if (acceleration_x_direction_ < 0) {
        velocity_.x = std::max(velocity_.x, -kMaxSpeedX);
    } else if (acceleration_x_direction_ > 0) {
        velocity_.x = std::min(velocity_.x, kMaxSpeedX);
    } else if (is_on_ground()) {
        velocity_.x = velocity_.x > 0.0
            ? std::max(0.0, velocity_.x - kFriction * elapsed_time.count())
            : std::min(0.0, velocity_.x + kFriction * elapsed_time.count());
    }
    // Calculate delta
    const units::Game delta = velocity_.x * elapsed_time.count();

    if (delta > 0.0) {
        // Check collision in the direction of delta
        CollisionInfo info = getWallCollisionInfo(map, rightCollision(delta));
        // React to collision
        if (info.collided) {
            pos_.x = units::tileToGame(info.col) - kCollisionX.getRight();
            velocity_.x = 0.0;
        } else {
            pos_.x += delta;
        }
        // Check collision in the direction opposite to delta
        info = getWallCollisionInfo(map, leftCollision(0));
        if (info.collided) {
            pos_.x = units::tileToGame(info.col) + kCollisionX.getRight();
        }
    } else {
        // Check collision in the direction of delta
        CollisionInfo info = getWallCollisionInfo(map, leftCollision(delta));
        // React to collision
        if (info.collided) {
            pos_.x = units::tileToGame(info.col) + kCollisionX.getRight();
            velocity_.x = 0.0;
        } else {
            pos_.x += delta;
        }
        // Check collision in the direction opposite to delta
        info = getWallCollisionInfo(map, rightCollision(0));
        if (info.collided) {
            pos_.x = units::tileToGame(info.col) - kCollisionX.getRight();
        }
    }
}

void Merc::createHeadBumpParticle(ParticleTools& particle_tools)
{
    auto bump_pos = Vector<units::Game>{
        pos_.x,
        pos_.y + kCollisionYTop
    };
    particle_tools.system.addNewParticle(
            std::make_shared<HeadBumpParticle>(particle_tools.graphics, bump_pos));

}

void Merc::updateY(const std::chrono::milliseconds elapsed_time,
                     const Map& map,
                     ParticleTools& particle_tools)
{
    // Update velocity
    const units::Acceleration gravity = is_jump_active_ && velocity_.y < 0
        ? kJumpGravity
        : kGravity;
    velocity_.y = std::min(velocity_.y + gravity * elapsed_time.count(),
                kMaxSpeedY);
    // Calculate delta
    const units::Game delta = velocity_.y * elapsed_time.count();
    if (delta > 0.0) {
        // Check collision in the direction of delta
        CollisionInfo info = getWallCollisionInfo(map, bottomCollision(delta));
        // React to collision
        if (info.collided) {
            pos_.y = units::tileToGame(info.row) - kCollisionYBottom;
            velocity_.y = 0.0;
            is_on_ground_ = true;
        } else {
            pos_.y += delta;
            is_on_ground_ = false;
        }
        // Check collision in the direction opposite to delta
        info = getWallCollisionInfo(map, topCollision(0));
        if (info.collided) {
            pos_.y = units::tileToGame(info.row) + kCollisionYHeight;
            createHeadBumpParticle(particle_tools);
        }
    } else {
        // Check collision in the direction of delta
        CollisionInfo info = getWallCollisionInfo(map, topCollision(delta));
        // React to collision
        if (info.collided) {
            pos_.y = units::tileToGame(info.row) + kCollisionYHeight;
            createHeadBumpParticle(particle_tools);
            velocity_.y = 0.0;
        } else {
            pos_.y += delta;
            is_on_ground_ = false;
        }
        // Check collision in the direction opposite to delta
        info = getWallCollisionInfo(map, bottomCollision(0));
        if (info.collided) {
            pos_.y = units::tileToGame(info.row) - kCollisionYBottom;
            is_on_ground_ = true;
        }
    }
}


