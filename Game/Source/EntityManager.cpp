#include "EntityManager.h"
#include "Physics.h"
#include "App.h"
#include "CheckPoint.h"
#include "Goomba.h"
#include "Player.h"
#include "Entity.h"
#include "Bullet.h"
#include "Coin.h"
#include "Audio.h"
#include "Window.h"
#include "Render.h"
#include "Log.h"
#include "Textures.h"

EntityManager::EntityManager(bool startEnabled) : Module(startEnabled)
{
	name.Create("entities");
}


// Destructor
EntityManager::~EntityManager()
{
	CleanUp();
}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	folder.Create(config.child("folder").child_value());
	audio.Create(config.child("sfx").child_value());

	return true;
}

bool EntityManager::Start()
{
	//Loading sprites
	SString pathPlayer("%s%s", folder.GetString(), "Mareo.png");
	SString pathEnemies("%s%s", folder.GetString(), "Gooma.png");
	SString pathCollect("%s%s", folder.GetString(), "goldCoin.png");
	SString pathCheck("%s%s", folder.GetString(), "Flag.png");
	SString pathPath("%s%s", folder.GetString(), "nav.png");

	SString jumpPath("%s%s", audio.GetString(), "Jump.wav");
	//SString superJumpPath("%s%s", audio.GetString(), "superJump.wav");
	SString coinPath("%s%s", audio.GetString(), "Coin.wav");
	//SString checkPath("%s%s", audio.GetString(), "check.wav");
	SString enemyPath("%s%s", audio.GetString(), "enemy.wav");
	//SString gemPath("%s%s", audio.GetString(), "gem.wav");
	//SString hurtPath("%s%s", audio.GetString(), "playerHurt.wav");
	playerTex = app->tex->Load(pathPlayer.GetString());
	enemiesTex = app->tex->Load(pathEnemies.GetString());
	checkTex = app->tex->Load(pathCheck.GetString());
	collectTex = app->tex->Load(pathCollect.GetString());
	path = app->tex->Load(pathPath.GetString());

	playerJumpSFX = app->audio->LoadFx(jumpPath.GetString());
	//playerSuperJumpSFX = app->audio->LoadFx(superJumpPath.GetString());
	//playerHitSFX = app->audio->LoadFx(hurtPath.GetString());
	
	//checkpointSFX = app->audio->LoadFx(checkPath.GetString());
	
	hitSFX = app->audio->LoadFx(enemyPath.GetString());
	//Loading audio


	return true;
}

// Called before quitting

bool EntityManager::PreUpdate()
{
	for (ListItem<Entity*>* ent = entities.start; ent != nullptr; ent = ent->next)
	{
		if (ent->data->setPendingToDelete)
		{
			DestroyEntity(ent->data);
			break;
		}
		if ((ent->data->type == EntityType::ENEMY_BULLET || ent->data->type == EntityType::ENEMY_GOOMBA) && ent->data->GetTarget() == nullptr)
		{
			ent->data->SetTarget(currentPlayer);
		}
	}
	
	return true;
}
void EntityManager::UpdateAll(float dt, bool canUpdate)
{
	if (canUpdate)
	{
		for (ListItem<Entity*>* ent = entities.start; ent != nullptr; ent = ent->next)
		{
			ent->data->Update(dt);
		}
	}
	return;
}
bool EntityManager::Update(float dt)
{
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle && app->pause) doLogic = true;

	UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}


	if (currentPlayer != nullptr)
	{
		if (!currentPlayer->god)
		{
			//Camera follows the player
			uint width, height;

			app->win->GetWindowSize(width, height);
			app->render->camera.x = -((currentPlayer->GetPos().x * (int)app->win->GetScale()) - ((int)width) / 2 + currentPlayer->pbody->width / 2);
			app->render->camera.y = -((currentPlayer->GetPos().y * (int)app->win->GetScale()) - ((int)height) / 2 + currentPlayer->pbody->height / 2);
			//Camera bounds
			if (app->render->camera.x > 0)
				app->render->camera.x = 0;
			if (app->render->camera.y > 0)
				app->render->camera.y = 0;
			if (-app->render->camera.x > app->map->bounds.x)
				app->render->camera.x = -app->map->bounds.x;
			if (-app->render->camera.y > app->map->bounds.y)
				app->render->camera.y = -app->map->bounds.y;
		}
	}
	

	return true;
}

bool EntityManager::PostUpdate()
{
	for (ListItem<Entity*>* ent = entities.start; ent != nullptr; ent = ent->next)
	{
		switch (ent->data->type)
		{
		case EntityType::PLAYER: app->render->DrawTexture(playerTex, ent->data->GetPos().x - (ent->data->w / 2), ent->data->GetPos().y - (ent->data->h / 2), &ent->data->currentAnimation->GetCurrentFrame()); break;
		case EntityType::ENEMY_BULLET:
		{
			SDL_RendererFlip rotate = SDL_FLIP_NONE;
			if (ent->data->pbody->body->GetLinearVelocity().x > 0) rotate = SDL_FLIP_HORIZONTAL;
			app->render->DrawTexture(enemiesTex, ent->data->GetPos().x - (ent->data->w / 2), ent->data->GetPos().y - (ent->data->h / 2), &ent->data->currentAnimation->GetCurrentFrame(), false, 1.0f, rotate);
		}break;
		case EntityType::ENEMY_GOOMBA:
		{
			SDL_RendererFlip rotate = SDL_FLIP_NONE;
			if (ent->data->pbody->body->GetLinearVelocity().x > 0) rotate = SDL_FLIP_HORIZONTAL;
			app->render->DrawTexture(enemiesTex, ent->data->GetPos().x - (ent->data->w / 2), ent->data->GetPos().y - (ent->data->h / 2), &ent->data->currentAnimation->GetCurrentFrame(), false, 1.0f, rotate);
		}break;
		case EntityType::COIN: app->render->DrawTexture(collectTex, ent->data->GetPos().x - (ent->data->w / 2), ent->data->GetPos().y - (ent->data->h / 2), &ent->data->currentAnimation->GetCurrentFrame()); break;
		
		case EntityType::CHECKPOINT: app->render->DrawTexture(checkTex, ent->data->GetPos().x - (ent->data->w / 2), ent->data->GetPos().y - (ent->data->h / 2), &ent->data->currentAnimation->GetCurrentFrame()); break;
		}
	}
	if (app->debug)
	{
		DrawPath(path);
	}
	

	return true;
}

bool EntityManager::CleanUp()
{
	DestroyAllEntities();
	app->tex->UnLoad(enemiesTex);
	app->tex->UnLoad(playerTex);
	app->tex->UnLoad(collectTex);
	app->tex->UnLoad(checkTex);
	return true;
}

void EntityManager::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA->eListener->type == PLAYER && bodyB->listener == this && bodyA->listener == this)
	{
		if (bodyB->eListener->type == ENEMY_BULLET || bodyB->eListener->type == ENEMY_GOOMBA)
		{

			float topA = bodyA->body->GetPosition().y - PIXEL_TO_METERS(bodyA->eListener->h / 2);
			float botA = bodyA->body->GetPosition().y + PIXEL_TO_METERS(bodyA->eListener->h / 2);
			float topB = bodyB->body->GetPosition().y + PIXEL_TO_METERS(bodyB->eListener->h / 2);
			float botB = bodyB->body->GetPosition().y - PIXEL_TO_METERS(bodyB->eListener->h / 2);

			if (topA <= botB)
			{
				bodyB->eListener->health--;
				app->audio->PlayFx(hitSFX);
				//app->ui->AddScore(150);
			}

			else if (topA > botB && currentPlayer->GetState() != EntityState::HURT)
			{
				if (!currentPlayer->god)
				{
					bodyA->eListener->health--;
					app->audio->PlayFx(playerHitSFX);
					currentPlayer->SetState(EntityState::HURT);
				}
			}
			return;
		}
	
		else if (bodyB->eListener->type == COIN)
		{
			bodyB->eListener->Use();
			app->audio->PlayFx(coinSFX);
			
			return;
		}
		else if (bodyB->eListener->type == CHECKPOINT && !bodyB->eListener->active)
		{

			app->SaveGameRequest();
			app->audio->PlayFx(checkpointSFX);
			bodyB->eListener->Use();

			return;
		}
		else if (bodyB->eListener->type == DOOR)
		{
			if (app->currentScene == 2)
			{
				app->win_ = true;
				app->currentScene = 3;
			}
			else
			{
				app->currentScene = 2;
			}

			bodyB->eListener->Use();
			return;
		}
	}
}

bool EntityManager::SaveState(pugi::xml_node& data) const
{
	bool ret = true;
	if (!this->IsEnabled())
		return ret;
	ListItem<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		data.append_child(item->data->name.GetString());
		ret = item->data->SaveState(data.child(item->data->name.GetString()));
		item = item->next;
	}
	data.append_attribute("currentScene").set_value(app->currentScene);
	return ret;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	bool ret = true;
	if (!this->IsEnabled())
		return ret;
	ListItem<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->LoadState(data.child(item->data->name.GetString()));
		item = item->next;
	}
	app->hasLoaded = data.first_child();

	app->currentScene = data.attribute("currentScene").as_int();
	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type, iPoint position)
{
	Entity* ret = nullptr;
	switch (type)
	{
	case EntityType::PLAYER:ret = new Player(position); break;
	case EntityType::ENEMY_BULLET:
	{
		ret = new Bullet(position, currentPlayer, numBullet);
		numBullet++;
	}break;
	case EntityType::ENEMY_GOOMBA:
	{
		ret = new Goomba(position, currentPlayer, numGoomba);
		numGoomba++;
	}break;
	case EntityType::COIN:
	{
		ret = new Coin(position, numCoin);
		numCoin++;
	}break;
	
	case EntityType::CHECKPOINT:
	{
		ret = new CheckPoint(position, numCheckPoint);
		numCheckPoint++;
	}break;
	/*case EntityType::DOOR:
	{
		ret = new Door(position, numDoor);
		numDoor++;
	}break;
	*/
	}

	if (ret != nullptr)
	{
		ret->pbody->listener = this;
		entities.Add(ret);
	}
	return ret;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	app->physics->world->DestroyBody(entity->pbody->body);
	entities.Del(entities.At(entities.Find(entity)));
}
void EntityManager::SetPlayer(Player* player)
{
	currentPlayer = (Player*)player;
}
void EntityManager::DestroyAllEntities()
{
	currentPlayer = nullptr;
	entities.Clear();
	numCheckPoint = 0;
	numGoomba = 0;
	numBullet = 0;
	numDoor = 0;
	numCoin = 0;
}
void EntityManager::DrawPath(SDL_Texture* tex)
{
	const DynArray<iPoint>* currentPath = app->pathfinding->GetLastPath();
	if (currentPath != nullptr)
	{
		for (uint i = 0; i < currentPath->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(currentPath->At(i)->x, currentPath->At(i)->y);
			app->render->DrawTexture(tex, pos.x, pos.y);
		}
	}
}

void EntityManager::PlayJump()
{
	app->audio->PlayFx(playerJumpSFX);
}

void EntityManager::PlaySuperJump()
{
	app->audio->PlayFx(playerSuperJumpSFX);
}
