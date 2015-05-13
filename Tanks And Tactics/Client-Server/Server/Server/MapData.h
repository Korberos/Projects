#ifndef _MAP_DATA_H_
#define _MAP_DATA_H_

#include "ServerConnection.h"
#include "WorldController.h"
#include "BattleController.h"

static void Send_Map_Data_To_Player( const ServerConnection& connection, int map_index )
{
	bool map_index_valid = ( map_index >= 0 ) && ( map_index < int(BattleController::MapList.size()) );
	writechar( (char)(map_index_valid), BUFFER_PACKET );

	if ( map_index_valid == true )
	{
		const char* map_name = BattleController::MapList[map_index].second.c_str();

		MapDataStruct data_struct;
		WorldController::Get_Instance()->Load_Map_Data_Struct( map_name, data_struct );

		writeuint( data_struct.Width, BUFFER_PACKET );
		writeuint( data_struct.Height, BUFFER_PACKET );
		for ( unsigned int i = 0; i < data_struct.Width * data_struct.Height; ++i )
		{
			writefloat( data_struct.HeightList[i], BUFFER_PACKET );
		}

		writechar( (char)(data_struct.Palette != 0), BUFFER_PACKET );
		if ( data_struct.Palette != 0 )
		{
			writeuint( data_struct.Palette->Width, BUFFER_PACKET );
			writeuint( data_struct.Palette->Height, BUFFER_PACKET );
			for ( unsigned int i = 0; i < data_struct.Palette->Width * data_struct.Palette->Height; ++i )
			{
				writefloat( data_struct.Palette->Colors[i].x, BUFFER_PACKET );
				writefloat( data_struct.Palette->Colors[i].y, BUFFER_PACKET );
				writefloat( data_struct.Palette->Colors[i].z, BUFFER_PACKET );
			}
		}
	}

	//clearbuffer( BUFFER_PACKET_WITH_SIZE );
	//writeint( buffsize( BUFFER_PACKET ) + sizeof(int), BUFFER_PACKET_WITH_SIZE );
	//copybuffer( BUFFER_PACKET_WITH_SIZE, BUFFER_PACKET );
	//sendmessage( connection.Socket, connection.IPAddress, SERVER_PORT, BUFFER_PACKET_WITH_SIZE );
}

#endif // _MAP_DATA_H_