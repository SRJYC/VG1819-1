#pragma once
#include "kitten\K_UIRenderable.h"
#include "puppy\Material.h"
#include "puppy\VertexEnvironment.h"
#include "UIElement.h"
#include <list>
#include "UIObject.h"

namespace userinterface 
{
	class UIFrame : public userinterface::UIElement
	{
	public:

		UIFrame(const char* p_pathToTex);
		UIFrame(const char* p_pathToTex, pivotType p_pivot, textureBehaviour p_texBehaviour);
		virtual ~UIFrame();
		
		void addToFrame(UIObject* p_uioToAdd);
		void removeFromFrame(UIObject* p_uioToRemove);
		void setInnerObjectsEnabled(bool p_enabled);

		//getter for inner objects(used for get cards in hand frame)
		std::list<UIObject*> getInnerObjects() { return m_innerObjects; };
	protected:

		void onDisabled() ;
		void onEnabled();
		std::list<UIObject*> m_innerObjects;

	};
}
