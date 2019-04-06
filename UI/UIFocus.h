#include "kitten\K_GameObject.h"

namespace userinterface
{
	class UIFocus
	{
	public:
		void createInstance();
		UIFocus* getInstance();
		~UIFocus();
	private:
		UIFocus();
		static UIFocus* m_instance;

	};
}