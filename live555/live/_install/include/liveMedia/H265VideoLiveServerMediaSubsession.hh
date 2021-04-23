/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2021 Live Networks, Inc.  All rights reserved.
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s
// on demand, from a H265 Elementary Stream video file.
// C++ header

#ifndef _H265_VIDEO_LIVE_SERVER_MEDIA_SUBSESSION_HH
#define _H265_VIDEO_LIVE_SERVER_MEDIA_SUBSESSION_HH

#include "ByteStreamLiveSource.hh"
#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif

class H265VideoLiveServerMediaSubsession: public OnDemandServerMediaSubsession {
public:
  static H265VideoLiveServerMediaSubsession*
  createNew(UsageEnvironment& env, GetFrameCB cb, int mchId,int msrcId, Boolean reuseFirstSource);

  // Used to implement "getAuxSDPLine()":
  void checkForAuxSDPLine1();
  void afterPlayingDummy1();

protected:
  H265VideoLiveServerMediaSubsession(UsageEnvironment& env,
				       GetFrameCB cb, int mchId,int msrcId, Boolean reuseFirstSource);
      // called only by createNew();
  virtual ~H265VideoLiveServerMediaSubsession();

  void setDoneFlag() { fDoneFlag = ~0; }

protected: // redefined virtual functions
  virtual char const* getAuxSDPLine(RTPSink* rtpSink,
				    FramedSource* inputSource);
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

private:
	GetFrameCB frameCb;
	int chId; //0-1
	int srcId;//0 main,1 sub
  char* fAuxSDPLine;
  char fDoneFlag; // used when setting up "fAuxSDPLine"
  RTPSink* fDummyRTPSink; // ditto
};

#endif
