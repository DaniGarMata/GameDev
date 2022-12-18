#include "Coin.h"


Coin::Coin(iPoint position_, int ID_) : Entity(EntityType::COIN, position_)
{
	this->ID = ID_;
	name.Create("coin%i", ID);
	this->anim.PushBack({ 0, 1, 13, 11 });
	this->anim.PushBack({ 15, 1, 13, 11 });
	this->anim.PushBack({ 30, 1, 13, 11 });
	this->anim.PushBack({ 45, 1, 13, 11 });
	this->anim.PushBack({ 60, 1, 13, 11 });
	this->anim.loop = true;
	this->anim.speed = 0.1f;
	this->w = 13;
	this->h = 11;
	pbody = app->physics->CreateRectangleSensor(position.x, position.y, w, h, STATIC);
	pbody->eListener = this;
	currentAnimation = &anim;
}

bool Coin::IsActivated()
{
	return this->active;
}

void Coin::Activate()
{
	active = true;
}

void Coin::Use()
{
	active = false;
	setPendingToDelete = true;
}

void Coin::Update(float dt)
{
	anim.Update();

	if (!this->IsActivated())
		this->setPendingToDelete = true;
}

