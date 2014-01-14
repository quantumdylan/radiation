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
	map1.mp (the main listing of maps)
	
It is neccesarry to note that tiles.tl IS modifiable, though you MUST follow the template provided as an example:
[int id]
id = an integer
file = /tiles/whatever.png

And also that the [data] section must be modified to accomodate this change ***(end MUST be equal to the final entries id)***

And of course, map1.mp is sort of temporary. Will be migrating to something else later.


TODO:
Audio system
Map loading/parsing
Menus/User interface
Custom content detection using config files
Cross-platform compatibility?
Collision detection
Artwork (oh boy)