#include "ModeDetailController.h"
#include "json.hpp"
#include "kibble\json\jsonextra.hpp"
#include "kitten/K_GameObjectManager.h"
#include "ChangeModeDetailOnClick.h"

ModeDetailController::ModeDetailController(const std::string & p_modeData, const std::string & p_buttonData, float p_x, float p_y, float p_offsetX, float p_offsetY, int p_numPerRow)
	:m_modeData(p_modeData),m_buttonData(p_buttonData),
	m_startX(p_x),m_startY(p_y),m_offsetX(p_offsetX),m_offsetY(p_offsetY),
	m_limitPerRow(p_numPerRow)
{
}

ModeDetailController::~ModeDetailController()
{
}

void ModeDetailController::start()
{
	readData();

	//get text box
	m_modeTextBox = m_attachedObject->getComponent<puppy::TextBox>();

	//create button
	for (int i = 0; i < m_imgList.size(); i++)
	{
		//the row
		int col = i % m_limitPerRow;
		//the col
		int row = i / m_limitPerRow;

		//create game object
		kitten::K_GameObject* go = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_buttonData);

		//add to list
		m_modeButtonList.push_back(go);

		//get component
		ChangeModeDetailOnClick* oc = go->getComponent<ChangeModeDetailOnClick>();

		//set controller
		oc->setController(this);

		//set img
		oc->setInfo(m_imgList[i], i);

		//set to child
		go->getTransform().setIgnoreParent(false);
		go->getTransform().setParent(&getTransform());

		//move position
		go->getTransform().place2D(m_startX + col * m_offsetX, m_startY - row * m_offsetY);
	}
}

void ModeDetailController::changeDescription(int p_id)
{
	if (m_currentId != p_id)
	{
		m_modeTextBox->setText(m_desList[p_id]);
		m_currentId = p_id;
	}
}

void ModeDetailController::readData()
{
	nlohmann::json masterFile = jsonIn(m_modeData);
	auto found = masterFile.find("Info");
	if (found != masterFile.end())
	{
		auto end = masterFile["Info"].end();
		for (nlohmann::json::iterator it = masterFile["Info"].begin(); it != end; ++it)
		{
			m_imgList.push_back(it->operator[]("image"));
			m_desList.push_back(it->operator[]("description"));
		}
	}
}
