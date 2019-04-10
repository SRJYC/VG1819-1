#pragma once

#define OBJECT_CLICKED_OBJ_KEY "clicked"
#define TILE_POS_X "pos_x"
#define TILE_POS_Y "pos_y"
#define TILE_OWNER_KEY "tile_owner_key"
#define MANIPULATE_TILE_KEY "manip_tile_key"
#define COMMANDER_GO_KEY "commander_go_key"
#define PLAYER_COMMANDER_DEATH "player_commander_death_key"
#define PAUSE_MENU_OPEN "pause_menu_open_key"
#define UPDATE_CARD_CONTEXT_KEY "update_card_context_key"
#define UPDATE_LAND_CONTEXT_KEY "update_land_context_key"
#define CARD_CONTEXT_SET_ENABLED_KEY "card_context_set_enabled_key"
#define TEXTCHAT_MESSAGE_KEY "textchat_message_key"
#define NEXT_SCENE_PATH_KEY "next_scene_path_key"
#define UNIT_GO_KEY "unit_go_key"
#define MAP_ID_KEY "map_id_key"
#define NETWORK_MESSAGE_KEY "network_message_key"

#define PLAYER_ID "playerID"
#define CARD_ID "cardID"
#define CARD_COUNT "count"
#define CARD_PLACEMENT "placement"
#define DECK_CARD_COUNT_LEFT "cardsleft"

#define CAM_FOV "FOV"
#define POSITION_X "x"
#define POSITION_Z "z"

#define FALSE 0
#define TRUE 1

#define DAMAGE_AS_INT "intDamage"

// Networking Events
#define GAME_END_RESULT "game_end_result_key"
#define HOST_COMMANDER_DIED 0
#define CLIENT_COMMANDER_DIED 1
#define PLAYER_DISCONNECTED 2
#define CLIENT_DESYNCED 3

#define SERVER_STATUS_KEY "server_status_key"
#define SERVER_PLAYER_COUNT_KEY "server_player_count_key"
#define SERVER_ACTIVE_SESSIONS_KEY "server_active_sessions_key"
#define SERVER_STATUS_FULL 2

// Scenes
#define DECK_MENU_SCENE "deck_menu.json"

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
			Right_Clicked,
			Object_Clicked,
			Set_Area_Pattern,
			Highlight_Tile, 
			Highlight_Tile_With_List,
			Unhighlight_Tile,
			Manipulate_Tile,
			Reset_Power,
			Highlight_Tile_Summon_Unit,
			Summon_Unit,
			Cancel_Summon,
			Draw_Card,
			Peek_Card,
			Discard_Card,
			Add_Card,
			Shuffle_Deck,
			Card_Drawn,
			Card_Peeked,
			Card_Discarded,
			Deck_Empty,
			Network_End_Game,
			Return_to_Main_Menu,
			Join_Direct_Address,
			IP_Address_Entered,
			Host_Button_Clicked,
			Pause_Menu_Open,
			Disconnect_From_Network,
			Tile_Clicked_Debug,
			Move_Camera,
			Update_Card_Context_By_ID,
			Update_Card_Context_By_GO,
			Update_Card_Context_Attrib,
			Update_Card_Context_Status,
			Card_Context_Set_Enabled,
			Land_Context_Set_Enabled,
			Update_Land_Context,
			Add_LandContext_Lerp_Callback,
			Client_Commander_Loaded,
			Poll_For_Server,
			Join_Localhost,
			Chat_Button_Clicked,
			TextChat_Receive_Message,
			TextChat_Scroll_Up,
			TextChat_Scroll_Down,
			Ready_Button_Clicked,
			Players_Ready,
			Player_Joined,
			Scene_Change,
			Remove_Loading_Screen,
			New_Game_Turn,
			New_Unit_Turn,
			Put_Card_To_Hand,
			Next_Units_Turn_Start,
			Action_Complete,
			Quickplay,
			Quickplay_Find_Game,
			Quickplay_Start_Game,
			Update_Server_Info,
			Board_Loaded,
			P2P_Start_Game,
			Update_Map_Detail,
			Game_Turn_End,
			Update_Deck_Builder_Frame,
			Network_Host_Not_Ready,
			Network_Cancel_Join,
			Enemy_Draw_Card,
			Send_Deck_Count,
			Damage_Commander,

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