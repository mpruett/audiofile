#include <stdio.h>
#include <audiofile.h>

int expectedError;
int verbose = 0;

void myerrorfunc (long error, const char *description)
{
	if (error != expectedError)
	{
		if (verbose)
		{
			printf("WRONG ERROR CODE:\n");
			printf("%s [error code %ld]--", description, error);
			printf("expected error code %d\n", expectedError);
		}
		exit(-1);
	}
	else
	{
		if (verbose)
		{
			printf("CORRECT ERROR CODE:\n");
			printf("%s [error code %ld]\n", description, error);
		}
	}
}

void test1 (void)
{
	expectedError = AF_BAD_FILEHANDLE;

	if (verbose) printf("closing null file handle\n");
	afCloseFile(AF_NULL_FILEHANDLE);

	expectedError = AF_BAD_FILESETUP;
	if (verbose) printf("freeing null file setup\n");
	afFreeFileSetup(AF_NULL_FILESETUP);

	expectedError = AF_BAD_OPEN;
	if (verbose) printf("opening nonexistent file\n");
	afOpenFile("sldkjflsdkfjalksdjflaksdjflsakfdj", "r", NULL);
}

void test2 (void)
{
	AFfilesetup	setup;
	setup = afNewFileSetup();

	expectedError = AF_BAD_FILEFMT;
	if (verbose) printf("initializing file format to invalid value\n");
	afInitFileFormat(setup, 91094);

	expectedError = AF_BAD_SAMPFMT;
	if (verbose) printf("initializing sample format and sample width to invalid value\n");
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, 3992, 3932);

	afFreeFileSetup(setup);
}

int main (int argc, char **argv)
{
	afSetErrorHandler(myerrorfunc);

	if (argc == 2 && strcmp(argv[1], "-v") == 0)
		verbose = 1;

	test1();
	test2();

	return 0;
}
