#include "UI.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Render.h"
#include "Fonts.h"
#include "Player.h"
#include "Input.h"
#include "Window.h"
#include "GuiManager.h"
#include "Log.h"
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
	guiFile.Create(config.child("gui").child_value());

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
	//SString tmp5("%s%s", guiFile.GetString(), "pauseMenu.png");
	SString tmp6("%s%s", guiFile.GetString(), "menu_options.png");
	app->guiManager->Start();
	mushroom = app->tex->Load(tmp1.GetString());
	coin = app->tex->Load(tmp2.GetString());
	anim = app->tex->Load(tmp4.GetString());
	char lookUpTable[] = { "abcdefghijklmnopqrstuvwxyz0123456789!.?   " };
	//pausetex = app->tex->Load(tmp5.GetString());
	settings = app->tex->Load(tmp6.GetString());
	font = app->fonts->Load(tmp3.GetString(), lookUpTable, 7);
	//GUI
	app->guiManager->Start();
	btn1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GuiButtonType::RESUME, 1, "Test1", {0,0, 288, 64 }, this);

	btn2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GuiButtonType::OPTIONS, 2, "Test2", { 0,0, 288, 64 }, this);

	btn3 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GuiButtonType::BACKTOTILE, 3, "Test3", { 0,0, 288, 64 }, this);

	btn4 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GuiButtonType::CLOSE, 4, "Test4", { 0,0, 32, 32 }, this);
	btn5 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GuiButtonType::EXIT, 5, "Test4", { 0,0, 288, 64 }, this);


	check1 = (GuiCheck*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, GuiButtonType::NONE, 6, "Check1", { 0, 0, 48, 32 }, this);
	check2 = (GuiCheck*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, GuiButtonType::NONE, 7, "Check2", { 0, 32, 48, 32 }, this);

	slid1 = (GuiSlidder*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, GuiButtonType::NONE, 8, "sld1", { 0, 0, 160, 32 }, this);
	slid2 = (GuiSlidder*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, GuiButtonType::NONE, 9, "sld2", { 0, 0 , 160, 32 }, this);


	btn1->state = GuiControlState::NONE;
	btn2->state = GuiControlState::NONE;
	btn3->state = GuiControlState::NONE;
	btn4->state = GuiControlState::NONE;
	btn5->state = GuiControlState::NONE;
	check1->state = GuiControlState::DISABLED;
	check2->state = GuiControlState::DISABLED;
	slid1->state = GuiControlState::DISABLED;
	slid2->state = GuiControlState::DISABLED;
	settingsShow = false;
	pauseShow = false;
	check2->checked = app->render->vsync;

	return true;
}

bool UI::Update()
{
	
	return true;
}


bool UI::PostUpdate()
{
	if (app->currentScene == 1 || app->currentScene == 2)
	{
		seconds += app->dt / 1000;

		if (seconds >= 60)
		{
			seconds = 0;
			minutes++;
		}
		btn1->bounds.x = -app->render->camera.x / app->win->GetScale() + 100;
		btn1->bounds.y = -app->render->camera.y / app->win->GetScale() + 0;

		btn2->bounds.x = -app->render->camera.x / app->win->GetScale() + 100;
		btn2->bounds.y = -app->render->camera.y / app->win->GetScale() + 70;

		btn3->bounds.x = -app->render->camera.x / app->win->GetScale() + 100;
		btn3->bounds.y = -app->render->camera.y / app->win->GetScale() + 150;

		btn4->bounds.x = -app->render->camera.x / app->win->GetScale() + 410;
		btn4->bounds.y = -app->render->camera.y / app->win->GetScale() + 5;

		btn5->bounds.x = -app->render->camera.x / app->win->GetScale() + 100;
		btn5->bounds.y = -app->render->camera.y / app->win->GetScale() + 230;

		check1->bounds.x = -app->render->camera.x / app->win->GetScale() + 250;
		check1->bounds.y = -app->render->camera.y / app->win->GetScale() + 125;
	
		check2->bounds.x = -app->render->camera.x / app->win->GetScale() + 250;
		check2->bounds.y = -app->render->camera.y / app->win->GetScale() + 175;

		slid1->bounds.x = -app->render->camera.x / app->win->GetScale() + 200;
		slid1->bounds.y = -app->render->camera.y / app->win->GetScale() + 25;

		slid2->bounds.x = -app->render->camera.x / app->win->GetScale() + 200;
		slid2->bounds.y = -app->render->camera.y / app->win->GetScale() + 75;

		LOG("x%i y%i", app->render->camera.x, app->render->camera.y);
		healthAnim.Update();
		coinAnim.Update();
		abAnim.Update();
		if (app->entman->currentPlayer != nullptr)
		{
			if (app->entman->currentPlayer->GetHealth() == 3)
			{
				app->render->DrawTexture(mushroom, 115, 10, &healthAnim.GetCurrentFrame(), true);
			}
			if (app->entman->currentPlayer->GetHealth() >= 2)
			{
				app->render->DrawTexture(mushroom, 60, 10, &healthAnim.GetCurrentFrame(), true);
			}
			if (app->entman->currentPlayer->GetHealth() >= 1)
			{
				app->render->DrawTexture(mushroom, 5, 10, &healthAnim.GetCurrentFrame(), true);
			}
		
		SString tmp("%4d", score);
		SString tmp2("%d", (360 - app->entman->currentPlayer->abilityCD) / app->framesPerSecond);
		SString tmp3("time %d.%.2f",minutes, seconds);
		scoreMult = app->entman->currentPlayer->GetHealth();
		app->render->DrawTexture(coin, 1550, 10, &coinAnim.GetCurrentFrame(), true);
		app->fonts->BlitText(480, 5, font, tmp.GetString());
		app->fonts->BlitText(200, 5, font, tmp3.GetString());
		if (app->entman->currentPlayer->abilityCD != 0)
		{
			app->fonts->BlitText(480, 270, font, tmp2.GetString());
		}
		else
		{
			app->render->DrawTexture(anim, 1420, 790, &abAnim.GetCurrentFrame(), true);
		}

}
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			pauseShow = !pauseShow;

		if (pauseShow == true)
		{
			app->render->DrawTexture(pausetex, 500, 25, NULL, true);
			if (onPause == true)
			{
				btn1->state = GuiControlState::NORMAL;
				btn2->state = GuiControlState::NORMAL;
				btn3->state = GuiControlState::NORMAL;
				btn5->state = GuiControlState::NORMAL;
				onPause = false;
				app->pause = false;
			}
		}
		else
		{
			btn1->state = GuiControlState::NONE;
			btn2->state = GuiControlState::NONE;
			btn3->state = GuiControlState::NONE;
			btn5->state = GuiControlState::NONE;
			onPause = true;
			app->pause = true;
		}
		if (settingsShow == true)
		{
			app->render->DrawTexture(settings, 75, 25, NULL, true);
		}

		app->guiManager->Draw();
	}
	else if (app->currentScene == 3 && app->win_)
	{
		//SCORING SYSTEM
		SString tmp("score %4d", score);
		SString tmp2("score mult %i", scoreMult);
		SString tmp3("total score %i", score* scoreMult);
		SString tmp5("time %i.%f", minutes, seconds);
		SString tmp4;



		counter++;
		if (highScore < scoreMult * score)
		{
			highScore = scoreMult * score;
			tmp4.Create("high score %4d", highScore);
		}
		else
		{
			tmp4.Create("new high score %4d", highScore);
		}


		//SCORE
		if (counter <= 120)
		{
			if ((counter / 60) % 2 == 0)
				app->fonts->BlitText(225, 100, font, tmp.GetString());
		}
		else
			app->fonts->BlitText(225, 100, font, tmp.GetString());


		//SCORE MULT
		if (counter <= 240)
		{
			if ((counter / 60) % 2 == 0)
				app->fonts->BlitText(225, 125, font, tmp2.GetString());
		}
		else
			app->fonts->BlitText(225, 125, font, tmp2.GetString());
		
		
		
		//TOTAL SCORE
		if (counter <= 360)
		{
			if ((counter / 60) % 2 == 0)
				app->fonts->BlitText(225, 150, font, tmp3.GetString());
		}
		else
			app->fonts->BlitText(225, 150, font, tmp3.GetString());


		//HIGHSCORE
	
		if (counter <= 480)
		{
			if ((counter / 60) % 2 == 0)
				app->fonts->BlitText(225, 175, font, tmp4.GetString());
		}
		else
			app->fonts->BlitText(225, 175, font, tmp4.GetString());

	}
	return true;
}



bool UI::CleanUp()
{
	app->tex->UnLoad(mushroom);
	app->tex->UnLoad(coin);
	app->fonts->UnLoad(font);
	app->guiManager->CleanUp();
	return true;
}

void UI::AddScore(int score)
{
	this->score += score;
}

void UI::ResetScore()
{
	score = 0;
}

bool UI::LoadState(pugi::xml_node&data)
{
	score = data.child("score").attribute("value").as_int();
	highScore = data.child("highscore").attribute("value").as_int();

	return true;
}

bool UI::SaveState(pugi::xml_node&data)
{
	pugi::xml_node scr = data.append_child("score");
	scr.append_attribute("value").set_value(score);
	pugi::xml_node hscr = data.append_child("highscore");
	hscr.append_attribute("value").set_value(highScore);
	return true;
}
bool UI::OnGuiMouseClickEvent(GuiControl* control)
{
	bool ret = true;
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		//Checks the GUI element ID
		if (control->id == 1)
		{
			pauseShow = false;
			btn1->state = GuiControlState::DISABLED;
			btn2->state = GuiControlState::DISABLED;
			btn3->state = GuiControlState::DISABLED;
			btn5->state = GuiControlState::DISABLED;
		}

		if (control->id == 2)
		{
			settingsShow = true;
			btn1->state = GuiControlState::DISABLED;
			btn2->state = GuiControlState::DISABLED;
			btn3->state = GuiControlState::DISABLED;
			btn4->state = GuiControlState::NORMAL;
			btn5->state = GuiControlState::DISABLED;
			slid1->state = GuiControlState::NORMAL;
			slid2->state = GuiControlState::NORMAL;
			check1->state = GuiControlState::NORMAL;
			check2->state = GuiControlState::NORMAL;
		}

		if (control->id == 3)
		{
			if (app->currentScene == 1)
			{
				app->fadeToBlack->MFadeToBlack((Module*)app->scene, (Module*)app->intro);
			}
			if (app->currentScene == 2)
			{
				app->fadeToBlack->MFadeToBlack((Module*)app->scene2, (Module*)app->intro);
			}
		}

		if (control->id == 4)
		{
			settingsShow = false;
			btn1->state = GuiControlState::NORMAL;
			btn2->state = GuiControlState::NORMAL;
			btn3->state = GuiControlState::NORMAL;
			btn5->state = GuiControlState::NORMAL;
			btn4->state = GuiControlState::DISABLED;
			slid1->state = GuiControlState::DISABLED;
			slid2->state = GuiControlState::DISABLED;
			check1->state = GuiControlState::DISABLED;
			check2->state = GuiControlState::DISABLED;
		}
		if (control->id == 5)
		{
			ret = false;
		}

	}break;
	case GuiControlType::CHECKBOX:
	{
		if (control->id == 6)
		{
			bool fullscreen = check1->checked;
			app->win->SetFullScreen(fullscreen);
			app->render->SetFullScreen();
		}
		if(control->id == 7)
		{
			app->render->vsync = !app->render->vsync;
			app->SaveGameRequest();
			if (app->currentScene == 1)
			{
				app->render->SetVsync(app->render->vsync, (Module*)app->scene);
			}
			else if (app->currentScene == 2)
			{
				app->render->SetVsync(app->render->vsync, (Module*)app->scene2);
			}
			app->LoadGameRequest();
		}
	}break;
	case GuiControlType::SLIDER:
	{
		if (control->id == 8)
		{
			app->audio->volMusic = slid1->value * 100;
		}
		if (control->id == 9)
		{
			app->audio->volFX = slid2->value * 100;
		}
	}break;
	//Other cases here

	default: break;
	}

	return ret;
}