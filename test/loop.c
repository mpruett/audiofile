#include <audiofile.h>

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFfilesetup	setup;
	int		markerIDs[] = {1, 2, 3, 4};
	int		loopIDs[] = {1, 2};
	short		frames[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_AIFF);

	afInitMarkIDs(setup, AF_DEFAULT_TRACK, markerIDs, 4);
	afInitMarkName(setup, AF_DEFAULT_TRACK, 1, "sustain loop start");
	afInitMarkName(setup, AF_DEFAULT_TRACK, 2, "sustain loop end");
	afInitMarkName(setup, AF_DEFAULT_TRACK, 3, "release loop start");
	afInitMarkName(setup, AF_DEFAULT_TRACK, 4, "release loop end");

	afInitLoopIDs(setup, AF_DEFAULT_INST, loopIDs, 2);

	file = afOpenFile(argv[1], "w", setup);

	afWriteFrames(file, AF_DEFAULT_TRACK, frames, 5);

	afSetMarkPosition(file, AF_DEFAULT_TRACK, 1, 0);
	afSetMarkPosition(file, AF_DEFAULT_TRACK, 2, 2);
	afSetMarkPosition(file, AF_DEFAULT_TRACK, 3, 4);
	afSetMarkPosition(file, AF_DEFAULT_TRACK, 4, 5);

	afSetLoopStart(file, AF_DEFAULT_INST, 1, 1);
	afSetLoopEnd(file, AF_DEFAULT_INST, 1, 2);
	afSetLoopStart(file, AF_DEFAULT_INST, 2, 3);
	afSetLoopEnd(file, AF_DEFAULT_INST, 2, 4);

	afCloseFile(file);
}
