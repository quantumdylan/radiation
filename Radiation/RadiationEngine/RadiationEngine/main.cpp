//RADIATION ENGINE v1.1.0
//New this version:
//I've completely re-written a few aspects of the code, though no visual changes since March.
//Instead of there being any globals (which holy hell, this code was completely filled with them)
//there is now a single game class, which will contain the basic information used by the engine.
//Though it's a bit clunky, it seems to be a step forward towards me splitting this one code file
//into individual code files for each grouping of functions/classes

//TODO:
//Fully implement audio system (multi-streams, mixers, etc...)
//Flesh out AI in the player class
//Fix collision detection (fix the hitboxes and such)
//Add hitboxes to player characters (for projectile/melee stuff)
//Possiblity of a rogue-like, random dungeon generator? (maybe)
//Implement entity system (non-geometry objects that the user can interact with)
// Examples of an entity (in this scope):
//  -Door
//  -Pickups (health, potions, etc...)
//  -Blah

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
/*
int tile_w, tile_h, level_h, level_w;

int tile_px = 32;

const int PLAYER_SPAWN = 18;
const int NPC_SPAWN = 19;

float FPS = 60;
int SCREEN_W = 640;
int SCREEN_H = 480;
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT};

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
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////





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


vector<sound> audio_reg; //Basic audio registry for sounds


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPING!
class game;
class entity;
class player;
class hud;

bool col_det(float x, float y, float dx, float dy, game engine);
cartes find_tile(int id, game engine);
double round(double d);
int get_player_type_id(int id, game engine);
//player* find_controlled_player();
int get_tile(int pos, game engine);
sound find_sound(string type, bool random);
float rng(int cap);
tile get_raw_tile(int pos, game engine);
tile get_raw_reg_tile(int pos, game engine);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN HUD CLASS DEFINITION!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class hud{
public:
	hud(string hudfile);
	void display();
	void update();
private:
	vector<text> display_str;
	vector<ALLEGRO_BITMAP*> display_img;
	void loadhud(string hudfile);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//hud(string hudfile)
//PURPOSE: Create a hud using the supplied HUD decleration file
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
hud::hud(string hudfile){
	loadhud(hudfile); //Load the HUD using internal function
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadhud(string hudfile)
//PURPOSE: 
//TODO:
//NOTES: Here is a comprehensive listing of the possible empty cases for variable display in the HUD
//	%CURPLAYERHEALTH% - The current health of the controlled player
//	%TOTPLAYERHEALTH% - The total health of the controlled player
//	%PLAYERNAME% - The name of the controlled player
//	%CURLEVELNAME% - The string name of the current level
//	%CURMAPNAME% - The string name of the current map
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hud::loadhud(string hudfile){
	ALLEGRO_CONFIG* temp_hud = al_load_config_file(hudfile.c_str());

	string name = al_get_config_value(temp_hud, "data", "name");
	int totalstr = atoi(al_get_config_value(temp_hud, "data", "totalstr"));
	int totalimg = atoi(al_get_config_value(temp_hud, "data", "totalimg"));

	stringstream converter;
	string temp_i;
	string curstr;
	string curimg;
	text temptxt;
	cartes tempcart;

	cout << "Detected HUD file named " + name + "\n";
	cout << "Processing...\n";

	for(int i = 0; i < totalstr; i++){
		curstr = "";
		converter.str("");
		converter.clear();
		converter << i;
		temp_i = converter.str();

		curstr = "str" + temp_i;

		temptxt.font = atoi(al_get_config_value(temp_hud, curstr.c_str(), "font"));
		cout << "Font: " + temptxt.font;
		cout << "\n";
		temptxt.st = al_get_config_value(temp_hud, curstr.c_str(), "text");
		cout << "Text: " + temptxt.st + "\n";
		tempcart.x = atoi(al_get_config_value(temp_hud, curstr.c_str(), "x"));
		tempcart.y = atoi(al_get_config_value(temp_hud, curstr.c_str(), "y"));

		temptxt.pos = tempcart;

		display_str.push_back(temptxt); //Add our current text to the stack of HUD messages
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//display()
//PURPOSE: Display the HUD to the screen. Will be called by the global draw() function
//TODO: Finish the parser
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hud::display(){
	string tempstr, finalstr;
	for(int i = 0; i < display_str.size(); i++){
		tempstr = display_str[i].st;
		for(int j = 0; j < tempstr.length(); j++){
			switch(tempstr[j]){
			case '%' : //Put in the parsing code similar to the map parser.
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN ENTITY CLASS DEFINITION!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class entity{
public:
	entity(int inx, int iny, int indx, int indy, int dxcap, int dycap, string type, bool col, bool move, float hurt, float face, int tileid, string inname, game engine);
	ALLEGRO_BITMAP* get_entity_tile();
	cartes get_pos();
	int getx();
	int gety();
	string getname();
	string gettype();
	void move(int dir);
	void update_state(game engine);
protected:
	tile entity_tile;
	string enttype, name;
	bool collision, moveable;
	float x, y, dx, dy, dxc, dyc, hurtfac, facingdir;

	void decay_motion();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//entity(...)
//PURPOSE: Constructs a new entity (given the supplied information)
//TODO: Keep up-to-date with the rest of the entity systems
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
entity::entity(int inx, int iny, int indx, int indy, int dxcap, int dycap, string type, bool col, bool move, float hurt, float face, int tileid, string inname, game engine){
	x = inx;
	y = iny;
	dxc = dxcap;
	dyc = dycap;
	hurtfac = hurt;
	collision = col;
	enttype = type;
	moveable = move;
	dx = indx;
	dy = indy;
	facingdir = face;
	entity_tile = get_raw_reg_tile(tileid, engine);
	name = inname;
	//This will have set all the variables/flags for the entity
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//getname()
//PURPOSE: To return the name of the entity
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
string entity::getname(){
	return name;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//gettype()
//PURPOSE: To return the type of the entity
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
string entity::gettype(){
	return enttype;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//getx()
//PURPOSE: Gets the x
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int entity::getx(){
	return x;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//gety()
//PURPOSE: Gets the y
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int entity::gety(){
	return y;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_entity_tile()
//PURPOSE: Gets the current tile (or the only tile if animation is static) for the pointed entity
//TODO:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
ALLEGRO_BITMAP* entity::get_entity_tile(){
	return entity_tile.image;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//update_state()
//PURPOSE: Updates the state of an entity (position and otherwise)
//TODO: Keep up-to-date
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void entity::update_state(game engine){
	decay_motion();

	if(!col_det(x, y, dx, dy, engine)){
		if(dx > dxc)
			dx = dxc;
		if(dx < -dxc)
			dx = -dxc;
		if(dy > dyc)
			dy = dyc;
		if(dy < -dyc)
			dy = -dyc;

		if(abs(dx) <= 0.3)
			dx = 0;
		if(abs(dy) <= 0.3)
			dy = 0;

		x += dx;
		y += dy;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//decay_motion()
//PURPOSE: I liked this code from the original player class, so I decided to add it to the base entity class
//TODO: Blah
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void entity::decay_motion(){
	dx *= 0.8;
	dy *= 0.8;

	if(abs(dx) <= 0.3)
		dx = 0;
	if(abs(dy) <= 0.3)
		dy = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN PLAYER CLASS DEFINITION!
//Basically, I want to be able to create player characters
//from nothing. In this case, it will be able to move (in conjuction
//with the col_det() function) and return its position and its
//assigned tile texture.
//TODO:
//Actually write the functions and test if we get more of those damned memory
//access errors again. I swear if I get another damned "assertion failed" or
//out_of_memory I am gonna kill someone. But not really, because I am a
//pacifist. But I may break my alcoholic abstinence and start drinking heavily.
//You've done this, you bastard. I hope you're happy.
class player{
public:
	player(int inx, int iny, tile player_tl, int dxc, int dyc, int type, int h);
	cartes get_pos();
	int getx();
	int gety();
	int get_health();
	string get_name();
	void hurt(int fact);
	int get_type();
	void move(int dir, game engine); 
	void setpos(float newx, float newy);
	tile get_player_tile();
	ALLEGRO_BITMAP* get_player_image();
	void change_vel(int mag, int id);
	void think(player* play, game engine); //These two functions will be used for autonomous systems later on. Think npcs.
	void decay();
private:
	tile player_tile;
	float x, y, dx, dy;
	int dxcap, dycap;
	int player_type, sound_tick, health;
	string name;

	void cap_vel();
	void act();
	void find_spawn(game engine);
	void walk();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Player constructor
//PURPOSE: To create a new player entity, determine its starting stats and such
//TODO: I don't know if anything else needs to be included
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player::player(int inx, int iny, tile player_tl, int dxc, int dyc, int type, int h){
	//Initialize the values either to null or the given values
	x = inx;
	y = iny;
	player_tile = player_tl;
	dx = 1;
	dy = -1;
	dxcap = dxc;
	dycap = dyc;
	player_type = type;
	sound_tick = 0;
	health = h;
	name = "TESTING";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//hurt(int fact)
//PURPOSE: Do damage to the player
//TODO: Add resistances
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::hurt(int fact){
	health -= fact;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_health()
//PURPOSE: Get the current health of the player
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int player::get_health(){
	return health;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_name()
//PURPOSE: Get the name of the player
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
string player::get_name(){
	return name;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//setpos(int x, int y)
//PURPOSE: To manually set the current position of the character
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::setpos(float newx, float newy){
	x = newx;
	y = newy;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_spawn()
//PURPOSE: To cycle through all of the tiles in the loaded map to determine the player's spawn
//TODO: Nothing, that I can tell. Probably wrong, though
//WARNING! This code may now be deprecated. Pending removal.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::find_spawn(game engine){
	cartes temp;
	switch(player_type){
	case 0 : temp = find_tile(18, engine); break;
	case 1 : temp = find_tile(19, engine); break;
	default : temp.x = x; temp.y = y; //If all else fails, fall back on this
	}
	x = temp.x;
	y = temp.y;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_type()
//PURPOSE: Returns the player type for this entity
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int player::get_type(){
	return player_type;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//think(player* play)
//PURPOSE: For automated "monsters", this function will take the information regarding the pointed
//player play. It will dictate the "monster's" actions towards the player.
//TODO: Actually code decent pathing/decision making
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::think(player* play, game engine){
	//This is where we can put code to automate the npc movement
	//For now, I'm just going to have them do simple pathing
	//Eventually, I hope to integrate the A* pathing algortihm
	//to this. But that will take quite some time.

	if(x < play->getx()){
		move(2, engine);
		if(dx <= 1 && dx >= -1){
			move(0, engine);
			dx = 0;
			if(y < play->gety())
				move(3, engine);
			if(y > play->gety())
				move(1, engine);
		}
	}
	if(x > play->getx()){
		move(0, engine);
		if(dx <= 1 && dx >= -1){
			move(2, engine);
			dx = 0;
			if(y < play->gety())
				move(3, engine);
			if(y > play->gety())
				move(1, engine);
		}	
	}
	if(y < play->gety()){
		move(3, engine);
		if(dy <= 1 && dy >= -1){
			move(1, engine);
			dy = 0;
			if(x < play->getx())
				move(2, engine);
			if(x > play->getx())
				move(0, engine);
		}
	}
	if(y > play->gety()){
		move(1, engine);
		if(dy <= 1 && dy >= -1){
			move(3, engine);
			dy = 0;
			if(x < play->getx())
				move(2, engine);
			if(x > play->getx())
				move(0, engine);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//act()
//PURPOSE: To carry out the preparations of think()
//TODO: Not even worrying about this for the moment, but basically everything
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::act(){
	//Eventually
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_image()
//PURPOSE: Grabs the current sprite used to represent the player
//TODO: Sprite orientation/state?
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
ALLEGRO_BITMAP* player::get_player_image(){
	return player_tile.image;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//getx()
//PURPOSE: Get the private x value
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int player::getx(){
	return x;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//gety()
//PURPOSE: Get the private y value
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int player::gety(){
	return y;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_pos()
//PURPOSE: Get the cartesian position of the player being referenced
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
cartes player::get_pos(){
	cartes position;

	position.x = x;
	position.y = y;

	return position;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cap_vel()
//PURPOSE: Prevent the player from going faster and faster, introduces a governor to the speed of player
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::cap_vel(){
	if(dx >= dxcap)
		dx = dxcap;
	if(dx <= -dxcap)
		dx = -dxcap;
	if(dy >= dycap)
		dy = dycap;
	if(dy <= -dycap)
		dy = -dycap;

	if(abs(dx) <= 0.3)
		dx = 0;
	if(abs(dy) <= 0.3)
		dy = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//change_vel(int mag, int id)
//PURPOSE: Basic velocity changer for player movement
//TODO: Finished (for now)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::change_vel(int mag, int id){
	switch(id){
	case 0 : dx -= mag; break;
	case 1 : dy -= mag; break;
	case 2 : dx += mag; break;
	case 3 : dy += mag; break;
	default : ;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//move(int dir)
//PURPOSE: Derived from previous moveguy engine, core of the player's movement
//TODO: Finished (for now)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::move(int dir, game engine){
	change_vel(2, dir);
	cap_vel(); //Make sure that we have not exceeded the maximum or minimum velocities respectively.

	switch(dir){
	case 0 : if(!col_det(x, y, dx, dy, engine)) x+=dx; break; //Just doing basic movement functions here. Make sure that the collisions are accounted for
	case 1 : if(!col_det(x, y, dx, dy, engine)) y+=dy; break;
	case 2 : if(!col_det(x, y, dx, dy, engine)) x+=dx; break;
	case 3 : if(!col_det(x, y, dx, dy, engine)) y+=dy; break;
	default : decay();
	}
	decay();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//walk()
//PURPOSE: Play a walking sound for every movement. This should get annoying
//TODO: Actually code it
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::walk(){
	if(sound_tick == 40){
		al_play_sample(find_sound("walking", true).sample, 1, ALLEGRO_AUDIO_PAN_NONE, 1, ALLEGRO_PLAYMODE_ONCE, NULL); //Lets see if this works
		sound_tick = 0;
	}
	else{ sound_tick++; }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_tile()
//PURPOSE: Return private player tile
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile player::get_player_tile(){
	return player_tile;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//decay()
//PURPOSE: Private function to slowly decrease the velocity of the player, makes things feel more natural
//movement-wise. We don't just instantly stop.
//TODO: Do I need to do more? Might have to.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::decay(){
	dx *= 0.8;
	dy *= 0.8;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//END PLAYER CLASS!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MORE FUNCTION DECLERATIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
level loadlvl(string filename, game engine);
void loadfonts(game engine);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//processmap(tile_map tmp)
//PURPOSE: Take the raw_data section of a tile_map variable and parse/interpret it into formatted_data
//TODO: Potential cleanup. So far, working well
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile_map processmap(tile_map tmp){
	string temp_s;
	bool ender = false;
	int increment = 1;

	for(int i = 0; i < tmp.raw_data.length(); i++){
		if(tmp.raw_data.at(i) == '-'){
			while(!ender){
				if(i+increment >= tmp.raw_data.length())
					break;
				if(tmp.raw_data.at(i+increment) == '+')
					ender = true; //Trigger the stop if we reach our next delimit
				else{
					temp_s.push_back(tmp.raw_data.at(i+increment));
					}
				increment++;
			}
			tmp.formatted_data.push_back(atoi(temp_s.c_str()));
			temp_s = "";
			increment = 1;
			ender = false;
		}
	}
	cout << "Finished processing.\n";
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadmap(string filename)
//PURPOSE: To load a map config file from the file-system and return the map
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile_map loadmap(string filename, game engine){
	tile_map tmpmap; //The temporary map for processing
	engine.map_cfg = al_load_config_file(filename.c_str()); //Load the specified file
	if(!engine.map_cfg){
		cout << "No map: " + filename + " found! Aborting...\n";
		return tmpmap;
	}
	string map_data = al_get_config_value(engine.map_cfg, "map_data", "m"); //Not all of the map is being captured. FIXED? Yeah.

	int total_ent = atoi(al_get_config_value(engine.map_cfg, "data", "total_e"));

	string temp, name, type;
	int n, e, s, w;
	stringstream convert;
	cartes temp_pos;
	tile_entity temp_ent;

	n = atoi(al_get_config_value(engine.map_cfg, "map_data", "n"));
	e = atoi(al_get_config_value(engine.map_cfg, "map_data", "e"));
	s = atoi(al_get_config_value(engine.map_cfg, "map_data", "s"));
	w = atoi(al_get_config_value(engine.map_cfg, "map_data", "w"));

	tmpmap.directions[0] = n;
	tmpmap.directions[1] = e;
	tmpmap.directions[2] = s;
	tmpmap.directions[3] = w;

	//Starting entity detection/loading
	for(int i = 0; i < total_ent; i++){
		convert.clear();
		convert.str("");
		convert << i;
		temp = convert.str();

		temp_pos.x = atoi(al_get_config_value(engine.map_cfg, temp.c_str(), "x"));
		temp_pos.y = atoi(al_get_config_value(engine.map_cfg, temp.c_str(), "y"));

		name = al_get_config_value(engine.map_cfg, temp.c_str(), "name");
		type = al_get_config_value(engine.map_cfg, temp.c_str(), "type");

		temp_ent.name = name;
		temp_ent.type = type;
		temp_ent.pos = temp_pos;

		tmpmap.entities.push_back(temp_ent); //Add this particular entity to the stack
	}

	tmpmap.raw_data = map_data;
	cout << "Processing map " + filename + "\n";
	return processmap(tmpmap);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadentities()
//PURPOSE: Read the entity rcg file and save all values to the entity catalogue
//TODO: Add more flags and shit. Bitches love flags.
//CURRENT FLAGS:
//	name (the name of the entity)
//	type (the type-set of the entity)
//	tile (the associated tile-id)
//	col (does it collide?)
//	move (can it be moved?)
//	hurtfact (how much the object hurts players)
//	dxc (cap of dx)
//	dyc (cap of dy)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadentities(game engine){
	int total = atoi(al_get_config_value(engine.entity_cfg, "data", "total")); //Grab our meta-data data section again
	int tile, temp_i;
	string pos, name, type, temp;
	bool collide, move;
	float hurt, dxc, dyc;
	entity* temp_entity;
	stringstream convert;

	for(int i = 0; i < total; i++){
		convert.clear();
		pos = "";
		temp_i = i + 1;
		convert.str("");
		convert << temp_i;
		pos = convert.str();

		name = al_get_config_value(engine.entity_cfg, pos.c_str(), "name");
		type = al_get_config_value(engine.entity_cfg, pos.c_str(), "type");
		tile = atoi(al_get_config_value(engine.entity_cfg, pos.c_str(), "tile"));
		temp = al_get_config_value(engine.entity_cfg, pos.c_str(), "col");
		
		if(temp == "t")
			collide = true;
		if(temp == "f")
			collide = false;

		temp = al_get_config_value(engine.entity_cfg, pos.c_str(), "move");

		if(temp == "t")
			move = true;
		if(temp == "f")
			move = false;

		hurt = atoi(al_get_config_value(engine.entity_cfg, pos.c_str(), "hurtfact"));
		dxc = atoi(al_get_config_value(engine.entity_cfg, pos.c_str(), "dxc"));
		dyc = atoi(al_get_config_value(engine.entity_cfg, pos.c_str(), "dyc"));

		temp_entity = new entity(0, 0, 0, 0, dxc, dyc, type, collide, move, hurt, 0, tile, name, engine);
		engine.entity_cat.push_back(temp_entity); //Add our temp entity to the stack
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadtiles()
//PURPOSE: Loads PNG tiles from file-system as per the already loaded tile config file (now a vector)
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadtiles(game engine){
	//Starting the automated file shit here
	int start, end;
	string data;
	
	data = al_get_config_value(engine.tile_cfg, "data", "start");
	start = atoi(data.c_str());

	data = al_get_config_value(engine.tile_cfg, "data", "end");
	end = atoi(data.c_str());
	cout << "\n\nTotal textures: " + (end+1);
	cout << "\n";

	string curpos;
	string filename;
	string action;
	string registry;
	vector<int> check_id;
	int id, check_end;
	stringstream convert;
	bool collide;
	string temp;
	string temp_int;
	tile temp_tile;
	check_end = atoi(al_get_config_value(engine.tiledec_cfg, "data", "total"));
	registry = al_get_config_value(engine.tiledec_cfg, "data", "registry");
	//This bit of code will detect which tiles are to be flagged as utility tiles
	for(int i = 0; i < registry.size(); i++){
		convert.clear();
		convert.str("");
		convert << i;
		temp = "";

		temp = convert.str();
		if(registry.at(i) == ','){
			check_id.push_back(atoi(temp_int.c_str()));
			temp_int = "";
		}
		else{
			temp_int.push_back(registry.at(i));
		}
	}
	for(int i = start; i < (end+1); i++){
		convert.clear();
		convert.str("");
		convert << i;
		curpos = convert.str();
		temp = "";
		filename = "";
		action = "";
		id = NULL; //Nullify all variables, just in case. This is loading, we can take a while with this.

		filename = al_get_config_value(engine.tile_cfg, curpos.c_str(), "file");
		id = atoi(al_get_config_value(engine.tile_cfg, curpos.c_str(), "id"));
		temp = al_get_config_value(engine.tile_cfg, curpos.c_str(), "col");
		

		if(temp == "t")
			collide = true;
		if(temp == "f")
			collide = false;

		temp_tile.image = al_load_bitmap(filename.c_str());
		temp_tile.id = id;
		temp_tile.collide = collide;

		for(int j = 0; j < check_id.size(); j++){
			if(atoi(curpos.c_str()) == check_id[j]){
				temp_tile.action = al_get_config_value(engine.tiledec_cfg, curpos.c_str(), "action");
			}
		}
		if(temp_tile.action == "")
			temp_tile.action = "nothing";


		engine.tile_reg.push_back(temp_tile); //Push back the temporary tile data to our registry

		fprintf(stdout, "\n");
		fprintf(stdout, al_get_config_value(engine.tile_cfg, curpos.c_str(), "id"));
		fprintf(stdout, "\n");
		fprintf(stdout, filename.c_str());
		cout << "\n" + temp + "\nAction:";
		cout << temp_tile.action;
		cout << "\n";
	}
	fprintf(stdout, "\n\n\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//rng(int cap)
//PURPOSE: Main global random number generator for engine. Returns floats.
//TODO: Refine generation so as to allow quick calling (upwards of 40 hertz) without repetition
//(or serious repetition). Also, making it truly random is going to be necesarry.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
float rng(int cap){
	int return_value;

	return_value = rand();
	return_value = return_value*32/cap + 5;
	return_value = return_value % cap;
	
	return return_value;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_total_sound_cat_count(string type)
//PURPOSE: To determine the amount of sounds in a particular category
//TODO: Make the name shorter? *Nah. This should be fine. I like descriptive function names.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_total_sound_cat_count(string type){
	int return_value = 0;

	for(int i = 0; i < audio_reg.size(); i++){
		if(audio_reg[i].type == type)
			return_value++;
	}

	return return_value;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_sound(int id, string type)
//PURPOSE: Gets sound at that id and with that type
//TODO: Nothing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
sound get_sound(int id, string type){
	for(int i = 0; i < audio_reg.size(); i++){
		if(audio_reg[i].type == type && audio_reg[i].id == id)
			return audio_reg[i];
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_sound(string type, bool random)
//PURPOSE: To find a sound with the given information. Returns the first found type, unless a rand is specified
//TODO: Work out RNG
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
sound find_sound(string type, bool random){
	if(random){
		float randomizer = rng(get_total_sound_cat_count(type)); //Find the amount of category sounds, and then set that to cap
		return get_sound(randomizer, type);
	}
	if(!random){
		return get_sound(0, type); //Find just the first one in the registry
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadworld()
//PURPOSE: To load the main world information from the main world config file
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadworld(game engine){
	if(!engine.level_cfg)
		fprintf(stderr, "Level cfg not found!\n");

	loadtiles(engine); //Load all of the tiles following the skeletal loading above
	loadentities(engine); //Load all of the entities into the catalogue
	loadfonts(engine); //Load all of the fonts to be used in the engine

	string display_t;

	engine.tile_w = atoi(al_get_config_value(engine.level_cfg, "global", "w"));
	display_t = engine.tile_w;
	engine.tile_h = atoi(al_get_config_value(engine.level_cfg, "global", "h"));
	display_t = engine.tile_h;
	
	engine.level_w = atoi(al_get_config_value(engine.level_cfg, "levels", "lw"));
	engine.level_h = atoi(al_get_config_value(engine.level_cfg, "levels", "lh"));

	world tempwrld;
	string file;
	stringstream convert;

	for(int i = 0; i < (engine.level_w*engine.level_h); i++){
		convert.clear();
		convert.str("");
		convert << i + 1; //Make sure we have no zeroes
		file = "levels/";
		file += convert.str();
		file += ".lvl"; //Set the filename to i and the extension to our level extension (.lvl)
		tempwrld.levels.push_back(loadlvl(file, engine));
	}

	engine.globalWorld = tempwrld; //Set the global world to this scope's world
	
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_entity(string type, string name)
//PURPOSE: Return the entity with the provided type and name
//TODO: Streamline the entity storage process?
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
entity* find_entity(string type, string name, game engine){
	for(int i = 0; i < engine.entity_cat.size(); i++){
		if(engine.entity_cat[i]->getname() == name && engine.entity_cat[i]->gettype() == type){
			return engine.entity_cat[i]; //We've found our entity. Return it.
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_entity(int id)
//PURPOSE: Return the entity with the specified id
//TODO: Actually code this
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player* get_player_entity(int id, game engine){
	return engine.player_reg.at(id); //Return the player at the specified id
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_controlled_player()
//PURPOSE: Helper function for think(). Automagically finds the controlled player in the entity stack
//TODO: Optimize code, these for loops can't be good for performance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player* find_controlled_player(game engine){
	for(int i = 0; i < engine.player_reg.size(); i++){
		switch(get_player_type_id(i, engine)){
		case 0 : return engine.player_reg[i]; break;
		case 1 : break;
		default : cout << "No player characters found! Can this even happen?\n";
		}
	}
	cout << "Error if this is being displayed. find_controlled_player() couldn't find a controlled player\n";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_type_id(int id)
//PURPOSE: Return the type id for the player in the registry at id
//TODO: Work out a quick and efficient means of getting the data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_player_type_id(int id, game engine){
	return engine.player_reg[id]->get_type();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//create_nonplayer_entity(int x, int y, tile tl)
//PURPOSE: Called dynamically to create a non-player entity
//TODO: Code, etc.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool create_nonplayer_entity(int x, int y, tile tl, game engine){
	player* temp = new player(x, y, tl, 3, 3, 1, 15); //We set type to one, as this is npcharacter
	if(temp){
		engine.player_reg.push_back(temp);
		return true; //If everything worked, we can add it to the stack and return true
	}
	return false; //If everything else completely fails, we will return false
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//create_entity(...)
//PURPOSE: To create a new entity based on the supplied template, at that particular point. This shouldn't need
//to be called for now, unless someone wants to spontaneously create an entity. Maybe this will come into play
//later.
//int inx, int iny, int dxcap, int dycap, string type, bool col, bool move, float hurt, int id, string name
//TODO: Keep shit up to date
bool create_entity(int x, int y, string type, string name, bool col, bool move, float hurt, int dycap, int dxcap, int id, game engine){
	entity* temp;
	temp = new entity(x, y, 0, 0, dxcap, dycap, type, col, move, hurt, 0, id, name, engine);
	engine.entity_reg.push_back(temp);
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//create_player_entity(int x, int y, tile tl)
//PURPOSE: Called dynamically to create a player entity
//TODO: Code, etc.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool create_player_entity(int x, int y, tile tl, game engine){
	player* temp = new player(x, y, tl, 5, 5, 0, 15); //We set type to zero, as this is player controlled
	if(temp){
		engine.player_reg.push_back(temp);
		return true; //If everything worked, we can add it to the stack and return true
	}
	return false; //If everything else completely fails, we will return false
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_lvl(level lvl)
//PURPOSE: Loads the level provided to the global, current level
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_lvl(level lvl, game engine){
	engine.curlvl = lvl; //Just something simple to clear some clutter
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_map(tile_map map)
//PURPOSE: Sets the provided map as the global current map for the engine
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_map(tile_map map, game engine){
	engine.curmap = map; //Also clearing clutter
	if(engine.entity_reg.size() == NULL){
		//Do nothing
	}
	if(engine.entity_reg.size() != NULL){
		for(int i = 0; i < engine.entity_reg.size(); i++){
			engine.entity_reg.pop_back(); //Remove all the entities currently in the registry...
		}
	}
	for(int i = 0; i < map.entities.size(); i++){
		engine.entity_reg.push_back(find_entity(map.entities[i].type, map.entities[i].name, engine)); //This will pull the entity data out of the map and find it in the catalogue.
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadfonts()
//PURPOSE: To load all fonts into the font registry for usage in displaying text
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadfonts(game engine){
	int total = atoi(al_get_config_value(engine.font_cfg, "data", "total"));
	string path = al_get_config_value(engine.font_cfg, "data", "path");
	cout << "\nTotal Fonts: " + total;
	string finalpath;
	string file;
	stringstream converter;
	string cur_id;
	int size;
	string name;

	//Starting loop
	for(int j = 0; j < total; j++){
		file = "";
		cur_id = "";
		converter.clear();
		converter.str("");
		converter << j;
		cur_id = converter.str();

		cout << "\nLoading font " + cur_id + "\n";

		file = al_get_config_value(engine.font_cfg, cur_id.c_str(), "file");
		name = al_get_config_value(engine.font_cfg, cur_id.c_str(), "name");

		size = atoi(al_get_config_value(engine.font_cfg, cur_id.c_str(), "size"));

		finalpath = path + file;

		cout << "Name: " + name + "\n";
		cout << "Path: " + finalpath + "\n";
		cout << "Size: " + size;
		cout << "\n";

		engine.font_reg.push_back(al_load_font(finalpath.c_str(), size, NULL)); //Might add support for flags later. They aren't terribly important

		cout << "Font loaded succesfully.\n";
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadlvl(string filename)
//PURPOSE: To load a level from the provided file path
//TODO: Possibly optimization. I'm sure this is too messy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
level loadlvl(string filename, game engine){
	ALLEGRO_CONFIG *temp_level_cfg = al_load_config_file(filename.c_str());
	if(!temp_level_cfg)
		cout << "Error loading level!\n";
	string curpos;
	string curfile;
	int id = atoi(al_get_config_value(temp_level_cfg, "data", "start"));
	int w = atoi(al_get_config_value(temp_level_cfg, "data",  "w"));
	int h = atoi(al_get_config_value(temp_level_cfg, "data", "h"));
	stringstream convert;
	level templvl;
	for(int i = id; i < (w*h + id); i++){
		convert.clear();
		convert.str("");
		convert << i + 1;
		curpos = "";
		curfile = ""; //Finished initializing variables (more like sterilizing)
		curpos = convert.str();
		curfile = "maps/" + curpos + ".mp"; //Get the map filename

		engine.temp_lvls = al_load_config_file(curfile.c_str());
		
		templvl.maps.push_back(loadmap(curfile.c_str(), engine));
	}
	al_destroy_config(temp_level_cfg); //Make sure we cleanup here.
	return templvl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//move_player(int dir)
//PURPOSE: Move all the player entities in the registry as per the direction
//TODO: Improve performance by putting the player entities further up in the stack
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_player(int dir, game engine){
	find_controlled_player(engine)->move(dir, engine);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//move_map(int dir)
//PURPOSE: Move the referenced map NSE or W and preserve the location of the player (or change dependent on what
//is happening). It will seek out the corresponding portal in the next map (N to S, E to W, etc.)
//TODO: Work out how to actually move fluidly between the two (maybe an animation?)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_map(int dir, game engine){
	switch(dir){
	case 0 : 
		if(engine.curmap.directions[0] != NULL){
			set_map(engine.curlvl.maps[engine.curmap.directions[0] - 1], engine); //This should grab the next map from our level set. Should.
			find_controlled_player(engine)->setpos(find_tile(22, engine).x, find_tile(22, engine).y - engine.tile_px);
		} break;
	case 1 :
		if(engine.curmap.directions[1] != NULL){
			set_map(engine.curlvl.maps[engine.curmap.directions[1] - 1], engine);
			find_controlled_player(engine)->setpos(find_tile(23, engine).x + engine.tile_px, find_tile(23, engine).y);
		} break;
	case 2 :
		if(engine.curmap.directions[2] != NULL){
			set_map(engine.curlvl.maps[engine.curmap.directions[2] - 1], engine);
			find_controlled_player(engine)->setpos(find_tile(20, engine).x, find_tile(20, engine).y + engine.tile_px);
		} break;
	case 3 :
		if(engine.curmap.directions[3] != NULL){
			set_map(engine.curlvl.maps[engine.curmap.directions[3] - 1], engine);
			find_controlled_player(engine)->setpos(find_tile(21, engine).x - engine.tile_px, find_tile(21, engine).y);
		} break;
	default : cout << "There is no map referenced in current map.\n";
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//do_tile_actions()
//PURPOSE: To automatically perform all regulated tile actions (as they are specified by tiledec.rcg)
//These actions, of course, are only the startup actions. No more than that.
//TODO:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void do_tile_actions(game engine){
	for(int y = 0; y < engine.tile_h; y++){
		for(int x = 0; x < engine.tile_w; x++){
			if(get_raw_tile((y*engine.tile_w)+x, engine).action == "spawn_player") //Spawn the player
				create_player_entity(x, y, engine.tile_reg[5], engine);
			if(get_raw_tile((y*engine.tile_w)+x, engine).action == "spawn_enemy") //SPawn the enemy
				create_nonplayer_entity(x, y, engine.tile_reg[6], engine);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//update_players()
//PURPOSE: To update all registered players present in a scenario
//TODO: Implement a registry based system for the players, then update them independently based
//on what form of player they are (player or non-player)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void update_players(game engine){
	for(int i = 0; i < engine.player_reg.size(); i++){
		switch(get_player_type_id(i, engine)){
		case 0 : engine.player_reg[i]->decay(); break;
		case 1 : engine.player_reg[i]->think(find_controlled_player(engine), engine); break; //Call our helper function for this random npc
		default : ; //Do nothing
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_tile(int pos)
//PURPOSE: Return the integer id of the tile located at the linear position provided
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_tile(int pos, game engine){ 
	return engine.curmap.formatted_data.at(pos); //Hopefully this new system will streamline things a bit
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_raw_reg_tile(int pos)
//PURPOSE: To pick out a tile from the registry based on tile id
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile get_raw_reg_tile(int pos, game engine){
	return engine.tile_reg[pos];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_raw_tile(int pos)
//PURPOSE: To simply pick out an entire tile from the tile registry (based on map)
//TODO: Nothing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile get_raw_tile(int pos, game engine){
	return engine.tile_reg[engine.curmap.formatted_data.at(pos)]; //This should work. Whoops, can't directly access tile_reg
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_image(int id)
//PURPOSE: To retrieve (in ALLEGRO_BITMAP form) the specified tile with the given id from the tile registry
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
ALLEGRO_BITMAP* get_image(int id, game engine){
	return engine.tile_reg[id].image;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_tile(int id)
//PURPOSE: Helper function to find a specific tile id in the current map
//TODO: Not sure. Perhaps clean-up?
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Used primarily for things like spawn location and stuff like that.
//Could be used for pathing eventually, if my AI ever gets that in depth.
cartes find_tile(int id, game engine){
	cartes temp;
	for(int x = 0; x < engine.tile_w; x++){
		for(int y = 0; y < engine.tile_h; y++){
			if(get_tile((engine.tile_w*y)+x, engine) == id){
				temp.x = x*engine.tile_px; //Make sure our points are converted to pixels rather than tiles
				temp.y = y*engine.tile_px;
				}
			}
		}
	return temp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw_map()
//PURPOSE: Render the current map to the screen
//TODO: Remove anything NOT RELATED to maps. This means the characters and entities. These should
//be handled (technically) by their own update function. Work on this next.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw_map(game engine){
	for(int x=0; x < engine.tile_w; x++){
		for(int y=0; y < engine.tile_h; y++){
			al_draw_bitmap(get_image(get_tile((engine.tile_w*y)+x, engine), engine), x*32, y*32, 0); //Minor fix here. Originally reliant on a variable, now actually reliant on a constant
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw_entities()
//PURPOSE: Draw out all non-map (dynamic) geometry
//TODO: Finished for now
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw_entities(game engine){
	//Player registry being printed to screen
	for(int i = 0; i < engine.player_reg.size(); i++){
		al_draw_bitmap(engine.player_reg[i]->get_player_image(), engine.player_reg[i]->getx(), engine.player_reg[i]->gety(), 0);
		//This should make the player entity drawing independent. No more variables dictating this stuff
	}
	if(engine.entity_reg.size() == NULL){
		//Do nothing
	}
	else{
		for(int i = 0; i < engine.entity_reg.size(); i++){
			al_draw_bitmap(engine.entity_reg[i]->get_entity_tile(), engine.entity_reg[i]->getx(), engine.entity_reg[i]->gety(), 0);
			//This will draw an entity at the specified point with specified position
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw_text()
//PURPOSE: Render all text currently active for this stage
//TODO: Finish coding this section
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw_text(game engine){

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw()
//PURPOSE: Automates all the draw functions with one function call
//TODO: Case by case rendering? (Possiblity to exclude certain aspects?)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw(game engine){
	update_players(engine); //Update the current states of the player characters present in player_reg
	draw_map(engine); //Draw the base map geometry
	draw_entities(engine); //Draw the entities on top of the base geometry
	draw_text(engine); //Draw the HUD text and everything else
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//apply_main_config()
//PURPOSE: To open and interpret the various config data points from config.ini
//TODO: Add options as UI is formed
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void apply_main_config(game engine){
	//Setting the screen resolution as per the configuration file
	engine.SCREEN_W = atoi(al_get_config_value(engine.config_ld, "SCREENRES", "w"));
	engine.SCREEN_H = atoi(al_get_config_value(engine.config_ld, "SCREENRES", "h"));
	engine.FPS = atoi(al_get_config_value(engine.config_ld, "FPS", "f"));

	fprintf(stdout, "Screen width: ");
	fprintf(stdout, al_get_config_value(engine.config_ld, "SCREENRES", "w"));
	fprintf(stdout, "\n");
	fprintf(stdout, "Screen height: ");
	fprintf(stdout, al_get_config_value(engine.config_ld, "SCREENRES", "h"));
	fprintf(stdout, "\n");
	fprintf(stdout, "FPS: ");
	fprintf(stdout, al_get_config_value(engine.config_ld, "FPS", "f"));
	fprintf(stdout, "\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//load_samples()
//PURPOSE: Load all samples pointed to from the audio registry specified by the audio config file
//TODO: Re-do the config system. Remember, paths do not need a root specifier in front, it is implied
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void load_samples(game engine){
	int total_cat = atoi(al_get_config_value(engine.sound_cfg, "data", "total"));
	int samples = atoi(al_get_config_value(engine.sound_cfg, "data", "samples"));
	cout << "Total Samples: " + samples;
	al_reserve_samples(samples);
	string category;
	string path;
	int cat_samples;
	string filename;
	string file;
	stringstream convert;
	stringstream converter;
	string cur_cat;
	string temp_i;
	sound temp;

	//Starting loop
	for(int j = 0; j < total_cat; j++){
		category = "";
		cur_cat = "";
		converter.clear();
		converter.str("");
		converter << j + 1;
		cur_cat = converter.str();
		category = al_get_config_value(engine.sound_cfg, "data", cur_cat.c_str()); //Start off with the first specified value
		cat_samples = atoi(al_get_config_value(engine.sound_cfg, category.c_str(), "end"));
		path = al_get_config_value(engine.sound_cfg, category.c_str(), "path");
		for(int i = 0; i < cat_samples; i++){
			convert.clear();
			convert.str("");
			convert << i;
			temp_i = convert.str();
			file = al_get_config_value(engine.sound_cfg, category.c_str(), temp_i.c_str());
			filename = path + file; //Combine the path with the filename

			temp.sample = al_load_sample(filename.c_str()); //Load the sample to our temporary sound
			temp.type = category; //Save the category type
			temp.id = i; //Also put down the general id for this category
			audio_reg.push_back(temp); //And add this to the stack

			cout << temp_i + "\n";

			if(audio_reg[i].sample){
				cout << "Loaded sample: " + filename + "\n";
			}
			else{ cout << "Failed to load sample: " + filename + "\n"; }
		}
	}	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//GENERIC HELPER FUNCTIONS!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//round(double d)
//PURPOSE: To return a rounded double of the input
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
double round(double d){
	return floor(d + 0.5);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Initialization and cleanup functions
int init_engine(game engine){
	srand(time(NULL));

	al_init_font_addon();

	if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }
   else{ fprintf(stdout, "Allegro initialized!\n"); }

   if(!al_init_image_addon()){
	   fprintf(stderr, "failed to initialize image addon!\n");
	   return -2;
   }
   else{ fprintf(stdout, "Allegro Image addon initialized!\n"); }

   if(!al_install_keyboard()){
	   fprintf(stderr, "failed to install keyboard!\n");
	   return -3;
   }
   else{ fprintf(stdout, "Keyboard initialized!\n"); }

   if(!al_install_audio()){
	   fprintf(stderr, "failed to install audio subsystem!\ncontinuing with standard load\n");
   }
   else{ fprintf(stdout, "Audio installed!\n"); }

   if(!al_init_acodec_addon()){
	   fprintf(stderr, "failed to install audio subsystem!\ncontinuing with standard load\n");
   }
   else{ fprintf(stdout, "Audio codecs installed!\n"); }

   if(!engine.config_ld){
	   fprintf(stderr, "failed to load config file! setting to defaults...\n");
   }
   else{
	   apply_main_config(engine); fprintf(stdout, "Main config file loaded!\n");
   }

   if(!engine.tile_cfg){
	   fprintf(stderr, "Failed to load tile config file!\n");
	   al_rest(5);
	   return -4;
   }
   else { fprintf(stdout, "Tile file loaded!\n"); }

   if(!engine.map_cfg){
	   fprintf(stderr, "Failed to load map!\n");
	   return -5;
   } else
   { fprintf(stdout, "Map file loaded!\n"); }

   if(!engine.tiledec_cfg){
	   fprintf(stderr, "Failed to load tile decleration config file!\n");
	   return -6;
   } else { cout << "Tile decleration file loaded!\n"; }

   if(!engine.entity_cfg){
	   fprintf(stderr, "Failed to load entity config file!\n");
	   return -11;
   } else { cout << "Entity decleration file loaded!\n"; }
 
   engine.display = al_create_display(engine.SCREEN_W, engine.SCREEN_H);
   if(!engine.display) {
      fprintf(stderr, "failed to create display!\n");
      return -12;
   }
   else{ fprintf(stdout, "Display initialized!\n"); }

   engine.evt_q = al_create_event_queue();
   if(!engine.evt_q){
	   al_destroy_display(engine.display);
	   fprintf(stderr, "failed to initialize event queue!\n");
	   return -7;
   }
   else{ fprintf(stdout, "Event queue initialized!\n"); }

   engine.timer = al_create_timer(1.0 / engine.FPS);
   if(!engine.timer){
	   al_destroy_display(engine.display);
	   al_destroy_event_queue(engine.evt_q);
	   fprintf(stderr, "failed to initialize timer!\n");
	   return -8;
   }
   else{ fprintf(stdout, "Timer initialized!\n"); }

  if(!engine.sound_cfg){
	  cout << "Fuck, the sound config isn't loading. FUCK.\n";
	  return -10;
  } else { cout << "Sound config file loaded!\n"; load_samples(engine); }

  loadworld(engine);
   set_lvl(engine.globalWorld.levels[0], engine);
   set_map(engine.curlvl.maps[0], engine);
   return 0;
}

void clean_up(game engine){
   al_destroy_timer(engine.timer);
   al_destroy_event_queue(engine.evt_q);
   al_destroy_display(engine.display);
   al_destroy_config(engine.config_ld);
   al_destroy_config(engine.tile_cfg);
   al_destroy_config(engine.map_cfg);
   al_destroy_config(engine.level_cfg);
   al_destroy_config(engine.temp_lvls);
   al_destroy_config(engine.tiledec_cfg);
   al_destroy_config(engine.entity_cfg);

   al_uninstall_audio();
   al_uninstall_keyboard();
   al_uninstall_system();
   al_shutdown_font_addon();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//col_det(float x, float y, float dx, float dy)
//PURPOSE: Return true if the next tick will place the current position within the collision region of a
//"stopping" object (e.g. a wall, a barricade, etc). Return false if the next tick will place the provided
//position within a non-colliding region (e.g. a floor)
//TODO: Clean things up
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool col_det(float x, float y, float dx, float dy, game engine){
	int tile_y = round((y+dy)/engine.tile_px);
	int tile_x = round((x+dx)/engine.tile_px);
	int temp = engine.curmap.formatted_data.at(tile_y*engine.tile_w + tile_x);
	tile temp_tile = get_raw_tile((tile_y*engine.tile_w)+tile_x, engine);

	if(temp_tile.action == "move_north"){
		move_map(0, engine);
		return engine.tile_reg[temp].collide;
	}
	if(temp_tile.action == "move_east"){
		move_map(1, engine);
		return engine.tile_reg[temp].collide;
	}
	if(temp_tile.action == "move_south"){
		move_map(2, engine);
		return engine.tile_reg[temp].collide;
	}
	if(temp_tile.action == "move_west"){
		move_map(3, engine);
		return engine.tile_reg[temp].collide;
	}
	
	return engine.tile_reg[temp].collide;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PRE-MAIN DECLARATIONS DONE!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//main(int argc, char **argv)
//PURPOSE: Duh.
//TODO: If I implement anything, it remains todo.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	bool redraw = true;
	bool doexit = false;
	bool key[4] = {false, false, false, false};

	game engine;
   
	char error_status = init_engine(engine);

   al_set_target_bitmap(al_get_backbuffer(engine.display));

   al_register_event_source(engine.evt_q, al_get_display_event_source(engine.display)); //Register event source from display
   al_register_event_source(engine.evt_q, al_get_timer_event_source(engine.timer)); //Register event source from timer
   al_register_event_source(engine.evt_q, al_get_keyboard_event_source()); //Register the keyboard for event sources
 
   al_clear_to_color(al_map_rgb(0,0,0));
 
   al_flip_display();

   al_start_timer(engine.timer);

   do_tile_actions(engine);

   while(!doexit){

	   ALLEGRO_EVENT ev; //Event variable for this scope
	   ALLEGRO_TIMEOUT timeout; //Timeout variable for refresh of event queue
	   al_init_timeout(&timeout, 0.06); //Set timeout to 60 milliseconds (60 Hz)

	   bool get_event = al_wait_for_event_until(engine.evt_q, &ev, &timeout); //Shorten the al_wait_for_event_until(...) to something a bit more manageable

	   if(ev.type == ALLEGRO_EVENT_TIMER){ //Basic ping from timer
		   if(key[engine.KEY_LEFT])
			   move_player(0, engine);
		   if(key[engine.KEY_RIGHT])
			   move_player(2, engine);
		   if(key[engine.KEY_UP])
			   move_player(1, engine);
		   if(key[engine.KEY_DOWN])
			   move_player(3, engine);

		   redraw = true;
	   }
	   else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
		   switch(ev.keyboard.keycode){
		   case ALLEGRO_KEY_UP : key[engine.KEY_UP] = true; break;
		   case ALLEGRO_KEY_DOWN : key[engine.KEY_DOWN] = true; break;
		   case ALLEGRO_KEY_LEFT : key[engine.KEY_LEFT] = true; break;
		   case ALLEGRO_KEY_RIGHT : key[engine.KEY_RIGHT] = true; break;
		   default : ;
		   }
	   }
	   else if(ev.type == ALLEGRO_EVENT_KEY_UP){
		   switch(ev.keyboard.keycode){
		   case ALLEGRO_KEY_UP : key[engine.KEY_UP] = false; break;
		   case ALLEGRO_KEY_DOWN : key[engine.KEY_DOWN] = false; break;
		   case ALLEGRO_KEY_LEFT : key[engine.KEY_LEFT] = false; break;
		   case ALLEGRO_KEY_RIGHT : key[engine.KEY_RIGHT] = false; break;
		   case ALLEGRO_KEY_ESCAPE : doexit = true; break;
		   default : ;
		   }
	   }
	   else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
		   break;
	   }

	   if(redraw && al_is_event_queue_empty(engine.evt_q)){
		al_clear_to_color(al_map_rgb(0,0,0)); //Clear the foreground (kinda expensive)
		redraw = false; //Make sure we don't redraw again till the next 60 frames
		draw(engine);
		al_flip_display(); //Bring buffer up
	   }


   }
 
	clean_up(engine);
 
   return 0;
}