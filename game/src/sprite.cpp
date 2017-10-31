#include "sprite.h"
#include "graphics.h"
#include "sprite.h"
#include "backdrop.h"

int source_x = 50;
int source_y = 0;


Sprite::Sprite(Graphics& graphics,
        const std::string& file_name,
        const units::Pixel source_x, const units::Pixel source_y,
        const units::Pixel width, const units::Pixel height) :
    texture_{graphics.loadImage(file_name.c_str(), true)},
    source_rect_{source_x, source_y , width, height}
{


}
Sprite spr;

void Sprite::draw(Graphics& graphics, const Vector<units::Game>& pos) const
{

    const units::Pixel dstx = units::gameToPixel(pos.x);
    const units::Pixel dsty = units::gameToPixel(pos.y);

     graphics.renderTexture(texture_, dstx, dsty,  &source_rect_);


}
 Sprite::Sprite(){
   //mPosX = 100;//0
    //mPosY = 0;//0



}

void Sprite::render(int camX, int camY){

  spr.render( mPosX - camX, mPosY - camY );

}
