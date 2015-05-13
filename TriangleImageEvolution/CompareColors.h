////////////////////////////////////////////////////////////////////////////////
//
//	Name:			CompareColors.h
//
//	Description:	This file contains a basic function for the
//					comparison of two colors. It ensures that if
//					a color is not similar in human terms, having
//					all three color values similar, then the
//					comparison is null.
//
//	Created:		July 3rd, 2009
//
//	Last Modified:	July 3rd, 2009
//
////////////////////////////////////////////////////////////////////////////////

struct ComparisonColor
{
	ComparisonColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

double CompareColors(ComparisonColor color_1, ComparisonColor color_2, bool compare_alpha)
{
	if ((abs((int)(color_1.R - color_2.R))) / 255.0 > 0.1f) return 0.0f;
	if ((abs((int)(color_1.G - color_2.G))) / 255.0 > 0.1f) return 0.0f;
	if ((abs((int)(color_1.B - color_2.B))) / 255.0 > 0.1f) return 0.0f;

	if (compare_alpha)
		if ((255 - abs((int)(color_1.A - color_2.A))) / 255.0 > 0.1f) return 0.0f;

	double comparison = 0.0f;
	comparison += (255 - abs((int)(color_1.R - color_2.R))) / 255.0;
	comparison += (255 - abs((int)(color_1.G - color_2.G))) / 255.0;
	comparison += (255 - abs((int)(color_1.B - color_2.B))) / 255.0;

	if (compare_alpha)
		comparison += (255 - abs((int)(color_1.A - color_2.A))) / 255.0;

	return comparison;
}