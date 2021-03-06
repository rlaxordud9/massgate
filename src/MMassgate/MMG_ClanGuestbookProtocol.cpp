// Massgate
// Copyright (C) 2017 Ubisoft Entertainment
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "StdAfx.h"
#include "MMG_ClanGuestbookProtocol.h"
#include "MMG_Messaging.h"
#include "MMG_ProtocolDelimiters.h"

void 
MMG_ClanGuestbookProtocol::PostReq::ToStream(MN_WriteMessage& theStream) const
{
	theStream.WriteDelimiter(MMG_ProtocolDelimiters::MESSAGING_CLAN_GUESTBOOK_POST_REQ); 
	theStream.WriteUInt(clanId); 
	theStream.WriteUInt(getGuestbook); 
	theStream.WriteUInt(requestId); 
	theStream.WriteLocString(msg.GetBuffer(), msg.GetLength()); 
}

bool 
MMG_ClanGuestbookProtocol::PostReq::FromStream(MN_ReadMessage& theStream)
{
	bool good = true; 

	good = good && theStream.ReadUInt(clanId); 
	good = good && theStream.ReadUInt(getGuestbook); 
	good = good && theStream.ReadUInt(requestId); 
	good = good && theStream.ReadLocString(msg.GetBuffer(), msg.GetBufferSize()); 

	return good; 
}

//////////////////////////////////////////////////////////////////////////

void 
MMG_ClanGuestbookProtocol::GetReq::ToStream(MN_WriteMessage& theStream) const
{
	theStream.WriteDelimiter(MMG_ProtocolDelimiters::MESSAGING_CLAN_GUESTBOOK_GET_REQ); 
	theStream.WriteUInt(requestId); 
	theStream.WriteUInt(clanId); 
}

bool 
MMG_ClanGuestbookProtocol::GetReq::FromStream(MN_ReadMessage& theStream)
{
	bool good = true; 

	good = good && theStream.ReadUInt(requestId); 
	good = good && theStream.ReadUInt(clanId); 

	return good; 
}

//////////////////////////////////////////////////////////////////////////

MMG_ClanGuestbookProtocol::GetRsp::GetRsp()
{
}

void 
MMG_ClanGuestbookProtocol::GetRsp::ToStream(MN_WriteMessage& theStream) const
{
	if(messages.Count() == 0)
	{
		theStream.WriteDelimiter(MMG_ProtocolDelimiters::MESSAGING_CLAN_GUESTBOOK_GET_RSP); 
		theStream.WriteUInt(requestId); 
		theStream.WriteUInt(0); 	
	}
	else 
	{
		for(int i = messages.Count() - 1; i >= 0;)
		{
			theStream.WriteDelimiter(MMG_ProtocolDelimiters::MESSAGING_CLAN_GUESTBOOK_GET_RSP); 
			theStream.WriteUInt(requestId); 

			if(i >= 7)
				theStream.WriteUInt(8); 
			else 
				theStream.WriteUInt(i + 1); 

			for(int p = 0; p < 8 && i >= 0; p++, i--)
			{
				theStream.WriteLocString(messages[i].msg.GetBuffer(), messages[i].msg.GetLength()); 
				theStream.WriteUInt(messages[i].timestamp); 
				theStream.WriteUInt(messages[i].profileId); 
				theStream.WriteUInt(messages[i].messageId); 			
			}
		}
	}
}

bool 
MMG_ClanGuestbookProtocol::GetRsp::FromStream(MN_ReadMessage& theStream)
{
	bool good = true; 

	good = good && theStream.ReadUInt(requestId);
	unsigned int numMessages; 
	good = good && theStream.ReadUInt(numMessages); 
	for(unsigned int i = 0; good && i < numMessages; i++)
	{
		MC_StaticLocString<MSG_MAX_LEN> msg;
		unsigned int timestamp, profileId, messageId; 
		good = good && theStream.ReadLocString(msg.GetBuffer(), msg.GetBufferSize()); 
		good = good && theStream.ReadUInt(timestamp); 
		good = good && theStream.ReadUInt(profileId); 
		good = good && theStream.ReadUInt(messageId); 
		if(good)
			messages.Add(GuestbookEntry(msg.GetBuffer(), timestamp, profileId, messageId)); 
	}

	return good; 
}

void 
MMG_ClanGuestbookProtocol::GetRsp::AddMessage(MC_LocChar* aMessage, 
											  unsigned int aTimestamp, 
											  unsigned int aProfileId, 
											  unsigned int aMessageId)
{
	messages.Add(GuestbookEntry(aMessage, aTimestamp, aProfileId, aMessageId)); 
}

//////////////////////////////////////////////////////////////////////////

void 
MMG_ClanGuestbookProtocol::DeleteReq::ToStream(MN_WriteMessage& theStream) const
{
	theStream.WriteDelimiter(MMG_ProtocolDelimiters::MESSAGING_CLAN_GUESTBOOK_DELETE_REQ); 
	theStream.WriteUInt(messageId); 
	theStream.WriteUChar(deleteAllByThisProfile ? 1 : 0);
}

bool 
MMG_ClanGuestbookProtocol::DeleteReq::FromStream(MN_ReadMessage& theStream)
{
	bool good = true; 

	good = good && theStream.ReadUInt(messageId); 
	unsigned char t; 
	good = good && theStream.ReadUChar(t);
	if(good)
		deleteAllByThisProfile = t ? true : false;

	return good; 
}