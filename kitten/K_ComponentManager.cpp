#include "K_ComponentManager.h"
#include "K_Component.h"
#include "K_GameObject.h"

#include "Camera.h"
#include "CubeRenderable.h"
#include "QuadRenderable.h"
#include "_Project\MoveByMouseRightClickDrag.h"
#include "_Project\ZoomByMouseWheel.h"
#include "_Project\DebugPrintOnce.h"
#include "gameworld\GrassLandInfoComponent.h"
#include "_Project\DestroyOnClick.h"
#include "unit/unitComponent/UnitGraphic.h"
#include "_Project\UseAbilityWhenClicked.h"
#include "_Project\FPSCalc.h"
#include "puppy\Text\TextBox.h"
#include "kitten\sprites\SpriteAnimator.h"
#include "kitten\sprites\SpriteRenderable.h"
#include "unit/unitComponent/UnitMove.h"
//ui
#include "UI\UIObject.h"
#include "UI\CardUIO.h"
#include "UI\HandFrame.h"
#include "UI\ClickableCard.h"
#include "UI\ContextMenu.h"
#include "UI\CommanderContext.h"
#include "mouse picking/ClickableUI.h"
#include "networking\menu\NetworkHostButton.h"
#include "networking\menu\NetworkJoinButton.h"
#include "_Project\StringInputDisplay.h"

#include "unit/InitiativeTracker/TrackerBlock.h"
#include "unit/InitiativeTracker/TrackerBlockClickable.h"
#include "unit/InitiativeTracker/TrackerPointer.h"
#include "unit/InitiativeTracker/PointerUI.h"

#include "components\PowerTracker.h"
#include "components\SelectAbility.h"
#include "networking\SpawnUnitOnKeyPress.h"
#include "networking\NetworkingConsoleMenu.h"
#include "UI\TabMenu\TabMenu.h"
#include "UI\TabMenu\ReturnToMainMenuButton.h"
#include "_Project\StringInputDisplay.h"
#include "kibble/json/Datatypes/ComponentDataType.hpp"

#include "components/ChangeSceneOnClick.hpp"
#include "kitten\K_ParticleSystem.h"
#include "kitten\BarRenderable.h"
#include "_Project\LerpController.h"

//board
#include "board/component/Highlighter.h"
#include "board/component/BoardCreator.h"
//clickable
#include "board/clickable/ManipulateTileOnClick.h"
#include "board/clickable/PrintWhenClicked.h"
#include "board/clickable/SendSelfOnClick.h"
//tile
#include "board/tile/TileInfo.h"

#include "unitInteraction/CounterGetterDisplay.h"

namespace kitten
{
	K_ComponentManager* K_ComponentManager::sm_instance = nullptr;

	K_ComponentManager::K_ComponentManager()
	{

	}

	K_ComponentManager::~K_ComponentManager()
	{

	}

	//Temporary unitl Kibble is ready
	K_Component* K_ComponentManager::createComponent(const std::string& p_componentName)
	{
		K_Component* comp;

		//Kibble version -1.0
		if (p_componentName == "K_ParticleSystem") //datadriven
		{
			comp = new kitten::K_ParticleSystem("");
		}
		else if (p_componentName == "BarRenderable") //datadriven
		{
			comp = new kitten::BarRenderable();
		}
		else if (p_componentName == "LerpController")
		{
			comp = new LerpController();
		}
		else if (p_componentName == "Camera")// Datadriven
		{
			comp = new Camera();
		}
		else if (p_componentName == "TextBox")  // Datadriven
		{
			comp = new puppy::TextBox(puppy::FontTable::getInstance()->getFont("../fonts/common_consolas.fnt"), "DEFAULT TEXT", 500, 500);
		}
		else if (p_componentName == "CubeRenderable")// Datadriven
		{
			comp = new CubeRenderable("textures/tiles/MISSING.tga");
		}
		else if (p_componentName == "QuadRenderable")// Datadriven
		{
			comp = new QuadRenderable("textures/tiles/Grassland.tga");
		}
		else if (p_componentName == "StaticQuadRenderable")// QuadRenderable Variant
		{
			comp = new QuadRenderable("textures/tiles/MISSING.tga", true);
		}
		else if (p_componentName == "Grassland")// datadriven
		{
			comp = new gameworld::GrasslandInfoComponent();
		}
		else if (p_componentName == "Frame")// Datadriven
		{
			comp = new userinterface::UIFrame("textures/ui/blankFrame.tga");
		}
		else if (p_componentName == "Hand") // Datadriven
		{
			comp = new userinterface::HandFrame("textures/ui/blankFrame.tga");
		}
		else if (p_componentName == "UIObject")
		{
			comp = new userinterface::UIObject("textures/ui/blankFrame.tga");
		}
		else if (p_componentName == "Card") // Datadriven
		{
			comp = new userinterface::CardUIO("textures/ui/cardBack.tga");
		}else if(p_componentName == "ClickableFrame") // Datadriven
		{
			comp = new ClickableFrame(ClickableFrame::piv_BotLeft);
		}
		else if (p_componentName == "ClickableUI") // Datadriven
		{
			comp = new ClickableUI();
		}
		else if (p_componentName == "ClickableCard")
		{
			comp = new userinterface::ClickableCard();
		}
		else if (p_componentName == "ContextMenu")
		{
			comp = new userinterface::ContextMenu();
		}
		else if (p_componentName == "CommanderContext")
		{
			comp = new userinterface::CommanderContext();
		}
		else if (p_componentName == "MoveByMouseRightClickDrag")// Datadriven
		{
			comp = new MoveByMouseRightClickDrag(0.005f, glm::vec2(), glm::vec2());
		} else if (p_componentName == "ZoomByMouseWheel")// Datadriven
		{
			comp = new ZoomByMouseWheel(2.0f);
		} else if (p_componentName == "DebugPrintOnce") // Datadriven
		{
			comp = new DebugPrintOnce("Some Message, kinda useless until we can change this easily");
		} else if (p_componentName == "PrintWhenClicked")// Datadriven
		{
			comp = new PrintWhenClicked("I WAS CLICKED!!");
		} else if (p_componentName == "DestroyOnClick") // Datadriven
		{
			comp = new DestroyOnClick();
		} else if (p_componentName == "ClickableBox") // Datadriven
		{
			comp = new ClickableBox(glm::vec3(-0.5f, 0, -0.5f), glm::vec3(0.5f, 0, 0.5f));
		} else if (p_componentName == "ClickableBoxBox") // Datadriven
		{
			comp = new ClickableBox(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));
		} else if (p_componentName == "ClickableBoxForPointUnit") // Datadriven
		{
			comp = new ClickableBox(glm::vec3(-0.5f, -1.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		} else if (p_componentName == "ClickableBoxForCubeUnit") // Datadriven
		{
			comp = new ClickableBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.5f, 3.0f, 0.0f));
		} else if (p_componentName == "ClickableBoxForTrackerBlock") // Datadriven
		{
			comp = new ClickableBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		} else if (p_componentName == "UnitMove") // Datadriven
		{
			comp = new unit::UnitMove();
		} else if (p_componentName == "UnitGraphic")//hard code, need special function for unit graphic, Data driven with these as defaults
		{
			comp = new unit::UnitGraphic(unit::point, "textures/unit/Default.tga");
		} else if (p_componentName == "ManipulateTileOnClick") // Datadriven
		{
			comp = new ManipulateTileOnClick();
		} else if (p_componentName == "UseAbilityWhenClicked")  // Datadriven
		{
			comp = new UseAbilityWhenClicked();
		} else if (p_componentName == "SendSelfOnClick") // DataDriven
		{
			comp = new SendSelfOnClick();
		} else if (p_componentName == "FPSCalc") // Datadriven
		{
			comp = new FPSCalc();
		} else if (p_componentName == "TextBoxAbilities")
		{
			comp = new puppy::TextBox(puppy::FontTable::getInstance()->getFont("../fonts/common_consolas.fnt"), "DEFAULT TEXT", 300, 500);
		} else if (p_componentName == "TrackerBlock") // Datadriven
		{
			comp = new unit::TrackerBlock();
		} else if (p_componentName == "TrackerBlockClickable") // Datadriven
		{
			comp = new unit::TrackerBlockClickable();
		} else if (p_componentName == "TrackerPointer") // Datadriven
		{
			comp = new unit::TrackerPointer();
		} else if (p_componentName == "PointerUI") // Datadriven
		{
			comp = new userinterface::PointerUI();
		} else if (p_componentName == "SelectAbility") // Datadriven
		{
			comp = new SelectAbility();
		} else if (p_componentName == "PowerTracker") // Datadriven
		{
			comp = new PowerTracker();
		}
		else if (p_componentName == "Highlighter") // Datadriven
		{
			comp = new Highlighter();
		}
		else if (p_componentName == "BoardCreator") // Datadriven
		{
			comp = new BoardCreator();
		}
		else if (p_componentName == "SpawnUnitOnKeyPress") // Datadriven
		{
			comp = new SpawnUnitOnKeyPress();
		}
		else if (p_componentName == "SpriteRenderable") // Datadriven
		{
			comp = new SpriteRenderable();
		}
		else if (p_componentName == "SpriteAnimator") // Datadriven
		{
			comp = new sprites::SpriteAnimator("");
		}
		else if (p_componentName == "NetworkingConsoleMenu")
		{
			comp = new NetworkingConsoleMenu();
		}
		else if (p_componentName == "TileInfo") // Datadriven
		{
			comp = new TileInfo();
		}
		else if (p_componentName == "CounterGetterDisplay")
		{
			comp = new CounterGetterDisplay();
		}
		else if (p_componentName == "StringInputDisplay")
		{
			comp = new StringInputDisplay();
		}
		else if (p_componentName == "ReturnToMainMenuButton")
		{
			comp = new userinterface::ReturnToMainMenuButton();
		}
		else if (p_componentName == "TabMenu")
		{
			comp = new TabMenu("textures/ui/blankFrame.tga");
		}
		else if (p_componentName == "NetworkHostButton")
		{
			comp = new userinterface::NetworkHostButton();
		} 
		else if (p_componentName == "NetworkJoinButton") 
		{
			comp = new userinterface::NetworkJoinButton();
		}
		else
		{
			//Not found..
			assert(false);
			return nullptr;
		}

		m_toAddToStart.insert(comp);

		//Successful
		return comp;
	}

	K_Component* K_ComponentManager::createComponent(nlohmann::json* p_jsonfile)
	{
		K_Component* comp = getRelatedComponentBy(p_jsonfile);
		if (comp == nullptr) return nullptr;

		m_toAddToStart.insert(comp);

		//Successful
		return comp;
	}


	//bool mostly for debugging
	bool K_ComponentManager::destroyComponent(K_Component* p_toDestroy)
	{
		assert(p_toDestroy != nullptr);
		m_toDelete.insert(p_toDestroy);

		return true;
	}

	void K_ComponentManager::destroyComponentImmediate(K_Component* p_toDestroy)
	{
		if (p_toDestroy->hasUpdate()) //&& isActive
		{
			removeFromUpdate(p_toDestroy);
		}

		if (!p_toDestroy->m_hasStarted)
		{
			removeFromStart(p_toDestroy);
		}

		p_toDestroy->m_attachedObject->removeComponent(p_toDestroy);

		delete (p_toDestroy);
	}

	void K_ComponentManager::addToUpdate(K_Component* p_toAdd)
	{
		m_toUpdate.insert(p_toAdd);
	}

	bool K_ComponentManager::removeFromUpdate(K_Component* p_toRemove)
	{
		//find in update list and remove
		m_toUpdate.erase(p_toRemove);

		//remove anything queued to add to update list
		m_toAddToUpdate.remove(p_toRemove);

		return true;
	}

	void K_ComponentManager::addToStart(K_Component* p_toStart)
	{
		assert(!p_toStart->m_hasStarted);
		assert(m_toStart.find(p_toStart) == m_toStart.end());

		m_toAddToStart.insert(p_toStart);
	}

	void K_ComponentManager::removeFromStart(K_Component* p_toRemove)
	{
		m_toAddToStart.erase(p_toRemove);
	}

	void K_ComponentManager::queueAddToUpdate(K_Component* p_toAdd)
	{
		m_toAddToUpdate.push_back(p_toAdd);
	}

	void K_ComponentManager::queueRemovalFromUpdate(K_Component* p_toRemove)
	{
		m_toRemoveFromUpdate.push_back(p_toRemove);
	}

	void K_ComponentManager::updateComponents()
	{
		if (!m_toAddToStart.empty())
		{
			for (auto it = m_toAddToStart.cbegin(); it != m_toAddToStart.cend(); ++it)
			{
				m_toStart.insert(*it);
			}
			m_toAddToStart.clear();
		}
		
		//Start components
		if (!m_toStart.empty())
		{
			for (auto it = m_toStart.cbegin(); it != m_toStart.cend(); ++it)
			{
				(*it)->m_hasStarted = true;
				(*it)->start();
				if ((*it)->hasUpdate())
				{
					m_toUpdate.insert(*it);
				}
			}
			m_toStart.clear();
		}
		
		//Delete queued deletions
		if (!m_toDelete.empty())
		{
			auto it = m_toDelete.begin();
			while(!m_toDelete.empty())
			{
				if ((*it)->hasUpdate()) //&& isActive
				{
					removeFromUpdate(*it);
				}

				if (!(*it)->m_hasStarted)
				{
					removeFromStart(*it);
				}

				(*it)->m_attachedObject->removeComponent(*it);

				
				delete (*it);

				m_toDelete.erase(it);
				it = m_toDelete.begin();
			}

		}
		
		//Add queued components to update
		if (!m_toAddToUpdate.empty())
		{
			for (auto it = m_toAddToUpdate.begin(); it != m_toAddToUpdate.end(); ++it)
			{
				m_toUpdate.insert(*it);
			}
			m_toAddToUpdate.clear();
		}
		
		//No if, likely to always have to update
		//Remove queued components from update
		for (auto it = m_toRemoveFromUpdate.begin(); it != m_toRemoveFromUpdate.end(); it = m_toRemoveFromUpdate.erase(it))
		{
			removeFromUpdate(*it);
		}

		//Update components
		auto updateEnd = m_toUpdate.cend();
		for (auto it = m_toUpdate.cbegin(); it != updateEnd; ++it)
		{
			(*it)->update();
		}

		//Remove queued components from update
		if (!m_toRemoveFromUpdate.empty())
		{
			for (auto it = m_toRemoveFromUpdate.begin(); it != m_toRemoveFromUpdate.end(); ++it)
			{
				removeFromUpdate(*it);
			}

			m_toRemoveFromUpdate.clear();
		}
		
	}
}