#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN STRUCTURE DECLARATION!
struct cartes{ //Helper to return two values simultaneously
	float x; //X-pos variable
	float y; //Y-pos variable
};

struct tile_entity{ //Helper structure for entities (containing x, y, type and name)
	cartes pos;
	string type, name;
};

struct tile{ //Basic tile declaration
	ALLEGRO_BITMAP* image; //The image referenced for each tile
	int id; //Image ref-id. Used for determing whether or not there are tile effects applied
	bool collide; //Determines whether or not the tile has collision on or off (t/f)
	string action; //A simple string which will point to an action for the tile. Loaded from tiledec.rcg
};

struct tile_map{ //Basic map declaration
	string raw_data; //Unprocessed data from map file
	vector<int> formatted_data; //Formatted data for map display
	int id; //Map ref-id. Used to determine which floor is being displayed
	string title; //Title of the map being displayed
	vector<tile_entity> entities; //A vector containing loaded entities from the map file
	int directions[4]; //An array of direction information (NESW)
};

struct level{ //Basic level declaration
	vector<tile_map> maps; //The store for the maps
	int id; //Level ref-id. Used to determine which level is being displayed
	string title; //Title of level being used
};

struct world{ //Basic world declaration
	vector<level> levels; //All the levels in the game
	string title; //Title of entire world
};

struct sound{ //Basic sound declaration
	ALLEGRO_SAMPLE *sample; //The actual sound loaded from file
	string type; //Category type from the config file
	int id; //Keeping in line with the other declarations
};

struct text{
	int font; //The specified font in the registry for this text
	string st; //The actual text itself
	cartes pos; //The position of the text on screen
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLASS PROTOTYPES (DECLARED ELSEWHERE)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class player;
class entity;
class hud;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN GAME CLASS DEFINITION!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class game{
public:
	ALLEGRO_DISPLAY *display = NULL; //Basic display pointer
	ALLEGRO_EVENT_QUEUE *evt_q = NULL; //Basic event queue
	ALLEGRO_TIMER *timer = NULL; //Basic timer pointer

	ALLEGRO_CONFIG *config_ld = al_load_config_file("config.ini"); //Main config for engine
	ALLEGRO_CONFIG *level_cfg = al_load_config_file("levels.rcg"); //Loads first level config file
	ALLEGRO_CONFIG *map_cfg = al_load_config_file("map.rcg"); //Loads first map file
	ALLEGRO_CONFIG *tile_cfg = al_load_config_file("tiles.rcg"); //Load the tile loading configuration file
	ALLEGRO_CONFIG *sound_cfg = al_load_config_file("sounds.rcg"); //Load the main audio config file
	ALLEGRO_CONFIG *tiledec_cfg = al_load_config_file("tiledec.rcg"); //Load the tile decleration file
	ALLEGRO_CONFIG *entity_cfg = al_load_config_file("entities.rcg"); //Load the entity config file
	ALLEGRO_CONFIG *font_cfg = al_load_config_file("fonts.rcg"); //Load the font config file
	ALLEGRO_CONFIG *temp_lvls; //Temp level config file holder

	int tile_px, tile_w, tile_h, level_w, level_h;
	float fps;

	int SCREEN_W, SCREEN_H, FPS;

	enum MYKEYS { KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT };

	//GLOBAL VARIABLE DECLARATION!
	tile_map curmap; //Memory-loaded map and level variables, something static (roughly) so as to decrease lag and shit
	level curlvl;

	world globalWorld; //The global world variable for the entire game

	vector<level> lvl_reg; //Registry for levels and shite, just raw levels and their predetermined ids

	vector<player*> player_reg; //Registry of memory addresses for player entities //Hehe, titties

	vector<entity*> entity_reg; //Registry of entities (or rather their address)

	vector<entity*> entity_cat; //A catalogue of loaded entities, which can be called and loaded into the registry

	vector<tile> tile_reg; //Registry of tiles for displaying

	vector<ALLEGRO_FONT*> font_reg; //Registry of fonts for displaying
private:

};
