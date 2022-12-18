#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
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
	this->w = 32;
	this->h = 64;
	pbody = app->physics->CreateRectangle(position.x, position.y, w, h, DYNAMIC);
	pbody->body->SetFixedRotation(true);
	this->pbody->eListener = this;
	useDownDash = false;
	god = false;
	this->health = 3;
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


bool Player::SaveState(pugi::xml_node& data)
{
	bool ret = true;
	pugi::xml_node posN = data.append_child("pos");
	posN.append_attribute("x").set_value(position.x);
	posN.append_attribute("y").set_value(position.y);
	pugi::xml_node values = data.append_child("values");
	values.append_attribute("lives").set_value(health);
	return true;
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
		pbody->body->SetTransform({ PIXEL_TO_METERS(startPos.x), PIXEL_TO_METERS(startPos.y) }, 0.0f);
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
		if (currentAnimation != &runAnimR)
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
		app->entityManager->PlayJump();
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && grounded)
	{
		counterDash++;
		if (counterDash == 60 && useDownDash == false)
		{
			pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, jumpVel * 1.5f });
			useDownDash = true;
			app->entityManager->PlaySuperJump();
		}
		if (useDownDash == false)
		{
			if (currentAnimation == &idleAnimL)
			{
				abilityAnimL.Reset();
				currentAnimation = &abilityAnimL;
			}
			if (currentAnimation == &idleAnimR)
			{
				abilityAnimR.Reset();
				currentAnimation = &abilityAnimR;
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
		if (abilityCD == 360)
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
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE
		&& app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE)
	{
		if (currentAnimation == &runAnimR || currentAnimation == &jumpAnimR || currentAnimation == &downAnimR
			|| currentAnimation == &abilityAnimR && hurt == false)
		{
			currentAnimation = &idleAnimR;
		}

		if (currentAnimation == &runAnimL || currentAnimation == &jumpAnimL || currentAnimation == &downAnimL
			|| currentAnimation == &abilityAnimL && hurt == false)
		{
			currentAnimation = &idleAnimL;
		}
	}
	if (!grounded)
	{
		if (pbody->body->GetLinearVelocity().y < -1.0f && pbody->body->GetLinearVelocity().x > 0.1f)
			if (currentAnimation != &jumpAnimR)
				currentAnimation = &jumpAnimR;
		if (pbody->body->GetLinearVelocity().y < -1.0f && pbody->body->GetLinearVelocity().x < -0.1f)
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
	idleAnimR.PushBack({ 3, 9, 31, 54 });
	
	idleAnimR.speed = 0.1f;
	idleAnimR.loop = true;

	runAnimR.PushBack({ 38, 9, 31, 54 });
	runAnimR.PushBack({ 74, 9, 31, 54 });
	
	runAnimR.speed = 0.1f;
	runAnimR.loop = true;

	
	runAnimL.speed = 0.1f;
	runAnimL.loop = true;

	jumpAnimR.PushBack({ 110, 9,  31, 54 });
	jumpAnimR.loop = false;
	

	hurtAnim.PushBack({ 146, 0, 31, 31 });
	hurtAnim.loop = true;
	hurtAnim.speed = 0.1f;

	/*abilityAnimR.PushBack({90, 129, 29, 27});
	abilityAnimR.PushBack({ 138, 129, 28, 27 });
	abilityAnimR.loop = true;
	abilityAnimR.speed = 0.1f;

	abilityAnimL.PushBack({ 88, 157, 28, 27 });
	abilityAnimL.PushBack({ 135, 157, 29, 27 });
	abilityAnimL.loop = true;
	abilityAnimL.speed = 0.1f;
	*/
}


/*Player::~Player() {

}
void Player::Use()
{

}
bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, width / 2, bodyType::DYNAMIC);

	// L07 TODO 5: Add physics to the player - initialize physics body
	jump = false;
	
	return true;
}

bool Player::Update()
{
	// L07 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_REPEAT && jump == false) {
		//position.y -= 1;
		velocity = b2Vec2(0, -10);
		
		
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		//position.y += 1;
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		//position.x -= 1;
		velocity = b2Vec2(-10, 0);
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		//position.x += 1;
		velocity = b2Vec2(10, 0);
	}

	pbody->body->SetLinearVelocity(velocity);

	// Updates the pixel coordinates from the phycical body
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - (width / 2);
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - (height / 2);

	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Player::CleanUp()
{

	return true;
}
*/


