#include "SMTags.h"

namespace SMTags::UI::Layers
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Frontend, "UI.Layers.Frontend", "Frontend UI tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Game, "UI.Layers.Game", "Game UI tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Notify, "UI.Layers.Notify", "Notify UI tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Popup, "UI.Layers.Popup", "Popup UI tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Menu, "UI.Layers.Menu", "Menu UI tag")
}

namespace SMTags::UI::Navigation::MainMenu
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Play, "UI.Navigation.MainMenu.Play", "Navigation for MainMenu - Play")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Settings, "UI.Navigation.MainMenu.Settings", "Navigation for MainMenu - Settings")
}