#ifndef PLAYER_H_
#define PLAYER_H_

#include <chrono>
#include "damageable.h"
#include "damage_text.h"
#include "number_sprite.h"
#include "motion_type.h"
#include "polar_star.h"
#include "sprite.h"
#include "sprite_state.h"
#include "stride_type.h"
#include "timer.h"
#include "units.h"
#include "varying_width_sprite.h"
#include "vector.h"

struct Graphics;
struct Map;
struct NumberSprite;
struct Projectile;
struct Rectangle;
struct ParticleTools;

struct Player : public Damageable {
   Player(Graphics& graphics, Vector<units::Game> pos);
   ~Player();

   int getPosX();
        int getPosY();

   void update(const std::chrono::milliseconds elapsed_time, const Map& map,
               ParticleTools& particle_tools);
   void draw(Graphics& graphics) const;
   void drawHUD(Graphics& graphics) const;
   //void render( int camX, int camY );


   void startMovingLeft();
   void startMovingRight();
   void stopMoving();
   const Rectangle getCollisionRectangle() const;
   void lookUp();
   void lookDown();
   void lookHorizontal();

   void startJump();
   void stopJump();

   void startFire();
   void stopFire();

   void takeDamage(units::HP damage);

   const Rectangle getDamageRectangle() const;
   const Vector<units::Game> getCenterPos() const override;
   const std::shared_ptr<DamageText> getDamageText() const override;
   std::vector<std::shared_ptr<GenericProjectile> > getProjectiles();

private:
   bool is_gun_up() const;

   struct SpriteState {
       SpriteState(MotionType motion_type=MotionType::STANDING,
               HorizontalFacing horizontal_facing=HorizontalFacing::LEFT,
               VerticalFacing vertical_facing=VerticalFacing::HORIZONTAL,
               StrideType stride_type=StrideType::MIDDLE
               ) :
           motion_type{motion_type},
           horizontal_facing{horizontal_facing},
           vertical_facing{vertical_facing},
           stride_type{stride_type}
       {}

       auto rank() const;

       MotionType motion_type;
       HorizontalFacing horizontal_facing;
       VerticalFacing vertical_facing;
       StrideType stride_type;
   };
   friend bool operator<(const SpriteState& a, const SpriteState& b);

   struct WalkingAnimation {
       WalkingAnimation();

       StrideType stride() const;
       void update();
       void reset();
   private:
       Timer frame_timer_;
       int current_index_;
       bool forward_;
   };
   units::Tile getFrameX(const SpriteState&) const;
   units::Tile getFrameY(const SpriteState&) const;

   void initializeSprites(Graphics& graphics);
   void initializeSprite(Graphics& graphics, const SpriteState& sprite_state);
   const SpriteState getSpriteState() const;

   struct Health {
       Health(Graphics& graphics);
       void update();
       void draw(Graphics& graphics) const;
       // returns true if we have died
       bool takeDamage(units::HP damage);
   private:
       units::Game fillOffset(units::HP health) const;
       units::HP damage_;
       Timer damage_timer_;

       units::HP max_health_;
       units::HP current_health_;
       Sprite health_bar_sprite_;
       VaryingWidthSprite health_fill_bar_sprite_;
       VaryingWidthSprite damage_fill_sprite_;
   };

   const Rectangle leftCollision(units::Game delta) const;
   const Rectangle rightCollision(units::Game delta) const;
   const Rectangle topCollision(units::Game delta) const;
   const Rectangle bottomCollision(units::Game delta) const;

   void createHeadBumpParticle(ParticleTools& particle_tools);
   void updateX(const std::chrono::milliseconds elapsed_time, const Map& map,
                ParticleTools& particle_tools);
   void updateY(const std::chrono::milliseconds elapsed_time, const Map& map,
                ParticleTools& particle_tools);

   //bool spriteIsVisible() const;

   MotionType getMotionType() const;
   bool is_on_ground() const;
   VerticalFacing vertical_facing() const;
   //const Rectangle getCollisionRectangle() const;
   Vector<units::Game> pos_;
   Vector<units::Velocity> velocity_;
   int acceleration_x_direction_;
   HorizontalFacing horizontal_facing_;
   VerticalFacing intended_vertical_facing_;
   bool is_on_ground_;
   bool is_jump_active_;
   bool is_interacting_;

   Health health_;
   Timer invincible_timer_;
   std::shared_ptr<DamageText> damage_text_;

   WalkingAnimation walking_animation_;
   PolarStar polar_star_;

   std::map<SpriteState, std::unique_ptr<Sprite> > sprites_;
};

#endif /* SRC/PLAYER_H_ */
