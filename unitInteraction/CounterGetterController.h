/*@Rock 1.9
This class control all the textbox and respond to button click
*/

#pragma once
#include "kitten/K_Common.h"
#include "puppy/Text/TextBox.h"


class CounterGetter;
class CounterGetterController : public kitten::K_Component
{
public:
	CounterGetterController();
	~CounterGetterController();

	void setGetter(CounterGetter* p_g);
	void start();

	void set(const std::string& p_name, int p_min, int p_max);

	void increase();//increase current by 1
	void decrease();//decrease current by 1
	void check();//pass current to counter getter
	void cancel();

	void onEnabled() override;
	void onDisabled() override;
private:
	int m_min;
	int m_max;
	int m_current;
	std::string m_counter;

	kitten::K_GameObject* m_maxGO;//game object handles "max limit" text box
	kitten::K_GameObject* m_minGO;//game object handles "min limit" text box
	kitten::K_GameObject* m_counterGO;//game object handles "counter name" text box
	kitten::K_GameObject* m_currentGO;//game object handles "current counter" text box
	
#define SLIDER_RANGE 430
#define SLIDER_START 400
	kitten::K_GameObject* m_sliderButtonGO;

	std::vector<kitten::K_GameObject*> m_ButtonGO;//game objects handles 4 buttons

	CounterGetter* m_getter;

	void addThisToButton();//give reference of this to those buttons
	void setCurrentNumber();
};