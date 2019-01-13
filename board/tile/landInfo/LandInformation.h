/*@Rock 11.7
Changed from Austin's TileInformationComponent.
This type of components handles the fixed data of land and its effect
It will be part of tile info
*/

#pragma once
#include "kitten/K_Common.h"
#include "unit/Unit.h"

class TileInfo;
class LandInformation
{
public:
	//========================================================
	//New types will be added here later
	enum TileType {
		Unknown,
		Grass_land,
		Swamp_land,
		Sand_land,
		Forest_land,
		Garden_land,
		Water_land,

		//special land, change by abilities
		Home_land,
	};
	//========================================================

	LandInformation() { };
	~LandInformation() { };

	virtual TileType GetType()
	{
		return m_Type;
	};

	virtual const std::string getTexturePath()
	{
		return m_TexturePath;
	}

	virtual int getMVCost()
	{
		return m_mvCost;
	}

	virtual const std::string getDescription()
	{
		return m_description;
	}

	virtual void effectOnPass(unit::Unit* p_unit, TileInfo* p_tInfo) { };
	//will be triggered when unit enter this tile

	virtual void effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo) { };
	//will be triggered when unit that's on this tile ends its turn

	virtual void effectOnStart(unit::Unit* p_unit, TileInfo* p_tInfo) { };
	//will be triggered when unit that's on this tile starts its turn

	virtual void effectOnLeave(unit::Unit* p_unit, TileInfo* p_tInfo) {};
protected:
	TileType m_Type = Unknown;
	std::string m_TexturePath = "textures/tiles/MISSING.tga";
	int m_mvCost = 1;
	std::string m_description = "Land Not Found.";
};

class GrassLand : public LandInformation
{
public:
	GrassLand()
	{
		m_Type = Grass_land;
		m_TexturePath = "textures/tiles/grass.png";
		m_mvCost = 1;
		m_description = "Ordinary land. Nothing special.";
	};
};

class SwampLand : public LandInformation
{
public:
	SwampLand()
	{
		m_Type = Swamp_land;
		m_TexturePath = "textures/tiles/dark_water.png";
		m_mvCost = 2;
		m_description = "It's hard to move cross it. When stay on it, unit will lost 1 HP.";
	};

	void effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo) override;
};

class SandLand : public LandInformation
{
public:
	SandLand()
	{
		m_Type = Sand_land;
		m_TexturePath = "textures/tiles/sand.png";
		m_mvCost = 1;
		m_description = "Unit will temporarily -1 IN";
	};

	void effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo) override;
};

class ForestLand : public LandInformation
{
public:
	ForestLand()
	{
		m_Type = Forest_land;
		m_TexturePath = "textures/tiles/grass.png";
		m_mvCost = 2;
		m_description = "It's hard to move cross it. Unit will temporarily -1 MV.";
	};

	void effectOnStart(unit::Unit* p_unit, TileInfo* p_tInfo) override;
};

class GardenLand : public LandInformation
{
public:
	GardenLand()
	{
		m_Type = Garden_land;
		m_TexturePath = "textures/tiles/grass.png";
		m_mvCost = 1;
		m_description = "Nice place to rest. Unit will heal 1 HP and temporarily +1 Max HP.";
	};

	void effectOnStart(unit::Unit* p_unit, TileInfo* p_tInfo) override;
};

class WaterLand : public LandInformation
{
public:
	WaterLand()
	{
		m_Type = Water_land;
		m_TexturePath = "textures/tiles/water.png";
		m_mvCost = 100;
		m_description = "Can not move across";
	};
};

class HomeLand : public LandInformation
{
public:
	HomeLand()
	{
		m_Type = Home_land;
		m_TexturePath = "textures/tiles/homeland.tga";
		m_mvCost = 1;
		m_description = "A place to protect you.";
	};

	void effectOnStay(unit::Unit* p_unit, TileInfo* p_tInfo) override;
	void effectOnLeave(unit::Unit* p_unit, TileInfo* p_tInfo) override;
};