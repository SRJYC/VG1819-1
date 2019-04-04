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
	ModeDetailController(const std::string& p_modeData, const std::string& p_buttonData,
		float p_x,float p_y, float p_offsetX, float p_offsetY, int p_numPerRow);
	~ModeDetailController();

	void start() override;

	void changeDescription(int p_id);

private:
	//the text box that explains each mode tile
	puppy::TextBox* m_modeTextBox;

	//the list of buttons of each mode tile
	std::vector<kitten::K_GameObject*> m_modeButtonList;
	//button data
	const std::string m_buttonData;

	//data of mode explain
	const std::string m_modeData;
	std::vector<std::string> m_imgList;
	std::vector<std::string> m_desList;

	int m_currentId;

	//pos of button
	const float m_startX;
	const float m_startY;
	const float m_offsetX;
	const float m_offsetY;
	const int m_limitPerRow;

	void readData();
};