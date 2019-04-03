/*@Rock
Apr 3
*/

#pragma once
#include "kitten/K_Common.h"
#include "UI/UIObject.h"
#include "puppy/Text/TextBox.h"
#include "kitten/event_system/EventManager.h"

class ModeDetailController : public kitten::K_Component
{
public:
	ModeDetailController();
	~ModeDetailController();

	void start() override;

private:
	//the text box that explains each mode tile
	puppy::TextBox* m_modeTextBox;

	//the list of buttons of each mode tile
	std::vector<kitten::K_GameObject*> m_modeButtonList;

	//the file path of mode explain
	const std::string m_modeData;
};