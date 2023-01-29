#include "Enemy.h"

class Goomba : public Enemy
{
public:
	Goomba(iPoint position_, Entity* target, int ID);

	void Update(float dt);
	void ComputePath(float dt);
	void MoveToPlayer(iPoint destination, float dt);



	int counterJump;
	bool canJump;
};
