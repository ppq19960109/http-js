#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "announceURL.hh"

#include "myRTSPServer.hh"

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;

static void announceStream(RTSPServer *rtspServer, ServerMediaSession *sms,
                           char const *streamName, char const *inputFileName)
{
  UsageEnvironment &env = rtspServer->envir();

  env << "\n\"" << streamName << "\" stream, from the file \""
      << inputFileName << "\"\n";
  announceURL(rtspServer, sms);
}

// static int getTestFrame(int chId, int srcId, unsigned char *buf, int size)
// {
//   return 0;
// }
// #define ACCESS_CONTROL
int main(int argc, char **argv)
{
  UsageEnvironment *env;
  if (SAMPLE_VENC_H265_H264(1))
    return -1;
  // Begin by setting up our usage environment:
  TaskScheduler *scheduler = BasicTaskScheduler::createNew(5000);
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase *authDB = NULL;

#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("ppq", "123"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server:
  RTSPServer *rtspServer = RTSPServer::createNew(*env, 8554, authDB);
  if (rtspServer == NULL)
  {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  // OutPacketBuffer::maxSize = 5000000;
  char const *descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

  // Set up each of the possible streams that can be served by the
  // RTSP server.  Each such stream is implemented using a
  // "ServerMediaSession" object, plus one or more
  // "ServerMediaSubsession" objects for each audio/video substream.

  // A MPEG-4 video elementary stream:
  {
    char const *streamName = "mpeg4ESVideoTest";
    char const *inputFileName = "test.m4e";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(MPEG4VideoFileServerMediaSubsession ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A H.264 video elementary stream:
  {
    char const *streamName = "h264ESVideoTest";
    char const *inputFileName = "test.264";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(H264VideoFileServerMediaSubsession ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // A H.264 live video elementary stream:
  {
    char const *streamName = "h264Live";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(H264VideoLiveServerMediaSubsession::createNew(*env, getVencFrame, 0, 0, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, "hisih264Live");
  }

  {
    char const *streamName = "h265Live";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(H265VideoLiveServerMediaSubsession::createNew(*env, getVencFrame, 1, 0, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, "hisih265Live");
  }

  // A H.265 video elementary stream:
  {
    char const *streamName = "h265ESVideoTest";
    char const *inputFileName = "test.265";
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, streamName, streamName,
                                                            descriptionString);
    sms->addSubsession(H265VideoFileServerMediaSubsession ::createNew(*env, inputFileName, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, inputFileName);
  }

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080))
  {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  }
  else
  {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return
  SAMPLE_VENC_H265_H264(0);
  return 0; // only to prevent compiler warning
}
