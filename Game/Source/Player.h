#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();


	bool CleanUp();
	bool dead = false;
	bool alive;
	bool jump;
	bool run;


	int PlayerPosition;
public:
	
private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	uint width = 32;
	uint height = 68;

	// L07 TODO 5: Add physics to the player - declare a Physics body

};

#endif // __PLAYER_H__