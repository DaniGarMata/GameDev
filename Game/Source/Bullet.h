#include "Enemy.h"

class Bullet : public Enemy
{
public:
	Bullet();
	~Bullet();

	void Update(float dt);
	void ComputePath(float dt);
	void MoveToPlayer(iPoint destination, float dt);
};