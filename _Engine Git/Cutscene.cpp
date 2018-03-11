/*****************************************************************/
/*             ___                          _  _                 */
/*            / _ \                        | |(_)                */
/*           / /_\ \ _ __   ___   __ _   __| | _   __ _          */
/*           |  _  || '__| / __| / _` | / _` || | / _` |         */
/*           | | | || |   | (__ | (_| || (_| || || (_| |         */
/*           \_| |_/|_|    \___| \__,_| \__,_||_| \__,_|         */
/*                                                               */
/*                                      Engine Version 01.00.00  */
/*****************************************************************/
/*  File:       Cutscene.cpp                                     */
/*                                                               */
/*  Purpose:    This file contains the system for loading and    */
/*              playing 2D cut-scenes made up of animations,     */
/*              sounds, and movement effects.                    */
/*                                                               */
/*  Created:    01/16/2011                                       */
/*  Last Edit:  04/09/2011                                       */
/*****************************************************************/

#include "Cutscene.h"

#include "DebugTools.h"
#include "Sprite.h"
#include "FontController.h"
#include "StringToValue.h"

Cutscene::Actor*		LoadCutsceneActor( const RapidXML_Node* actor_tag );
Cutscene::Resource*	LoadCutsceneResource( const RapidXML_Node* asset_tag );
Cutscene::Event*		LoadCutsceneEvent( const RapidXML_Node* event_tag );
Cutscene::Resource*	LoadCutsceneSprite( const char* name, const char* file );
Cutscene::Resource*	LoadCutsceneSound( const char* name, const char* file );
Cutscene::Resource*	LoadCutsceneFont( const char* name, const char* file );

class Cutscene::Resource
{
public:
	enum Type { RES_IMAGE, RES_SOUND, RES_FONT, RESOURCE_TYPE_COUNT };

	Resource() : 
		ResourceType( RESOURCE_TYPE_COUNT ),
		Name( "" ),
		Asset( "" )
	{}

	Type ResourceType;
	std::string Name;
	std::string Asset;

	virtual void Render( int x, int y, float alpha ) {}
};

class ResourceSprite : public Cutscene::Resource
{
public:
	Sprite Image;

	void Render( int x, int y, float alpha ) { Image.Render( x, y, alpha ); }
};

class ResourceSound : public Cutscene::Resource
{
};

class ResourceFont : public Cutscene::Resource
{
public:
	Font* CutsceneFont;
	std::string Text;
	
	void Render( int x, int y, float alpha ) { CutsceneFont->Render_Text( Text.c_str(), x, y ); }
};

class Cutscene::Actor
{
public:
	Actor() : ActorName( "" ), X( -1.0f ), Y( -1.0f ), Alpha( 1.0f ), Asset( NULL ) {}
	std::string ActorName;
	float X;
	float Y;
	float Alpha;
	Cutscene::Resource* Asset;

	void Render( int x, int y, float alpha ) { Asset->Render( x, y, alpha ); }
};

class Cutscene::Event
{
public:
	enum Type { ET_SPRITECHANGE, ET_WARPTO, ET_MOVETO, ET_SOUNDPLAY, ET_WAIT, ET_ALPHA, ET_SETFONT, ET_SETTEXT, EVENT_TYPE_COUNT };
	
	Event() : Type( EVENT_TYPE_COUNT ), Trigger( "" ), PassMethod( "" ), EndingEvent( "" ), Target( "" ), X( -1.0f ), Y( -1.0f ), Time( -1.0f ), Asset( "" ) {}

	Type Type;
	std::string Trigger;
	std::string PassMethod;
	std::string EndingEvent;
	std::string Target;
	float X;
	float Y;
	float Time;
	std::string Asset;
};

Cutscene::Cutscene() : 
	CutsceneTime( 0.0f )
{}


Cutscene::~Cutscene()
{
	Shutdown();
}

bool Cutscene::Load( const char* cutscene_file )
{
	//  Attempt to load the specified XML file
	FAIL_IF ( cutscene_file == NULL ) { return false; }
	const RapidXML_Doc* cutsceneDoc = XMLWrapper::Get_Instance()->LoadXMLFile( cutscene_file );
	FAIL_IF ( cutsceneDoc == NULL ) { return false; }

	//  Locate the encompassing tag, labeled "Cutscene"
	const RapidXML_Node* mainNode = cutsceneDoc->first_node();
	FAIL_IF ( "Cutscene file has no encompassing 'Cutscene' tag." && mainNode == NULL )
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( cutscene_file );
		return false;
	}

	//  Locate the tag for the list of actor objects, labeled "Actors"
	const RapidXML_Node* actorsNode = mainNode->first_node( "Actors" );
	FAIL_IF ( "Cutscene has no Actors list." && actorsNode == NULL )
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( cutscene_file );
		return false;
	}

	//  Locate the tag for the list of assets, labeled "Assets"
	const RapidXML_Node* resourcesNode = mainNode->first_node( "Resources" );
	FAIL_IF ( "Cutscene has no Assets list." && resourcesNode == NULL )
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( cutscene_file );
		return false;
	}

	//  Locate the tag for the list of events, labeled "Events"
	const RapidXML_Node* eventsNode = mainNode->first_node( "Events" );
	FAIL_IF ( "Cutscene has no Events list." && eventsNode == NULL )
	{
		XMLWrapper::Get_Instance()->RemoveXMLFile( cutscene_file );
		return false;
	}

	//  Load in the list of Actors
	for ( const RapidXML_Node* actorsIter = actorsNode->first_node(); actorsIter != NULL; actorsIter = actorsIter->next_sibling() )
	{
		ActorList.push_back( LoadCutsceneActor( actorsIter ) );
	}

	//  Load in the list of Assets
	for ( const RapidXML_Node* resourcesIter = resourcesNode->first_node(); resourcesIter != NULL; resourcesIter = resourcesIter->next_sibling() )
	{
		Resource* new_resource = LoadCutsceneResource( resourcesIter );
		ResourceList.push_back( new_resource );
	}

	//  Load in the list of Events
	for ( const RapidXML_Node* eventsIter = eventsNode->first_node(); eventsIter != NULL; eventsIter = eventsIter->next_sibling() )
	{
		Event* new_event = LoadCutsceneEvent( eventsIter );
		EventList.push_back( new_event );

		//  If the new event has a Trigger of "Start", then throw it into the ActiveEvents immediately
		if ( new_event->Trigger.compare( "Start" ) == 0 )
		{
			ActiveEventList.push_back( new_event );
		}
	}
	
	XMLWrapper::Get_Instance()->RemoveXMLFile( cutscene_file );
	return true;
}


void Cutscene::Update( float time_slice )
{
	//  Iterate the current cutscene time with the time slice
	CutsceneTime += time_slice;

	std::vector< Event* > added_events;

	//  Look through the Active Events list for any events that are expiring
	for ( std::vector< Event* >::iterator iter = ActiveEventList.begin(); iter != ActiveEventList.end(); )
	{
		bool remove_event = false;

		//  Process the event according to it's type
		//  TODO:  Optimize the shit out of this
		switch ( (*iter)->Type )
		{
		case Event::ET_WARPTO:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Actor name and keep a hold of it
				Actor* target_actor = NULL;
				for ( std::vector< Actor* >::iterator actor_iter = ActorList.begin(); actor_iter != ActorList.end(); ++actor_iter )
				{
					if ( (*actor_iter)->ActorName.compare( (*iter)->Target ) == 0 )
					{
						target_actor = (*actor_iter);
						break;
					}
				}

				target_actor->X = (*iter)->X;
				target_actor->Y = (*iter)->Y;
				remove_event = true;
			}
			break;

		case Event::ET_SPRITECHANGE:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  If there is no Asset given, remove the event
				if ( (*iter)->Asset.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Actor name and keep a hold of it
				Actor* target_actor = NULL;
				for ( std::vector< Actor* >::iterator actor_iter = ActorList.begin(); actor_iter != ActorList.end(); ++actor_iter )
				{
					if ( (*actor_iter)->ActorName.compare( (*iter)->Target ) == 0 )
					{
						target_actor = (*actor_iter);
						break;
					}
				}

				//  If no Actor by the given name was found, remove the event
				if ( target_actor == NULL )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Asset name and keep a hold of it
				Resource* target_asset = NULL;
				for ( std::vector< Resource* >::iterator asset_iter = ResourceList.begin(); asset_iter != ResourceList.end(); ++asset_iter )
				{
					if ( (*asset_iter)->Name.compare( (*iter)->Asset ) == 0 )
					{
						target_asset = (*asset_iter);
						break;
					}
				}

				//  If no Asset by the given name was found, remove the event
				if ( target_asset == NULL )
				{
					remove_event = true;
					break;
				}

				//  Assign the Asset to the Actor and break out of the event process
				target_actor->Asset = target_asset;
				remove_event = true;
			}
			break;

		case Event::ET_MOVETO:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  If there is no target Time given (usually set to -1.0f), remove the event
				if ( (*iter)->Time < 0.0f )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Actor name and keep a hold of it
				Actor* target_actor = NULL;
				for ( std::vector< Actor* >::iterator actor_iter = ActorList.begin(); actor_iter != ActorList.end(); ++actor_iter )
				{
					if ( (*actor_iter)->ActorName.compare( (*iter)->Target ) == 0 )
					{
						target_actor = (*actor_iter);
						break;
					}
				}

				if ( time_slice > (*iter)->Time )
				{
					target_actor->X = (*iter)->X;
					target_actor->Y = (*iter)->Y;
					remove_event = true;
					break;
				}
				else
				{
					//  Move the appropriate amount based on the time slice
					float percentage = time_slice / (*iter)->Time;
					target_actor->X +=  ( (*iter)->X - target_actor->X ) * percentage;
					target_actor->Y += ( (*iter)->Y - target_actor->Y ) * percentage;

					//  Update the time on the event
					(*iter)->Time -= time_slice;
				}
			}
			break;

		case Event::ET_SOUNDPLAY:
			{
				//  TODO:  Play the sound file if the event is just starting

				//  Update the time on the event
				(*iter)->Time -= time_slice;
			}
			break;

		case Event::ET_WAIT:
			{
				//  Update the time on the event
				(*iter)->Time -= time_slice;
			}
			break;

		case Event::ET_ALPHA:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  If there is no target Asset given (set to a valid float), remove the event
				float value;
				if ( !StringToFloat( (*iter)->Asset.c_str(), value ) )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Actor name and keep a hold of it
				Actor* target_actor = NULL;
				for ( std::vector< Actor* >::iterator actor_iter = ActorList.begin(); actor_iter != ActorList.end(); ++actor_iter )
				{
					if ( (*actor_iter)->ActorName.compare( (*iter)->Target ) == 0 )
					{
						target_actor = (*actor_iter);
						break;
					}
				}

				if ( time_slice > (*iter)->Time )
				{
					target_actor->Alpha = value;
					remove_event = true;
					break;
				}
				
				//  Assign the new alpha value based on the time slice and the amount needed to cover
				float percentage = time_slice / (*iter)->Time;
				target_actor->Alpha += ( value - target_actor->Alpha ) * percentage;

				//  Update the time on the event
				(*iter)->Time -= time_slice;
			}
			break;

		case Event::ET_SETFONT:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  If there is no Asset given, remove the event
				if ( (*iter)->Asset.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Actor name and keep a hold of it
				Actor* target_actor = NULL;
				for ( std::vector< Actor* >::iterator actor_iter = ActorList.begin(); actor_iter != ActorList.end(); ++actor_iter )
				{
					if ( (*actor_iter)->ActorName.compare( (*iter)->Target ) == 0 )
					{
						target_actor = (*actor_iter);
						break;
					}
				}

				//  If no Actor by the given name was found, remove the event
				if ( target_actor == NULL )
				{
					remove_event = true;
					break;
				}

				//  Loop through the ActorList for the Asset name and keep a hold of it
				Resource* target_asset = NULL;
				for ( std::vector< Resource* >::iterator asset_iter = ResourceList.begin(); asset_iter != ResourceList.end(); ++asset_iter )
				{
					if ( (*asset_iter)->Name.compare( (*iter)->Asset ) == 0 )
					{
						target_asset = (*asset_iter);
						break;
					}
				}

				//  If no Asset by the given name was found, remove the event
				if ( target_asset == NULL )
				{
					remove_event = true;
					break;
				}

				//  Assign the Asset to the Actor and break out of the event process
				target_actor->Asset = target_asset;
				remove_event = true;
			}
			break;

		case Event::ET_SETTEXT:
			{
				//  If there is no target Actor given, remove the event
				if ( (*iter)->Target.empty() == true )
				{
					remove_event = true;
					break;
				}

				//  Find the font and alter it's position
				for ( std::vector< Resource* >::iterator font_iter = ResourceList.begin(); font_iter != ResourceList.end(); ++font_iter )
				{
					if ( (*font_iter)->Name.compare( (*iter)->Target ) == 0 )
					{
						if ( (*font_iter)->ResourceType == Resource::RES_FONT )
						{
							ResourceFont* font = static_cast<ResourceFont*>( (*font_iter) );
							font->Text = (*iter)->Asset;
						}
					}
				}

				//  Immediately remove the event
				remove_event = true;
			}
			break;

		default:
			break;
		}
		
		//  If the time left on an ActiveEvent is less than 0, remove the event
		remove_event |= ( (*iter)->Time < 0.001  );

		//  If we need to remove the event, remove it. Otherwise, just iterate and continue
		if ( remove_event == true )
		{
			//  If we have an ending event for the event that is being removed...
			if ( !(*iter)->EndingEvent.empty() )
			{
				//  Send out a call for the events EndingEvent, adding them to a list of events to make active after updating
				for ( std::vector< Event* >::iterator events_iter = EventList.begin(); events_iter != EventList.end(); ++events_iter )
				{
					if ( (*events_iter)->Trigger.compare( (*iter)->EndingEvent ) == 0 )
					{
						added_events.push_back( (*events_iter) );
					}
				}
			}

			//  Remove the event from the ActiveEventList
			iter = ActiveEventList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//  Iterate through the list of EndingEvent calls and start events if needed
	for ( std::vector< Event* >::iterator iter = added_events.begin(); iter != added_events.end(); iter = added_events.erase( iter ) )
	{
		ActiveEventList.push_back( (*iter) );
	}
};


void Cutscene::Render( void )
{
	//  Render all actors that have renderable assets
	for ( std::vector< Actor* >::iterator iter = ActorList.begin(); iter != ActorList.end(); ++iter )
	{
		if ( (*iter)->Asset == NULL ) { break; }
		(*iter)->Render( int((*iter)->X), int((*iter)->Y), (*iter)->Alpha );
	}
};


void Cutscene::Shutdown( void )
{
	//  Free up the memory from any Assets
	for ( std::vector< Resource* >::iterator iter = ResourceList.begin(); iter != ResourceList.end(); )
	{
		switch ( (*iter)->ResourceType )
		{
		case Resource::RES_IMAGE:
			{
				ResourceSprite* resource = static_cast<ResourceSprite*>( (*iter) );
				delete resource;
			}
			break;

		case Resource::RES_SOUND:
			{
				ResourceSound* resource = static_cast<ResourceSound*>( (*iter) );
				delete resource;
			}
			break;

		case Resource::RES_FONT:
			{
				ResourceFont* resource = static_cast<ResourceFont*>( (*iter) );
				FontController::Get_Instance()->Unload_Font( resource->Asset.c_str() );
				delete resource;
			}
			break;

		default:
			break;
		}

		iter = ResourceList.erase( iter );
	}
}


Cutscene::Actor* LoadCutsceneActor( const RapidXML_Node* actorNode )
{
	//  Ensure that the given tag has a value
	FAIL_IF ( "Actor tag has no value." && actorNode->value() == NULL ) { return false; }

	//  Create a new actor and put the given tag's value in as it's name
	Cutscene::Actor* new_actor = new Cutscene::Actor;
	new_actor->ActorName = actorNode->value();
	new_actor->X = 0;
	new_actor->Y = 0;

	//  Submit the new actor to the actors list
	return new_actor;
}


Cutscene::Resource* LoadCutsceneResource( const RapidXML_Node* resourceNode )
{
	//  Find the asset type
	std::string asset_type = resourceNode->first_attribute( "type" )->value();

	//  Ensure that the given tag has a Name and that it has a value
	const RapidXML_Node* nameNode = resourceNode->first_node( "Name" );
	FAIL_IF ( "Resource tag has no Name." && nameNode == NULL ) { return false; }
	FAIL_IF ( "Resource Name tag has no value." && nameNode->value() == NULL ) { return false; }

	//  Ensure that the given tag has an Asset and that it has a value
	const RapidXML_Node* assetNode = resourceNode->first_node( "Asset" );
	FAIL_IF ( "Resource tag has no Asset." && assetNode == NULL ) { return false; }
	FAIL_IF ( "Resource Asset tag has no value." && assetNode->value() == NULL ) { return false; }

	//  Go through the list of types and call the appropriate loading function (TODO: Optimize)
	if		  ( asset_type.compare( "Sprite" ) == 0 )	{ return LoadCutsceneSprite( nameNode->value(), assetNode->value() ); }
	else if ( asset_type.compare( "Sound" ) == 0	)	{ return LoadCutsceneSound( nameNode->value(), assetNode->value() ); }
	else if ( asset_type.compare( "Font" ) == 0 )	{ return LoadCutsceneFont( nameNode->value(), assetNode->value() ); }

	return false;
}


Cutscene::Event* LoadCutsceneEvent( const RapidXML_Node* eventNode )
{
	//  Ensure that the given tag has a type
	const std::string event_type = eventNode->first_attribute( "type" )->value();
	FAIL_IF ( event_type.length() == 0 ) { return false; }
	
	//  Go through the list of event types and act accordingly (TODO: Optimize )
	Cutscene::Event* new_event = new Cutscene::Event;
	if		  ( event_type.compare( "SpriteChange" ) == 0 )	{ new_event->Type = Cutscene::Event::ET_SPRITECHANGE; }
	else if ( event_type.compare( "WarpTo" ) == 0 )			{ new_event->Type = Cutscene::Event::ET_WARPTO; }
	else if ( event_type.compare( "MoveTo" ) == 0 )			{ new_event->Type = Cutscene::Event::ET_MOVETO; }
	else if ( event_type.compare( "SoundPlay" ) == 0 )		{ new_event->Type = Cutscene::Event::ET_SOUNDPLAY; }
	else if ( event_type.compare( "Wait" ) == 0 )			{ new_event->Type = Cutscene::Event::ET_WAIT; }
	else if ( event_type.compare( "Alpha" ) == 0 )			{ new_event->Type = Cutscene::Event::ET_ALPHA; }
	else if ( event_type.compare( "SetFont" ) == 0 )		{ new_event->Type = Cutscene::Event::ET_SETFONT; }
	else if ( event_type.compare( "SetText" ) == 0 )		{ new_event->Type = Cutscene::Event::ET_SETTEXT; }
	else { return false; }

	//  Ensure that the event has a Trigger and that it has a value, then assign it
	const RapidXML_Node* triggerNode = eventNode->first_node( "Trigger" );
	FAIL_IF ( "Event tag has no Trigger." && triggerNode == NULL ) { return false; }
	FAIL_IF ( "Event Trigger tag has no value." && triggerNode->value() == NULL ) { return false; }
	new_event->Trigger = triggerNode->value();

	//  Check if the event has a PassMethod and see if it has a value, then assign it if it does
	const RapidXML_Node* passMethodNode = eventNode->first_node( "PassMethod" );
	if ( passMethodNode != NULL && passMethodNode->value() != 0 )
	{
		new_event->PassMethod = passMethodNode->value();
	}

	//  Check if the event has an EndingEvent and see if it has a value, then assign it if it does
	const RapidXML_Node* endingEventNode = eventNode->first_node( "EndingEvent" );
	if ( endingEventNode != NULL && endingEventNode->value() != 0 )
	{
		new_event->EndingEvent = endingEventNode->value();
	}

	//  Check if the event has a Target and see if it has a value, then assign it if it does
	const RapidXML_Node* targetNode = eventNode->first_node( "Target" );
	if ( targetNode != NULL && targetNode->value() != 0 )
	{
		new_event->Target = targetNode->value();
	}

	//  Check if the event has a X and see if it has a value, then assign it if it does
	const RapidXML_Node* xNode = eventNode->first_node( "X" );
	if ( xNode != NULL )
	{
		StringToFloat( xNode->value(), new_event->X );
	}

	//  Check if the event has a Y and see if it has a value, then assign it if it does
	const RapidXML_Node* yNode = eventNode->first_node( "Y" );
	if ( yNode != NULL )
	{
		StringToFloat( yNode->value(), new_event->Y );
	}

	//  Check if the event has an Time and see if it has a value, then assign it if it does
	const RapidXML_Node* timeNode = eventNode->first_node( "Time" );
	if ( timeNode != NULL )
	{
		StringToFloat( timeNode->value(), new_event->Time );
	}

	//  Check if the event has an Asset and see if it has a value, then assign it if it does
	const RapidXML_Node* assetNode = eventNode->first_node( "Asset" );
	if ( assetNode != NULL )
	{

		new_event->Asset = assetNode->value() ? assetNode->value() : "";
	}

	return new_event;
}


Cutscene::Resource* LoadCutsceneSprite( const char* name, const char* file )
{
	//  Ensure that both the name and file are valid strings
	FAIL_IF ( "No name given for sprite object." && name == NULL ) { return false; }
	FAIL_IF ( "No name given for sprite file." && file == NULL ) { return false; }

	//  Create and fill in the new Sprite
	ResourceSprite* new_asset = new ResourceSprite;
	new_asset->ResourceType = Cutscene::Resource::RES_IMAGE;
	new_asset->Name = name;
	new_asset->Asset = file;
	new_asset->Image.LoadSprite( file );

	return (Cutscene::Resource*)(new_asset);
}


Cutscene::Resource* LoadCutsceneSound( const char* name, const char* file )
{
	//  Ensure that both the name and file are valid strings
	FAIL_IF ( "No name given for sound object." && name == NULL ) { return false; }
	FAIL_IF ( "No name given for sound file." && file == NULL ) { return false; }

	//  Create and fill in the new Sound
	ResourceSound* new_asset = new ResourceSound;
	new_asset->ResourceType = Cutscene::Resource::RES_SOUND;
	new_asset->Name = name;
	new_asset->Asset = file;
	//  TODO:  Load in the sound (requires a sound system, lol)

	return (Cutscene::Resource*)(new_asset);
}


Cutscene::Resource* LoadCutsceneFont( const char* name, const char* file )
{
	//  Ensure that both the name and file are valid strings
	FAIL_IF ( "No name given for font object." && name == NULL ) { return false; }
	FAIL_IF ( "No name given for font file." && file == NULL ) { return false; }

	//  Create and fill in the new Font
	ResourceFont* new_asset = new ResourceFont;
	new_asset->ResourceType = Cutscene::Resource::RES_FONT;
	new_asset->Name = name;
	new_asset->Asset = file;
	FontController::Get_Instance()->Load_Font( new_asset->Asset.c_str() );
	new_asset->CutsceneFont = FontController::Get_Instance()->Get_Font( new_asset->Asset.c_str() );

	return (Cutscene::Resource*)(new_asset);
}