#pragma once

#ifndef MINION_H
#define MINION_H
#include <memory>
#include "units.h"
#include "vector.h"

struct DamageText;


class Minion
{
public:
	Minion(void);
	//~Minion(void);
	 //virtual ~Minion() = default;
   virtual const Vector<units::Game> getCenterPos() const = 0;
   virtual const std::shared_ptr<DamageText> getDamageText() const = 0;



	int minionState;

	bool killOtherUnits;

	int iBlockID;
	float fXPos, fYPos;
	int iHitBoxX, iHitBoxY;
	bool minionSpawned;
	bool collisionOnlyWithPlayer;

	int deadTime;

	bool onAnotherMinion;

	// ----- true = LEFT, false = RIGHT
	bool moveDirection;
	int moveSpeed;

	int jumpState;

	float startJumpSpeed;
	float currentJumpSpeed;
	float jumpDistance;
	float currentJumpDistance;
	float currentFallingSpeed;

	// ---------- Methods
	virtual void Update();
	//virtual void Draw(SDL_Renderer* rR, CIMG* iIMG);

	/*virtual void updateYPos(int iN);
	virtual void updateXPos();

	virtual bool updateMinion();
	virtual void minionPhysics();

	virtual void collisionEffect();

	virtual void minionDeathAnimation();

	void physicsState1();
	void physicsState2();

	void Spawn();*/
	void startJump(int iH);
	/*void resetJump();

	// ----- COLLISON

	virtual void collisionWithPlayer(bool TOP);
	virtual void points(int iPoints);

	virtual void collisionWithAnotherUnit(); // -- PLAYERFIREBALL

	virtual void lockMinion();

	// ----- get & set -----
	int getBloockID();
	void setBlockID(int iBlockID);
	int getMinionState();

	virtual void setMinionState(int minionState);
	virtual bool getPowerUP();*/

	int getXPos();
	int getYPos();
	void setYPos(int iYPos);
};

#endif
