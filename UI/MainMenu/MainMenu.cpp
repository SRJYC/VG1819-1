#include "UI\MainMenu\MainMenu.h"
#include "kitten\K_GameObjectManager.h"

#define MAINSCENE_PATH "mainscene.json"
#define TESTINGSCENE_PATH "testinggrounds.json"
#define AI_VS_AI_PATH "AI_vs_AI.json"
#define PLAYER_VS_AI_PATH "Player_vs_AI.json"
#define MAINMENU_PATH "mainmenu.json"
#define DECKBUILDER_PATH "deck-builder-deck_selection_screen.json"

kitten::K_GameObject* MainMenu::sm_menuBottom = nullptr;
kitten::K_GameObject* MainMenu::sm_menuTop = nullptr;

MainMenu::MainMenu()
{
	
}

MainMenu::~MainMenu()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Scene_Change, this);
}

void MainMenu::start()
{
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Scene_Change,
		this,
		std::bind(&MainMenu::sceneChangeListener, this, std::placeholders::_1, std::placeholders::_2));

	if (sm_menuBottom == nullptr)
	{
		sm_menuBottom = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/main_menu/menu_bottom.json");
		kitten::K_GameObjectManager::getInstance()->flagGameObjectToSurvive(sm_menuBottom);
	}
	if (sm_menuTop == nullptr)
	{
		sm_menuTop = kitten::K_GameObjectManager::getInstance()->createNewGameObject("UI/main_menu/menu_top.json");
		kitten::K_GameObjectManager::getInstance()->flagGameObjectToSurvive(sm_menuTop);
	}
	m_menuTopController = sm_menuTop->getComponent<LerpController>();
}

void MainMenu::sceneChangeListener(kitten::Event::EventType p_type, kitten::Event* p_data)
{
	std::string scenePath = p_data->getString(NEXT_SCENE_PATH_KEY);

	if (scenePath == MAINSCENE_PATH || scenePath == TESTINGSCENE_PATH 
		|| scenePath == AI_VS_AI_PATH || scenePath == PLAYER_VS_AI_PATH)
	{
		kitten::K_GameObjectManager::getInstance()->destroyGameObject(sm_menuBottom);
		kitten::K_GameObjectManager::getInstance()->destroyGameObject(sm_menuTop);
		sm_menuBottom = nullptr;
		sm_menuTop = nullptr;
	}
	else if (scenePath == MAINMENU_PATH)
	{
		int width, height;
		glfwGetWindowSize(&width, &height);
		glm::vec2 scale2d = sm_menuTop->getTransform().getScale2D();
		glm::vec3 translation = sm_menuTop->getTransform().getTranslation();
		m_menuTopController->positionLerp(glm::vec3(0.0f, height - scale2d.y, translation.z), MENU_LERP_TIME);
	}
	else if (scenePath == DECKBUILDER_PATH)
	{
		int width, height;
		glfwGetWindowSize(&width, &height);
		glm::vec2 scale2d = sm_menuTop->getTransform().getScale2D();
		glm::vec3 translation = sm_menuTop->getTransform().getTranslation();
		m_menuTopController->positionLerp(glm::vec3(0.0f, height + scale2d.y, translation.z), MENU_LERP_TIME);
	}
}
