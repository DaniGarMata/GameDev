#include "Enemy.h"

class Bullet : public Enemy
{
public:
	Bullet(iPoint position_, Entity* target_, int ID);


	void Update(float dt);
	void ComputePath(float dt);
	void MoveToPlayer(iPoint destination, float dt);


};