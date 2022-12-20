#include "UI.h"
#include "Textures.h"
#include "Player.h"
#include "Render.h"
#include "Fonts.h"

UI::UI(bool startEnabled) : Module(startEnabled)
{
	name.Create("ui");
}

UI::~UI()
{
}

bool UI::Awake(pugi::xml_node& config)
{
	folder.Create(config.child("folder").child_value());

	healthAnim.PushBack({ 0, 0, 16, 16 });
	healthAnim.speed = 0.1f;
	healthAnim.loop = true;

	coinAnim.PushBack({ 0, 0, 13, 13 });
	coinAnim.loop = true;
	coinAnim.speed = 0.1f;

	abAnim.PushBack({ 5, 13, 20, 19 });
	abAnim.PushBack({ 31, 13, 20, 19 });
	abAnim.loop = true;
	abAnim.speed = 0.1f;

	return true;
}

bool UI::Start()
{
	SString tmp1("%s%s", folder.GetString(), "health.png");
	SString tmp2("%s%s", folder.GetString(), "coin.png");
	SString tmp3("%s%s", folder.GetString(), "font.png");
	SString tmp4("%s%s", folder.GetString(), "superPower.png");

	health = app->tex->Load(tmp1.GetString());
	coin = app->tex->Load(tmp2.GetString());
	anim = app->tex->Load(tmp4.GetString());
	char lookUpTable[] = { "abcdefghijklmnopqrstuvwxyz0123456789!.?   " };

	font = app->fonts->Load(tmp3.GetString(), lookUpTable, 7);

	return true;
}


bool UI::PostUpdate()
{
	healthAnim.Update();
	coinAnim.Update();
	abAnim.Update();
	if (app->player->lives == 3)
	{
		app->render->DrawTexture(health, 115, 10, &healthAnim.GetCurrentFrame(), true);
	}
	if (app->player->lives >= 2)
	{
		app->render->DrawTexture(health, 60, 10, &healthAnim.GetCurrentFrame(), true);
	}
	if (app->player->lives >= 1)
	{
		app->render->DrawTexture(health, 5, 10, &healthAnim.GetCurrentFrame(), true);
	}
	SString tmp("%4d", app->player->score);
	SString tmp2("%d", (360 - app->player->superPowerCD) / app->framesPerSecond);

	app->render->DrawTexture(coin, 1550, 10, &coinAnim.GetCurrentFrame(), true);
	app->fonts->BlitText(480, 5, font, tmp.GetString()); 
	if (app->player->superPowerCD != 0)
	{
		app->fonts->BlitText(480, 270, font, tmp2.GetString());
	}
	else
	{
		
		app->render->DrawTexture(anim, 1420, 790, &abAnim.GetCurrentFrame(), true);
	}
	return true;
}



bool UI::CleanUp()
{
	app->tex->UnLoad(health);
	app->tex->UnLoad(coin);
	app->fonts->UnLoad(font);
	return true;
}
