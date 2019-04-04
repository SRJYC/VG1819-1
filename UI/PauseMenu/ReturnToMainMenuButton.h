#pragma once

#include "UI\ClickableButton.h"

namespace userinterface
{

	class ReturnToMainMenuButton : public ClickableButton
	{

	public:
		ReturnToMainMenuButton();
		~ReturnToMainMenuButton();

		virtual void onClick() override;
	};

}