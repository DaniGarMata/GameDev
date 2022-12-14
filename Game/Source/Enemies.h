#pragma once
#include "Module.h"
#include "Enemy.h"
#include "List.h"

enum EnemyType
{
	GOOMBA = -1,
	BULLET,

	MAX
};

class Enemies : public Module
{
public:
	Enemies(bool startEnabled);
	~Enemies();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool DestroyEnemies();
	

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&) const;
	bool CleanUp();

	void CreateEnemy(EnemyType type, float x, float y, int health);
private:
	List<Enemy*> enemies;
	int hitSFX;
	int playerHit;
	SString sfx;
	SString folder;
	SDL_Texture* tex;
	SDL_Texture* path;
};