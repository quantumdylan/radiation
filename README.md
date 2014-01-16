RADIATION ENGINE v1.0.1

Top-down, tile-based game engine, designed to be reusable for multiple projects (flexible
loading of tile images, maps, sounds, etc.)

Built with the 5.0.10 build of Allegro

This is still in incredibly early development (started 1/08/2014), so don't expect much for now.

Written and compiled using MSVC++2012 framework, will port once stability and usability is achieved.

README!
This is still incredibly unstable and has nigh on no functionality at the moment. For now, the Debug folder
contains all the executables and resources neccesarry to run the Radiation engine.

The engine runs using three config files:
	tiles.tl (the main list for the tile texture registry)
	levels.lvl (the main list of levels and their associated maps)
	map.mp (the main listing of maps)
	
It is neccesarry to note that tiles.tl IS modifiable, though you MUST follow the template provided as an example:
[int id]
id = an integer
file = /tiles/whatever.png

And also that the [data] section must be modified to accomodate this change ***(end MUST be equal to the final entries id)***

map.mp is generated using the Gamma Editor map editor. This can be found on my git as well. I've also supplied a generated
map for testing purposes if anyone wants to try it out

As for now, I have a basic player class created. The player class can be used for creation of both the user's player and also
non-player characters, or npc's. The player class movement is driven by a small snippet of code which is taken from the previous
"bouncer" test code. This function can be called whenever an update in input (ie the user) or from the think() function (for npcs)
occurs. It utilizes the col_det() function (which is globally available) to determine whether or not the character is colliding
with world geometry

All artwork at the moment is temporary and was made by myself in GIMP. The tiles are 32x32 pixels by default. I may add in scaling at some point
and the ability to set the size of the image dynamically via the config file.

The movement keys are currently (and permanently) mapped to the arrow keys. When I implement the menu system, I will allow for key remapping. But
as this is more of a cosmetic thing, I probably won't throw in remapping until much later, possibly at the same time that controller/joystick control
is implemented (hopefully).


TODO:
Audio system -> Very basically implemented.
Map loading/parsing -> Work on moving towards a different method of loading. At the moment, I can only use ten total tiles.
Menus/User interface
Custom content detection using config files
Cross-platform compatibility?
Collision detection -> better collision detection. Currently, it is very bare bones
Artwork (oh boy)
Gamepad/Joystick input? (maaaaaybeeee...)
Add in entity system (non-map geometry entities)
HUD
Inventory and items (AND GOOOLD!)