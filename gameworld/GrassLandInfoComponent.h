#pragma once

#include "TileInformationComponent.h"

namespace gameworld
{
	class GrasslandInfoComponent : public gameworld::TileInformationComponent
	{
	public:
		GrasslandInfoComponent() {
			m_Type = TileInformationComponent::type_Grassland;
			m_TexturePath = "tiles/Grassland.tga";
		}
	};
}