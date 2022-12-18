#pragma once
#include "Entity.h"

class Coin : public Entity
{
public:
	Coin(iPoint position_, int ID);


	bool IsActivated();
	void Activate();

	void Use();

	void Update(float dt);
};