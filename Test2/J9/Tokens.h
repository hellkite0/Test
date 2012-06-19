// $Id: Tokens.h 6220 2010-06-22 03:04:10Z jinsang.kim $

#ifndef __J9_TOKENS_H__
#define __J9_TOKENS_H__

#include "StringKey.h"

#define TOKEN(TokenName) \
	namespace TokenName \
	{ \
		__inline J9::StringKeyA	KA()	{ static const J9::StringKeyA cKey(#TokenName); return cKey; } \
		__inline J9::StringKeyW	KW()	{ static const J9::StringKeyW cKey(_W(#TokenName)); return cKey; } \
		__inline J9::PathKeyA	PA()	{ static const J9::PathKeyA cKey(KA().c_str()); return cKey; } \
		__inline J9::PathKeyW	PW()	{ static const J9::PathKeyW cKey(KW().c_str()); return cKey; } \
		__inline const AChar*	A()		{ return KA().c_str(); } \
		__inline const WChar*	W()		{ return KW().c_str(); } \
	}

#define TOKEN2(TokenName, TokenValue) \
	namespace TokenName \
	{ \
		__inline J9::StringKeyA	KA()	{ static const J9::StringKeyA cKey(TokenValue); return cKey; } \
		__inline J9::StringKeyW	KW()	{ static const J9::StringKeyW cKey(_W(TokenValue)); return cKey; } \
		__inline J9::PathKeyA	PA()	{ static const J9::PathKeyA cKey(KA().c_str()); return cKey; } \
		__inline J9::PathKeyW	PW()	{ static const J9::PathKeyW cKey(KW().c_str()); return cKey; } \
		__inline const AChar*	A()		{ return KA().c_str(); } \
		__inline const WChar*	W()		{ return KW().c_str(); } \
	}

namespace Tokens
{
	TOKEN(networks);
	TOKEN(networked);
	TOKEN(network);
	TOKEN(addr);
	TOKEN(tcp_port);
	TOKEN(man_port);
	TOKEN(udp_port);
	TOKEN(port);
	TOKEN2(true_, "true");
	TOKEN2(false_, "false");
	TOKEN(id);
	TOKEN(ip);
	TOKEN(pwd);
	TOKEN(min);
	TOKEN(max);
	TOKEN(mails);

	TOKEN(receiver);
	TOKEN(receivers);

	TOKEN(sender);	
	TOKEN(smtp);
	TOKEN(subject);

	TOKEN2(install_path, "InstallPath");
	TOKEN(full_screen);
	TOKEN(publisher);
	TOKEN(company);
	TOKEN(product);

	TOKEN(image);
	TOKEN(hp);
	TOKEN(speed);
	TOKEN(attack);
	TOKEN(defense);
	TOKEN(reward);
	TOKEN(range);
	TOKEN(count);
	TOKEN(delay);

	TOKEN(sprite_ids);
	TOKEN(sprite);
	TOKEN(sprite_id);
	TOKEN(state_type);

	TOKEN(sprites);
	TOKEN(create_type);
	TOKEN(texture_filename);
	TOKEN(image_start_pos);
	TOKEN(image_rect);
	TOKEN(hotspot);
	TOKEN(time);
	TOKEN(dx);
	TOKEN(dy);

	TOKEN(valid);
	TOKEN(version);
	TOKEN(versions);

	TOKEN(x);
	TOKEN(y);
	TOKEN(z);

	TOKEN(unit_type);
	TOKEN(delay_to_next);

	TOKEN(scripts);
	TOKEN(script);
	TOKEN(script_type);
	TOKEN(script_entries);
	TOKEN(script_entry);
	TOKEN(script_entry_type);
	TOKEN(script_entry_data);

	TOKEN(mainimg);
	TOKEN(subimg);

	TOKEN(game_life);
	TOKEN(game_money);
	TOKEN(game_level);

	TOKEN(game_deck_1);
	TOKEN(game_deck_2);
	TOKEN(game_deck_3);
	TOKEN(game_deck_4);
	TOKEN(game_deck_5);
	TOKEN(game_deck_6);
	TOKEN(game_deck_7);
	TOKEN(game_deck_8);
	TOKEN(game_deck_9);
	TOKEN(game_deck_10);
	TOKEN(game_deck_11);
	TOKEN(game_deck_12);

	TOKEN(game_deck_1_imageid);
	TOKEN(game_deck_2_imageid);
	TOKEN(game_deck_3_imageid);
	TOKEN(game_deck_4_imageid);
	TOKEN(game_deck_5_imageid);
	TOKEN(game_deck_6_imageid);
	TOKEN(game_deck_7_imageid);
	TOKEN(game_deck_8_imageid);
	TOKEN(game_deck_9_imageid);
	TOKEN(game_deck_10_imageid);
	TOKEN(game_deck_11_imageid);
	TOKEN(game_deck_12_imageid);

	TOKEN(game_selected_tower_name);
	TOKEN(game_selected_tower_portrait_id);
	TOKEN(game_selected_tower_damage_min);
	TOKEN(game_selected_tower_damage_max);
	TOKEN(game_selected_tower_attack_range_min);
	TOKEN(game_selected_tower_attack_range_max);
	TOKEN(game_selected_tower_attack_speed);
	TOKEN(game_selected_tower_missiles);
	TOKEN(game_selected_tower_specials);
	TOKEN(game_selected_tower_attack_type_heat);
	TOKEN(game_selected_tower_attack_type_physical);
	TOKEN(game_selected_tower_attack_type_wave);
	TOKEN(game_selected_tower_attack_type_psychic);
	TOKEN(game_selected_tower_cost_money);
	TOKEN(game_selected_tower_cost_time);
	TOKEN(game_selected_tower_width);
	TOKEN(game_selected_tower_height);

	TOKEN(game_selected_monster_name);
	TOKEN(game_selected_monster_move_speed);
	TOKEN(game_selected_monster_defense_type_heat);
	TOKEN(game_selected_monster_defense_type_physical);
	TOKEN(game_selected_monster_defense_type_wave);
	TOKEN(game_selected_monster_defense_type_psychic);
	TOKEN(game_selected_monster_specials);

	TOKEN(optimization);
}

#endif // Tokens.h