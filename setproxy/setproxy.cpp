// setproxy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <atlbase.h>
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#pragma comment(lib, "wininet.lib")

enum SETPROXY_ACTION
{
	AUTO,
	DIRECT,
	MANUAL,
	MANUALSERVER,
	BYPASS,
	AUTOCONFIG,
	USAGE
};


void LogString(const char *lpsz, ...)
{
	char szBuffer[1024] = { 0 };
	va_list     list;
	va_start(list, lpsz);
	HRESULT hr = StringCchVPrintf(szBuffer, ARRAYSIZE(szBuffer), lpsz, list);

	if (SUCCEEDED(hr) && strlen(szBuffer))
	{
		printf("%s\r\n", szBuffer);	
	}
	else
	{
		printf("LogString error\n");
	}
	va_end(list);
}


void NotifyProxyChanged()
{
	//To alert all available WinInet instances, set the Buffer parameter of InternetSetOption to NULL and BufferLength to 0 when passing this option
	InternetSetOption(NULL, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, NULL, 0);
}

BOOL ConfigureProxy(SETPROXY_ACTION action)
{
	INTERNET_PER_CONN_OPTION_LIST    List;
	INTERNET_PER_CONN_OPTION         Option[2];
	unsigned long                    cbList = sizeof(INTERNET_PER_CONN_OPTION_LIST);

	BOOL bReturn = FALSE;

	/*
	** First have to query for autodiscovery flags. They're used later in the
	** case of setting autodetect.
	*/
	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	List.pszConnection = NULL;
	List.dwOptionCount = 1;
	List.dwOptionError = 0;
	List.pOptions = Option;

	Option[0].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
	LogString("Calling InternetQueryOption with NULL handle and option INTERNET_OPTION_PER_CONNECTION_OPTION");
	if (FALSE != InternetQueryOption(
		NULL,
		INTERNET_OPTION_PER_CONNECTION_OPTION,
		&List,
		&cbList))
	{
		LogString("InternetQueryOption succeeded");
		Option[1].dwOption = INTERNET_PER_CONN_FLAGS;

		/*
		** InetCPL sets PROXY_TYPE_DIRECT regardless of the state of the
		** checkboxes on the dialog.
		**
		** The reason this needs to be done is so that if autodetect fails for
		** whatever reason (autodetect script fails, doesn't produce a correct
		** result, etc.), wininet will fall back to direct.
		**
		** InetCPL also sets AUTO_PROXY_FLAG_USER_SET when the user checks the
		** autodetect box, which is essentially what's happening here. It also
		** clears AUTO_PROXY_FLAG_DETECTION_RUN when autodetect is turned off.
		**
		** AUTO_PROXY_FLAG_USER_SET needs to be set to inform wininet that the
		** user explicitly chose autodetection. This disables a mechanism
		** wininet employs to disable autodetection if it fails, useful in the
		** case of an out-of-box machine that comes pre-configured with
		** autodetect enabled.
		*/

		Option[1].Value.dwValue = PROXY_TYPE_DIRECT;

		if (AUTO == action)
		{
			Option[1].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
			Option[0].Value.dwValue |= AUTO_PROXY_FLAG_USER_SET;
		}
		else
		{
			Option[0].Value.dwValue &= ~AUTO_PROXY_FLAG_DETECTION_RUN;

			if (MANUAL == action)
			{
				Option[1].Value.dwValue |= PROXY_TYPE_PROXY;
			}
			else if (AUTOCONFIG == action)
			{
				Option[1].Value.dwValue |= PROXY_TYPE_AUTO_PROXY_URL;
			}
			// nothing left to do for DIRECT case
		}

		List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
		List.pszConnection = NULL;
		List.dwOptionCount = 2;
		List.dwOptionError = 0;
		List.pOptions = Option;

		if (InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, cbList))
		{
			//Notify IE of the Proxy Setting change
			NotifyProxyChanged();
			bReturn = TRUE;
		}
		else
		{
			LogString("InternetSetOption failed for ConfigureProxy! (%d)\n", GetLastError());
		}
	}
	else
	{
		LogString("InternetQueryOption failed for ConfigureProxy! (%d)\n", GetLastError());
	}

	return bReturn;

}

BOOL SetProxyOption(__in_opt INT iProxyOption, __in_opt char * pszValue)
{
	BOOL bReturn = FALSE;

	INTERNET_PER_CONN_OPTION_LIST    List;
	INTERNET_PER_CONN_OPTION         Option[1];
	unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

	Option[0].dwOption = iProxyOption;
	Option[0].Value.pszValue = pszValue;

	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	List.pszConnection = NULL;
	List.dwOptionCount = 1;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if (InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, nSize))
	{
		NotifyProxyChanged();
		bReturn = TRUE;
	}
	else
	{
		LogString("InternetSetOption failed for SetProxyOption! (%d)\n", GetLastError());
	}
	return bReturn;

}
BOOL SetProxyServer(__in_opt char * pszHostPort)
{
	return SetProxyOption(INTERNET_PER_CONN_PROXY_SERVER, pszHostPort);
}
BOOL SetProxyByPass(__in_opt char * pszByPass)
{
	return SetProxyOption(INTERNET_PER_CONN_PROXY_BYPASS, pszByPass);
}
BOOL SetProxyAutoConfig(__in_opt char * pszAutoURL)
{
	return SetProxyOption(INTERNET_PER_CONN_AUTOCONFIG_URL, pszAutoURL);
}

INT Usage()
{
	printf("SetProxy.exe Version 1.0\n");
	printf("SetProxy.exe autoconfigURL|auto|direct|manual|manual HOST:PORT[;https=HOST:PORT][;ftp=HOST:PORT]|bypass <bypass ports>\n");
	printf("autoconfigURL http://proxy/autoconfig.pac \n");
	printf("auto    --  Auto detect proxy settings.\n");
	printf("direct  --  Direct Internet Access, proxy disabled.\n");
	printf("manual  --  Manually Set proxy settings (use existing settings).\n");
	printf("bypass  --  Set the Proxy Bypass addresses.\n");
	printf("manual http=HOST:PORT[;ftp=HOST:PORT]  --  (configure server)\n");
	printf("manual http=http://proxy:80;https=https://proxy:80\n");
	printf("bypass \"172.*;157.*;10.*;127.*;<local>\"\n");
	printf("\nReferences\n**********\n");
	printf("Setting and Retrieving Internet Options https://msdn.microsoft.com/en-us/library/aa385384(v=vs.85).aspx\n");
	printf("How to programmatically query and set proxy settings under Internet Explorer https://support.microsoft.com/en-us/help/226473/how-to-programmatically-query-and-set-proxy-settings-under-internet-ex\n");
	printf("InternetQueryOption function https://msdn.microsoft.com/en-us/library/aa385101(v=vs.85).aspx\n");
	return 0; 
}



INT __cdecl main(int argc, __in_ecount(argc) LPSTR  *argv)
{
	INT iReturn = -1;

	SETPROXY_ACTION action = USAGE;

	HRESULT hrCoInit = CoInitialize(NULL);

	// this not getting called if the CoInit() fails won't be fatal
	// g_pIStatus will still be NULL, so all the LogString() calls will 
	// just skip over making the call into Piper for that portion of the logging
	if (FAILED(hrCoInit))
	{
		exit(-1L);
	}

	char * pszBuffer = NULL;

	if (argc >1)
	{
		// AutoconfigURL should be before Auto since first 4 letters are the same 
		if (0 == _strnicmp("autoconfigURL", argv[1], 13))
		{
			// Adding the Action before if (argc >2) statement since user might specify blank as the autoconfig 
			// Incase user specifies blank in Inetcpl.cpl then the connection is returned to Direct. We are simulating this behaviour			
			action = AUTOCONFIG;
			LogString("Setting AutoConfig url");
			if (argc >2)
			{
				pszBuffer = argv[2];
			}
			else
			{
				pszBuffer = "";
			}

		}
		else if (0 == _strnicmp("auto", argv[1], 4))
		{
			action = AUTO;
			LogString("Setting Auto detect proxy settings.");
		}
		else if (0 == _strnicmp("manual", argv[1], 6))
		{
			action = MANUAL;
			if (argc >2)
			{
				action = MANUALSERVER;
				LogString("Setting Hardcoded proxy");
				pszBuffer = argv[2];
			}
		}
		else if (0 == _strnicmp("direct", argv[1], 6))
		{
			action = DIRECT;
			LogString("Setting Direct Internet Access, proxy disabled.");
		}
		else if (0 == _strnicmp("bypass", argv[1], 6))
		{
			if (argc >2)
			{
				action = BYPASS;
				LogString("Setting the Proxy Bypass addresses");
				pszBuffer = argv[2];
			}
		}

	}

	switch (action)
	{
	case AUTO:
		if (TRUE == ConfigureProxy(AUTO))
		{
			LogString("Successfully configured Interenet Explorer to use the Automatic Proxy detection.");
			iReturn = 0;
		}
		else
		{
			LogString("Failed to configure the Automatic Proxy detection.");
		}
		break;
	case MANUAL:
		if (TRUE == ConfigureProxy(MANUAL))
		{
			LogString("Successfully configured Internet Explorer to use Manual Proxy settings.");
			iReturn = 0;
		}
		else
		{
			LogString("Failed to configure the Proxy settings to Manual.");
		}
		break;

	case MANUALSERVER:
		if (TRUE == ConfigureProxy(MANUAL))
		{
			if (TRUE == SetProxyServer(pszBuffer))
			{
				LogString("Successfully configured Internet Explorer Manual Proxy server to %s.", pszBuffer);
				iReturn = 0;
			}
			else
			{
				LogString("Failed to configure the Manual Proxy server using %s.", pszBuffer);
			}
		}
		else
		{
			LogString("Failed to configure the Proxy settings to Manual.");
		}
		break;
	case DIRECT:
		if (TRUE == ConfigureProxy(DIRECT))
		{
			LogString("Successfully configured Internet Explorer to use Direct Internet access. The Proxy setting have been turned off.");
			iReturn = 0;
		}
		else
		{
			LogString("Failed to configure the Proxy setting for Direct Internet Access.");
		}
		break;
	case BYPASS:
		if (TRUE == SetProxyByPass(pszBuffer))
		{
			LogString("Successfully configured Internet Explorer Proxy ByPass settings to %s.", pszBuffer);
			iReturn = 0;
		}
		else
		{
			LogString("Failed to configure the Proxy ByPass settings.");
		}
		break;
	case AUTOCONFIG:
		if (TRUE == ConfigureProxy(AUTOCONFIG))
		{

			if (TRUE == SetProxyAutoConfig(pszBuffer))
			{
				LogString("Successfully configured Internet Explorer AutoconfigURL to %s.", pszBuffer);
				iReturn = 0;
			}
			else
			{
				LogString("Failed to configure IE to AutoconfigURL using %s.", pszBuffer);
			}
		}
		else
		{
			LogString("Failed to configure the Proxy settings to Manual.");
		}
		break;
	case USAGE:
	default:
		iReturn = Usage();
		break;
	}


	if (SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	return iReturn;

}
