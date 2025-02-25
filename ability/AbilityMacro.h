#pragma once

#define ADD_ELEMENT(n, list, name,className) n = new className(); list[name] = n;

//ability
#define ABILITY_MANIPULATE_TILE "Manipulate Tile"
#define ABILITY_HEAL "Heal"
#define ABILITY_FIGHT "Fight"
#define ABILITY_SUMMON_UNIT "Summon Unit"
#define ABILITY_SHOOT "Shoot"
#define ABILITY_SABOTAGE "Sabotage"
#define ABILITY_BUILD_WALL "Build the Wall"
#define ABILITY_ENCOURAGE "Encourage"
#define ABILITY_QUICK_SHOOT "Quick Shoot"
#define ABILITY_SLAY "Slay"
#define ABILITY_DEFEND "Defend"
#define ABILITY_BLAST "Blast"
#define ABILITY_BURN "Burn"
#define ABILITY_FIRE_DANCE "Fire Dance"
#define ABILITY_FIRE "Fire"
#define ABILITY_LOAD "Load"
#define ABILITY_VOLLEY "Volley"
#define ABILITY_ARM "Arm"
#define ABILITY_THE_LIGHT "The Light"
#define ABILITY_POWER_SLAM "Power Slam"
#define ABILITY_ENERGY_CONTROL "Energy Control"
#define ABILITY_POWER_SURGE "Power Surge"
#define ABILITY_RALLY "Rally"
#define ABILITY_HOMELAND "Here is Homeland"

#define ABILITY_CURSE "Curse"
#define ABILITY_OPPOSE "Oppose"
#define ABILITY_THRUST "Thrust"
#define ABILITY_FEAR_STRIKE "Fear Strike"
#define ABILITY_RELEASE_GAS "Release Gas"
#define ABILITY_MUCUS "Mucus"
#define ABILITY_SUMMON "Summon"
#define ABILITY_ACTIVATE "Activate"
#define ABILITY_STAB "Stab"
#define ABILITY_SACRIFICE "Sacrifice"
#define ABILITY_EROSION "Erosion"
#define ABILITY_DRAIN "Drain"
#define ABILITY_EXECUTE "Execute"
#define ABILITY_HORROR "Horror"
#define ABILITY_ATTACH "Attach"
#define ABILITY_HEALTH_LINK "Health Link"
#define ABILITY_CRASH "Crash"
#define ABILITY_CURSED_PROTECTION "Cursed Protection"
#define ABILITY_CLONE "Clone"
#define ABILITY_DEVOUR "Devour"
#define ABILITY_BREW "Brew"
#define ABILITY_CORRUPT "Corrupt"
#define ABILITY_TOXIC_AURA "Toxic Aura"
#define ABILITY_LORD_ORDER "Lord's Order"
#define ABILITY_ANCIENT_OFFERING "Ancient Offering"
#define ABILITY_ANCIENT_GIFT "Ancient Gift"

//ability or status required info
#define PUSH_BACK "push_back"
#define CARD_DRAWN "card_drawn"
#define COUNTER_POWER "counter_power"
#define COUNTER_CHANGE "counter_change"

#define STATUS_ARMOR "armor"

//ability-action
#define ACTION_MOVE "Move"
#define ACTION_JOIN "Join"

//status
#define STATUS_TEMP_CHANGE "Status_Temp_Change"
#define STATUS_AD_CHANGE "Status_AD_Change"
#define STATUS_IN_CHANGE "Status_IN_Change"

// buff/debuff
#define STATUS_ENCOURAGE "Status_Encourage"
#define STATUS_DODGE "Status_Dodge"
#define STATUS_LOAD "Status_Load"
#define STATUS_SHIELD "Status_Shield"
#define STATUS_BLOCK "Status_Block"
#define STATUS_CURSE "Status_Curse"
#define STATUS_ATTACH "Status_Attach"
#define STATUS_VAMPIRIC_CURSE "Status_Vampiric_Curse"
#define STATUS_CURSED_BEING "Status_Cursed_Being"
#define STATUS_LAST_WORD_SUMMON "Status_Last_Word_Summon"
#define STATUS_POISON "Status_Poison"
#define STATUS_LORD_ORDER "Status_Lord_Order"

//lv
#define STATUS_LV "Status_LV"
#define STATUS_PRIEST_LV3 "Status_Priest_LV3"
#define STATUS_ARCHER_LV3 "Status_Archer_LV3"
#define STATUS_DUELIST_LV3 "Status_Duelist_LV3"
#define STATUS_ETERNAL_EYE_LV3 "Status_Eternal_Eye_LV3"
#define STATUS_LANCER_LV2 "Status_Lancer_LV2"
#define STATUS_WRAITH_LV2 "Status_Wraith_LV2"
#define STATUS_EVIL_FIEND_LV "Status_Evil_Fiend_LV"
#define STATUS_GOREFIEND_LV3 "Status_Gorefiend_LV3"
#define STATUS_SLIME_LV3 "Status_Slime_LV3"

//demonic restriction
#define STATUS_DEMONIC_RESTRICTION "Status_Demonic_Restriction"
#define STATUS_DR_AD_CHANGE "Status_DR_AD_Change"
#define STATUS_DR_ETERNAL_EYE "Status_DR_Eternal_Eye"
#define STATUS_DR_MUD_DEMON "Status_DR_Mud_Demon"
#define STATUS_DR_WRAITH "Status_DR_Wraith"

//status property
#define STATUS_NAME(id) "status_name_"+id
#define STATUS_DESCRIPTION(id) "status_description_"+id

#define STATUS_EFFECTED_AD "effectedAD"
#define STATUS_AD_NAME(n) "ad_name_"+std::to_string(n)
#define STATUS_AD_ATTRIBUTE_NUM(n) "ad_attribute_num_"+std::to_string(n)
#define STATUS_AD_ATTRIBUTE(adN,attrN) "ad_attribute_"+std::to_string(adN)+"_"+std::to_string(attrN)
#define STATUS_AD_VALUE(adN,attrN) "ad_value_"+std::to_string(adN)+"_"+std::to_string(attrN)
