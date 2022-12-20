#ifndef __UI_H__
#define __UI_H__

#include "Module.h"
#include "Animation.h"
#include "App.h"

struct SDL_Texture;
class UI : public Module
{
public:
	UI(bool startEnabled);
	~UI();
	bool Awake(pugi::xml_node&);

	bool Start();

	bool PostUpdate();

	bool CleanUp();


private:
	SDL_Texture* health;
	SDL_Texture* coin;
	SDL_Texture* anim;
	
	int font;

	Animation healthAnim;
	Animation coinAnim;
	Animation abAnim;

	SString folder;
};

#endif // __UI_H__