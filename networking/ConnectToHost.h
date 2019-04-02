#pragma once
#include "kitten\K_Component.h"
#include "kitten\event_system\EventManager.h"
#include "_Project\StringInputDisplay.h"
#include "kitten\InputManager.h"

class ConnectToHost : public kitten::K_Component
{
private:
	puppy::TextBox* m_ipInputTextBox;
	puppy::TextBox* m_localHostTextBox;
	StringInputDisplay* m_stringInputDisplay;
	input::InputManager* m_inputMan;
	kitten::K_GameObject* m_loadingMessage;

	bool m_bConnect;
	bool m_bLoadingMsgEnabled;
	bool m_bJoiningGame;
	bool m_bQuickplay;
public:
	ConnectToHost();
	~ConnectToHost();

	virtual void start() override;

	virtual bool hasUpdate() const override { return true; };
	virtual void update() override;
	
	void joinDirectAddressListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void joinDirectAddress();
	void pollForLocalhostListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void pollForLocalhost();
	void joinLocalhostListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void joinLocalhost();
	void quickplayListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void joinDedicatedServer();
	void startGameListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void startGame();	
	void lostConnectionListener(kitten::Event::EventType p_type, kitten::Event* p_event);
};
