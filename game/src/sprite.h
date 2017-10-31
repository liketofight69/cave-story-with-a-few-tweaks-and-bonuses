#ifndef SPRITE_H
#define SPRITE_H

#include <chrono>
#include <string>
#include <SDL.h>
#include "vector.h"
#include "units.h"

struct Graphics;

class CamTexture
{
  CamTexture();

   ~CamTexture();

    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );


};




class Sprite {//struct

    public:
    Sprite(
            Graphics& graphics,
            const std::string& file_name,
            const units::Pixel source_x, const units::Pixel source_y,
            const units::Pixel width, const units::Pixel height
            );
    virtual ~Sprite() = default;

    Sprite();


    Sprite(const Sprite&)=delete;
    Sprite& operator=(const Sprite&)=delete;


    virtual void update() {}
    void draw(Graphics& graphics, const Vector<units::Game>& pos) const;
    SDL_Texture *texture_;
    SDL_Rect source_rect_;

    //source_rect_.x = 0;

    SDL_Rect camerRect;

     //Shows the sprite on the screen relative to the camera
		void render( int camX, int camY );
    int getPosX();
    int getPosY();
    int mPosX, mPosY;

private:
    //SDL_Texture *texture_;

protected:
    //SDL_Rect source_rect_;
};

#endif /* SPRITE_H */
