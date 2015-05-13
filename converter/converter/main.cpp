#include <fstream>
using namespace std;

int main( int argc, char argv[] )
{
	unsigned int UINT[2];
	float FLOAT[1600];

	ifstream input( "TestMapFile.ttm" );
	if ( input.bad() || !input.good() ) return 1;

	input.read( (char*)&UINT[0], sizeof(UINT[0]) );
	input.read( (char*)&UINT[1], sizeof(UINT[1]) );

	for ( unsigned int i = 0; i < UINT[0] * UINT[1]; ++i )
	{
		input.read( (char*)&FLOAT[i], sizeof(FLOAT[i]) );
		FLOAT[i] *= 2.0f;
	}

	input.close();

	char output_line[128];

	ofstream output( "test_world.xml" );
	if ( output.bad() || !output.good() ) return 1;
	
	sprintf( output_line, "<World>\n" );
	output.write( output_line, strlen( output_line ) + 1 );

	sprintf( output_line, "\t<Rows> %d </Rows>\n", UINT[0] );
	output.write( output_line, strlen( output_line ) + 1 );

	sprintf( output_line, "\t<Columns> %d </Columns>\n", UINT[1] );
	output.write( output_line, strlen( output_line ) + 1 );

	sprintf( output_line, "\t<HeightList>\n" );
	output.write( output_line, strlen( output_line ) + 1 );

	for ( unsigned int i = 0; i < UINT[0] * UINT[1]; ++i )
	{
		sprintf( output_line, "\t\t<entry> %f </entry>\n", FLOAT[i] );
		output.write( output_line, strlen( output_line ) + 1 );
	}

	sprintf( output_line, "\t</HeightList>\n" );
	output.write( output_line, strlen( output_line ) + 1 );
	
	sprintf( output_line, "</World>" );
	output.write( output_line, strlen( output_line ) + 1 );

	output.close();
	return 0;
}