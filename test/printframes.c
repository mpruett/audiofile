#include <stdio.h>
#include <stdlib.h>
#include <audiofile.h>

main (int argc, char **argv)
{
	AFframecount	frameCount;
	AFfilehandle	file;
	void		*data;
	int		i, frameSize, channelCount;

	file = afOpenFile(argv[1], "r", AF_NULL_FILESETUP);
	_af_print_filehandle(file);

	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	data = malloc(frameCount * frameSize);

	afReadFrames(file, AF_DEFAULT_TRACK, data, frameCount);
	_af_print_filehandle(file);

	for (i=0; i<frameCount * channelCount; i++)
		printf("%d ", ((short *) data)[i]);

	printf("\n");

	afCloseFile(file);
}
