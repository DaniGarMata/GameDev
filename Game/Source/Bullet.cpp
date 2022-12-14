#include "Bullet.h"
#include "Physics.h"
#include "Player.h"
#include "Pathfinding.h"
#include "Map.h"
#include "SDL/include/SDL.h"

Bullet::Bullet() : Enemy()
{
	anim.PushBack({ 0,28,36,42 });
	/*anim.PushBack({40,31,40,40});
	anim.PushBack({ 80,40,32,30 });
	anim.PushBack({ 117,40,40,30 });*/
	anim.speed = 0.1f;
	anim.loop = true;
	
	h = 30;
	w = 30;
	health = 1;
	range = 300;
	pathUpdateTime = 1.5f;
	pathUpdateTimer = pathUpdateTime;
	type = BULLET;
}

Bullet::~Bullet()
{
}


void Bullet::Update(float dt)
{
	anim.Update();

	hasTarget = CheckIfHasTarget();

	//The enemy has only to move if it's in range of the player
	pos.x = METERS_TO_PIXELS(pbody->body->GetPosition().x);
	pos.y = METERS_TO_PIXELS(pbody->body->GetPosition().y);
	if (hasTarget && health > 0 && app->player->lives > 0)
	{
		printf("\nPos x: %i y %i", pos.x, pos.y);
		ComputePath(dt);
	}
	else
	{
		pbody->body->SetLinearVelocity({ 0, 0 });
		currentPath = nullptr;
	}
	if (pbody->body->GetLinearVelocity().x <= 0.1)
	{
		facingLeft = true;
	}
	else
	{
		facingLeft = false;
	}
	
}

void Bullet::ComputePath(float dt)
{
	iPoint playerPos = app->player->pos;
	float dist = Distance(pos.x, pos.y, playerPos.x, playerPos.y);
	
	pathUpdateTimer += dt;
	if (dist > range) {
		return;
	}
	else
	{
		if (!app->player->hurt)
		{
			if (pathUpdateTimer >= pathUpdateTime) {
				pathUpdateTimer = 0.0f;
				pathIndex = 0;

				iPoint origin = app->map->WorldToMap(pos.x, pos.y);
				iPoint destination = app->map->WorldToMap(app->player->pos.x, app->player->pos.y);
				int res = app->pathfinding->CreatePath(origin, destination);

				if (res > 0) {

					currentPath = app->pathfinding->GetLastPath();
					if (currentPath != nullptr)
					{
						if (currentPath->Count() > 1)
						{
							pathIndex = 1;
							activeNode = app->map->MapToWorld(currentPath->At(pathIndex)->x, currentPath->At(pathIndex)->y);
						}
						else if (currentPath->Count() > 0)
						{
							activeNode = app->map->MapToWorld(currentPath->At(0)->x, currentPath->At(0)->y);
						}
					}
				}
				if (currentPath != nullptr)
				{
					if (currentPath->Count() > 0) {
						if (pos == activeNode) {
							pathIndex++;

							if (pathIndex < currentPath->Count()) {
								activeNode = app->map->MapToWorld(currentPath->At(pathIndex)->x, currentPath->At(pathIndex)->y);
							}
						}

						if (pathIndex < currentPath->Count()) {
							MoveToPlayer(activeNode, dt);
						}
					}
				}
			}
		}
	}
	
}

void Bullet::MoveToPlayer(iPoint destination, float dt)
{
	iPoint diff = destination - pos;
	
	fPoint dir = { (float)diff.x, (float)diff.y };
	dir.Normalize();
	dir *= speed * 3;

	fPoint step = { dir.x / dt, dir.y / dt };
	pbody->body->SetLinearVelocity({ step.x, step.y });
	
}
