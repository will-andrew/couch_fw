#include <xc.h>

#include "TCPIP Stack/HTTP2.h"

// HTTP server callbacks. Declared in HTTP2.h

HTTP_IO_RESULT HTTPExecuteGet(void)
{
	return HTTP_IO_DONE;
}

HTTP_IO_RESULT HTTPExecutePost(void)
{
	return HTTP_IO_DONE;
}

BYTE HTTPNeedsAuth(BYTE* cFile)
{
	return 0x80;
}

BYTE HTTPCheckAuth(BYTE* cUser, BYTE* cPass)
{
	return 0x80;
}