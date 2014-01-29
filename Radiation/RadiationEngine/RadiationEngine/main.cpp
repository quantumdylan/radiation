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
ALLEGRO_SAMPLE *sample_test; //Testing out the audio system

double round(double d){
	return floor(d + 0.5);
}

struct tile{ //Basic tile declaration
	ALLEGRO_BITMAP* image; //The image referenced for each tile
	int id; //Image ref-id. Used for determing whether or not there are tile effects applied
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
	tile_map lvl[10][10]; //10x10 grid of maps which make up the entire level DEPRECATED
	vector<tile_map> maps; //The store for the maps
	int id; //Level ref-id. Used to determine which level is being displayed
	string title; //Title of level being used
};

struct world{ //Basic world declaration
	vector<level> levels; //All the levels in the game
};

struct cartes{ 
	float x; //X-pos variable
	float y; //Y-pos variable
};

bool col_det(float x, float y, float dx, float dy);

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
	player(int inx, int iny, tile player_tl, int dxc, int dyc);
	//~player();
	cartes get_pos();
	int getx();
	int gety();
	void move(int dir); //
	tile get_player_tile();
	ALLEGRO_BITMAP* get_player_image();
	void change_vel(int mag, int id);
	void think(player* play); //These two functions will be used for autonomous systems later on. Think npcs.
	void decay();
private:
	tile player_tile;
	float x, y, dx, dy;
	int dxcap, dycap;

	void cap_vel();
	void act();
};

player::player(int inx, int iny, tile player_tl, int dxc, int dyc){
	//Initialize the values either to null or the given values
	x = inx;
	y = iny;
	player_tile = player_tl;
	dx = 0;
	dy = 0;
	dxcap = dxc;
	dycap = dyc;
}

void player::think(player* play){
	//This is where we can put code to automate the npc movement
	//For now, I'm just going to have them do simple pathing
	//Eventually, I hope to integrate the A* pathing algortihm
	//to this. But that will take quite some time.

	if(x < play->getx())
		move(2);
	if(x > play->getx())
		move(0);
	if(y < play->gety())
		move(3);
	if(y > play->gety())
		move(1);
}

void player::act(){

}

ALLEGRO_BITMAP* player::get_player_image(){
	return player_tile.image;
}

int player::getx(){
	return x;
}

int player::gety(){
	return y;
}

cartes player::get_pos(){
	cartes position;

	position.x = x;
	position.y = y;

	return position;
}

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

void player::change_vel(int mag, int id){
	switch(id){
	case 0 : dx -= mag; break;
	case 1 : dy -= mag; break;
	case 2 : dx += mag; break;
	case 3 : dy += mag; break;
	default : ;
	}
}

void player::move(int dir){
	change_vel(2, dir);
	cap_vel(); //Make sure that we have not exceeded the maximum or minimum velocities respectively.

	switch(dir){
	case 0 : if(!col_det(x, y, dx, dy)) x+=dx; break; //Just doing basic movement functions here. Make sure that the collisions are accounted for
	case 1 : if(!col_det(x, y, dx, dy)) y+=dy; break;
	case 2 : if(!col_det(x, y, dx, dy)) x+=dx; break;
	case 3 : if(!col_det(x, y, dx, dy)) y+=dy; break;
	default : decay();
	}
	decay();
}

tile player::get_player_tile(){
	return player_tile;
}

void player::decay(){
	dx *= 0.8;
	dy *= 0.8;
}

tile_map curmap; //Memory-loaded map and level variables, something static (roughly) so as to decrease lag and shit
level curlvl;

world globalWorld; //The global world variable for the entire game

vector<level> lvl_reg; //Registry for levels and shite, just raw levels and their predetermined ids

tile tile_reg[100]; //There is space for 100 unique tile entities with this
//Can we make this one into a vector? Please, Dylan? Seriously, this shit is NOT going to work in the
//long run, nor is it professional. Come on, fix it soon.

player* character; //Just creating this as a temporary
player* tempnpc; //Also a temporary npc entity

level loadlvl(string filename);

/*NOTE!
This code has been included as a part of my initiative to allow a large amount of tile data to be stored
in the tile_map system. The raw data (raw_data) is going to be the direct string streamed from the config
file, which will include both the data and the delimits as they are saved via Gamma (see map editor).
The post-process data (formatted_data) is a vector<int> containing all of the data points.
Perhaps in the future I will add in a specification for the tile_w and tile_h amounts, rather than having
them be constant for a game as per the levels.lvl file. This might be a better way of handling the input.
*/
//As of yet, this way seems to work not only more efficiently, but also opens up the amount of tiles to 
//as many as can be created by the content creator. These changes have since been made permanent.
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
					//curmap.formatted_data.push_back(atoi(temp_s.c_str()));
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
	for(int i = start; i < (end+1); i++){
		convert.clear();
		convert.str("");
		convert << i;
		curpos = convert.str();

		filename = al_get_config_value(tile_cfg, curpos.c_str(), "file");
		id = atoi(al_get_config_value(tile_cfg, curpos.c_str(), "id"));

		tile_reg[id].image = al_load_bitmap(filename.c_str());
		tile_reg[id].id = id;
		fprintf(stdout, "\n");
		fprintf(stdout, al_get_config_value(tile_cfg, curpos.c_str(), "id"));
		fprintf(stdout, "\n");
		fprintf(stdout, filename.c_str());
	}
	fprintf(stdout, "\n\n\n");
}

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

void set_lvl(level lvl){
	curlvl = lvl; //Just something simple to clear some clutter
}

void set_map(tile_map map){
	curmap = map; //Also clearing clutter
}

level loadlvl(string filename){
	ALLEGRO_CONFIG *temp_level_cfg = al_load_config_file(filename.c_str());
	if(!temp_level_cfg)
		cout << "Error loading level!\n";
	string curpos;
	string curfile;
	//string filename;
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

		//filename = al_get_config_value(tile_cfg, curpos.c_str(), "file");
		
		templvl.maps.push_back(loadmap(curfile.c_str()));
	}

	return templvl;
}

void update_players(){
	//Attempting to have the motion decay quickly, looks more natural or whatever
	character->decay(); //Okay, real talk. These are going to need to be automated, so implementing a registry
	//of npcs and players and shit is going to become imperative within the next month or so. You've been warned.
	tempnpc->think(character);
}

int get_tile(int pos){ //Work on converting to a parser, where a certain symbol will delimit what is each individual entry
	return curmap.formatted_data.at(pos); //Hopefully this new system will streamline things a bit
}

ALLEGRO_BITMAP* get_image(int id){
	return tile_reg[id].image;
}

void draw_map(){
	for(int x=0; x < tile_w; x++){
		for(int y=0; y < tile_h; y++){
			al_draw_bitmap(get_image(get_tile((tile_w*y)+x)), x*32, y*32, 0); //Minor fix here. Originally reliant on a variable, now actually reliant on a constant
		}
	}
	al_draw_bitmap(character->get_player_image(), character->getx(), character->gety(), 0); //Need to seperate this from the map drawing function.
	al_draw_bitmap(tempnpc->get_player_image(), tempnpc->getx(), tempnpc->gety(), 0);
}

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

void load_samples(){
	al_reserve_samples(1);
	sample_test = al_load_sample("walk01.wav");
	if(!sample_test){
		cout << "Fucking fuck, this shit didn't load. Check the load_samples() function. Stupid fucking program...\n";
	}
}

int init_engine(){
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
}

bool col_det(float x, float y, float dx, float dy){
	int tile_y = round((y+dy)/tile_px);
	int tile_x = round((x+dx)/tile_px);
	int temp = curmap.formatted_data.at(tile_y*tile_w + tile_x);

	if((x+dx > tile_w*tile_px) || (y+dx < tile_h*tile_px))
		return false;
	if((y+dy > tile_h*tile_px) || (y+dy < tile_h*tile_px))
		return false;

	switch(temp){
	case 0 : return false; break;
	default : return true;
	}
}

int main(int argc, char **argv)
{
	bool redraw = true;
	bool doexit = false;
	bool key[4] = {false, false, false, false};
   
	char error_status = init_engine();

   loadworld();
   set_lvl(globalWorld.levels[0]);
   set_map(curlvl.maps[0]);
   //loadmap(); //Working on moving this to the level loader. I want these to be more automated

   character = new player(55, 10, tile_reg[5], 5, 5);
   tempnpc = new player(44, 30, tile_reg[6], 2, 2);

   al_set_target_bitmap(al_get_backbuffer(display));

   al_register_event_source(evt_q, al_get_display_event_source(display)); //Register event source from display
   al_register_event_source(evt_q, al_get_timer_event_source(timer)); //Register event source from timer
   al_register_event_source(evt_q, al_get_keyboard_event_source()); //Register the keyboard for event sources
 
   al_clear_to_color(al_map_rgb(0,0,0));
 
   al_flip_display();

   al_start_timer(timer);

   while(!doexit){

	   ALLEGRO_EVENT ev; //Event variable for this scope
	   ALLEGRO_TIMEOUT timeout; //Timeout variable for refresh of event queue
	   al_init_timeout(&timeout, 0.06); //Set timeout to 60 milliseconds (60 Hz)

	   bool get_event = al_wait_for_event_until(evt_q, &ev, &timeout); //Shorten the al_wait_for_event_until(...) to something a bit more manageable

	   if(ev.type == ALLEGRO_EVENT_TIMER){ //Basic ping from timer
		   if(key[KEY_LEFT])
			   character->move(0);
		   if(key[KEY_RIGHT])
			   character->move(2);
		   if(key[KEY_UP])
			   character->move(1);
		   if(key[KEY_DOWN])
			   character->move(3);

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
		   update_players();
		al_clear_to_color(al_map_rgb(0,0,0)); //Clear the foreground (kinda expensive)
		redraw = false; //Make sure we don't redraw again till the next 60 frames
		draw_map(); //Call the custom map printing function
		al_flip_display(); //Bring buffer up
	   }


   }
 
   al_destroy_timer(timer);
   al_destroy_event_queue(evt_q);
   al_destroy_display(display);
   al_destroy_config(config_ld);

   al_uninstall_audio();
   al_uninstall_keyboard();
   al_uninstall_system();
 
   return 0;
}