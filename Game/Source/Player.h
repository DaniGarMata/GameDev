#ifndef __PLAYER_H__
#define __PLAYER_H__


#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Box2D/Box2D/Box2D.h"
//#include "FadeToBlack.h"
#include "List.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player(iPoint position_);
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();
	void Update(float dt);
	void Use();
	void LoadAnims();

	bool CleanUp();
	bool god;
	bool dead = false;
	bool alive;
	bool jump;
	bool run;
	bool hurt;
	bool useDownDash;
	int abilityCD;
	bool grounded;


	int PlayerPosition;
	bool LoadState(pugi::xml_node&);

	bool SaveState(pugi::xml_node&);
public:
	
private:
	//Jump sound
	int jumpSFX;
	int superJumpSFX;
	//Jump SFX folder path
	iPoint startPos;

	SString jumpSFXFile;
	//Variable for double jump
	int numJumps;
	//Velocity values
	float minVel;
	float maxVel;
	float jumpVel;
	//Sprite sheet folder path

	//Positions where the player should spawn in diferent levels
	int counterDash;

	int counter;
	//Player's b2 Body
	b2Body* c = nullptr;

	//Player texture
	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	
	uint width = 32;
	uint height = 68;
	Animation idleAnimL;
	Animation idleAnimR;

	Animation runAnimL;
	Animation runAnimR;

	Animation jumpAnimL;
	Animation downAnimL;
	Animation jumpAnimR;
	Animation downAnimR;

	Animation hurtAnim;

	Animation abilityAnimL;
	Animation abilityAnimR;

	// L07 TODO 5: Add physics to the player - declare a Physics body

};

#endif // __PLAYER_H__