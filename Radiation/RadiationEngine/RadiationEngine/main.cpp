//RADIATION ENGINE v1.0.0
#include <stdio.h>
#include <string>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std;

int tile_w, tile_h;

float FPS = 60;
int BOUNCER_SIZE = 32;
int SCREEN_W = 640;
int SCREEN_H = 480;
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT};

ALLEGRO_DISPLAY *display = NULL; //Basic display pointer
ALLEGRO_EVENT_QUEUE *evt_q = NULL; //Basic event queue
ALLEGRO_TIMER *timer = NULL; //Basic timer pointer
ALLEGRO_BITMAP *bouncer = NULL; //Testing out bitmaps and shite here
ALLEGRO_CONFIG *config_ld = al_load_config_file("config.ini"); //Main config for engine
ALLEGRO_CONFIG *level_cfg = al_load_config_file("levels.lvl"); //Loads first level config file
ALLEGRO_CONFIG *map_cfg = al_load_config_file("map1.mp"); //Loads first map file
ALLEGRO_CONFIG *tile_cfg = al_load_config_file("tiles.tl");
ALLEGRO_MIXER *main_mix; //Main mixer for audio system
ALLEGRO_VOICE *audio_hw; //= al_create_voice(440, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2); //Main voice for audio hardware
ALLEGRO_AUDIO_STREAM *a_stream; //Main audio stream
ALLEGRO_FILE *file_handler; //Main file pointer

bool audio_active = false;

struct tile{ //Basic tile declaration
	ALLEGRO_BITMAP* image; //The image referenced for each tile
	int id; //Image ref-id. Used for determing whether or not there are tile effects applied
};

struct tile_map{ //Basic map declaration
	string raw_data; //Really, this fucking vector thing is being a bitch. I'm not down with dynamic
	int id; //Map ref-id. Used to determine which floor is being displayed
	string title; //Title of the map being displayed
};

struct level{ //Basic level declaration
	tile_map lvl[10][10]; //10x10 grid of maps which make up the entire level 
	int id; //Level ref-id. Used to determine which level is being displayed
	string title; //Title of level being used
};

tile_map curmap; //Memory-loaded map and level variables, something static (roughly) so as to decrease lag and shit
level curlvl;

tile tile_reg[100]; //There is space for 100 unique tile entities with this

//TEMPORARY CODE TO TEST OUT LEVEL/MAP/TILE SYSTEM!
tile_map map1;
level level1;
//END TEMPORARY CODE!

void loadmap(){
	string map_data = al_get_config_value(map_cfg, "map_data", "m"); //Not all of the map is being captured. FIXED?
	//int w = atoi(al_get_config_value(map_cfg, "map_data", "w"));
	//int h = atoi(al_get_config_value(map_cfg, "map_data", "h"));

	//tile_w = w;
	//tile_h = h;

	for(int i = 0; i < (tile_w*tile_h); i++){
		curmap.raw_data.push_back(map_data.at(i));
	}

	fprintf(stdout, curmap.raw_data.c_str());
}

void loadtiles(){
	string filename = al_get_config_value(tile_cfg, "0", "file");
	int id = atoi(al_get_config_value(tile_cfg, "0", "id"));

	tile_reg[id].image = al_load_bitmap(filename.c_str());
	tile_reg[id].id = id;

	filename = "";
	id = NULL;

	//This will be replaced with an automated system that cycles through all entries.
	//This is just for testing purposes
	filename = al_get_config_value(tile_cfg, "1", "file");
	id = atoi(al_get_config_value(tile_cfg, "1", "id"));

	tile_reg[id].image = al_load_bitmap(filename.c_str());
	tile_reg[id].id = id;
}

void set_lvl(level lvl){
	curlvl = lvl; //Just something simple to clear some clutter
}

//THIS FUNCTION MUST BE CALLED FIRST! IT SETS THE GLOBAL VARIABLES THAT ALL THE OTHER FUNCTIONS NEED TO
//OPERATE PROPERLY!!!
void loadlvl(){ //Wierd bug has been fixed within loadlvl(). It was a simple misalignment of function calls.
	if(!level_cfg)
		fprintf(stderr, "Level cfg not found!\n");

	string display_t;

	tile_w = atoi(al_get_config_value(level_cfg, "global", "w"));
	display_t = tile_w;
	fprintf(stdout, display_t.c_str());
	tile_h = atoi(al_get_config_value(level_cfg, "global", "h"));
	display_t = tile_h;
	fprintf(stdout, display_t.c_str());
	int id = atoi(al_get_config_value(level_cfg, "levelone", "start"));
	display_t = id;
	fprintf(stdout, display_t.c_str());

	switch(id){
	case 0 : set_lvl(level1); break;
	}
}

int get_tile(int pos){
	int temp;
	
	char temp_s = (int)curmap.raw_data.at(pos);
	switch(temp_s){
	case '0' : temp = 0; break;
	case '1' : temp = 1; break;
	default : temp = 0;
	}

	return temp;
}

ALLEGRO_BITMAP* get_image(int id){
	tile tl;

	tl = tile_reg[id];

	//fprintf(stdout, "%i", id);

	return tl.image;
}


void draw_map(){
	for(int x=0; x < tile_w; x++){
		for(int y=0; y < tile_h; y++){
			al_draw_bitmap(get_image(get_tile((tile_w*y)+x)), x*32, y*32, 0); //Minor fix here. Originally reliant on a variable, now actually reliant on a constant
		}
	}
}


void apply_main_config(){
	//Setting the screen resolution as per the configuration file
	SCREEN_W = atoi(al_get_config_value(config_ld, "SCREENRES", "w"));
	SCREEN_H = atoi(al_get_config_value(config_ld, "SCREENRES", "h"));
	FPS = atoi(al_get_config_value(config_ld, "FPS", "f"));
	BOUNCER_SIZE = atoi(al_get_config_value(config_ld, "TESTING", "bouncer_s"));

	
	fprintf(stdout, al_get_config_value(config_ld, "SCREENRES", "w"));
	fprintf(stdout, "\n");
	fprintf(stdout, al_get_config_value(config_ld, "SCREENRES", "h"));
	fprintf(stdout, "\n");
	fprintf(stdout, al_get_config_value(config_ld, "FPS", "f"));
	fprintf(stdout, "\n");
	fprintf(stdout, al_get_config_value(config_ld, "TESTING", "bouncer_s"));
	fprintf(stdout, "\n");
	fprintf(stdout, al_get_config_value(tile_cfg, "0", "id"));
	fprintf(stdout, "\n");
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
   else{ audio_active = true; fprintf(stdout, "Audio installed!\n"); }

   if(!al_init_acodec_addon()){
	   fprintf(stderr, "failed to install audio subsystem!\ncontinuing with standard load\n");
   }
   else{ audio_active = true; fprintf(stdout, "Audio codecs installed!\n"); }

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

   //This is still just for testing purposes. Potential for a top down bullet-hell?
   bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
   if(!bouncer){
	   al_destroy_display(display);
	   al_destroy_event_queue(evt_q);
	   al_destroy_timer(timer);
	   fprintf(stderr, "failed to initialize basic bitmap!\n");
	   return -9;
   }
}

int main(int argc, char **argv)
{
	bool redraw = true;
	bool doexit = false;
	bool key[4] = {false, false, false, false};
   
	char error_status = init_engine();

   loadlvl();
   loadtiles(); //We need to call this first, to make sure that the tiles are loaded into memory
   loadmap();
   //set_lvl(level1);

   al_reserve_samples(1); //Reserve the sample amounts for the main mixer

   //TEMPORARY CODE TO TEST MOVING SPRITE REDRAW AND IMAGE BITMAPPING!
   float bouncer_x = SCREEN_W/2.0 - BOUNCER_SIZE/2.0;
   float bouncer_y = SCREEN_H/2.0 - BOUNCER_SIZE/2.0;
   float bouncer_dx = 4.0; float bouncer_dy = -4.0;
   //END TEMPORARY CODE!

   //TEMPORARY CODE TO MAKE THE PRETTY PURPLE BLOCK!
   al_set_target_bitmap(bouncer);
   al_clear_to_color(al_map_rgb(125,65,215));
   //END TEMPORARY CODE!

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
		   if(bouncer_x < 0 || bouncer_x > SCREEN_W - BOUNCER_SIZE){
			   bouncer_dx *= -1;
		   }
		   if(bouncer_y < 0 || bouncer_y > SCREEN_H - BOUNCER_SIZE){
			   bouncer_dy *= -1;
		   }

		   if(key[KEY_UP] && bouncer_dy > -6){
			   bouncer_dy -= 1;
		   }
		   if(key[KEY_DOWN] && bouncer_dy < 6){
			   bouncer_dy += 1;
		   }
		   if(key[KEY_RIGHT] && bouncer_dx < 6){
			   bouncer_dx += 1;
		   }
		   if(key[KEY_LEFT] && bouncer_dx > -6){
			   bouncer_dx -= 1;
		   }

		   bouncer_x += bouncer_dx;
		   bouncer_y += bouncer_dy;
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
		draw_map(); //Call the custom map printing function
		al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0); //Actually draw out bitmap to the buffer
		//al_draw_bitmap(tile_reg[1].image, 100, 200, 0); //Does this fucking tile even exist?
		al_flip_display(); //Bring buffer up
	   }


   }
 
   al_destroy_timer(timer);
   al_destroy_event_queue(evt_q);
   al_destroy_display(display);
   al_destroy_bitmap(bouncer);
   al_destroy_mixer(main_mix);
   al_destroy_config(config_ld);

   al_uninstall_audio();
   al_uninstall_keyboard();
   al_uninstall_system();
 
   return 0;
}