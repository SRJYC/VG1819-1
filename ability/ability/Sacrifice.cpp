#include "ability/ability/Ability.h"
#include "unit/Unit.h"

//Rock

namespace ability
{
	int Sacrifice::effect(AbilityInfoPackage* p_info)
	{
		//get lv
		int lv = p_info->m_source->m_attributes[UNIT_LV];
		//if it's lv3, draw a card as addition
		if (lv >= 3)
		{
			//get id
			int id = p_info->m_sourceClientId;

			//get card num
			int num = p_info->m_intValue[CARD_DRAWN];

			//trigger card draw event
			drawCard(id, num);
		}

		//get all cards in hand
		std::vector<kitten::K_GameObject*> cards = getCardsInHand();

		//there must be cards in hand, so it can reduce cost
		if (cards.size() > 0)
		{
			//get a random card(unit)
			int index = rand() % cards.size();
			unit::Unit* target = cards[index]->getComponent<unit::Unit>();

			//get power
			int pow = -(p_info->m_intValue[UNIT_POWER]);

			//reduce that unit's cost
			AbilityNode* node = ability::AbilityNodeManager::getInstance()->findNode(ChangeAttribute);
			node->effect(target, UNIT_COST, pow);
		}

		UniversalSounds::playSound("downer");

		//destroy itself
		p_info->m_source->destroy();

		//done(p_info);
		delete p_info;

		return 0;
	}
}