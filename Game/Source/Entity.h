#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Render.h"
#include "Animation.h"
#include "Physics.h"

struct Collider;

enum class EntityType
{
	PLAYER,
	ITEM,
	UNKNOWN,
	COIN,
	ENEMY_GOOMBA,
	ENEMY_BULLET,
	CHECKPOINT
};

struct SDL_Texture;
class PhysBody;
class EntityManager;
enum EntityState
{
	NONE = -1,
	HURT
};
class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}
	Entity() {}
	Entity(EntityType type_, iPoint position_) : type(type_), position(position_) {}

	virtual ~Entity() {};

	virtual void Update(float dt) {};
	virtual void Use() {};


	iPoint GetPos() { return position; }
	int GetHealth() { return health; }
	EntityState GetState() { return currentState; }
	void SetState(EntityState state) { currentState = state; }
	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(Collider* c1, Collider* c2) {
	
	};
	virtual void SetTarget(Entity* target) {};
	virtual Entity* GetTarget() { return nullptr; };
	friend class EntityManager;

public:

	SString name;
	EntityType type;
	bool active = true;
	pugi::xml_node parameters;
	int ID;
	SString name;
	PhysBody* pbody;
	EntityType type;
	bool active = true;
	bool setPendingToDelete;
	iPoint position;
	
	EntityState currentState = EntityState::NONE;
	int health;
	int h, w;
	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;       
	bool renderable = true;
};

#endif // __ENTITY_H__