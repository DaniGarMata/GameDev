#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "List.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class EntityManager;
class Map;
class Physics;
class FadeToBlack;
class PathFinding;
class Death;
//L07 TODO 2: Add Physics module

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	// L03: DONE 1: Create methods to control that the real Load and Save happens at the end of the frame
	void LoadGameRequest();
	void SaveGameRequest() const;
	bool LoadFromFile();
	bool SaveToFile() ;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:
	// Settings
	bool debug = false;
	bool hasLost = false;
	bool win_ = false;
	bool die = false;
	bool hasLoaded = false;
	bool canContinue = false;
	int currentScene = 1;

	bool pause = false;
	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	Scene* scene;
	EntityManager* entityManager;
	Map* map;
	Physics* physics;
	FadeToBlack* fadeToBlack;
	PathFinding* pathfinding;
	Death* death;
	//L07 TODO 2: Add Physics module


	bool fpsCap = false;
	float dt = 0.0f;
	uint32 framesPerSecond = 0;
private:

	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module*> modules;

	// L01: DONE 2: Create new variables from pugui namespace:
	// xml_document to store the config file and
	// xml_node(s) to read specific branches of the xml
	pugi::xml_document configFile;
	pugi::xml_node configNode;


	uint frames;
	

	// L03: DONE 1: Create control variables to control that the real Load and Save happens at the end of the frame
	mutable bool saveGameRequested;
	bool loadGameRequested;
	
	PerfTimer* ptimer;
	PerfTimer* frameDuration;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;

	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;


	uint32 maxFrameRate = 0;
};

extern App* app;

#endif	// __APP_H__