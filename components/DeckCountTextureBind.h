#pragma once 
#include "kitten/K_Component.h"
#include "kitten/event_system/Event.h"
#include "puppy\Text\TextBox.h"

#include <map>
#include <string>

class DeckCountTextureBind : public kitten::K_Component
{
protected:	
	int m_deckCount = 1000;

	puppy::TextBox* m_countText;
	puppy::TextBox* m_enemyCountText;
	std::map<int,std::string> m_texPairs;
	std::map<int,std::string>::reverse_iterator m_currentTexPair;
	void deckEventReceiver(kitten::Event::EventType p_type, kitten::Event* p_data);
	void enemyDrawEventReceiver(kitten::Event::EventType p_type, kitten::Event* p_data);
public:
	void start();

	void addTexPair(int p_atCount, const std::string& p_tex);

	DeckCountTextureBind();
	~DeckCountTextureBind();
};