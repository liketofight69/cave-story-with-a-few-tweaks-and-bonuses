#include <string>
#include "animated_sprite.h"
#include "Min.h"
#include "graphics.h"
#include "sprite.h"

const units::Frame kNumFlyFrames{3};
const units::FPS kFlyFps{13};
const units::AngularVelocity kAngularVelocity{120.0 / 1000.0};
const units::Game kFlightAmplitude{5 * units::kHalfTile};

const std::string kSpritePath{"NpcCemet"};

const units::HP kContactDamage{1};

/*Min::Min(Graphics& graphics, Vector<units::Game> pos) {
    pos_(std::move(pos)),
    flight_center_y_{pos_.y},
    alive_{true},
    facing_{HorizontalFacing::RIGHT},
    flight_angle_{0.0},
    polar_star_(graphics),
    sprites_(),//,
    //damage_text_(std::make_shared<DamageText>())
{
    initializeSprites(graphics);
}

//Min::~Min() {}
}
*/
void Min::initializeSprites(Graphics& graphics)
{
    for (auto hf = HorizontalFacing::FIRST; hf != HorizontalFacing::LAST; ++hf) {
        initializeSprite(graphics, SpriteState(hf));
    }


}
void Min::initializeSprite(Graphics& graphics,
        const SpriteState& sprite_state)
{
    auto tile_y = sprite_state.facing == HorizontalFacing::RIGHT ? 3 : 2;//RIGHT
    sprites_[sprite_state] = std::make_shared<AnimatedSprite>(
            graphics,
            kSpritePath,
            units::tileToPixel(2), units::tileToPixel(tile_y),
            units::tileToPixel(1), units::tileToPixel(1),
            kFlyFps, kNumFlyFrames
            );
}

const Min::SpriteState Min::getSpriteState() const
{
    return SpriteState(facing_);
}


void Min::draw(Graphics& graphics) const
{


    sprites_.at(getSpriteState())->draw(graphics, pos_);
}
