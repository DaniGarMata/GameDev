#include "Player.h"
#include "Physics.h"
#include "Input.h"
#include "Textures.h"
#include "App.h"
#include "Render.h"
#include "Log.h"
#include "Window.h"
#include "Audio.h"
#include "Map.h"
#include "UI.h"
#include "Scene.h"
#include "Scene2.h"


Player::Player(bool startEnabled) : Module(startEnabled)
{
	name.Create("player");
	
}

Player::~Player()
{
}

bool Player::Awake(pugi::xml_node&config)
{
	LOG("Loading Player");
	bool ret = true;
	currentScene = config.child("start").attribute("value").as_int();
	numJumps = config.child("num_jumps").attribute("value").as_int();
	minVel = config.child("min_vel").attribute("value").as_float();
	maxVel = config.child("max_vel").attribute("value").as_float();
	jumpVel = config.child("jump_vel").attribute("value").as_float();
	folder.Create(config.child("folder").child_value());
	jumpSFXFile.Create(config.child("jump_SFX").child_value());
	scene1.x = config.child("scene1").attribute("x").as_int();
	scene1.y = config.child("scene1").attribute("y").as_int();

	scene2.x = config.child("scene2").attribute("x").as_int();
	scene2.y = config.child("scene2").attribute("y").as_int();

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

	return ret;
}

bool Player::Start()
{
	bool ret = true;
	switch (currentScene)
	{
	case 1:
	{
		pos.x = scene1.x;
		pos.y = scene1.y;
	} break;
	case 2:	
	{		
		pos.x = scene2.x;
		pos.y = scene2.y;

	}break;
	default:
		break;
	}
	app->ui->Enable();
	lives = 3;
	tex = app->tex->Load(folder.GetString());
	currentAnimation = &idleAnimR;

	SString tmp("%s%s", jumpSFXFile.GetString(), "jump.wav");
	SString tmp2("%s%s", jumpSFXFile.GetString(), "superPower.wav");
	jumpSFX = app->audio->LoadFx(tmp.GetString());
	superPowerSFX = app->audio->LoadFx(tmp2.GetString());
	grounded = true;

	b2BodyDef cbody;
	cbody.type = b2_dynamicBody;
	cbody.position.Set(PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y));
	cbody.fixedRotation = true;
	c = app->physics->world->CreateBody(&cbody);
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(12);
	b2FixtureDef fixturec;
	fixturec.shape = &circle;
	fixturec.density = 20.0f;
	fixturec.friction = 100.0f;
	c->ResetMassData();
	c->CreateFixture(&fixturec);
	
	pbody = new PhysBody();
	pbody->body = c;
	c->SetUserData(pbody);
	pbody->width = 16 * 0.5f;
	pbody->height = 32 * 0.5f;
	pbody->listener = this;
	useDownDash = false;
	die = false;
	debug = false;
	win = false;
	god = false;


	return ret;
}

bool Player::Update(float dt)
{
	bool ret = true;


	grounded = false;

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		god = !god;
	if (app->input->GetKey(SDL_SCANCODE_F3))
	{
		if(currentScene == 1)
			pbody->body->SetTransform({ PIXEL_TO_METERS(scene1.x), PIXEL_TO_METERS(scene1.y) }, 0.0f);
		if(currentScene == 2)
			pbody->body->SetTransform({ PIXEL_TO_METERS(scene2.x), PIXEL_TO_METERS(scene2.y) }, 0.0f);
	}
	if (pbody->body->GetLinearVelocity().y < 0.1f && pbody->body->GetLinearVelocity().y > -0.1f)
		grounded = true;
			
	if (pbody->body->GetLinearVelocity().y == 0)
		numJumps = 2;
	
	if (!god)
	{
		//Camera follows the player
		uint width, height;
		app->win->GetWindowSize(width, height);
		app->render->camera.x = -((pos.x * (int)app->win->GetScale()) - ((int)width) / 2 + pbody->width / 2);
		app->render->camera.y = -((pos.y * (int)app->win->GetScale()) - ((int)height) / 2 + pbody->height / 2);
		//Camera bounds
		if (app->render->camera.x > 0)
			app->render->camera.x = 0;
		if (app->render->camera.y > 0)
			app->render->camera.y = 0;
		if (-app->render->camera.x > app->map->bounds.x)
			app->render->camera.x = -app->map->bounds.x;
		if (-app->render->camera.y > app->map->bounds.y)
			app->render->camera.y = -app->map->bounds.y;
		//Die state
		if (pbody->body->GetPosition().y > PIXEL_TO_METERS( 1049))
			die = true;
	}


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
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && grounded)
	{
		counterDash++;
		if (counterDash == 60 && useDownDash == false)
		{
			pbody->body->SetLinearVelocity({ pbody->body->GetLinearVelocity().x, jumpVel * 1.5f });
			useDownDash = true;
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
		superPowerCD++;
		if(superPowerCD == 360)
		{
			superPowerCD = 0;
			useDownDash = false;
		}
	}

	if (hurt)
	{
		counter++;
		if (currentAnimation != &hurtAnim)
		{
			currentAnimation = &hurtAnim;
		}
		if (counter == 60)
		{
			lives--;
			hurt = false;
		}
	}
	if (!hurt)
	{
		counter = 0;
	}
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		hurt = true;
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


	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		app->audio->volMusic++;
		app->audio->volFX++;
	}
	if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		app->audio->volMusic--;
		app->audio->volFX--;
	}

	if (lives <= 0)
	{
		die = true;
	}

	currentAnimation->Update();

	return ret;
}

bool Player::PostUpdate()
{
	bool ret = true;

	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	pbody->GetPosition(pos.x, pos.y);
	app->render->DrawTexture(tex, pos.x, pos.y, &rect);
	return ret;
}

void Player::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

}

bool Player::LoadState(pugi::xml_node&data)
{
	bool ret = true;
	pos.x = data.child("pos").attribute("x").as_int();
	pos.y = data.child("pos").attribute("y").as_int();
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y) }, 0.0f);
	currentScene = data.child("start").attribute("value").as_int();
	lives = data.child("values").attribute("lives").as_int();
	score = data.child("values").attribute("score").as_int();
	return ret;
}


bool Player::SaveState(pugi::xml_node&data) const
{
	bool ret = true;
	pugi::xml_node posN = data.append_child("pos");
	posN.append_attribute("x").set_value(pos.x);
	posN.append_attribute("y").set_value(pos.y);
	pugi::xml_node currentState = data.append_child("start");
	currentState.append_attribute("value").set_value(currentScene);
	pugi::xml_node values = data.append_child("values");
	values.append_attribute("lives").set_value(lives);
	values.append_attribute("score").set_value(score);
	return true;
}

bool Player::CleanUp()
{
	bool ret = true;
	app->tex->UnLoad(tex);
	app->ui->Disable();

	return ret;
}