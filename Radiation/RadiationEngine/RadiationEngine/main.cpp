#include <stdio.h>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

float FPS = 60;
int BOUNCER_SIZE = 32;
int SCREEN_W = 640;
int SCREEN_H = 480;
enum MYKEYS {KEY_UP, KEY_DOWN, KEY_RIGHT, KEY_LEFT};

ALLEGRO_DISPLAY *display = NULL; //Basic display pointer
ALLEGRO_EVENT_QUEUE *evt_q = NULL; //Basic event queue
ALLEGRO_TIMER *timer = NULL; //Basic timer pointer
ALLEGRO_BITMAP *bouncer = NULL; //Testing out bitmaps and shite here
ALLEGRO_BITMAP *image_tst = NULL; //Testing out images here
ALLEGRO_CONFIG *config_ld = al_load_config_file("config.ini"); //Main config for engine
ALLEGRO_MIXER *main_mix; //Main mixer for audio system
ALLEGRO_VOICE *audio_hw; //= al_create_voice(440, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2); //Main voice for audio hardware
ALLEGRO_AUDIO_STREAM *a_stream; //Main audio stream

struct tile{ //Basic tile declaration
	ALLEGRO_BITMAP* image; //The image referenced for each tile
	int id; //Image ref-id. Used for determing whether or not there are tile effects applied
};

tile map[10][10]; //Basic map setup, provides for tiles at specified locations. Similar to MoveGuy, but with sprites

void draw_map(){
	for(int x=0; x < 10; x++){
		for(int y=0; y < 10; y++){
			al_draw_bitmap(map[x][y].image, x*32, y*32, 0);
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
	fprintf(stdout, al_get_config_value(config_ld, "SCREENRES", "h"));
	fprintf(stdout, al_get_config_value(config_ld, "FPS", "f"));
}

int main(int argc, char **argv)
{
	bool redraw = true;
	bool doexit = false;
	bool key[4] = {false, false, false, false};
 
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

   if(!al_init_image_addon()){
	   fprintf(stderr, "failed to initialize image addon!\n");
	   return -1;
   }

   if(!al_install_keyboard()){
	   fprintf(stderr, "failed to install keyboard!\n");
	   return -1;
   }

   if(!al_install_audio()){
	   fprintf(stderr, "failed to install audio subsystem!\n");
	   return -1;
   }

   if(!al_init_acodec_addon()){
	   fprintf(stderr, "failed to install audio subsystem!\n");
   }

   if(!config_ld){
	   fprintf(stderr, "failed to load config file! setting to defaults...\n");
   }
   else{
	   apply_main_config();
   }
 
   display = al_create_display(SCREEN_W, SCREEN_H);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }

   evt_q = al_create_event_queue();
   if(!evt_q){
	   al_destroy_display(display);
	   fprintf(stderr, "failed to initialize event queue!\n");
	   return -1;
   }

   timer = al_create_timer(1.0 / FPS);
   if(!timer){
	   al_destroy_display(display);
	   al_destroy_event_queue(evt_q);
	   fprintf(stderr, "failed to initialize timer!\n");
	   return -1;
   }

   bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
   if(!bouncer){
	   al_destroy_display(display);
	   al_destroy_event_queue(evt_q);
	   al_destroy_timer(timer);
	   fprintf(stderr, "failed to initialize basic bitmap!\n");
	   return -1;
   }

   image_tst = al_load_bitmap("check.png");
   if(!image_tst){
	   al_destroy_display(display);
	   al_destroy_event_queue(evt_q);
	   al_destroy_timer(timer);
	   al_destroy_bitmap(bouncer);
	   fprintf(stderr, "failed to initialize image load!\n");
	   return -1;
   }

   //al_attach_mixer_to_voice(main_mix, audio_hw); //Set our mixer to the given hardware configuration

   al_reserve_samples(1); //Reserve the sample amounts for the main mixer


   //TEMPORARY CODE TO TEST MOVING SPRITE REDRAW AND IMAGE BITMAPPING!
   float bouncer_x = SCREEN_W/2.0 - BOUNCER_SIZE/2.0;
   float bouncer_y = SCREEN_H/2.0 - BOUNCER_SIZE/2.0;
   float bouncer_dx = 4.0; float bouncer_dy = -4.0;

   float image_x = SCREEN_W / 2.0;
   float image_y = SCREEN_H / 2.0;
   //END TEMPORARY CODE!

   //TEMPORARY CODE TO TEST OUT THE TILE STRUCTURE!
   tile tst_tile;
   tst_tile.image = image_tst;
   tst_tile.id = 0;
   //END TEMPORARY CODE!

   //TEMPORARY CODE TO POPULATE TEMP MAP!
   for(int x=0; x < 10; x++){
	   for(int y=0; y < 10; y++){
		   map[x][y] = tst_tile;
	   }
   }
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

	   /*if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){ //Check if any events have occurred in the past 60 milliseconds. If no, return false. If yes, check input.
		   break; //If it is closed, exit the game loop. No crashing this time.
	   }*///Older code, not really needed but left in just in case shit hits a fan somewhere

	   if(redraw && al_is_event_queue_empty(evt_q)){
		al_clear_to_color(al_map_rgb(0,0,0)); //Clear the foreground (kinda expensive)
		redraw = false; //Make sure we don't redraw again till the next 60 frames
		draw_map(); //Call the custom map printing function
		al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0); //Actually draw out bitmap to the buffer
		al_draw_bitmap(tst_tile.image, image_x, image_y, 0); //Draw out the image as well
		al_flip_display(); //Bring buffer up
	   }


   }
 
   al_destroy_timer(timer);
   al_destroy_event_queue(evt_q);
   al_destroy_display(display);
   al_destroy_bitmap(bouncer);
   al_destroy_bitmap(image_tst);
   al_destroy_mixer(main_mix);
   al_destroy_config(config_ld);

   al_uninstall_audio();
   al_uninstall_keyboard();
   al_uninstall_system();
 
   return 0;
}