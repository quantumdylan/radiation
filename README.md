RADIATION ENGINE v1.0.1

Top-down, tile-based game engine, designed to be reusable for multiple projects (flexible
loading of tile images, maps, sounds, etc.)

Built with the 5.0.10 build of Allegro

This is still in incredibly early development (started 1/08/2014), so don't expect much for now.

Written and compiled using MSVC++2012 framework, will port once stability and usability is achieved.

README!
This is still incredibly unstable and has nigh on no functionality at the moment. For now, the Debug folder
contains all the executables and resources necessary to run the Radiation engine.

As of yet, there is no real way to "install" the engine, as it is still in its debug phase. On the first main
release, binaries will be introduced for easy installation.

For now, running the engine takes a few steps. These will be explained after a breakdown of the file-system is
presented.

Explanation of file-hierarchy:
root/ - Contains executable and necessary libraries
	/tiles - Contains tiles referenced by tiles.tl (*.png)
	/maps - Contains ALL maps to be loaded into levels (*.mp)
	/levels - Contains ALL level declaration files (*.lvl)
	/ui - Contains various engine resources for the GUI
		/fonts - Contains fonts used by the engine (*.ttf)
	/bin-ext - DEPRECATED (contains unused libraries for Allegro)
	/sounds - Contains ALL level sounds and their declaration files (*.wav)
	
As long as this hierarchy is kept constant, the engine will work. I wil be implementing a "mod"
folder, to make it easier to drop in custom content without overwriting engine files.

Explanation of file types:
	*.mp - The main map file type. Built using the ALLEGRO_CONFIG structure, these files contain all the data
			necessary for display and interpretation of a map. The map file itself is composed of raw map data,
			in the form of a series of nodes separated by delimiters
	*.tl - The main tile declaration file type. These config files contain references to all of the tiles placed
			in the tile folder, as per the hierarchy above. The files consist of several nodes, each with a file path
			and a numerical identifier. These will be dynamically loaded by the engine into the tile registry, which
			will then be read to access the images during execution. The images MUST be in PNG format, and generally
			must remain at a pixel size of 32x32.
	*.lvl - The main level file type. These config files are used to "grab" a range of maps and arrange them into a grid
			of maps. Each map, in this sense, constitutes one "room", and the level can be likened to an entire "floor".
			In this way, it allows the user to move between rooms without having to read the disk again, helpful for
			slow hard-drives.
	*.sd - The main sound system config file type. This file will be used to point the engine to the correct paths
			for various sounds, such as walking sounds, melee sounds, etc. It is also used to setup the audio system
			at load time.
			
Now, to use the engine is rather tricky. With the currently provided tools, it is possible to make a single room in which
the player can walk around in. To do this, you will need the Gamma Editor, which is the map editing software I've made along
side this project. The current compiled version can be found on my github page.

After outputting the map you have made (follow the instructions on Gamma), it is now time to start implementing that file
into the level/map system. To do so, rename map.mp to 1.mp. The file-system reads all map and level files as numbers in
sequence. Place this into the /maps/ folder. As of now, the 1.lvl file in /levels/ will be adequate to run the engine as is.
Ensure that the map you've made does not go out of the bounds of the tiles that are declared, as using tiles that don't exist
currently results in a crash. Keep this in mind, if the engine loads everything and crashes on display, it is most likely a
tile error.

This should do it. Remember, the player will collide with anything that isn't 0 (floor), so plan accordingly.

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