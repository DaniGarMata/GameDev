#include "GuiManager.h"
#include "App.h"
#include "Textures.h"

#include "GuiButton.h"
#include "GuiCheck.h"
#include "GuiSlidder.h"
#include "Audio.h"

GuiManager::GuiManager(bool startEnabled) : Module(startEnabled)
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Awake(pugi::xml_node&config)
{
	folder.Create(config.child("folder").child_value());
	return true;
}

bool GuiManager::Start()
{
	//button
	SString tmp("%s%s", folder.GetString(), "button_newgame.png");
	SString tmp2("%s%s", folder.GetString(), "button_continue.png");
	SString tmp3("%s%s", folder.GetString(), "button_options.png");
	SString tmp4("%s%s", folder.GetString(), "button_exit.png");
	SString tmp5("%s%s", folder.GetString(), "button_credits.png");
	SString tmp6("%s%s", folder.GetString(), "button_resume.png");
	SString tmp7("%s%s", folder.GetString(), "button_to_title.png");
	SString tmp8("%s%s", folder.GetString(), "button_X.png");
	SString tmp9("%s%s", folder.GetString(), "checkbox.png");
	SString tmp10("%s%s", folder.GetString(), "slider.png");


	newGameTex = app->tex->Load(tmp.GetString());
	continueTex = app->tex->Load(tmp2.GetString());
	optionsTex = app->tex->Load(tmp3.GetString());
	exitTex = app->tex->Load(tmp4.GetString());
	creditsTex = app->tex->Load(tmp5.GetString());
	resumeTex = app->tex->Load(tmp6.GetString());
	bttTex = app->tex->Load(tmp7.GetString());
	closeTex = app->tex->Load(tmp8.GetString());

	checkbox = app->tex->Load(tmp9.GetString());

	slider = app->tex->Load(tmp10.GetString());
	return true;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, GuiButtonType btype, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	// L14: TODO1: Create a GUI control and add it to the list of controls

	GuiControl* control = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:

		control = new GuiButton(id, bounds, btype, text);
		break;

		// More Gui Controls can go here
	case GuiControlType::CHECKBOX:

		control = new GuiCheck(id, bounds, text, false);
		break;
	
	case GuiControlType::SLIDER:

		control = new GuiSlidder(id, bounds, text,0,100);
		break;

	default:
		break;
	}

	//Set the observer

	control->SetObserver(observer);
	//control->SetTexture(texture);

	// Created GuiControls are added to the list of controls
	if (control != nullptr) controls.Add(control);

	return control;
}

void GuiManager::DestroyGuiControl(GuiControl* entity)
{
	
}

bool GuiManager::Update(float dt)
{
	bool ret = true;
	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	ret = UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	return ret;
}

bool GuiManager::UpdateAll(float dt, bool doLogic) {
	bool ret = true;
	if (doLogic) {

		ListItem<GuiControl*>* control = controls.start;

		while (control != nullptr)
		{
			ret = control->data->Update(dt);
			control = control->next;
			if (!ret)
				break;
		}

	}
	return ret;

}

bool GuiManager::Draw() {

	ListItem<GuiControl*>* control = controls.start;

	while (control != nullptr)
	{
		if (control->data->state != GuiControlState::NONE)
		{
			switch (control->data->type)
			{
			default:
				break;
			case GuiControlType::BUTTON:
				switch (control->data->buttonType)
				{
				default:
					break;
				case GuiButtonType::NEW_GAME:
					control->data->Draw(app->render, newGameTex);
					break;
				case GuiButtonType::CONTINUE:
					control->data->Draw(app->render, continueTex);
					break;
				case GuiButtonType::OPTIONS:
					control->data->Draw(app->render, optionsTex);
					break;
				case GuiButtonType::EXIT:
					control->data->Draw(app->render, exitTex);
					break;
				case GuiButtonType::CREDITS:
					control->data->Draw(app->render, creditsTex);
					break;
				case GuiButtonType::RESUME:
					control->data->Draw(app->render, resumeTex);
					break;
				case GuiButtonType::BACKTOTILE:
					control->data->Draw(app->render, bttTex);
					break;
				case GuiButtonType::CLOSE:
					control->data->Draw(app->render, closeTex);
					break;
				}
					break;

			case GuiControlType::CHECKBOX:
				control->data->Draw(app->render, checkbox);
					break;

			case GuiControlType::SLIDER:
				control->data->Draw(app->render, slider);
					break;
			}
			
		}

		control = control->next;
	}

	return true;

}

bool GuiManager::CleanUp()
{
	controls.Clear();
	app->tex->UnLoad(newGameTex);
	app->tex->UnLoad(continueTex);
	app->tex->UnLoad(optionsTex);
	app->tex->UnLoad(exitTex);
	app->tex->UnLoad(creditsTex);
	app->tex->UnLoad(resumeTex);
	app->tex->UnLoad(bttTex);
	app->tex->UnLoad(closeTex);
	return true;
}