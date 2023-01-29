#include "Player.h"
#include "Physics.h"
#include "Input.h"
#include "Audio.h"
#include "App.h"
#include "Render.h"
#include "Map.h"
#include "UI.h"
#include "Textures.h"
#include "Log.h"
#include "EntityManager.h"

Player::Player(iPoint position_) : Entity(EntityType::PLAYER, position_)
{
	name.Create("player");
	LoadAnims();
	maxVel = 5;
	minVel = 2.5;
	jumpVel = -5;
	startPos = position_;
	currentAnimation = &idleAnimR;
	grounded = true;
	LOG("Player spawned at x%i, y%i", position.x, position.y);
	this->w = 20;
	this->h = 26;
	pbody = app->physics->CreateRectangle(position.x, position.y, w, h, DYNAMIC);
	pbody->body->SetFixedRotation(true);
	this->pbody->eListener = this;
	useDownDash = false;
	god = false;
	this->health = 3;
}
void Player::Use()
{

}

void Player::Update(float dt)
{
	bool ret = true;

	if (this->health <= 0 || this->position.y >= 1000)
	{
		app->die = true;
		return;
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetPosition().x);
	position.y = METERS_TO_PIXELS(pbody->body->GetPosition().y);

	grounded = false;

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		god = !god;
	if (app->input->GetKey(SDL_SCANCODE_F3))
	{
		pbody->body->SetTransform({PIXEL_TO_METERS(startPos.x), PIXEL_TO_METERS(startPos.y) }, 0.0f);
	}
	if (pbody->body->GetLinearVelocity().y < 0.1f && pbody->body->GetLinearVelocity().y > -0.1f)
		grounded = true;
			
	if (pbody->body->GetLinearVelocity().y == 0)
		numJumps = 2;
	
	


	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		b2Vec2 vel = pbody->body->GetLinearVelocity();
		if (grounded)
			vel.x = maxVel;
		else
			vel.x = minVel;
		pbody->body->SetLinearVelocity(vel);
		if(currentAnimation != &runAnimR )
		{
			runAnimR.Reset();
			currentAnimation = &runAnimR;
		} 
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		b2Vec2 vel = pbody->body->GetLinearVelocity();
		if (grounded)
			vel.x = -maxVel;
		else
			vel.x = -minVel;
		pbody->body->SetLinearVelocity(vel);
		if (currentAnimation != &runAnimL)
		{
			runAnimL.Reset();
			currentAnimation = &runAnimL;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && numJumps > 0)
	{
		app->audio->PlayFx(jumpSFX);
		pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, jumpVel });
		numJumps--;
		app->entman->PlayJump();
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && grounded)
	{
		counterDash++;
		if (counterDash == 60 && useDownDash == false)
		{
			pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, jumpVel * 1.5f });
			useDownDash = true;
			app->entman->PlaySuperJump();
		}
		if (useDownDash == false)
		{
			if (currentAnimation == &idleAnimL)
			{
				superPowerAnimL.Reset();
				currentAnimation = &superPowerAnimL;
			}
			if (currentAnimation == &idleAnimR)
			{
				superPowerAnimR.Reset();
				currentAnimation = &superPowerAnimR;
			}
		}
	}
	else
	{
		counterDash = 0;
	}
	if (useDownDash)
	{
		abilityCD++;
		if(abilityCD == 360)
		{
			abilityCD = 0;
			useDownDash = false;
		}
	}

	if (currentState == EntityState::HURT)
	{
		counter++;
		if (currentAnimation != &hurtAnim)
		{
			currentAnimation = &hurtAnim;
		}
		if (counter == 60)
		{
			currentState = NONE;
		}
	}
	if (currentState != EntityState::HURT)
	{
		counter = 0;
	}
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		currentState = HURT;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE 
		&& app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE)
	{
		if (currentAnimation == &runAnimR || currentAnimation == &jumpAnimR || currentAnimation == &downAnimR
			|| currentAnimation == &superPowerAnimR && hurt == false)
		{
			currentAnimation = &idleAnimR;
		}
		
		if (currentAnimation == &runAnimL || currentAnimation == &jumpAnimL || currentAnimation == &downAnimL 
			 || currentAnimation == &superPowerAnimL && hurt == false)
		{
			currentAnimation = &idleAnimL;
		}
	}
	if (!grounded)
	{
		if (pbody->body->GetLinearVelocity().y < -1.0f && pbody->body->GetLinearVelocity().x > 0.1f)
			if (currentAnimation != &jumpAnimR)
				currentAnimation = &jumpAnimR;
		if(pbody->body->GetLinearVelocity().y < -1.0f && pbody->body->GetLinearVelocity().x < -0.1f)
			if (currentAnimation != &jumpAnimL)
				currentAnimation = &jumpAnimL;
		
		if (pbody->body->GetLinearVelocity().y > 0.1f && pbody->body->GetLinearVelocity().x < 0.1f)
			if (currentAnimation != &downAnimL)
				currentAnimation = &downAnimL;
		if (pbody->body->GetLinearVelocity().y > 0.1f && pbody->body->GetLinearVelocity().x > -0.1f)
			if (currentAnimation != &downAnimR)
				currentAnimation = &downAnimR;
	}

	
	
	currentAnimation->Update();

}

void Player::LoadAnims()
{
	idleAnimR.PushBack({ 0, 5, 16, 28 });
	idleAnimR.speed = 0.1f;
	idleAnimR.loop = true;

	idleAnimL.PushBack({ 68, 36, 16, 28 });
	idleAnimL.speed = 0.1f;
	idleAnimL.loop = true;

	runAnimR.PushBack({ 15, 5, 18, 28 });
	runAnimR.PushBack({ 34, 5, 18, 28 });
	runAnimR.speed = 0.1f;
	runAnimR.loop = true;

	runAnimL.PushBack({ 53, 37, 16, 28 });
	runAnimL.PushBack({ 34, 37, 16, 28 });
	runAnimL.speed = 0.1f;
	runAnimL.loop = true;

	jumpAnimR.PushBack({ 52, 5,  16, 28 });
	jumpAnimR.loop = false;
	downAnimR.PushBack({ 52, 5,  16, 28 });
	downAnimR.loop = false;
	jumpAnimL.PushBack({ 16, 37, 16, 28 });
	jumpAnimL.loop = false;
	downAnimL.PushBack({ 16, 37, 16, 28 });
	downAnimL.loop = false;

	hurtAnim.PushBack({ 70, 0, 16, 16 });
	hurtAnim.loop = true;
	hurtAnim.speed = 0.5f;

	superPowerAnimR.PushBack({ 0, 5, 16, 28 });
	superPowerAnimR.PushBack({ 0, 5, 16, 28 });
	superPowerAnimR.loop = true;
	superPowerAnimR.speed = 0.1f;

	superPowerAnimL.PushBack({ 68, 36, 16, 28 });
	superPowerAnimL.PushBack({ 68, 36, 16, 28 });
	superPowerAnimL.loop = true;
	superPowerAnimL.speed = 0.1f;
}


bool Player::LoadState(pugi::xml_node& data)
{
	bool ret = true;
	position.x = data.child("pos").attribute("x").as_int();
	position.y = data.child("pos").attribute("y").as_int();
	pbody->body->SetTransform({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) }, 0.0f);
	health = data.child("values").attribute("lives").as_int();
	
	return ret;
}


bool Player::SaveState(pugi::xml_node&data) 
{
	bool ret = true;
	pugi::xml_node posN = data.append_child("pos");
	posN.append_attribute("x").set_value(position.x);
	posN.append_attribute("y").set_value(position.y);
	pugi::xml_node values = data.append_child("values");
	values.append_attribute("lives").set_value(health);
	return true;
}
