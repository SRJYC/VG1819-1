#include "DisplayFrame.h"
#include "kitten/K_GameObjectManager.h"
#include <math.h>
#include "CustomDataComponent.h"
#include "components/DecksDisplay/DisplayFrameSetChangeOnClick.h"
#include "components/DecksDisplay/DisplayFramePickerOnClick.h"
#include "UI/ClickableButton.h"

#define SIZE_X "sizex"
#define SIZE_Y "sizey"

DisplayFrame::DisplayFrame(int p_marginX, int p_marginY, const std::string& p_displayObject, const std::string& p_arrowLeft, std::string& p_arrowRight, const std::string& p_highlight, const std::string p_empty)
	: m_marginX(p_marginX), m_marginY(p_marginY), m_displayObject(p_displayObject), m_arrowFileLeft(p_arrowLeft), m_arrowFileRight(p_arrowRight), m_highlightFile(p_highlight), m_emptyFile(p_empty)
{

}
DisplayFrame::~DisplayFrame() 
{

}

void DisplayFrame::start()
{
	// Set up all base gameobjects
	m_objectsToDisplay.resize(1);
	m_objectsToDisplay[0] = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_displayObject); // Remember that the scale of the whole object will be reset to 1,1. It's scale will be used to see how huge each object will be like
	m_objectsToDisplay[0]->getComponent<DisplayFramePickerOnClick>()->setParentDisplayFrame(this);
	m_arrows[0] = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_arrowFileLeft);
	m_arrows[0]->getComponent<DisplayFrameSetChangeOnClick>()->setParentDisplayFrame(this);
	m_arrows[1] = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_arrowFileRight);
	m_arrows[1]->getComponent<DisplayFrameSetChangeOnClick>()->setParentDisplayFrame(this);
	m_highlight = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_highlightFile); 
	m_highlightOffsetX = m_highlight->getTransform().getTranslation().x;
	m_highlightOffsetY = m_highlight->getTransform().getTranslation().y;
	m_empty = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_emptyFile); // Make sure whatever image is pivoted in the center for proper placement

	// Set up variables 
	const glm::vec2 &displayFrameScale = this->m_attachedObject->getTransform().getScale2D(), // The scale of the frame itself to see how big it is
		&arrowButtons = m_arrows[0]->getTransform().getScale2D(), // get the scales of  the arrows, to check how big those are
		&objectScale = m_objectsToDisplay[0]->getTransform().getScale2D();
	const glm::vec3 &displayFrameTrans = m_attachedObject->getTransform().getTranslation();

	// Calculate how many objects can fit into our frame
	int fittableX = (int)((displayFrameScale.x) / (objectScale.x + 2 * m_marginX)), // Formula: Usable Space/ Needed Space.  X: available width / (object width + margins) 
		fittableY = (int)((displayFrameScale.y) / (objectScale.y + 2 * m_marginY)); // Y: (available height) / (object height + margins) 

	// resize object vector to hold maximum possible to display
	this->m_objectsToDisplay.resize(std::fmax(fittableX*fittableY, 1));
	// Go through children. P.S. this is because frames go invisible for some reason
	for (kitten::Transform* child : m_objectsToDisplay[0]->getTransform().getChildren())
	{
		child->setIgnoreParent(true);
		child->place2D(child->getTranslation().x + displayFrameTrans.x + m_marginX,
			child->getTranslation().y + displayFrameTrans.y + displayFrameScale.y - m_marginY - objectScale.y);
	}
	m_objectsToDisplay[0]->getTransform().place2D(displayFrameTrans.x + m_marginX, displayFrameTrans.y + displayFrameScale.y - m_marginY - objectScale.y); // remember, based on bottom left pivot

	switch (m_arrowPlacement)
	{// Add here the different cases of placement of the arrows. 
	case 0: { // Places arrows on horizontal sides to the left and right of the display frame
		m_arrows[0]->getTransform().place2D(displayFrameTrans.x - m_marginX - arrowButtons.x, displayFrameTrans.y + displayFrameScale.y / 2); // left
		m_arrows[1]->getTransform().place2D(displayFrameTrans.x + displayFrameScale.x + m_marginX, displayFrameTrans.y + displayFrameScale.y / 2); // right
		break;
	}
	case 1: { // Places arrows on the vertical sides to the top and bottom of the display frame
		m_arrows[0]->getTransform().place2D(displayFrameTrans.x + displayFrameScale.x / 2 - arrowButtons.x / 2
			, displayFrameTrans.y + displayFrameScale.y + m_marginY);// Top
		m_arrows[1]->getTransform().place2D(displayFrameTrans.x + displayFrameScale.x / 2 - arrowButtons.x / 2
			, displayFrameTrans.y - arrowButtons.y - m_marginY); // Bottom
		break;
	}
	}

	for (int i = 1; i < m_objectsToDisplay.size(); ++i)
	{
		m_objectsToDisplay[i] = kitten::K_GameObjectManager::getInstance()->createNewGameObject(m_displayObject);
		m_objectsToDisplay[i]->getComponent<DisplayFramePickerOnClick>()->setParentDisplayFrame(this);

		// Go through children. P.S. this is because frames go invisible for some reason
		for (kitten::Transform* child : m_objectsToDisplay[i]->getTransform().getChildren())
		{
			child->setIgnoreParent(true);
			child->place2D(child->getTranslation().x + displayFrameTrans.x + (i%fittableX)*(objectScale.x + 2 * m_marginX) + m_marginX,
				child->getTranslation().y + displayFrameTrans.y + displayFrameScale.y - (i / fittableX)*(objectScale.y + 2 * m_marginY) - m_marginY - objectScale.y
			);
		}

		m_objectsToDisplay[i]->getTransform().place2D(
			displayFrameTrans.x + (i%fittableX)*(objectScale.x + 2 * m_marginX) + m_marginX,
			displayFrameTrans.y + displayFrameScale.y - (i / fittableX)*(objectScale.y + 2 * m_marginY) - m_marginY - objectScale.y
		);

	}

	m_empty->getTransform().place2D(
		displayFrameTrans.x+displayFrameScale.x/2 + m_empty->getTransform().getTranslation().x - m_empty->getTransform().getScale().x / 2,
		displayFrameTrans.y+displayFrameScale.y/2 + m_empty->getTransform().getTranslation().y - m_empty->getTransform().getScale().y / 2
	);

	if (m_currentPick >= 0)
	{
		m_currentSet = m_currentPick / m_objectsToDisplay.size();
	}

	updateDisplay();
}


void DisplayFrame::offsetCurrentSet(const int p_offset)
{
	if ((m_currentSet + p_offset)*m_objectsToDisplay.size() >= getTargetAvailable() || // checks if its going over the max
		m_currentSet + p_offset < 0)												   // checks if its going underneath minimum
		return;
	this->m_currentSet += p_offset;
	updateDisplay();
}


void DisplayFrame::updateDisplay()
{
	m_currentActive = fmin(getTargetAvailable() - (m_currentSet*m_objectsToDisplay.size()), m_objectsToDisplay.size());

	for (int i = 0; i < m_currentActive; ++i)
	{
		m_objectsToDisplay[i]->setEnabled(true);
		updateIndividualDisplayObject(i);
	}
	for (int i = m_currentActive; i < m_objectsToDisplay.size();++i)
	{
		m_objectsToDisplay[i]->setEnabled(false);
	}

	if (m_currentSet == 0) m_arrows[0]->getComponent<userinterface::ClickableButton>()->setActive(false);
	else m_arrows[0]->getComponent<userinterface::ClickableButton>()->setActive(true);

	if (m_objectsToDisplay.size() * (m_currentSet + 1) >= getTargetAvailable())
		m_arrows[1]->getComponent<userinterface::ClickableButton>()->setActive(false);
	else m_arrows[1]->getComponent<userinterface::ClickableButton>()->setActive(true);

	if (getTargetAvailable() == 0)
	{
		m_empty->setEnabled(true);
	}
	else
	{
		m_empty->setEnabled(false);
	}

	updateHighlight();
}

void DisplayFrame::updateHighlight()
{
	if (m_currentPick >= 0 && m_currentPick >= m_currentSet * m_objectsToDisplay.size() && m_currentPick < (m_currentSet + 1) * m_objectsToDisplay.size())
	{
		m_highlight->setEnabled(true);
		m_highlight->getTransform().place2D(
			m_objectsToDisplay[m_currentPick%m_objectsToDisplay.size()]->getTransform().getTranslation()[0] + m_highlightOffsetX,
			m_objectsToDisplay[m_currentPick%m_objectsToDisplay.size()]->getTransform().getTranslation()[1] +
			m_objectsToDisplay[m_currentPick%m_objectsToDisplay.size()]->getTransform().getScale2D().y +
			-m_highlight->getTransform().getScale2D().y +
			-m_highlightOffsetY
		);
	}
	else
	{
		m_highlight->setEnabled(false);
	}
}

void DisplayFrame::onDisabled()
{
	for (auto slot : m_objectsToDisplay)
	{
		slot->setEnabled(false);
	}
	for (auto arrow : m_arrows)
	{
		arrow->setEnabled(false);
	}
}

void DisplayFrame::onEnabled()
{
	for (int index = 0; index < m_currentActive; ++index)
	{
		m_objectsToDisplay[index]->setEnabled(true);
	}
	for (auto arrow : m_arrows)
	{
		arrow->setEnabled(true);
	}

	if (m_currentSet == 0) m_arrows[0]->getComponent<userinterface::ClickableButton>()->setActive(false);
	else m_arrows[0]->getComponent<userinterface::ClickableButton>()->setActive(true);

	if (m_currentActive != m_objectsToDisplay.size() &&
		m_objectsToDisplay.size() * (m_currentSet + 1) > getTargetAvailable())
		m_arrows[1]->getComponent<userinterface::ClickableButton>()->setActive(false);
	else m_arrows[1]->getComponent<userinterface::ClickableButton>()->setActive(true);
}

void DisplayFrame::registerEvent()
{
}

void DisplayFrame::deregisterEvent()
{
}

void DisplayFrame::listenEvent(kitten::Event::EventType p_type, kitten::Event * p_data)
{
}

const int& DisplayFrame::getCurrentPickedItemIndex() const {
	return m_currentPick;
}

void DisplayFrame::pickDisplayedObject(const kitten::K_GameObject* p_gameObject) {
	m_currentPick = std::find(m_objectsToDisplay.begin(), m_objectsToDisplay.end(), p_gameObject) - m_objectsToDisplay.begin() + (m_currentSet*m_objectsToDisplay.size());
	updateHighlight();
	onObjectClicked(m_currentPick);
}

int DisplayFrame::getItemIndexTiedTo(const kitten::K_GameObject* p_gameObject) {
	return std::find(m_objectsToDisplay.begin(), m_objectsToDisplay.end(), p_gameObject) - m_objectsToDisplay.begin() + (m_currentSet*m_objectsToDisplay.size());
}

void DisplayFrame::resetCurrentPick()
{
	m_currentPick = -1;
	updateHighlight();
}
