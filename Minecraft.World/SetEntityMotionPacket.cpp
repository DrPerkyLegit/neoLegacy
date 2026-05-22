#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "net.minecraft.world.entity.h"
#include "PacketListener.h"
#include "SetEntityMotionPacket.h"



void SetEntityMotionPacket::_init(int id, double xd, double yd, double zd)
{
	this->id = id;
	double m = 3.9;
	if (xd < -m) xd = -m;
	if (yd < -m) yd = -m;
	if (zd < -m) zd = -m;
	if (xd > m) xd = m;
	if (yd > m) yd = m;
	if (zd > m) zd = m;
	xa = static_cast<int>(xd * 8000.0);
	ya = static_cast<int>(yd * 8000.0);
	za = static_cast<int>(zd * 8000.0);
	// 4J - if we could transmit this as bytes (in 1/16 accuracy) then flag to do so
	if( ( xa >= (-128 * 16 ) ) && ( ya >= (-128 * 16 ) ) && ( za >= (-128 * 16 ) ) &&
		( xa < (128 * 16 ) ) && ( ya < (128 * 16 ) ) && ( za < (128 * 16 ) ) )
	{
		useBytes = true;
	}
	else
	{
		useBytes = false;
	}
}

SetEntityMotionPacket::SetEntityMotionPacket() 
{
	_init(0, 0.0f, 0.0f, 0.0f);
}

SetEntityMotionPacket::SetEntityMotionPacket(shared_ptr<Entity> e)
{
	_init(e->entityId, e->xd, e->yd, e->zd);
}

SetEntityMotionPacket::SetEntityMotionPacket(int id, double xd, double yd, double zd)
{
	_init(id, xd, yd, zd);   
}

void SetEntityMotionPacket::read(DataInputStream *dis) //throws IOException 
{
	useBytes = dis->readBoolean();
	id = dis->readInt();
	if(useBytes)
	{
		xa = static_cast<int>(dis->readByte());
		ya = static_cast<int>(dis->readByte());
		za = static_cast<int>(dis->readByte());
		xa = ( xa << 24 ) >> 24;
		ya = ( ya << 24 ) >> 24;
		za = ( za << 24 ) >> 24;
		xa *= 16;
		ya *= 16;
		za *= 16;
	}
	else
	{
		xa = dis->readShort();
		ya = dis->readShort();
		za = dis->readShort();
	}
}

void SetEntityMotionPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeBoolean(useBytes);
	if( useBytes )
	{
		dos->writeInt(id);
		dos->writeByte(xa/16);
		dos->writeByte(ya/16);
		dos->writeByte(za/16);
	}
	else
	{
		dos->writeInt(id);
		dos->writeShort(xa);
		dos->writeShort(ya);
		dos->writeShort(za);
	}
}

void SetEntityMotionPacket::handle(PacketListener *listener)
{
	listener->handleSetEntityMotion(shared_from_this());
}

int SetEntityMotionPacket::getEstimatedSize()
{
	return useBytes ? 8 : 11;
}

bool SetEntityMotionPacket::canBeInvalidated()
{
	return true;
}

bool SetEntityMotionPacket::isInvalidatedBy(shared_ptr<Packet> packet)
{
	shared_ptr<SetEntityMotionPacket> target = dynamic_pointer_cast<SetEntityMotionPacket>(packet);
	return target->id == id;
}
