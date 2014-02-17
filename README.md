RADIATION ENGINE v1.0.2

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
	/entities - Contains ALL entity declaration files
	
As long as this hierarchy is kept constant, the engine will work. I will be implementing a "mod"
folder, to make it easier to drop in custom content without overwriting engine files.

Explanation of file types:
	*.mp - The main map file type. Built using the ALLEGRO_CONFIG structure, these files contain all the data
			necessary for display and interpretation of a map. The map file itself is composed of raw map data,
			in the form of a series of nodes separated by delimiters
	*.lvl - The main level file type. These config files are used to "grab" a range of maps and arrange them into a grid
			of maps. Each map, in this sense, constitutes one "room", and the level can be likened to an entire "floor".
			In this way, it allows the user to move between rooms without having to read the disk again, helpful for
			slow hard-drives.
	*.rcg - The main config file extension. Any file with this extension is a base config file for the radiation engine
			and CANNOT be removed or changed without knowing exactly what you're doing. Changing these without cause
			or reason or the complete removal of these files will render the engine unusable.
	*.ent - The main entity file extension. A file with this extension provides data used to spawn in certain entities
			with predetermined properties
			
Explanation of file structures:
	*.mp - Being the map file, this file contains all the raw tile data needed to render the world geometry. This is stored
			under the map_data section, and is placed within the variable m. The identification number is set via id and the
			working title of the map is set under title.
	*.lvl - The level files are structured similarly to the map files, but without the m data. Instead, the level file will
			point the engine to a range of map files which belong to the level. It also declares the height and width of the
			level in maps, which is used to place the maps relative to each other in their rectangular grid.
	*.rcg - All rcg files contain data which is used to either load engine resources, or initialize certain variables used
			by the engine. Each file contains a [data] section, which provides global data for all the sections beneath it.
			Generally speaking, the [data] section will tell the engine how many things are going to be loaded, whilst the
			sections underneath [data] generally represent individual items that are to be loaded. Most often, every section
			will follow a simple numerical identifying scheme, with each section having an integer number as it's title.
	*.ent - The entity files are similar to model files for other game engines; they provide the engine with a material to be
			used as the rendered prop, they provide collision data, and they also have several flags which can be set during
			spawn times which change the way the entity, the player, and the world interact with one another. Each entity
			consists of several data points and flags, which are loaded into a catalogue of entities to be dynamically loaded
			by the maps.
			
A Brief (Not Really) Explanation on How the Config Files Work:

Each config file represents a dynamic or changeable aspect of the engine. They are provided so as to make it easier for an
individual to completely create a game that can be considered their own via customization.
	
Now, to use the engine is rather tricky. With the currently provided tools, it is possible to make a single room in which
the player can walk around in. To do this, you will need the Gamma Editor, which is the map editing software I've made along
side this project. The current compiled version can be found on my github page.

The entire engine works through both the rcg (config) files, and also through the various files located in the resource folders.
For example, the world is loaded from levels.rcg, which specifies the range of levels to be loaded for the entire world. Each
level specifies a range of maps to be loaded for that particular level. Then each map contains numerical tile ids, which are
specified in tiles.rcg. Then the tiles are loaded, the first map is loaded, and the first level loaded. The whole engine follows
this flowchart:

World <- Level <- Map <- Tiles & Entities

So, to have a fully functional game, it is necessary to have all of the above (with the exclusion being audio. Technically audio is
not required). To create maps, it is easier to use the mapping tools I have created (known as GammaEdit) rather than manually typing
the tile ids. However, at the moment, it is necessary to code the entities, the tiles, the levels and world by hand. Eventually
I will automate the process with a GUI to make things that much easier.

Use the provided files as a template. I will eventually add to this readme (or append with a creator's manual) instructions in detail
on how to create a completely new game using the engine.


The latest addition to the engine is the multimap system, the entity system and the tile declaration systems. The multimap system
allows a content creator to link multiple maps and stitch them into some form of larger map filled with maps. The entity system
introduces non-world geometry and various props that can be included into a game. Similar to models or props in other engines, these
entities (will or do) allow for animations, physics interactions, etc. Tile declarations allow tiles to be declared as function tiles
in a separate file. Each tile listed in tiledec.rcg will perform that particular action either when called at map load or at collision,
dependent on what action is specified.

TODO:
Audio system -> Work on positional audio
Menus/User interface
Customizable controls
Better artificial intelligence
Custom content detection using config files
Cross-platform compatibility?
Collision detection -> better collision detection. Currently, it is very bare bones.
Artwork (oh boy)
Gamepad/Joystick input? (maaaaaybeeee...)
Refine entity system
HUD
Inventory and items