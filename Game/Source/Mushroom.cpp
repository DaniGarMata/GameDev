#include "Mushroom.h"


Mushroom::Mushroom(iPoint position_, int ID_) : Entity(EntityType::MUSHROOM, position_)
{
	this->ID = ID_;
	name.Create("cherry%i", ID);
	this->anim.PushBack({ 0, 14, 16, 16 });
	this->anim.PushBack({ 22, 14, 16, 16 });
	this->anim.PushBack({ 44, 14, 16, 16 });
	this->anim.PushBack({ 67, 14, 16, 16 });
	this->anim.loop = true;
	this->anim.speed = 0.1f;
	this->w = 16;
	this->h = 16;
	pbody = app->physics->CreateRectangleSensor(position.x, position.y, w, h, STATIC);
	pbody->eListener = this;
	currentAnimation = &anim;
}

bool Mushroom::IsActivated()
{
	return this->active;
}

void Mushroom::Activate()
{
	this->active = true;
}

void Mushroom::Update(float dt)
{
	anim.Update();
	if (!this->IsActivated())
		setPendingToDelete = true;
	
}

void Mushroom::Use()
{
	active = false;
	setPendingToDelete = true;
}


