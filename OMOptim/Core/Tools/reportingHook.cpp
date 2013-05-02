#if defined(WIN32)
 
#include <string.h>
#ifdef _MSC_VER
#include "crtdbg.h"
#endif
 
#define FALSE   0
#define TRUE    1

#ifdef _MSC_VER
_CRT_REPORT_HOOK prevHook;
#endif

int reportingHook(int reportType, char* userMessage, int* retVal)
{
  // This function is called several times for each memory leak.
  // Each time a part of the error message is supplied.
  // This holds number of subsequent detail messages after
  // a leak was reported
  const int numFollowupDebugMsgParts = 2;
  static bool ignoreMessage = false;
  static int debugMsgPartsCount = 0;
 
  // check if the memory leak reporting starts
  if ((strncmp(userMessage,"Detected memory leaks!\n", 10) == 0)
    || ignoreMessage)
  {
    // check if the memory leak reporting ends
    if (strncmp(userMessage,"Object dump complete.\n", 10) == 0)
    {
      #ifdef _MSC_VER
      _CrtSetReportHook(prevHook);
      #endif
      ignoreMessage = false;
    } else
      ignoreMessage = true;
 
    // something from our own code?
    if(strstr(userMessage, ".cpp") == NULL)
    {
      if(debugMsgPartsCount++ < numFollowupDebugMsgParts)
  // give it back to _CrtDbgReport() to be printed to the console
  return FALSE;
      else
  return TRUE;  // ignore it
    } else
    {
      debugMsgPartsCount = 0;
      // give it back to _CrtDbgReport() to be printed to the console
      return FALSE;
    }
  } else
    // give it back to _CrtDbgReport() to be printed to the console
    return FALSE;
};
 
void setFilterDebugHook(void)
{
  //change the report function to only report memory leaks from program code
  #ifdef _MSC_VER
  prevHook = _CrtSetReportHook(reportingHook);
  #endif
}
 
#endif
