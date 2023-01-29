#pragma once
#include "Entity.h"

class Mushroom : public Entity
{
public:
	Mushroom(iPoint position_, int ID);


	bool IsActivated();
	void Activate();

	void Update(float dt);
	void Use();
};