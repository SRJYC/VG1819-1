#pragma once
#include "kitten\K_Component.h"
#include "_Project\StringInputDisplay.h"
#include "UI\ClickableButton.h"
#include "kitten\mouse picking\ClickableFrame.h"

class PlayerNameController : public kitten::K_Component
{
private:
	puppy::TextBox* m_inputTextbox;
	puppy::TextBox* m_nameStatusTextBox;
	StringInputDisplay* m_stringInputDisplay;
	userinterface::ClickableButton* m_confirmButton;
	kitten::ClickableFrame* m_confirmButtonFrame;

	std::string m_playerName = "";
	int m_nameMaxLimit;
	int m_nameMinLimit;

	bool m_update;

	virtual void start() override;
	virtual void onEnabled() override;
	virtual void onDisabled() override;
	virtual bool hasUpdate() const override { return true; };
	virtual void update() override;

	void sceneChangeListener(kitten::Event::EventType p_type, kitten::Event* p_event);
	void setConfirmButtonEnabled(bool p_enabled);
public:
	PlayerNameController(int p_nameMinLimit, int p_nameMaxLimit);
	~PlayerNameController();

	void changePlayerName();
	void confirmPlayerName();
	const std::string& getCurrentPlayerName() const { return m_playerName; }
	void setUpdate(bool p_update) { m_update = p_update; }
};
