#include "Enemy.h"

class Goomba : public Enemy
{
public:
	Goomba();
	~Goomba();

	void Update(float dt);
	void ComputePath(float dt);
	void MoveToPlayer(iPoint destination, float dt);

	int counterJump;
	bool canJump;
};
