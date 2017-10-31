#include "Minion.h"


/*Minion::Minion(){


}*/



void Minion::startJump(int iH) {
	jumpState = 1;
	currentJumpSpeed = startJumpSpeed;
	jumpDistance = 32 * iH + 16.0f;
	currentJumpDistance = 0;
}
