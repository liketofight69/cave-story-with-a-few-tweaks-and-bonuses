#include "game.h"
#include <iostream>
#include "IMGLOAD.h"
#include <random>
#include "swarmz.h"
#include "timing.h"
#include "vector.h"
#include "graphics.h"
#include "SDL_ttf.h"
#include <SFML/Audio.hpp>
#include <memory>
using namespace std;
using namespace sw;


struct Graphics *graph;



int main(int argc, char *argv[])
{


    std::random_device rd;
    std::mt19937 eng(rd());

    vector<Boid> boids;
        boids.push_back(Boid(Vec3(1, 0, 0), Vec3(1, 0, 0)));
    boids.push_back(Boid(Vec3(1.5, 0, 0), Vec3(1, 1, 0)));
    boids.push_back(Boid(Vec3(1, 0.5, 0.5), Vec3(0, 1, 0)));
    boids.push_back(Boid(Vec3(4, 4, -2), Vec3(1, 0, 0)));
    Swarm swarm(&boids);
    swarm.SteeringTargets.push_back(Vec3(0, 0, 0));
    swarm.SteeringTargets.push_back(Vec3(4, 0, 0));
    swarm.UpdateAcceleration();
//test




     //test
     sf::Music music;
    if (!music.openFromFile("music.wav"))
        return EXIT_FAILURE;

    music.play();
   //test






    //test
    Imginit IMG;
    Game game;
    IMG.PNGInit();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_EXIT,
                "Exit",
                "Press OK to exit",
                NULL);

    //std::cout << "Bye!\n";}//test
    return 0;
}
