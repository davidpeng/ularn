#!/usr/bin/env bash
emcc \
  action.c \
  dungeon.c \
  dungeon_obj.c \
  fortune.c \
  help.c \
  itm.c \
  MemoryFile.c \
  monster.c \
  object.c \
  player.c \
  potion.c \
  savegame.c \
  saveutils.c \
  scores.c \
  scroll.c \
  show.c \
  spell.c \
  sphere.c \
  store.c \
  ularn.c \
  ularn_ask.c \
  ularn_game.c \
  ularn_win.c \
  -Wall \
  -Wextra \
  -pedantic-errors \
  -O2 \
  --memory-init-file 0 \
  --embed-file lib \
  -o ../web/ularn.js \
  -s EXPORTED_FUNCTIONS="[
    '_ULarnSetup',
    '_act',
    '_act_on_object',
    '_action_teleport',
    '_appraise2',
    '_appraise3',
    '_buy_dope',
    '_buy_something',
    '_cast',
    '_deposit_money',
    '_drop_gold',
    '_dropobj',
    '_eat',
    '_free_savegame',
    '_get_difficulty',
    '_get_effects_json',
    '_get_game_time',
    '_get_header_html',
    '_get_inventory_json',
    '_get_map_effect_json',
    '_get_map_height',
    '_get_map_json',
    '_get_map_tile',
    '_get_map_width',
    '_get_option_json',
    '_get_player_score_json',
    '_get_player_x',
    '_get_player_y',
    '_get_savegame',
    '_get_stat_hp',
    '_get_stat_hp_max',
    '_get_stat_spells',
    '_get_stat_spell_max',
    '_get_stat_level_name',
    '_get_stat_gold',
    '_get_status_json',
    '_get_text_html',
    '_move_world',
    '_oaltar_m',
    '_obanksub3',
    '_obrasslamp',
    '_ohome2',
    '_olrs2',
    '_pay_taxes',
    '_pick_gender',
    '_prompt_gender',
    '_quaff',
    '_readscr',
    '_restoregame',
    '_sell_gem',
    '_sell_item',
    '_set_difficulty',
    '_showcell',
    '_spell_bal',
    '_spell_cld',
    '_spell_cre',
    '_spell_drl',
    '_spell_dry',
    '_spell_fgr',
    '_spell_gen',
    '_spell_lit',
    '_spell_mkw',
    '_spell_mle',
    '_spell_pha',
    '_spell_ply',
    '_spell_sle',
    '_spell_sph',
    '_spell_ssp',
    '_spell_sum',
    '_spell_tel',
    '_spell_web',
    '_spulverize',
    '_take_course',
    '_takeoff',
    '_unwield',
    '_wear',
    '_wield',
    '_withdraw_money']"
