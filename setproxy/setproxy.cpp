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
	LogString("Calling InternetSetOption with NULL handle and option INTERNET_OPTION_PROXY_SETTINGS_CHANGED");
	InternetSetOption(NULL, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, NULL, 0);
}


/*
#define INTERNET_PER_CONN_FLAGS                         1
#define INTERNET_PER_CONN_PROXY_SERVER                  2
#define INTERNET_PER_CONN_PROXY_BYPASS                  3
#define INTERNET_PER_CONN_AUTOCONFIG_URL                4
#define INTERNET_PER_CONN_AUTODISCOVERY_FLAGS           5
#define INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL      6
#define INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS  7
#define INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME   8
#define INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL    9
#define INTERNET_PER_CONN_FLAGS_UI                      10
*/

void DumpPerConnOption(INTERNET_PER_CONN_OPTION &Option)
{
	if (Option.Value.dwValue == 0)
		return;
	switch (Option.dwOption)
	{
	case INTERNET_PER_CONN_FLAGS: //1
		printf("(1)INTERNET_PER_CONN_FLAGS. Connection type: %d\n", Option.Value.dwValue);
		/*Sets or retrieves the connection type. The Value member will contain one or more of the following values: 
			PROXY_TYPE_DIRECT
			The connection does not use a proxy server. 
			PROXY_TYPE_PROXY
			The connection uses an explicitly set proxy server. 
			PROXY_TYPE_AUTO_PROXY_URL
			The connection downloads and processes an automatic configuration script at a specified URL. 
			PROXY_TYPE_AUTO_DETECT
			The connection automatically detects settings. 
			//
			// PER_CONN_FLAGS
			//
			#define PROXY_TYPE_DIRECT                               0x00000001   // direct to net
			#define PROXY_TYPE_PROXY                                0x00000002   // via named proxy
			#define PROXY_TYPE_AUTO_PROXY_URL                       0x00000004   // autoproxy URL
			#define PROXY_TYPE_AUTO_DETECT                          0x00000008   // use autoproxy detection
		*/

		if (Option.Value.dwValue&  PROXY_TYPE_DIRECT)
		{
			printf("\tDIRECT(1)\tThe connection does not use a proxy server.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_PROXY)
		{
			printf("\tPROXY(2)\tThe connection uses an explicitly set proxy server.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_AUTO_PROXY_URL)
		{
			printf("\tAUTO_PROXY_URL(4)\tThe connection downloads and processes an automatic configuration script at a specified URL.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_AUTO_DETECT)
		{
			printf("\tAUTO_DETECT(8)\tThe connection automatically detects settings.\n");
		}
		break;

	case INTERNET_PER_CONN_PROXY_SERVER: //2
		printf("\n(2)INTERNET_PER_CONN_PROXY_SERVER\n");
		//Sets or retrieves a string containing the proxy servers.

		if (Option.Value.pszValue != NULL)
		{
			printf("\tProxy server :%s\n", Option.Value.pszValue);
			GlobalFree(Option.Value.pszValue);
		}
		break;
	case INTERNET_PER_CONN_PROXY_BYPASS:  //3
		printf("\n(3)INTERNET_PER_CONN_PROXY_BYPASS\n");
		//Sets or retrieves a string containing the URLs that do not use the proxy server. 
		if (Option.Value.pszValue != NULL)
		{
			printf("\tProxy bypass list :%s\n", Option.Value.pszValue);
			GlobalFree(Option.Value.pszValue);
		}
		break;
	case INTERNET_PER_CONN_AUTOCONFIG_URL: //4
		printf("\n(4)INTERNET_PER_CONN_AUTOCONFIG_URL\n");
		//Sets or retrieves a string containing the URL to the automatic configuration script. 
		if (Option.Value.pszValue != NULL)
		{
			printf("AutoConfig url :%s\n", Option.Value.pszValue);
			GlobalFree(Option.Value.pszValue);
		}
		break;
	case INTERNET_PER_CONN_AUTODISCOVERY_FLAGS: //5
		printf("\n(5)INTERNET_PER_CONN_AUTODISCOVERY_FLAGS : %d\n", Option.Value.dwValue);
		//Sets or retrieves the automatic discovery settings.The Value member will contain one or more of the following values :
		/*	AUTO_PROXY_FLAG_ALWAYS_DETECT
			Always automatically detect settings.
			AUTO_PROXY_FLAG_CACHE_INIT_RUN
			Indicates that the cached results of the automatic proxy configuration script should be used, instead of actually running the script, unless the cached file has expired.
			AUTO_PROXY_FLAG_DETECTION_RUN
			Automatic detection has been run at least once on this connection.
			AUTO_PROXY_FLAG_DETECTION_SUSPECT
			Not currently supported.
			AUTO_PROXY_FLAG_DONT_CACHE_PROXY_RESULT
			Do not allow the caching of the result of the automatic proxy configuration script.
			AUTO_PROXY_FLAG_MIGRATED
			The setting was migrated from a Microsoft Internet Explorer 4.0 installation, and automatic detection should be attempted once.
			AUTO_PROXY_FLAG_USER_SET
			The user has explicitly set the automatic detection.*/
		switch (Option.Value.dwValue)
		{
		case AUTO_PROXY_FLAG_ALWAYS_DETECT:
			printf("\tALWAYS_DETECT(2)\tAlways automatically detect settings.\n");
			break;
		case AUTO_PROXY_FLAG_CACHE_INIT_RUN:
			printf("\tCACHE_INIT_RUN(20)\tThe cached results of the automatic proxy configuration script should be used, instead of actually running the script, unless the cached file has expired.\n");
			break;
		case AUTO_PROXY_FLAG_DETECTION_RUN:
			printf("\tDETECTION_RUN(4)\tAutomatic detection has been run at least once on this connection.\n");
			break;
		case AUTO_PROXY_FLAG_DETECTION_SUSPECT:
			printf("\tDETECTION_SUSPECT(40)\tNot currently supported. \n");
			break;
		case AUTO_PROXY_FLAG_DONT_CACHE_PROXY_RESULT:
			printf("\tDONT_CACHE_PROXY_RESULT(10)\tDo not allow the caching of the result of the automatic proxy configuration script.\n");
			break;
		case AUTO_PROXY_FLAG_MIGRATED:
			printf("\tFLAG_MIGRATED(8)\tThe setting was migrated from a Microsoft Internet Explorer 4.0 installation, and automatic detection should be attempted once.\n");
			break;
		case AUTO_PROXY_FLAG_USER_SET:
			printf("\tUSER_SET(1)\tThe user has explicitly set the automatic detection.\n");
			break;
		default:
			printf("\tUnknown automatic descovery setting\n");
			break;
		}
		break;
	case INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL: //6
		printf("\n(6)INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL\n");
		//Chained autoconfig URL.Used when the primary autoconfig URL points to an INS file that sets a second autoconfig URL for proxy information.
		break;
	case INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS: //7
		printf("\n(7)INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS\n");
		//of minutes until automatic refresh of autoconfig URL by autodiscovery
		break;
	case INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME: //8
		printf("\n(8)INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME\n");
		//Read only option.Returns the time the last known good autoconfig URL was found using autodiscovery.
		break;
	case INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL: //9
		printf("(9)INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL\n");
		//Read o\nnly option.Returns the last known good URL found using autodiscovery.
		break;
	case INTERNET_PER_CONN_FLAGS_UI: //10
		printf("\nINTERNET_PER_CONN_FLAGS_UI(10):  %d\n", Option.Value.dwValue);
		/*
		Sets or retrieves the connection type. The Value member will contain one or more of the following values: 
				PROXY_TYPE_DIRECT
				The connection does not use a proxy server. 
				PROXY_TYPE_PROXY
				The connection uses an explicitly set proxy server. 
				PROXY_TYPE_AUTO_PROXY_URL
				The connection downloads and processes an automatic configuration script at a specified URL. 
				PROXY_TYPE_AUTO_DETECT
				The connection automatically detects settings. */
		if (Option.Value.dwValue &  PROXY_TYPE_DIRECT)
		{
			printf("\tDIRECT(1)\tThe connection does not use a proxy server.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_PROXY)
		{
			printf("\tPROXY(2)\tThe connection uses an explicitly set proxy server.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_AUTO_PROXY_URL)
		{
			printf("\tAUTO_PROXY_URL(4)\tThe connection downloads and processes an automatic configuration script at a specified URL.\n");
		}
		if (Option.Value.dwValue &   PROXY_TYPE_AUTO_DETECT)
		{
			printf("\tAUTO_DETECT(8)\tThe connection automatically detects settings.\n");
		}
		break;
	default:
		printf("Unknown option\n");
		break;
	}
}

BOOL GetProxySettings()
{
#define MAX_OPTIONS_NUMBER 10

	INTERNET_PER_CONN_OPTION_LIST    List;
	INTERNET_PER_CONN_OPTION         Option[10];
	unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

	Option[0].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	Option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
	Option[4].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
	Option[5].dwOption = INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL;
	Option[6].dwOption = INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS;
	Option[7].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME;
	Option[8].dwOption = INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL;
	Option[9].dwOption = INTERNET_PER_CONN_FLAGS_UI;

	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	List.pszConnection = NULL;
	List.dwOptionCount = MAX_OPTIONS_NUMBER;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if (!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
		printf("InternetQueryOption failed! (%d)\n", GetLastError());

	INTERNET_VERSION_INFO      Version;
	nSize = sizeof(INTERNET_VERSION_INFO);

	InternetQueryOption(NULL, INTERNET_OPTION_VERSION, &Version, &nSize);

	for (unsigned int numOption = 0; numOption < MAX_OPTIONS_NUMBER; numOption++)
	{
		DumpPerConnOption(Option[numOption]);
	}

	return TRUE;
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
	LogString("Calling InternetQueryOption with NULL handle and option INTERNET_OPTION_PER_CONNECTION_OPTION/INTERNET_PER_CONN_AUTODISCOVERY_FLAGS");
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
		LogString("Option:INTERNET_PER_CONN_FLAGS Value:PROXY_TYPE_DIRECT");
		if (AUTO == action)
		{
			Option[1].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
			LogString("Option:INTERNET_PER_CONN_FLAGS Value |= PROXY_TYPE_AUTO_DETECT");
			Option[0].Value.dwValue |= AUTO_PROXY_FLAG_USER_SET;
			LogString("Option:INTERNET_PER_CONN_AUTODISCOVERY_FLAGS Value |= AUTO_PROXY_FLAG_USER_SET");
		}
		else
		{
			Option[0].Value.dwValue &= ~AUTO_PROXY_FLAG_DETECTION_RUN;
			LogString("Option:INTERNET_PER_CONN_AUTODISCOVERY_FLAGS Value &= ~AUTO_PROXY_FLAG_DETECTION_RUN");
			if (MANUAL == action)
			{
				Option[1].Value.dwValue |= PROXY_TYPE_PROXY;
				LogString("Option:INTERNET_PER_CONN_FLAGS Value |= PROXY_TYPE_PROXY");
			}
			else if (AUTOCONFIG == action)
			{
				Option[1].Value.dwValue |= PROXY_TYPE_AUTO_PROXY_URL;
				LogString("Option:INTERNET_PER_CONN_FLAGS Value |= PROXY_TYPE_AUTO_PROXY_URL");
			}
			// nothing left to do for DIRECT case
		}

		List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
		List.pszConnection = NULL;
		List.dwOptionCount = 2;
		List.dwOptionError = 0;
		List.pOptions = Option;

		LogString("Calling InternetSetOption with NULL handle and option INTERNET_OPTION_PER_CONNECTION_OPTION");

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

	LogString("Calling InternetSetOption with NULL handle and option INTERNET_OPTION_PER_CONNECTION_OPTION with option %d/%X value : %s", iProxyOption, iProxyOption, pszValue);
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
	printf("SetProxy.exe Version 1.1\n");
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
	printf("How to programmatically query and set proxy settings under WinINet https://support.microsoft.com/en-us/help/226473/how-to-programmatically-query-and-set-proxy-settings-under-internet-ex\n");
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
	GetProxySettings();

	switch (action)
	{
	case AUTO:
		if (TRUE == ConfigureProxy(AUTO))
		{
			LogString("Successfully configured WinINet to use the Automatic Proxy detection.");
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
			LogString("Successfully configured WinINet to use Manual Proxy settings.");
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
				LogString("Successfully configured WinINet Manual Proxy server to %s.", pszBuffer);
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
			LogString("Successfully configured WinINet to use Direct Internet access. The Proxy setting have been turned off.");
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
			LogString("Successfully configured WinINet Proxy ByPass settings to %s.", pszBuffer);
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
				LogString("Successfully configured WinINet AutoconfigURL to %s.", pszBuffer);
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
