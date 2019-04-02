// ConnectToHost
// 
// Simplified version of NetworkingConsoleMenu, used with the IP
// entering screen, with connecting to the host as the only function
//
// Note: This was created to isolate a bug with clicking instead of pressing
// enter to confirm an IP address, but still using this as the IP screen doesn't 
// need everything that NetworkConsoleMenu provides
//
// @Ken

#include "networking\ConnectToHost.h"
#include "kitten\K_Instance.h"
#include "networking\ClientGame.h"

#define NO_GAME_DETECTED_MSG "No Game Found"
#define GAME_DETECTED_MSG "Game Found!"
#define LOST_CONNECTION_MSG "Lost Connection"

ConnectToHost::ConnectToHost()
	:
	m_bConnect(false),
	m_bLoadingMsgEnabled(false),
	m_bJoiningGame(false),
	m_loadingMessage(nullptr),
	m_bQuickplay(false)
{
	
}

ConnectToHost::~ConnectToHost()
{
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Join_Direct_Address, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Poll_For_Server, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Join_Localhost, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Network_End_Game, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::Quickplay, this);
	kitten::EventManager::getInstance()->removeListener(kitten::Event::EventType::P2P_Start_Game, this);
	m_inputMan->setPollMode(true);

	if (m_bQuickplay && networking::ClientGame::isNetworkValid())
	{
		networking::ClientGame::getInstance()->disconnectFromNetwork();
	}
	else if (!m_bJoiningGame && networking::ClientGame::isNetworkValid())
	{
		networking::ClientGame::getInstance()->disconnectFromNetwork();
		networking::ClientGame::destroyInstance();
	}
}

void ConnectToHost::start()
{
	m_ipInputTextBox = m_attachedObject->getComponent<puppy::TextBox>();
	assert(m_ipInputTextBox != nullptr);

	m_stringInputDisplay = m_attachedObject->getComponent<StringInputDisplay>();
	assert(m_stringInputDisplay != nullptr);

	m_inputMan = input::InputManager::getInstance();
	assert(m_inputMan != nullptr);

	// Add Listeners
	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Join_Direct_Address,
		this,
		std::bind(&ConnectToHost::joinDirectAddressListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Poll_For_Server,
		this,
		std::bind(&ConnectToHost::pollForLocalhostListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Join_Localhost,
		this,
		std::bind(&ConnectToHost::joinLocalhostListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Quickplay,
		this,
		std::bind(&ConnectToHost::quickplayListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::Network_End_Game,
		this,
		std::bind(&ConnectToHost::lostConnectionListener, this, std::placeholders::_1, std::placeholders::_2));

	kitten::EventManager::getInstance()->addListener(
		kitten::Event::EventType::P2P_Start_Game,
		this,
		std::bind(&ConnectToHost::startGameListener, this, std::placeholders::_1, std::placeholders::_2));

	// Create loading message game object to display when directly connecting to a host
	auto goMan = kitten::K_GameObjectManager::getInstance();
	m_loadingMessage = goMan->createNewGameObject("UI/loading_screen.json");
	m_loadingMessage->setEnabled(false);

	// Textbox to display info when polling for a host locally
	m_localHostTextBox = goMan->createNewGameObject("network_menu/join_screen_textbox.json")->getComponent<puppy::TextBox>();
	m_localHostTextBox->setText("");

	// Disable polling so the player can immediately start typing an address
	m_inputMan->setPollMode(false);

	// Poll localhost to see if someone is hosting
	pollForLocalhost();
}

void ConnectToHost::update()
{
	// Flag setup so that K_Instance can go through an update loop once to actually display the loading message
	// before we attempt to connect
	// If coroutines get working again, this shouldn't be needed
	if (m_bConnect)
	{
		m_bConnect = false;
		joinDirectAddress();
	}

	if (m_bLoadingMsgEnabled)
	{
		m_bLoadingMsgEnabled = false;
		m_bConnect = true;
	}

	if (m_inputMan->keyDown(GLFW_KEY_ENTER) && !m_inputMan->keyDownLast(GLFW_KEY_ENTER))
	{
		m_bLoadingMsgEnabled = true;
		m_loadingMessage->setEnabled(true);
	}

	// Update ClientGame if there is an instance
	if (networking::ClientGame::getInstance() != nullptr && networking::ClientGame::isNetworkValid() && !m_bJoiningGame)
	{
		networking::ClientGame::getInstance()->update();
	}
}

void ConnectToHost::joinDirectAddressListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	m_loadingMessage->setEnabled(true);
	m_bLoadingMsgEnabled = true;
}

void ConnectToHost::joinDirectAddress()
{
	std::string address = m_stringInputDisplay->getString();

	networking::ClientGame* client = networking::ClientGame::getInstance();

	// Check if we've already created an instance of ClientGame
	if (client)
	{
		// Check if we're already polled for a host and disconnect from it if we have
		if (networking::ClientGame::isNetworkValid())
		{
			client->disconnectFromNetwork();
		}
		client->setupNetwork(address);
	} 
	else // If not, get address and create ClientGame instance
	{
		networking::ClientGame::createInstance(address);
	}

	if (networking::ClientGame::isNetworkValid())
	{
		m_ipInputTextBox->setText("Joined host");
		networking::ClientGame::getInstance()->sendBasicPacket(JOIN_GAME);
	} 
	else
	{
		m_ipInputTextBox->setText("Network Error");
		m_inputMan->setPollMode(false);
		m_loadingMessage->setEnabled(false);
		m_bLoadingMsgEnabled = false;
	}

	m_bConnect = false;
}

void ConnectToHost::pollForLocalhostListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	pollForLocalhost();
}

void ConnectToHost::pollForLocalhost()
{
	networking::ClientGame* client = networking::ClientGame::getInstance();

	// Check if we've already created an instance of ClientGame
	if (client)
	{
		// Check if we're already connected to a host
		if (!networking::ClientGame::isNetworkValid())
		{
			client->setupNetwork("localhost");
		}
	} 
	else // If not, get address and create ClientGame instance
	{
		networking::ClientGame::createInstance("localhost");
	}

	if (networking::ClientGame::isNetworkValid())
	{
		m_localHostTextBox->setText(GAME_DETECTED_MSG);
	}
	else
	{
		m_localHostTextBox->setText(NO_GAME_DETECTED_MSG);
	}
}

void ConnectToHost::joinLocalhostListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	joinLocalhost();
}

void ConnectToHost::joinLocalhost()
{
	if (networking::ClientGame::isNetworkValid())
	{
		networking::ClientGame::getInstance()->sendBasicPacket(JOIN_GAME);
	}
	else
	{
		kitten::EventManager::getInstance()->queueEvent(kitten::Event::Remove_Loading_Screen, nullptr);
	}
}

void ConnectToHost::quickplayListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	joinDedicatedServer();
}

void ConnectToHost::joinDedicatedServer()
{
	kitten::K_Instance::changeScene("quickplay_screen.json");
	m_bQuickplay = true;
	m_bConnect = false;
}

void ConnectToHost::startGameListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	int mapId = p_event->getInt(MAP_ID_KEY);
	startGame();
}

void ConnectToHost::startGame()
{
	m_bJoiningGame = true;
	kitten::K_Instance::changeScene("mainscene.json");
}

void ConnectToHost::lostConnectionListener(kitten::Event::EventType p_type, kitten::Event* p_event)
{
	int gameResult = p_event->getInt(GAME_END_RESULT);
	if (gameResult == PLAYER_DISCONNECTED)
	{
		// Clean up connection
		networking::ClientGame::getInstance()->disconnectFromNetwork(true);
		m_localHostTextBox->setText(LOST_CONNECTION_MSG);
	}
}