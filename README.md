# sfxObjectDump
Parses 3d mesh data from Starfox rom

Usage: sfxobjdump <romfile> <vertex list address> <face data address>

	-f <n>	Frame n only
	-b <basename>	Set base filename.
			Output filenames will be be in format "Basename nn"
	-v	Verbose output
	-t	Validate only.

Currently only outputs the data in .obj format. For animated objects, it will output one .obj for each frame.

Does not currently handle textures from the rom, but is intended to be added. Contributors welcome.

Should also work with Starfox 2, and is expected to work with all other SuperFX games such as Vortex, Stunt Race FX, and Dirt Trax.
