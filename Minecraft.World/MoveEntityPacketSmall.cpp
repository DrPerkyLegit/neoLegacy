#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "MoveEntityPacketSmall.h"


MoveEntityPacketSmall::MoveEntityPacketSmall() 
{
	hasRot = false;

	id = -1;
	xa = 0;
	ya = 0;
	za = 0;
	yRot = 0;
	xRot = 0;
}

MoveEntityPacketSmall::MoveEntityPacketSmall(int id)
{
	if( (id < 0 ) || (id >= 16384 ) )
	{
		// We shouln't be tracking an entity that doesn't have a short type of id
		DEBUG_BREAK();
	}

	this->id = id;
	hasRot = false;

	xa = 0;
	ya = 0;
	za = 0;
	yRot = 0;
	xRot = 0;
}

void MoveEntityPacketSmall::read(DataInputStream *dis) //throws IOException 
{
	id = dis->readInt();
}

void MoveEntityPacketSmall::write(DataOutputStream *dos) //throws IOException
{
	if( (id < 0 ) || (id >= 16384 ) )
	{
		// We shouln't be tracking an entity that doesn't have a short type of id
		DEBUG_BREAK();
	}
	dos->writeInt(id);
}

void MoveEntityPacketSmall::handle(PacketListener *listener)
{
	listener->handleMoveEntitySmall(shared_from_this());
}

int MoveEntityPacketSmall::getEstimatedSize() 
{
	return 4;
}

bool MoveEntityPacketSmall::canBeInvalidated()
{
	return true;
}

bool MoveEntityPacketSmall::isInvalidatedBy(shared_ptr<Packet> packet)
{
	shared_ptr<MoveEntityPacketSmall> target = dynamic_pointer_cast<MoveEntityPacketSmall>(packet);
	return target != nullptr && target->id == id;
}

MoveEntityPacketSmall::PosRot::PosRot()
{
	hasRot = true;
}

MoveEntityPacketSmall::PosRot::PosRot(int id, char xa, char ya, char za, char yRot, char xRot) : MoveEntityPacketSmall( id )
{
	this->xa = xa;
	this->ya = ya;
	this->za = za;
	this->yRot = yRot;
	this->xRot = xRot;
	hasRot = true;
}

void MoveEntityPacketSmall::PosRot::read(DataInputStream *dis) //throws IOException 
{
	this->id = dis->readInt();
	this->yRot = dis->readChar();
	int XandYandZ = (int)dis->readShort();
	this->xa = XandYandZ >> 11;
	this->ya = (XandYandZ << 21 ) >> 26;
	this->za = (XandYandZ << 27 ) >> 27;
}

void MoveEntityPacketSmall::PosRot::write(DataOutputStream *dos) //throws IOException 
{
	if( (id < 0 ) || (id >= 16384 ) )
	{
		// We shouln't be tracking an entity that doesn't have a short type of id
		DEBUG_BREAK();
	}
	dos->writeInt(id);
	dos->writeChar(yRot);
	short XandYandZ = ( xa << 11 ) | ( ( ya & 0x3f ) << 5 ) | ( za & 0x1f );
	dos->writeShort(XandYandZ);
}

int MoveEntityPacketSmall::PosRot::getEstimatedSize() 
{
	return 7;
}

MoveEntityPacketSmall::Pos::Pos() 
{
}

MoveEntityPacketSmall::Pos::Pos(int id, char xa, char ya, char za) : MoveEntityPacketSmall(id)
{
	this->xa = xa;
	this->ya = ya;
	this->za = za;
}

void MoveEntityPacketSmall::Pos::read(DataInputStream *dis) //throws IOException 
{
	this->id = dis->readInt();
	this->ya = dis->readChar();
	int XandZ = (int)static_cast<signed char>(dis->readByte());
	xa = XandZ >> 4;
	za = ( XandZ << 28 ) >> 28;
}

void MoveEntityPacketSmall::Pos::write(DataOutputStream *dos) //throws IOException
{
	if( (id < 0 ) || (id >= 16384 ) )
	{
		// We shouln't be tracking an entity that doesn't have a short type of id
		DEBUG_BREAK();
	}
	dos->writeInt(id);
	dos->writeChar(ya);
	char XandZ = ( xa << 4 ) | ( za & 0x0f );
	dos->writeByte(XandZ);
}

int MoveEntityPacketSmall::Pos::getEstimatedSize()
{
	return 7;
}

MoveEntityPacketSmall::Rot::Rot() 
{
	hasRot = true;
}

MoveEntityPacketSmall::Rot::Rot(int id, char yRot, char xRot) : MoveEntityPacketSmall(id)
{

	this->yRot = yRot;
	this->xRot = xRot;
	hasRot = true;
}

void MoveEntityPacketSmall::Rot::read(DataInputStream *dis) //throws IOException 
{
	this->id = dis->readInt();
	this->yRot = dis->readChar();
}

void MoveEntityPacketSmall::Rot::write(DataOutputStream *dos) //throws IOException 
{
	if( (id < 0 ) || (id >= 16384 ) )
	{
		// We shouln't be tracking an entity that doesn't have a short type of id
		DEBUG_BREAK();
	}
	dos->writeInt(id);
	dos->writeChar(yRot);
}

int MoveEntityPacketSmall::Rot::getEstimatedSize()
{
	return 5;
}
