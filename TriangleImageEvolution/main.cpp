/*

	Triangle Image Evolution

	-	An image is recreated as best as possible by the creation of
		polygons within a certain range through an evolutionary process

*/

#define		SCREEN_W		1280
#define		SCREEN_H		640
#define		POLYGON_COUNT	200
#define		VERTEX_COUNT	3
#define		IMAGE_MAX_W		1024
#define		IMAGE_MAX_H		1024
#define		RENDER_MAX_W	512
#define		RENDER_MAX_H	512
#define		IMAGE_1_X		10
#define		IMAGE_1_Y		10
#define		IMAGE_2_X		420
#define		IMAGE_2_Y		10
#define		IMAGE_3_X		830
#define		IMAGE_3_Y		10
#define		FITNESS_X		20
#define		FITNESS_Y		240
#define		DEFINED_BIT		1


#pragma comment(lib, "SDLgfx.lib")
#include <stdlib.h>
#include <ctime>
#include <fstream>

#include "Window.h"
#include "SDL_image/SDL_image.h"
#include "SDL_gfxPrimitives.h"
#include "CompareColors.h"

////////////////////
// Structures
////////////////////
struct Poly
{
	char	BitFlag;
	Sint16	X[VERTEX_COUNT];
	Sint16	Y[VERTEX_COUNT];
	Uint8	Color[4];
};

////////////////////
// Variables
////////////////////
CWindow				Window;
SDL_Event			event;
int					nKeys;
Uint8*				nKeyArray;

long				OpeningTick;
double				TotalTime;

SDL_Surface*		Target;
SDL_Surface*		Best;
SDL_Surface*		Evolve;

unsigned int		RenderWidth;
unsigned int		RenderHeight;
double				RenderScale;

unsigned int		TargetPixelSize;
unsigned int		EvolvePixelSize;
unsigned int		TargetPixelShift_R;
unsigned int		TargetPixelShift_G;
unsigned int		TargetPixelShift_B;
unsigned int		EvolvePixelShift_R;
unsigned int		EvolvePixelShift_G;
unsigned int		EvolvePixelShift_B;
unsigned int		TargetBitMask_R;
unsigned int		TargetBitMask_G;
unsigned int		TargetBitMask_B;
unsigned int		EvolveBitMask_R;
unsigned int		EvolveBitMask_G;
unsigned int		EvolveBitMask_B;

double				Fitness;
double				TempFitness;
unsigned int		Attempts;
unsigned int		Mutations;
unsigned int		GoodMutations;

Poly				PolygonList[POLYGON_COUNT];
Poly				TempPolygon;
unsigned int		ListIterator;

Uint32				RMASK;
Uint32				GMASK;
Uint32				BMASK;
Uint32				AMASK;
SDL_Rect			SourceRect;
SDL_Rect			DestinationRect;
bool				RenderSwitch;

////////////////////
// Methods
////////////////////

// Set up three types of mutations. Every 100 mutations, choose a random one and continue;
typedef	void		MutationFormula(Poly* PolyList);
MutationFormula*	MutationType;
unsigned int		MutationsSinceSwap;

void	Mutate1(Poly* PolyList);
void	Mutate2(Poly* PolyList);
void	Mutate3(Poly* PolyList);

bool	TestEvolution(void);

double	CalculateFitness(void);
void	Clockwise(Poly* Polygon);
void	RenderScreen(double Scale);

void	ClearSurface(SDL_Surface* Surface = 0, Uint8 R = 0, Uint8 G = 0, Uint8 B = 0, Uint8 A = 255);
void	OutputInformation(double Fit = 0.0, unsigned int Goods = 0, unsigned int Muts = 0, unsigned int Tries = 0, double Total = 0.0);

void	ExportBinary();
void	ImportBinary();
void	NullPolygon(Poly* P);
void	RandPolygon(Poly* P);

// PROGRAM START
int main(int argc, char *argv[])
{
	srand((unsigned int)(time(0)));

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	RMASK = 0xff000000;
	GMASK = 0x00ff0000;
	BMASK = 0x0000ff00;
	AMASK = 0x000000ff;
#else
	RMASK = 0x000000ff;
	GMASK = 0x0000ff00;
	BMASK = 0x00ff0000;
	AMASK = 0xff000000;
#endif

	// Load in the target image and create the two additional surfaces
	Target = IMG_Load("TestImage.png");
	if (Target->w > IMAGE_MAX_W) return -1;
	if (Target->h > IMAGE_MAX_H) return -2;
	Best		= SDL_CreateRGBSurface(SDL_SWSURFACE, Target->w, Target->h, 32, RMASK, GMASK, BMASK, AMASK);
	Evolve		= SDL_CreateRGBSurface(SDL_SWSURFACE, Target->w, Target->h, 32, RMASK, GMASK, BMASK, AMASK);

	// If the surfaces are wider or taller than the maximum render area, decide how to scale it to fit
	RenderScale = 1.0;
	if ((Target->w > RENDER_MAX_W) || (Target->h > RENDER_MAX_H))
		RenderScale = ((Target->w > Target->h) ? (double)(Target->w) / (double)(RENDER_MAX_W) : (double)(Target->h) / (double)(RENDER_MAX_H));

	// Find the pixel byte size (to avoid dividing over and over later (probably 4)
	TargetPixelSize = Target->pitch / Target->w;
	EvolvePixelSize = Evolve->pitch / Evolve->w;

	// Create bit shifting elements for later based on the system endian-ness and whether the image is 24-bit or 32-bit
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	TargetBitMask_R		= 0xff << (8 * (TargetPixelSize - 1));
	TargetPixelShift_R	= 8 * (TargetPixelSize - 1);
	TargetBitMask_G		= 0xff << (8 * (TargetPixelSize - 2));
	TargetPixelShift_G	= 8 * (TargetPixelSize - 2);
	TargetBitMask_B		= 0xff << (8 * (TargetPixelSize - 3));
	TargetPixelShift_B	= 8 * (TargetPixelSize - 3);
	EvolveBitMask_R 	= 0xff << (8 * (EvolvePixelSize - 1));
	EvolvePixelShift_R	= 8 * (EvolvePixelSize - 1);
	EvolveBitMask_G 	= 0xff << (8 * (EvolvePixelSize - 2));
	EvolvePixelShift_G	= 8 * (EvolvePixelSize - 2);
	EvolveBitMask_B 	= 0xff << (8 * (EvolvePixelSize - 3));
	EvolvePixelShift_B	= 8 * (EvolvePixelSize - 3);
#else
	TargetBitMask_R		= 0xff << (8 * (TargetPixelSize - 3));
	TargetPixelShift_R	= 8 * (TargetPixelSize - 3);
	TargetBitMask_G 	= 0xff << (8 * (TargetPixelSize - 2));
	TargetPixelShift_G	= 8 * (TargetPixelSize - 2);
	TargetBitMask_B 	= 0xff << (8 * (TargetPixelSize - 1));
	TargetPixelShift_B	= 8 * (TargetPixelSize - 1);
	EvolveBitMask_R 	= 0xff << (8 * (EvolvePixelSize - 3));
	EvolvePixelShift_R	= 8 * (EvolvePixelSize - 3);
	EvolveBitMask_G 	= 0xff << (8 * (EvolvePixelSize - 2));
	EvolvePixelShift_G	= 8 * (EvolvePixelSize - 2);
	EvolveBitMask_B 	= 0xff << (8 * (EvolvePixelSize - 1));
	EvolvePixelShift_B	= 8 * (EvolvePixelSize - 1);
#endif

	// Load in the parts of the render rects that don't change
	SourceRect.x		= SourceRect.y	= 0;
	DestinationRect.w	= SourceRect.w	= Target->w;
	DestinationRect.h	= SourceRect.h	= Target->h;

	// Create the window
	if(!Window.DefineWindow(SCREEN_W, SCREEN_H, 32, false, "Triangle Image Evolution")) return -1;
	OpeningTick = SDL_GetTicks();
	TotalTime = 0.0;

	// Define all ListPolygon instances to a RANDOM state
	for (unsigned int i = 0; i < POLYGON_COUNT; i++) RandPolygon(&PolygonList[i]);

	// Calculate the starting fitness
	TempFitness		= 0.0;
	Fitness			= CalculateFitness();

	// Set the mutation information to NULL
	Attempts		= 0;
	Mutations		= 0;
	GoodMutations	= 0;
	RenderSwitch	= true;

	// Define the mutation type
	MutationType = Mutate1;

	while (true)
	{
		// Poll for input
		if(SDL_PollEvent(&event) > 0)
		{
			if(event.type & (SDL_KEYUP | SDL_KEYDOWN))
			{
				nKeyArray = SDL_GetKeyState(&nKeys);
				if (nKeyArray[SDLK_ESCAPE]) break;
				if (nKeyArray[SDLK_RETURN]) RenderSwitch = !RenderSwitch;
				if (nKeyArray[SDLK_1])		ExportBinary();
				if (nKeyArray[SDLK_2])		ImportBinary();
				if (nKeyArray[SDLK_3])		MutationType = Mutate1;
				if (nKeyArray[SDLK_4])		MutationType = Mutate2;
			}
		}

		// Go through a mutation sequence
		MutationType(PolygonList);

		if (RenderSwitch) RenderScreen(RenderScale);
	}

	SDL_FreeSurface(Target);
	SDL_FreeSurface(Best);
	SDL_FreeSurface(Evolve);
	Window.Undefine();
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	return 0;
}

void Mutate1(Poly* PolyList)
{
	// Mutate1: Choose a random triangle and commit X random mutations, where X is 
	// a partially random number between 1 and 5.
	unsigned int MutationCount = rand() % 10;
	if		(MutationCount < 3)	MutationCount = 1;
	else if	(MutationCount < 5)	MutationCount = 2;
	else if	(MutationCount < 7)	MutationCount = 3;
	else if	(MutationCount < 9)	MutationCount = 4;
	else						MutationCount = 5;

	unsigned int	RandomMutator;
	for (unsigned int i = 0; i < MutationCount; i++)
	{
		ListIterator = rand() % POLYGON_COUNT;
		TempPolygon = PolyList[ListIterator];
		RandomMutator = rand() % (4 + VERTEX_COUNT * 2);
		switch (RandomMutator)
		{
		case 0:
		case 1:
		case 2:
			PolyList[ListIterator].Color[RandomMutator] = rand() % 256;
			if (~PolyList[ListIterator].BitFlag & DEFINED_BIT) PolyList[ListIterator].Color[3] = rand() % 256;
			break;

		case 3:
			PolyList[ListIterator].Color[RandomMutator] = rand() % 256;
			break;

		default:
			if (RandomMutator - 4 < VERTEX_COUNT)	PolyList[ListIterator].X[RandomMutator - 4] = rand() % Target->w;
			else									PolyList[ListIterator].Y[RandomMutator - 4 - VERTEX_COUNT] = rand() % Target->h;
			Clockwise(&PolyList[ListIterator]);
			break;
		}
		PolyList[ListIterator].BitFlag |= DEFINED_BIT;

		// Test to see if the evolution was successful. If not, place the temporary polygon back in the list.
		if (TestEvolution() == false) PolyList[ListIterator] = TempPolygon;
	}
}

void Mutate2(Poly* PolyList)
{
	// Mutate2: Iterate through the list and for each triangle, attempt to mutate 10 times in a row before continuing
	static int Iterator = 0;
	static int IterCount = -1;

	IterCount++;
	if (IterCount >= 10)
	{
		IterCount = 0;
		Iterator += 1;
		if (Iterator >= POLYGON_COUNT) Iterator = 0;
	}

	unsigned int MutationCount = rand() % 10;
	if		(MutationCount < 3)	MutationCount = 1;
	else if	(MutationCount < 5)	MutationCount = 2;
	else if	(MutationCount < 7)	MutationCount = 3;
	else if	(MutationCount < 9)	MutationCount = 4;
	else						MutationCount = 5;

	unsigned int	RandomMutator;
	for (unsigned int i = 0; i < MutationCount; i++)
	{
		TempPolygon = PolyList[Iterator];
		RandomMutator = rand() % (4 + VERTEX_COUNT * 2);
		switch (RandomMutator)
		{
		case 0:
		case 1:
		case 2:
			PolyList[Iterator].Color[RandomMutator] = rand() % 256;
			if (~PolyList[Iterator].BitFlag & DEFINED_BIT) PolyList[Iterator].Color[3] = rand() % 256;
			break;

		case 3:
			PolyList[Iterator].Color[RandomMutator] = rand() % 256;
			break;

		default:
			if (RandomMutator - 4 < VERTEX_COUNT)	PolyList[Iterator].X[RandomMutator - 4] = rand() % Target->w;
			else									PolyList[Iterator].Y[RandomMutator - 4 - VERTEX_COUNT] = rand() % Target->h;
			Clockwise(&PolyList[ListIterator]);
			break;
		}
		PolyList[Iterator].BitFlag |= DEFINED_BIT;

		// Test to see if the evolution was successful. If not, place the temporary polygon back in the list.
		if (TestEvolution() == false) PolyList[Iterator] = TempPolygon;
	}
}

void Mutate3(Poly* PolyList)
{
}

bool TestEvolution(void)
{
	// Test the temp fitness against the current fitness. If it is greater or equal, replace 
	// all pixels in Best with pixels from Evolve and update the current fitness.
	Attempts++;
	TempFitness = CalculateFitness();
	if (TempFitness > Fitness)
	{
		Mutations++;
		if (TempFitness != Fitness) GoodMutations++;

		// Copy over all pixels to the Best surface and write in the new fitness
		memcpy((void*)((Uint32*)Best->pixels), (void*)((Uint32*)Evolve->pixels), Evolve->pitch * Evolve->h);
		Fitness = TempFitness;

		// Output information to console
		OutputInformation(Fitness, GoodMutations, Mutations, Attempts, (SDL_GetTicks() - OpeningTick) / 1000.0);

		// Return an indication that the triangle list mutation was successful
		return true;
	}
	else PolygonList[ListIterator] = TempPolygon;
	
	// Return an indication that the triangle list mutation was not successful
	return false;
}

double CalculateFitness(void)
{
	double			Return;
	//unsigned char	Pixel[2];

	// Draw the Evolve Image
	ClearSurface(Evolve);
	for (unsigned int i = 0; i < POLYGON_COUNT; i++)
		if (PolygonList[i].BitFlag & DEFINED_BIT)
			filledPolygonRGBA(Evolve, PolygonList[i].X, PolygonList[i].Y, VERTEX_COUNT, PolygonList[i].Color[0], PolygonList[i].Color[1], PolygonList[i].Color[2], PolygonList[i].Color[3]);

	// Look through the pixels of both surfaces and determine the fitness of the evolving surface
	Return = 0;
	for (int y = 0; y < Target->h; y++)
		for (int x = 0; x < Target->w; x++)
		{
			ComparisonColor evolve_color(((unsigned char*)Evolve->pixels)[y * Target->pitch + x * TargetPixelSize], ((unsigned char*)Evolve->pixels)[y * Target->pitch + x * TargetPixelSize + 1], ((unsigned char*)Evolve->pixels)[y * Target->pitch + x * TargetPixelSize + 2]);
			ComparisonColor target_color(((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize], ((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize + 1], ((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize + 2]);
			Return += CompareColors(evolve_color, target_color, false);
			//// Red
			//Pixel[0] = ((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize];
			//Pixel[1] = ((unsigned char*)Evolve->pixels)[y * Evolve->pitch + x * EvolvePixelSize];
			//Return += (255 - abs((int)(Pixel[0] - Pixel[1]))) / 255.0;
			//
			//// Green
			//Pixel[0] = ((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize + 1];
			//Pixel[1] = ((unsigned char*)Evolve->pixels)[y * Evolve->pitch + x * EvolvePixelSize + 1];
			//Return += (255 - abs((int)(Pixel[0] - Pixel[1]))) / 255.0;
			//
			//// Blue
			//Pixel[0] = ((unsigned char*)Target->pixels)[y * Target->pitch + x * TargetPixelSize + 2];
			//Pixel[1] = ((unsigned char*)Evolve->pixels)[y * Evolve->pitch + x * EvolvePixelSize + 2];
			//Return += (255 - abs((int)(Pixel[0] - Pixel[1]))) / 255.0;
		}

	Return = (Return / (double)(Target->w * Target->h * 3) * 100.0);
	return Return;
}

void Clockwise(Poly* Polygon)
{
	int dx1, dx2, dy1, dy2;
	
	dx1 = Polygon->X[1] - Polygon->X[0];
	dy1 = Polygon->Y[1] - Polygon->Y[0];
	dx2 = Polygon->X[2] - Polygon->X[0];
	dy2 = Polygon->Y[2] - Polygon->Y[0];
	
	if ((dx1 * dy2 > dy1 * dx2) || ((dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2))) return;

	Polygon->X[1] ^= Polygon->X[2];
	Polygon->X[2] ^= Polygon->X[1];
	Polygon->X[1] ^= Polygon->X[2];
	Polygon->Y[1] ^= Polygon->Y[2];
	Polygon->Y[2] ^= Polygon->Y[1];
	Polygon->Y[1] ^= Polygon->Y[2];
}

void RenderScreen(double Scale)
{
	// Don't render anything if the image is above the maximum render size
	if (abs(Scale - 1.0) > 0.01)	return;

	// Draw the Target Image
	DestinationRect.x = IMAGE_1_X;
	DestinationRect.y = IMAGE_1_Y;
	SDL_BlitSurface(Target, &SourceRect, Window.GetScreen(), &DestinationRect);
	
	// Draw the Best Image
	DestinationRect.x = IMAGE_2_X;
	DestinationRect.y = IMAGE_2_Y;
	SDL_BlitSurface(Best, &SourceRect, Window.GetScreen(), &DestinationRect);

	// Draw the Evolve Image
	DestinationRect.x = IMAGE_3_X;
	DestinationRect.y = IMAGE_3_Y;
	SDL_BlitSurface(Evolve, &SourceRect, Window.GetScreen(), &DestinationRect);

	// Render everything to the screen
	SDL_Flip(Window.GetScreen());
}

void ClearSurface(SDL_Surface* Surface, Uint8 R, Uint8 G, Uint8 B, Uint8 A)
{
	Sint16 X[4];
	Sint16 Y[4];
	X[0] = 0;
	Y[0] = 0;
	X[1] = Surface->w;
	Y[1] = 0;
	X[2] = Surface->w;
	Y[2] = Surface->h;
	X[3] = 0;
	Y[3] = Surface->h;
	filledPolygonRGBA(Surface, X, Y, 4, R, G, B, A);
}

void OutputInformation(double Fit, unsigned int Goods, unsigned int Muts, unsigned int Tries, double Total)
{
	printf("\n\n\n\n\n\n");
	printf("Current Fitness:  %f\n", Fit);
	printf("Mutation Success: %d  /  %d  /  %d\n", Goods, Muts, Tries);
	printf("Time Processing:  %d days, %d hours, %d minutes, and %d seconds", (unsigned int)Total / 216000, ((unsigned int)Total % 5180000) / 3600, ((unsigned int)Total % 216000) / 3600, ((unsigned int)Total % 60));
}

void ExportBinary(void)
{
	// Output the Polygon list to a binary file
	std::fstream Output("SaveFile.tie", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	if (Output.bad())		return;
	if (!Output.good())		return;
	for (unsigned int i = 0; i < POLYGON_COUNT; i++) Output.write((char*)&PolygonList[i], sizeof(Poly));
	Output.close();

	printf("\n\n\nTriangle List Exported!");
}

void ImportBinary(void)
{
	// Import the Polygon list from a binary file
	std::fstream Input("SaveFile.tie", std::ios_base::binary | std::ios_base::in);
	if (Input.bad())		return;
	if (!Input.good())		return;
	for (unsigned int i = 0; i < POLYGON_COUNT; i++) Input.read((char*)&PolygonList[i], sizeof(Poly));
	Input.close();

	// Clear the Evolve surface and allow a new fitness to be calculated
	ClearSurface(Evolve);
	Fitness = 0.0;
	
	printf("\n\n\nTriangle List Imported!");

	// Normally after this occurs, a single mutation would be set and the resulting fitness would be higher than
	// what is was before importing. To prevent this, calculate the fitness before returning to the loop
	
	// Draw the Evolve Image
	ClearSurface(Evolve);
	for (unsigned int i = 0; i < POLYGON_COUNT; i++)
		if (PolygonList[i].BitFlag & DEFINED_BIT)
			filledPolygonRGBA(Evolve, PolygonList[i].X, PolygonList[i].Y, VERTEX_COUNT, PolygonList[i].Color[0], PolygonList[i].Color[1], PolygonList[i].Color[2], PolygonList[i].Color[3]);

	Fitness = CalculateFitness();
	memcpy((void*)((Uint32*)Best->pixels), (void*)((Uint32*)Evolve->pixels), Evolve->pitch * Evolve->h);

	GoodMutations = 0;
	Mutations = 0;
	Attempts = 0;
	OpeningTick = SDL_GetTicks();
	OutputInformation(Fitness, GoodMutations, Mutations, Attempts, 0.0);
}

void NullPolygon(Poly* P)
{
	for(unsigned int j = 0; j < VERTEX_COUNT; j++)
	{
		P->X[j] = rand() % Target->w;
		P->Y[j] = rand() % Target->h;
	}
	for (unsigned int j = 0; j < 3; j++) P->Color[j] = 0;
	P->Color[3] = 0;
	P->BitFlag &= ~DEFINED_BIT;
}

void RandPolygon(Poly* P)
{
	for(unsigned int j = 0; j < VERTEX_COUNT; j++)
	{
		P->X[j] = rand() % Target->w;
		P->Y[j] = rand() % Target->h;
	}
	for (unsigned int j = 0; j < 3; j++) P->Color[j] = rand() % 256;
	P->Color[3] = rand() % 256;
	P->BitFlag &= ~DEFINED_BIT;
}