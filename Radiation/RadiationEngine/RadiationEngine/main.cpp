//RADIATION ENGINE v1.0.1
//New this version:
//Just about everything.
//No seriously, the first version could do one thing: tile one fucking image
//Well, that, and also send an animated square bouncing around the screen, controllable
//partially via the arrow keys (modified velocity)
//Now, for the actual features currently implemented:
//Basic map/tile loading (dynamic tiles, static maps (for now))
//Basic sounds (only one for the moment)
//Basic player class (for both user controlled and non-player characters)
// Player class allows for both user-controlled movements (just move()) and for autonomous
// (think() and act()) movements
// The player class also allows for automatic collision detection, storage of a sprite as the
// visual representation of said player, and dynamic creation of new players (allowing for an
// eventual "rogue-like" enemy spawning system to be implemented)
//Scalable screen resolutions (though the tiles DO NOT scale yet)
//Fixed FPS (changeable through config.ini)
//Dynamic real-time event system (via the event queueing system in Allegro)

//TODO:
//Change the tile id system (currently has only 10 empty slots for tile textures)
//Actually implement (fully) the level/map systems (for loading maps based off of level[x].lvl)
//Fully implement audio system (multi-streams, mixers, etc...)
//Flesh out AI in the player class
//Fix collision detection (fix the hitboxes and such)
//Add hitboxes to player characters (for projectile/melee stuff)
//Add audio config file (for dynamic loading of sounds and such)
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

using namespace std;

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
ALLEGRO_CONFIG *level_cfg = al_load_config_file("levels.lvl"); //Loads first level config file
ALLEGRO_CONFIG *map_cfg = al_load_config_file("map.mp"); //Loads first map file
ALLEGRO_CONFIG *tile_cfg = al_load_config_file("tiles.tl"); //Load the tile loading configuration file
ALLEGRO_CONFIG *sound_cfg = al_load_config_file("sounds.sd"); //Load the main audio config file
ALLEGRO_CONFIG *temp_lvls; //Temp level config file holder
///////////////////////////////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN STRUCTURE DECLARATION!
struct tile{ //Basic tile declaration
	ALLEGRO_BITMAP* image; //The image referenced for each tile
	int id; //Image ref-id. Used for determing whether or not there are tile effects applied
	bool collide; //Determines whether or not the tile has collision on or off (t/f)
};

struct tile_map{ //Basic map declaration
	string raw_data; //Really, this fucking vector thing is being a bitch. I'm not down with dynamic
	vector<int> formatted_data; //I'm hoping this works. I really need it to.
	//So far this new vector is working fine, no problems with push_back()
	//I am now going to attempt to completely change the way that input is processed by the engine
	int id; //Map ref-id. Used to determine which floor is being displayed
	string title; //Title of the map being displayed
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

struct cartes{ //Helper to return two values simultaneously
	float x; //X-pos variable
	float y; //Y-pos variable
};

struct sound{ //Basic sound declaration
	ALLEGRO_SAMPLE *sample; //The actual sound loaded from file
	string type; //Category type from the config file
	int id; //Keeping in line with the other declarations
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


vector<sound> audio_reg; //Basic audio registry for sounds


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPING!
bool col_det(float x, float y, float dx, float dy);
level loadlvl(string filename);
cartes find_tile(int id);
double round(double d);
int get_player_type_id(int id);
int get_tile(int pos);
sound find_sound(string type, bool random);
float rng(int cap);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAIN PLAYER CLASS DEFINITION!
//Basically, I wan't to be able to create player characters
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
	player(int inx, int iny, tile player_tl, int dxc, int dyc, int type);
	//~player();
	cartes get_pos();
	int getx();
	int gety();
	int get_type();
	void move(int dir); 
	tile get_player_tile();
	ALLEGRO_BITMAP* get_player_image();
	void change_vel(int mag, int id);
	void think(player* play); //These two functions will be used for autonomous systems later on. Think npcs.
	void decay();
private:
	tile player_tile;
	float x, y, dx, dy;
	int dxcap, dycap;
	int player_type, sound_tick;

	void cap_vel();
	void act();
	void find_spawn();
	void walk();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Player constructor
//PURPOSE: To create a new player entity, determine its starting stats and such
//TODO: I don't know if anything else needs to be included
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player::player(int inx, int iny, tile player_tl, int dxc, int dyc, int type){
	//Initialize the values either to null or the given values
	x = inx;
	y = iny;
	player_tile = player_tl;
	dx = 0;
	dy = 0;
	dxcap = dxc;
	dycap = dyc;
	player_type = type;
	sound_tick = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_spawn()
//PURPOSE: To cycle through all of the tiles in the loaded map to determine the player's spawn
//TODO: Nothing, that I can tell. Probably wrong, though
//WARNING! This code may now be deprecated. Pending removal.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void player::find_spawn(){
	cartes temp;
	switch(player_type){
	case 0 : temp = find_tile(18); break;
	case 1 : temp = find_tile(19); break;
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
void player::think(player* play){
	//This is where we can put code to automate the npc movement
	//For now, I'm just going to have them do simple pathing
	//Eventually, I hope to integrate the A* pathing algortihm
	//to this. But that will take quite some time.

	if(x < play->getx()){
		move(2);
		if(dx <= 1 && dx >= -1){
			move(0);
			dx = 0;
			if(y < play->gety())
				move(3);
			if(y > play->gety())
				move(1);
		}
	}
	if(x > play->getx()){
		move(0);
		if(dx <= 1 && dx >= -1){
			move(2);
			dx = 0;
			if(y < play->gety())
				move(3);
			if(y > play->gety())
				move(1);
		}	
	}
	if(y < play->gety()){
		move(3);
		if(dy <= 1 && dy >= -1){
			move(1);
			dy = 0;
			if(x < play->getx())
				move(2);
			if(x > play->getx())
				move(0);
		}
	}
	if(y > play->gety()){
		move(1);
		if(dy <= 1 && dy >= -1){
			move(3);
			dy = 0;
			if(x < play->getx())
				move(2);
			if(x > play->getx())
				move(0);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//act()
//PURPOSE: To carry out the preperations of think()
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
void player::move(int dir){
	change_vel(2, dir);
	cap_vel(); //Make sure that we have not exceeded the maximum or minimum velocities respectively.

	switch(dir){
	case 0 : if(!col_det(x, y, dx, dy)) x+=dx; walk(); break; //Just doing basic movement functions here. Make sure that the collisions are accounted for
	case 1 : if(!col_det(x, y, dx, dy)) y+=dy; walk(); break;
	case 2 : if(!col_det(x, y, dx, dy)) x+=dx; walk(); break;
	case 3 : if(!col_det(x, y, dx, dy)) y+=dy; walk(); break;
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
//GLOBAL VARIABLE DECLARATION!
tile_map curmap; //Memory-loaded map and level variables, something static (roughly) so as to decrease lag and shit
level curlvl;

world globalWorld; //The global world variable for the entire game

vector<level> lvl_reg; //Registry for levels and shite, just raw levels and their predetermined ids

vector<player*> player_reg; //Registry of memory addresses for player entities //Hehe, titties

vector<tile> tile_reg; //Registry of tiles for displaying

//tile tile_reg[100]; //There is space for 100 unique tile entities with this
//Can we make this one into a vector? Please, Dylan? Seriously, this shit is NOT going to work in the
//long run, nor is it professional. Come on, fix it soon.
//Alright, alright, you irritating little bitch. I've fixed it for you.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
	cout << "Finished processing map...\n";
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadmap(string filename)
//PURPOSE: To load a map config file from the file-system and return the map
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tile_map loadmap(string filename){
	tile_map tmpmap; //The temporary map for processing
	map_cfg = al_load_config_file(filename.c_str()); //Load the specified file
	if(!map_cfg){
		cout << "No map: " + filename + " found! Aborting...\n";
		return loadmap("map.mp"); //Load our backup map file, though it will not work for now.
	}
	string map_data = al_get_config_value(map_cfg, "map_data", "m"); //Not all of the map is being captured. FIXED?

	tmpmap.raw_data = map_data;
	return processmap(tmpmap);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadtiles()
//PURPOSE: Loads PNG tiles from file-system as per the already loaded tile config file (now a vector)
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadtiles(){
	//Starting the automated file shit here
	int start, end;
	string data;
	
	data = al_get_config_value(tile_cfg, "data", "start");
	start = atoi(data.c_str());

	data = al_get_config_value(tile_cfg, "data", "end");
	end = atoi(data.c_str());
	fprintf(stdout, "\n\nTotal textures: %i\n", (end+1));

	string curpos;
	string filename;
	int id;
	stringstream convert;
	bool collide;
	string temp;
	tile temp_tile;
	for(int i = start; i < (end+1); i++){
		convert.clear();
		convert.str("");
		convert << i;
		curpos = convert.str();
		temp = "";
		filename = "";
		id = NULL; //Nullify all variables, just in case. This is loading, we can take a while with this.

		filename = al_get_config_value(tile_cfg, curpos.c_str(), "file");
		id = atoi(al_get_config_value(tile_cfg, curpos.c_str(), "id"));
		temp = al_get_config_value(tile_cfg, curpos.c_str(), "col");

		if(temp == "t")
			collide = true;
		if(temp == "f")
			collide = false;

		temp_tile.image = al_load_bitmap(filename.c_str());
		temp_tile.id = id;
		temp_tile.collide = collide;

		//tile_reg[id].image = al_load_bitmap(filename.c_str());
		//tile_reg[id].id = id;
		//tile_reg[id].collide = collide;

		tile_reg.push_back(temp_tile);

		fprintf(stdout, "\n");
		fprintf(stdout, al_get_config_value(tile_cfg, curpos.c_str(), "id"));
		fprintf(stdout, "\n");
		fprintf(stdout, filename.c_str());
		cout << "\n" + temp + "\n";
	}
	fprintf(stdout, "\n\n\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//rng(int cap)
//PURPOSE: Main global random number generator for engine. Returns floats.
//TODO: Refine generation so as to allow quick calling (upwards of 40 hertz) without repetition
//(or serious repetition)
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
void loadworld(){
	if(!level_cfg)
		fprintf(stderr, "Level cfg not found!\n");

	string display_t;

	tile_w = atoi(al_get_config_value(level_cfg, "global", "w"));
	display_t = tile_w;
	tile_h = atoi(al_get_config_value(level_cfg, "global", "h"));
	display_t = tile_h;
	
	level_w = atoi(al_get_config_value(level_cfg, "levels", "lw"));
	level_h = atoi(al_get_config_value(level_cfg, "levels", "lh"));

	world tempwrld;
	string file;
	stringstream convert;

	for(int i = 0; i < (level_w*level_h); i++){
		convert.clear();
		convert.str("");
		convert << i + 1; //Make sure we have no zeroes
		file = "levels/";
		file += convert.str();
		file += ".lvl"; //Set the filename to i and the extension to our level extension (.lvl)
		tempwrld.levels.push_back(loadlvl(file));
	}

	globalWorld = tempwrld; //Set the global world to this scope's world
	
    loadtiles(); //Load all of the tiles following the skeletal loading above
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_entity(int id)
//PURPOSE: Return the entity with the specified id
//TODO: Actually code this
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player* get_player_entity(int id){
	return player_reg.at(id); //Return the player at the specified id
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_controlled_player()
//PURPOSE: Helper function for think(). Automagically finds the controlled player in the entity stack
//TODO: Optimize code, these for loops can't be good for performance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
player* find_controlled_player(){
	for(int i = 0; i < player_reg.size(); i++){
		switch(get_player_type_id(i)){
		case 0 : return player_reg[i]; break;
		case 1 : break;
		default : cout << "No player characters found! Can this even happen?\n";
		}
	}
	cout << "Error if this is being displayed. find_controlled_player() couldn't find a controlled player\n";
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_player_type_id(int id)
//PURPOSE: Return the type id for the player in the registry at id
//TODO: Work out a quick and efficient means of getting the data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_player_type_id(int id){
	return player_reg[id]->get_type();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//create_nonplayer_entity(int x, int y, tile tl)
//PURPOSE: Called dynamically to create a non-player entity
//TODO: Code, etc.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool create_nonplayer_entity(int x, int y, tile tl){
	player* temp = new player(x, y, tl, 3, 3, 1); //We set type to one, as this is npcharacter
	if(temp){
		player_reg.push_back(temp);
		return true; //If everything worked, we can add it to the stack and return true
	}
	return false; //If everything else completely fails, we will return false
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//create_player_entity(int x, int y, tile tl)
//PURPOSE: Called dynamically to create a player entity
//TODO: Code, etc.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool create_player_entity(int x, int y, tile tl){
	player* temp = new player(x, y, tl, 5, 5, 0); //We set type to zero, as this is player controlled
	if(temp){
		player_reg.push_back(temp);
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
void set_lvl(level lvl){
	curlvl = lvl; //Just something simple to clear some clutter
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//set_map(tile_map map)
//PURPOSE: Sets the provided map as the global current map for the engine
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void set_map(tile_map map){
	curmap = map; //Also clearing clutter
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//loadlvl(string filename)
//PURPOSE: To load a level from the provided file path
//TODO: Possibly optimization. I'm sure this is too messy
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
level loadlvl(string filename){
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

		temp_lvls = al_load_config_file(curfile.c_str());
		
		templvl.maps.push_back(loadmap(curfile.c_str()));
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
void move_player(int dir){
	find_controlled_player()->move(dir);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//spawn_players()
//PURPOSE: To spawn specified NPCs at predetermined spawn points
//TODO: Work on code
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void spawn_players(){
	for(int y = 0; y < tile_h; y++){
		for(int x = 0; x < tile_w; x++){
			switch(get_tile((y*tile_w)+x)){
			case 18 : create_player_entity(x, y, tile_reg[5]); break; //Spawn a player
			case 19 : create_nonplayer_entity(x, y, tile_reg[6]); break; //Spawn a NPC
			default : ; //Do nothing
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//update_players()
//PURPOSE: To update all registered players present in a scenario
//TODO: Implement a registry based system for the players, then update them independently based
//on what form of player they are (player or non-player)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void update_players(){
	for(int i = 0; i < player_reg.size(); i++){
		switch(get_player_type_id(i)){
		case 0 : player_reg[i]->decay(); break;
		case 1 : player_reg[i]->think(find_controlled_player()); break; //Call our helper function for this random npc
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
int get_tile(int pos){ 
	return curmap.formatted_data.at(pos); //Hopefully this new system will streamline things a bit
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//get_image(int id)
//PURPOSE: To retrieve (in ALLEGRO_BITMAP form) the specified tile with the given id from the tile registry
//TODO: Finished
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
ALLEGRO_BITMAP* get_image(int id){
	return tile_reg[id].image;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//find_tile(int id)
//PURPOSE: Helper function to find a specific tile id in the current map
//TODO: Not sure. Perhaps clean-up?
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Used primarily for things like spawn location and stuff like that.
//Could be used for pathing eventually, if my AI ever gets that in depth.
cartes find_tile(int id){
	cartes temp;
	for(int x = 0; x < tile_w; x++){
		for(int y = 0; y < tile_h; y++){
			if(get_tile((tile_w*y)+x) == id){
				temp.x = x*tile_px; //Make sure our points are converted to pixels rather than tiles
				temp.y = y*tile_px;
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
void draw_map(){
	for(int x=0; x < tile_w; x++){
		for(int y=0; y < tile_h; y++){
			al_draw_bitmap(get_image(get_tile((tile_w*y)+x)), x*32, y*32, 0); //Minor fix here. Originally reliant on a variable, now actually reliant on a constant
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw_entities()
//PURPOSE: Draw out all non-map (dynamic) geometry
//TODO: Finished for now
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw_entities(){
	//Player registry being printed to screen
	for(int i = 0; i < player_reg.size(); i++){
		al_draw_bitmap(player_reg[i]->get_player_image(), player_reg[i]->getx(), player_reg[i]->gety(), 0);
		//This should make the player entity drawing independent. No more variables dictating this stuff
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//draw()
//PURPOSE: Automates all the draw functions with one function call
//TODO: Case by case rendering? (Possiblity to exclude certain aspects?)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw(){
	update_players(); //Update the current states of the player characters present in player_reg
	draw_map(); //Draw the base map geometry
	draw_entities(); //Draw the entities on top of the base geometry
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//apply_main_config()
//PURPOSE: To open and interpret the various config data points from config.ini
//TODO: Add options as UI is formed
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void apply_main_config(){
	//Setting the screen resolution as per the configuration file
	SCREEN_W = atoi(al_get_config_value(config_ld, "SCREENRES", "w"));
	SCREEN_H = atoi(al_get_config_value(config_ld, "SCREENRES", "h"));
	FPS = atoi(al_get_config_value(config_ld, "FPS", "f"));

	fprintf(stdout, "Screen width: ");
	fprintf(stdout, al_get_config_value(config_ld, "SCREENRES", "w"));
	fprintf(stdout, "\n");
	fprintf(stdout, "Screen height: ");
	fprintf(stdout, al_get_config_value(config_ld, "SCREENRES", "h"));
	fprintf(stdout, "\n");
	fprintf(stdout, "FPS: ");
	fprintf(stdout, al_get_config_value(config_ld, "FPS", "f"));
	fprintf(stdout, "\n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//load_samples()
//PURPOSE: Load all samples pointed to from the audio registry specified by the audio config file
//TODO: Re-do the config system. Remember, paths do not need a root specifier in front, it is implied
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void load_samples(){
	int total_cat = atoi(al_get_config_value(sound_cfg, "data", "total"));
	int samples = atoi(al_get_config_value(sound_cfg, "data", "samples"));
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
		category = al_get_config_value(sound_cfg, "data", cur_cat.c_str()); //Start off with the first specified value
		cat_samples = atoi(al_get_config_value(sound_cfg, category.c_str(), "end"));
		path = al_get_config_value(sound_cfg, category.c_str(), "path");
		for(int i = 0; i < cat_samples; i++){
			convert.clear();
			convert.str("");
			convert << i;
			temp_i = convert.str();
			file = al_get_config_value(sound_cfg, category.c_str(), temp_i.c_str());
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
int init_engine(){
	srand(time(NULL));

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

   if(!config_ld){
	   fprintf(stderr, "failed to load config file! setting to defaults...\n");
   }
   else{
	   apply_main_config(); fprintf(stdout, "Main config file loaded!\n");
   }

   if(!tile_cfg){
	   fprintf(stderr, "Failed to load tile config file!\n");
	   al_rest(5);
	   return -4;
   }
   else { fprintf(stdout, "Tile file loaded!\n"); }

   if(!map_cfg){
	   fprintf(stderr, "Failed to load map!\n");
	   return -5;
   }
   { fprintf(stdout, "Map file loaded!\n"); }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -6;
   }
   else{ fprintf(stdout, "Display initialized!\n"); }

   evt_q = al_create_event_queue();
   if(!evt_q){
	   al_destroy_display(display);
	   fprintf(stderr, "failed to initialize event queue!\n");
	   return -7;
   }
   else{ fprintf(stdout, "Event queue initialized!\n"); }

   timer = al_create_timer(1.0 / FPS);
   if(!timer){
	   al_destroy_display(display);
	   al_destroy_event_queue(evt_q);
	   fprintf(stderr, "failed to initialize timer!\n");
	   return -8;
   }
   else{ fprintf(stdout, "Timer initialized!\n"); }

  if(!sound_cfg){
	  cout << "Fuck, the sound config isn't loading. FUCK.\n";
	  return -10;
  } else { cout << "Sound config file loaded!\n"; load_samples(); }

  loadworld();
   set_lvl(globalWorld.levels[0]);
   set_map(curlvl.maps[0]);
}

void clean_up(){
   al_destroy_timer(timer);
   al_destroy_event_queue(evt_q);
   al_destroy_display(display);
   al_destroy_config(config_ld);
   al_destroy_config(tile_cfg);
   al_destroy_config(map_cfg);
   al_destroy_config(level_cfg);
   al_destroy_config(temp_lvls);

   al_uninstall_audio();
   al_uninstall_keyboard();
   al_uninstall_system();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//col_det(float x, float y, float dx, float dy)
//PURPOSE: Return true if the next tick will place the current position within the collision region of a
//"stopping" object (e.g. a wall, a barricade, etc). Return false if the next tick will place the provided
//position within a non-colliding region (e.g. a floor)
//TODO: Clean things up
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool col_det(float x, float y, float dx, float dy){
	int tile_y = round((y+dy)/tile_px);
	int tile_x = round((x+dx)/tile_px);
	int temp = curmap.formatted_data.at(tile_y*tile_w + tile_x);

	return tile_reg[temp].collide; //Completely eliminate case statements with extra meta-data!
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
   
	char error_status = init_engine();

   al_set_target_bitmap(al_get_backbuffer(display));

   al_register_event_source(evt_q, al_get_display_event_source(display)); //Register event source from display
   al_register_event_source(evt_q, al_get_timer_event_source(timer)); //Register event source from timer
   al_register_event_source(evt_q, al_get_keyboard_event_source()); //Register the keyboard for event sources
 
   al_clear_to_color(al_map_rgb(0,0,0));
 
   al_flip_display();

   al_start_timer(timer);

   spawn_players();

   //TESTING CODE!
   


   while(!doexit){

	   ALLEGRO_EVENT ev; //Event variable for this scope
	   ALLEGRO_TIMEOUT timeout; //Timeout variable for refresh of event queue
	   al_init_timeout(&timeout, 0.06); //Set timeout to 60 milliseconds (60 Hz)

	   bool get_event = al_wait_for_event_until(evt_q, &ev, &timeout); //Shorten the al_wait_for_event_until(...) to something a bit more manageable

	   if(ev.type == ALLEGRO_EVENT_TIMER){ //Basic ping from timer
		   if(key[KEY_LEFT])
			   move_player(0);
		   if(key[KEY_RIGHT])
			   move_player(2);
		   if(key[KEY_UP])
			   move_player(1);
		   if(key[KEY_DOWN])
			   move_player(3);

		   redraw = true;
	   }
	   else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
		   switch(ev.keyboard.keycode){
		   case ALLEGRO_KEY_UP : key[KEY_UP] = true; break;
		   case ALLEGRO_KEY_DOWN : key[KEY_DOWN] = true; break;
		   case ALLEGRO_KEY_LEFT : key[KEY_LEFT] = true; break;
		   case ALLEGRO_KEY_RIGHT : key[KEY_RIGHT] = true; break;
		   default : ;
		   }
	   }
	   else if(ev.type == ALLEGRO_EVENT_KEY_UP){
		   switch(ev.keyboard.keycode){
		   case ALLEGRO_KEY_UP : key[KEY_UP] = false; break;
		   case ALLEGRO_KEY_DOWN : key[KEY_DOWN] = false; break;
		   case ALLEGRO_KEY_LEFT : key[KEY_LEFT] = false; break;
		   case ALLEGRO_KEY_RIGHT : key[KEY_RIGHT] = false; break;
		   case ALLEGRO_KEY_ESCAPE : doexit = true; break;
		   default : ;
		   }
	   }
	   else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
		   break;
	   }

	   if(redraw && al_is_event_queue_empty(evt_q)){
		al_clear_to_color(al_map_rgb(0,0,0)); //Clear the foreground (kinda expensive)
		redraw = false; //Make sure we don't redraw again till the next 60 frames
		draw();
		al_flip_display(); //Bring buffer up
	   }


   }
 
	clean_up();
 
   return 0;
}