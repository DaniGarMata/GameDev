#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Physics.h"
#include "Player.h"
//#include "FadeToBlack.h"
//#include "UI.h"
#include "Defs.h"
#include "Log.h"
#include "CheckPoint.h"
#include "Pathfinding.h"
#include "EntityManager.h"
//#include "GuiManager.h"


#include "Defs.h"
#include "Log.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	folder.Create(config.child("folder").child_value());
	audioFile.Create(config.child("audio").child_value());
	startX = config.child("startX").attribute("value").as_int();
	startY = config.child("startY").attribute("value").as_int();
	LOG("%s", folder.GetString());
	return ret;

}

// Called before the first frame
bool Scene::Start()
{
	titleScreen = app->tex->Load("Assets/textures/TitleScreen.png");
	//img = app->tex->Load("Assets/Textures/test.png");
	app->audio->PlayMusic("Assets/Audio/Music/BG_Music.ogg", 0);
	
	// L03: DONE: Load map
	if (app->map->Load("map.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data))
		{
			app->pathfinding->SetMap(w, h, data);
		}

		RELEASE_ARRAY(data);
	}
	if (app->hasLost)
	{
		app->LoadGameRequest();
	}

	app->entityManager->Enable();

	if (app->hasLoaded && app->canContinue)
	{
		app->LoadGameRequest();
		app->canContinue = false;
	}
	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	LOG("%i\n", app->currentScene);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();
	if (app->debug)
	{
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y += 30;

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y -= 30;

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x += 30;

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x -= 30;
	}
	/*if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || app->currentScene == 2)
	{
		app->fadeToBlack->MFadeToBlack(this, (Module*)app->scene2);
	}

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN || app->die)
	{
		app->fadeToBlack->MFadeToBlack(this, (Module*)app->death);
		app->die = true;

	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		app->fadeToBlack->MFadeToBlack(this, (Module*)app->death);
		app->win_ = true;
	}
	*/


	// Draw map
	/*app->render->DrawTexture(background, 0, 0, NULL, false, 0.75f);
	app->render->DrawTexture(jungle, 0, 284, NULL, false, 0.5f);
	app->map->Draw();
	*/
	return true;
}

// Called each loop iteration
/*bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}*/
bool Scene::PostUpdate()
{
	bool ret = true;


	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->map->Unload();
	app->map->Disable();
	app->entityManager->DestroyAllEntities();
	app->physics->Disable();
	//app->ui->Disable();
	return true;
	return true;
}
