#include "VirtualBox.h"

/*
Registry key values
*/

VOID vbox_reg_key_value()
{
	/* Array of strings of blacklisted registry key values */
	TCHAR *szEntries[][3] = {
		{ _T("HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0"), _T("Identifier"), _T("VBOX") },
		{ _T("HARDWARE\\Description\\System"), _T("SystemBiosVersion"), _T("VBOX") },
		{ _T("HARDWARE\\Description\\System"), _T("VideoBiosVersion"), _T("VIRTUALBOX") },
		{ _T("HARDWARE\\Description\\System"), _T("SystemBiosDate"), _T("06/23/99") },
	};

	WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);

	for (int i = 0; i < dwLength; i++)
	{
		_tprintf(_T("[*] Checking reg key %s:"), szEntries[i][0]);
		if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, szEntries[i][0], szEntries[i][1], szEntries[i][2]))
			print_detected();
		else
			print_not_detected();
	}
}

/*
Check against virtualbox registry keys
*/
VOID vbox_reg_keys()
{
	/* Array of strings of blacklisted registry keys */
	TCHAR* szKeys[] = {
		_T("HARDWARE\\ACPI\\RSDT\\VBOX__"),
		_T("HARDWARE\\ACPI\\FADT\\VBOX__"),
		_T("HARDWARE\\ACPI\\RSDT\\VBOX__"),
		_T("SOFTWARE\\Oracle\\VirtualBox Guest Additions"),
		_T("SYSTEM\\ControlSet001\\Services\\VBoxGuest"),
		_T("SYSTEM\\ControlSet001\\Services\\VBoxMouse"),
		_T("SYSTEM\\ControlSet001\\Services\\VBoxService"),
		_T("SYSTEM\\ControlSet001\\Services\\VBoxSF"),
		_T("SYSTEM\\ControlSet001\\Services\\VBoxVideo")
	};

	WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

	/* Check one by one */
	for (int i = 0; i < dwlength; i++)
	{
		_tprintf(TEXT("[*] Checking reg key %s: "), szKeys[i]);
		if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, szKeys[i]))
			print_detected();
		else
			print_not_detected();
	}
}



/*
Check against virtualbox blacklisted files
*/
VOID vbox_files()
{
	/* Array of strings of blacklisted paths */
	TCHAR* szPaths[] = {
		_T("system32\\drivers\\VBoxMouse.sys"),
		_T("system32\\drivers\\VBoxGuest.sys"),
		_T("system32\\drivers\\VBoxSF.sys"),
		_T("system32\\drivers\\VBoxVideo.sys"),
		_T("system32\\vboxdisp.dll"),
		_T("system32\\vboxhook.dll"),
		_T("system32\\vboxmrxnp.dll"),
		_T("system32\\vboxogl.dll"),
		_T("system32\\vboxoglarrayspu.dll"),
		_T("system32\\vboxoglcrutil.dll"),
		_T("system32\\vboxoglerrorspu.dll"),
		_T("system32\\vboxoglfeedbackspu.dll"),
		_T("system32\\vboxoglpackspu.dll"),
		_T("system32\\vboxoglpassthroughspu.dll"),
		_T("system32\\vboxservice.exe"),
		_T("system32\\vboxtray.exe"),
		_T("system32\\VBoxControl.exe"),
	};

	/* Getting Windows Directory */
	WORD dwlength = sizeof(szPaths) / sizeof(szPaths[0]);
	TCHAR szWinDir[MAX_PATH] = _T("");
	TCHAR szPath[MAX_PATH] = _T("");
	GetWindowsDirectory(szWinDir, MAX_PATH);

	/* Check one by one */
	for (int i = 0; i < dwlength; i++)
	{
		PathCombine(szPath, szWinDir, szPaths[i]);
		_tprintf(TEXT("[*] Checking file %s: "), szPath);
		if (is_FileExists(szPath))
			print_detected();
		else
			print_not_detected();
	}
}



/*
Check against virtualbox blacklisted directories
*/
BOOL vbox_dir()
{
	TCHAR szProgramFile[MAX_PATH];
	TCHAR szPath[MAX_PATH] = _T("");
	TCHAR szTarget[MAX_PATH] = _T("oracle\\virtualbox guest additions\\");

	if (IsWoW64())
		ExpandEnvironmentStrings(_T("%ProgramW6432%"), szProgramFile, ARRAYSIZE(szProgramFile));
	else
		SHGetSpecialFolderPath(NULL, szProgramFile, CSIDL_PROGRAM_FILES, FALSE);

	PathCombine(szPath, szProgramFile, szTarget);
	return is_DirectoryExists(szPath);
}


/*
Check virtualbox NIC MAC address
*/
BOOL vbox_check_mac()
{
	/* VirtualBox mac starts with 08:00:27 */
	return check_mac_addr("\x08\x00\x27");
}



/*
Check against pseaudo-devices
*/
VOID vbox_devices()
{
	TCHAR *devices[] = {
		_T("\\\\.\\VBoxMiniRdrDN"),
		_T("\\\\.\\VBoxGuest"),
		_T("\\\\.\\pipe\\VBoxMiniRdDN"),
		_T("\\\\.\\VBoxTrayIPC"),
		_T("\\\\.\\pipe\\VBoxTrayIPC")
	};

	WORD iLength = sizeof(devices) / sizeof(devices[0]);
	for (int i = 0; i < iLength; i++)
	{
		HANDLE hFile = CreateFile(devices[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		_tprintf(TEXT("[*] Checking device %s: "), devices[i]);
		if (hFile != INVALID_HANDLE_VALUE)
			print_detected();
		else
			print_not_detected();
	}
}

/*
Check for Window class
*/
BOOL vbox_window_class()
{
	HWND hClass = FindWindow(_T("VBoxTrayToolWndClass"), NULL);
	HWND hWindow = FindWindow(NULL, _T("VBoxTrayToolWnd"));

	if (hClass || hWindow)
		return TRUE;
	else
		return FALSE;
}

/*
Check for shared folders network profider
*/
BOOL vbox_network_share()
{
	TCHAR szProviderName[MAX_PATH] = _T("");
	DWORD lpBufferSize = MAX_PATH;

	if (WNetGetProviderName(WNNC_NET_RDR2SAMPLE, szProviderName, &lpBufferSize) == NO_ERROR)
	{
		if (StrCmpI(szProviderName, _T("VirtualBox Shared Folders")) == 0)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

/*
Check for process list
*/

VOID vbox_processes()
{
	TCHAR *szProcesses[] = {
		_T("vboxservice.exe"),
		_T("vboxtray.exe")
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		_tprintf(TEXT("[*] Checking virtual box processe %s: "), szProcesses[i]);
		if (GetProcessIdFromName(szProcesses[i]))
			print_detected();
		else
			print_not_detected();
	}
}

/*
Check vbox devices using WMI
*/
BOOL vbox_devices_wmi()
{
	IWbemServices *pSvc = NULL;
	IWbemLocator *pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	// Init WMI
	bStatus = InitWMI(&pSvc, &pLoc);
	
	if (bStatus)
	{
		// If success, execute the desired query
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, _T("SELECT * FROM Win32_PnPEntity"));
		if (bStatus)
		{
			// Get the data from the query
			IWbemClassObject *pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				// Get the value of the Name property
				hRes = pclsObj->Get(_T("DeviceId"), 0, &vtProp, 0, 0);
				
				// Do our comparaison
				if (_tcsstr(vtProp.bstrVal, _T("PCI\\VEN_80EE&DEV_CAFE")) != 0)
				{
					bFound = TRUE;
					break;
				}

				// release the current result object
				VariantClear(&vtProp);
				pclsObj->Release();
			}

			// Cleanup
			pSvc->Release();
			pLoc->Release();
			pEnumerator->Release();
			CoUninitialize();
		}
	}

	return bFound;
}


/*
Check vbox mac @ using WMI
*/
BOOL vbox_mac_wmi()
{
	IWbemServices *pSvc = NULL;
	IWbemLocator *pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	// Init WMI
	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
		// If success, execute the desired query
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, _T("SELECT * FROM Win32_NetworkAdapterConfiguration"));
		if (bStatus)
		{
			// Get the data from the query
			IWbemClassObject *pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			// Iterate over our enumator
			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				// Get the value of the Name property
				hRes = pclsObj->Get(_T("MACAddress"), 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

					// Do our comparaison
					if (_tcsstr(vtProp.bstrVal, _T("08:00:27")) != 0){
						bFound = TRUE; break;
					}

					// release the current result object
					VariantClear(&vtProp);
					pclsObj->Release();
				}
			}

			// Cleanup
			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
		}
	}

	return bFound;
}

/*
Check vbox event log using WMI
*/
BOOL vbox_eventlogfile_wmi()
{
	IWbemServices *pSvc = NULL;
	IWbemLocator *pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	// Init WMI
	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
		// If success, execute the desired query
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, _T("SELECT * FROM Win32_NTEventlogFile"));
		if (bStatus)
		{
			// Get the data from the query
			IWbemClassObject *pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			// Iterate over our enumator
			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;


				// Get the value of the FileName property
				hRes = pclsObj->Get(_T("FileName"), 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

					// Do our comparaison
					if (StrCmpI(vtProp.bstrVal, _T("System")) == 0) {

						// Now, grab the Source property
						VariantClear(&vtProp);
						hRes = pclsObj->Get(_T("Source"), 0, &vtProp, 0, 0);
						bFound = TRUE; break;
					}

					// release the current result object
					VariantClear(&vtProp);
					pclsObj->Release();
				}

			}

			// Cleanup
			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();

		}
	}

	return bFound;
}
