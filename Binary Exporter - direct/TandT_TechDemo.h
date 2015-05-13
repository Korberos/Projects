#include <windows.h>
#include <fstream>
#include <math.h>
#include <stdlib.h>

#define TAT_UNIT	0.005f		// Typically equal to one foot
#define BITMAP_ID	0x4D42		// Universal Bitmap ID (?)

#define MAP_WIDTH	40
#define MAP_HEIGHT	40

unsigned int	Uint;
unsigned char	Byte;
float			Float;

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							      // the file pointer
	BITMAPFILEHEADER	bitmapFileHeader;		// bitmap file header
	unsigned char		*bitmapImage = NULL;	// bitmap image data
	unsigned int		imageIdx = 0;		   // image index counter
	unsigned char		tempRGB;				   // swap variable

	// open filename in "read binary" mode
	fopen_s(&filePtr, filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
}

void TestMapFile(void)
{
	BITMAPINFOHEADER BMP_Header;
	unsigned char* HeightMap;
	unsigned int W;
	unsigned int H;
	float* HeightList;
	float AverageHeight;

	// Load in the HeightMap bitmap
	HeightMap = LoadBitmapFile("Heightmap.bmp", &BMP_Header);
	if (HeightMap == NULL) return;

	// Gather the Width and Height of the image
	W = (unsigned int)BMP_Header.biWidth;
	H = (unsigned int)BMP_Header.biHeight;

	// Read in all heights to a list array
	HeightList = new float[W * H];
	AverageHeight = 0.0f;
	for (unsigned int i = 0; i < W * H; i++)
	{
		HeightList[i] = (float)HeightMap[i * 3];
		AverageHeight += (HeightList[i] / (float)(W * H));
	}
	for (unsigned int i = 0; i < W * H; i++) HeightList[i] -= AverageHeight;

	// Open the map file
	std::fstream Output("../Tanks and Tactics/Tech Demo/TestMapFile.ttm", std::ios_base::out | std::ios_base::binary);

	// Output the sizes of the image
	Output.write((char*)&W, sizeof(W));
	Output.write((char*)&H, sizeof(H));

	// Output the Height list
	for (unsigned int i = 0; i < W * H; i++)
	{
		HeightList[i] /= 127.5f;
		Output.write((char*)&HeightList[i], sizeof(HeightList[i]));
	}

	// Close the map file
	Output.close();

	// De-allocate any dynamic memory
	delete [] HeightMap;
	delete [] HeightList;
}