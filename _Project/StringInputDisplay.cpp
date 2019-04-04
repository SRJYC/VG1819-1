#include "StringInputDisplay.h"
#include "kitten\K_GameObject.h"

StringInputDisplay::StringInputDisplay(unsigned int p_limit) : m_textBox(nullptr), m_strEnteredString(""),
	m_charLimit(p_limit)
{

}

StringInputDisplay::~StringInputDisplay()
{
	input::InputManager::getInstance()->removeStringListener(this);
}

void StringInputDisplay::start()
{
	m_textBox = m_attachedObject->getComponent<puppy::TextBox>();
	
	assert(m_textBox != nullptr);
	//m_textBox->setText("");

	input::InputManager::getInstance()->addStringListener(this);
}

void StringInputDisplay::onStringFinished(const std::string& p_string)
{
	std::string string = p_string;
	if (m_charLimit > 0 && p_string.length() > m_charLimit)
	{
		string = p_string.substr(0, m_charLimit);
	}
	m_textBox->setText("");
	m_strEnteredString = string;
}

void StringInputDisplay::onStringChanged(const std::string& p_string)
{
	std::string string = p_string;
	if (m_charLimit > 0 && p_string.length() > m_charLimit)
	{
		string = p_string.substr(0, m_charLimit);
	}
	m_textBox->setText(string);
	m_strEnteredString = string;
}

void StringInputDisplay::setCharLimit(unsigned int p_limit)
{
	m_charLimit = p_limit;

	if (m_strEnteredString.length() > m_charLimit)
	{
		m_strEnteredString = m_strEnteredString.substr(p_limit);
	}
}