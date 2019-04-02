#include "TileFliter.h"

bool WaterFilter::shouldRemove(kitten::K_GameObject * p_tileGO)
{
	LandInformation::TileType type = p_tileGO->getComponent<TileInfo>()->getType();
	return type == LandInformation::Water_land;
}