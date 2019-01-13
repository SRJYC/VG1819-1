#pragma once

#define OBJECT_CLICKED_OBJ_KEY "clicked"
#define TILE_POS_X "pos_x"
#define TILE_POS_Y "pos_y"
#define TILE_OWNER_KEY "tile_owner_key"
#define MANIPULATE_TILE_KEY "manip_tile_key"
#define COMMANDER_GO_KEY "commander_go_key"
#define PLAYER_COMMANDER_DEATH "player_commander_death_key"
#define PAUSE_MENU_OPEN "pause_menu_open_key"

#define PLAYER_ID "playerID"
#define CARD_ID "cardID"
#define CARD_COUNT "count"
#define CARD_PLACEMENT "placement"

#define CAM_FOV "FOV"
#define POSITION_X "x"
#define POSITION_Z "z"

#define FALSE 0
#define TRUE 1

// Networking Events
#define GAME_END_RESULT "game_end_result_key"
#define HOST_COMMANDER_DIED 0
#define CLIENT_COMMANDER_DIED 1
#define PLAYER_DISCONNECTED 2
#define CLIENT_DESYNCED 3

#include "kitten/K_GameObject.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace kitten
{
	class Event
	{
	public:
		enum EventType 
		{ 
			Tile_Clicked,
			Object_Clicked,
			Set_Area_Pattern,
			Highlight_Tile, 
			Unhighlight_Tile,
			Manipulate_Tile,
			Reset_Power,
			Highlight_Tile_Summon_Unit,
			Summon_Unit,
			Draw_Card,
			Peek_Card,
			Discard_Card,
			Add_Card,
			Shuffle_Deck,
			Card_Drawn,
			Card_Peeked,
			Card_Discarded,
			Deck_Empty,
			End_Game_Screen,
			Return_to_Main_Menu,
			Join_Button_Clicked,
			IP_Address_Entered,
			Host_Button_Clicked,
			Pause_Menu_Open,
			Disconnect_From_Network,
			Tile_Clicked_Debug,
			Move_Camera,
			Client_Commander_Loaded,

			Test_Event, 
			MAX_VAL = Test_Event
		};

		typedef std::vector<std::pair<int, int>> TileList;

		Event(EventType p_event);
		~Event();

		void putInt(const std::string& p_key, int p_value);
		void putFloat(const std::string& p_key, float p_value);
		void putString(const std::string& p_key, const std::string& p_value);
		void putGameObj(const std::string& p_key, K_GameObject* p_value);
		void putConstGameObj(const std::string& p_key, const K_GameObject* p_value);

		void putTileList(const TileList* p_tileList);


		int getInt(const std::string& p_key);
		float getFloat(const std::string& p_key);
		std::string getString(const std::string& p_key);
		K_GameObject* getGameObj(const std::string& p_key);
		const K_GameObject* getConstGameObj(const std::string& p_key);

		const TileList* getTileList();

	private:
		//------------------------------------------------------------------------------
		// Private members.
		//------------------------------------------------------------------------------
		EventType m_type; //ID

		//Maps allocated when needed
		std::unordered_map<std::string, int> *m_intMap;
		std::unordered_map<std::string, float> *m_floatMap;
		std::unordered_map<std::string, std::string> *m_stringMap;
		std::unordered_map<std::string, const K_GameObject*> *m_constGameObjMap;
		std::unordered_map<std::string, K_GameObject*> *m_gameObjMap;

		const TileList* m_tileList;
		
	};
}