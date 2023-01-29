#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Window.h"
#include "Log.h"
GuiButton::GuiButton(uint32 id, SDL_Rect bounds, GuiButtonType btype, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->buttonType = btype;
	//TODO Poner las animaciones
	if (buttonType == GuiButtonType::NEW_GAME || buttonType == GuiButtonType::CREDITS || buttonType == GuiButtonType::EXIT)
	{
		pressed.PushBack({ 0,128,288,64});
		focused.PushBack({ 0,64,288,64 });
		normal.PushBack({ 0,0,288,64 });
		disabled.PushBack({ 0,50,288,64 });
		
	}
	else if(buttonType == GuiButtonType::CONTINUE || buttonType == GuiButtonType::OPTIONS || buttonType == GuiButtonType::RESUME || buttonType == GuiButtonType::BACKTOTILE)
	{
		pressed.PushBack({ 0,128,288,64 });
		focused.PushBack({ 0,64,288,64 });
		normal.PushBack({ 0,0,288,64 });
		disabled.PushBack({ 0,139,154,45 });
	}
	else if (buttonType == GuiButtonType::CLOSE)
	{
		pressed.PushBack({ 64,0,32,36 });
		focused.PushBack({ 32,0,32,36 });
		normal.PushBack({ 0,0,32,36 });
		disabled.PushBack({ 0,139,44,42 });
	}
	noDraw.PushBack({ 0, 0, 1, 1 });
	canClick = true;
	drawBasic = false;
	onClickFX = app->audio->LoadFx("Assets/audio/fx/onClickFX.wav");
	onHoverFX = app->audio->LoadFx("Assets/audio/fx/onHoverFX.wav");
}

GuiButton::~GuiButton()
{

}

bool GuiButton::Update(float dt)
{
	bool ret = true;
	if (state != GuiControlState::DISABLED && state != GuiControlState::NONE)
	{
		// L14: TODO 3: Update the state of the GUiButton according to the mouse position
		int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);

		int offsetX = -app->render->camera.x / app->win->GetScale();
		int offsetY = -app->render->camera.y / app->win->GetScale();
		if ((mouseX + offsetX > bounds.x) && (mouseX + offsetX < (bounds.x + bounds.w)) &&
			(mouseY + offsetY > bounds.y) && (mouseY + offsetY < (bounds.y + bounds.h)))
		{
			state = GuiControlState::FOCUSED;
			if (isPlaying == false)
			{
				app->audio->PlayFx(onHoverFX);
				isPlaying = true;
			}
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				state = GuiControlState::PRESSED;
				app->audio->PlayFx(onClickFX);
			}

			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				ret = NotifyObserver();
			}
		}
		else if(state != GuiControlState::NONE) 
		{
			state = GuiControlState::NORMAL;
			isPlaying = false;
		}
	}

	return ret;
}

bool GuiButton::Draw(Render* render, SDL_Texture* tex)
{

	// Draw the right button depending on state

	

	switch (state)
	{
	case GuiControlState::DISABLED:
	{
		render->DrawTexture(tex, bounds.x, bounds.y, &disabled.GetCurrentFrame());
	} break;

	case GuiControlState::NORMAL:
	{
		render->DrawTexture(tex, bounds.x, bounds.y, &normal.GetCurrentFrame());

	} break;
	case GuiControlState::FOCUSED:
	{
		render->DrawTexture(tex, bounds.x, bounds.y, &focused.GetCurrentFrame());
	} break;
	case GuiControlState::PRESSED:
	{
		render->DrawTexture(tex, bounds.x, bounds.y, &pressed.GetCurrentFrame());
	} break;
	default:
		break;
	}
	
	if (app->debug)
	{
		switch (state)
		{

		case GuiControlState::DISABLED:
		{
			render->DrawRectangle(bounds, 0, 0, 0, 0);
		} break;

		case GuiControlState::NORMAL:
		{
			render->DrawRectangle(bounds, 255, 0, 0, 255);

		} break;

		//L14: TODO 4: Draw the button according the GuiControl State
		case GuiControlState::FOCUSED:
		{
			render->DrawRectangle(bounds, 255, 255, 255, 160);
		} break;
		case GuiControlState::PRESSED:
		{
			render->DrawRectangle(bounds, 255, 255, 255, 255);
		} break;
		case GuiControlState::SELECTED: render->DrawRectangle(bounds, 0, 255, 0, 255);
			break;

		default:
			break;
		}
	}

	return true;
}