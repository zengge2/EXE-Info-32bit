#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include "resource.h"
#include "main.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//Main dialog-box handle
HWND hMainDlg;

//Icons
HICON hMainIcon32, hMainIcon16;

//The color of main edit control
COLORREF crMainEdit;

//The brush for painting the main edit control
HBRUSH hMainEditBrush = NULL;
//The font for the main edit control
HFONT hEditFont;

//Error buffer.
char szError[ERROR_BUFFER_SIZE];

char szBytesWord[] = " �ֽ�";

//Display error message.
void ErrorMsg(DWORD dwErr)
{
	char *lpMessageBuffer; 
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMessageBuffer, 0, NULL);
	if (strlen(lpMessageBuffer) < ERROR_BUFFER_SIZE)
		strcpy(szError, lpMessageBuffer);

	LocalFree(lpMessageBuffer); 					
}


void SetMainText(LPCSTR lpszText)
{
	SetDlgItemText(hMainDlg, IDC_EDIT_MAIN, lpszText);
}


//Display error in the main window
void ShowError(LPCSTR lpszFilename, LPCSTR szMsg)
{
	DWORD dwError = GetLastError();
	char szMessage[ERROR_BUFFER_SIZE];

	ErrorMsg(dwError);
	sprintf(szMessage, szMsg, lpszFilename, dwError, szError);
	SetMainText(szMessage);	
}


//Fill LOGFONT structure.
void FillFontStruct(LOGFONT *logfont, LPSTR name, int size, BOOL bold)
{
	logfont->lfHeight = size;
	logfont->lfWidth = 0;
	logfont->lfEscapement = 0;
	logfont->lfOrientation = 0;
	logfont->lfWeight = 400 + 300 * (bold == TRUE);
	logfont->lfItalic = FALSE;
	logfont->lfUnderline = FALSE;
	logfont->lfStrikeOut = FALSE;
	logfont->lfCharSet = DEFAULT_CHARSET;
	logfont->lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont->lfQuality = DEFAULT_QUALITY;
	logfont->lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont->lfFaceName, name);
}

//Set the font for the main edit control
void SetEditFont(HWND hWnd)
{
	LOGFONT lf;

	FillFontStruct(&lf, "����", 18, FALSE);
	hEditFont = CreateFontIndirect(&lf);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hEditFont, FALSE);
}


void StrTrim(char *source, char *dest)
{
	int nIndex = 0;

	while (source[nIndex] == ' ')
	{
		nIndex++;
	}
	strcpy(dest, &source[nIndex]);
}


void RemoveQuotes(LPSTR szCommand)
{
	int nGet = 0, nSet = 0;

	while (szCommand[nGet] != '\0')
	{
		if  (szCommand[nGet] == '\"') 
			nGet++;
		else
		{
			szCommand[nSet] = szCommand[nGet];
			nSet++;
			nGet++;
		}
	}

	szCommand[nSet] = '\0';
}


//Get the number of color bits in the current screen device.
int GetColorsBits()
{
	HDC hScreenDC = GetDC(0);
	int nColorBits = GetDeviceCaps(hScreenDC, BITSPIXEL) * GetDeviceCaps(hScreenDC, PLANES);
	ReleaseDC(NULL, hScreenDC);
	return nColorBits;
}

//Create brush for the main edit control
void PrepareEditBrush()
{
	int nColorBits = GetColorsBits();
	
	if (hMainEditBrush != NULL) DeleteObject(hMainEditBrush);
	if (nColorBits >= 16) 
	{
		crMainEdit = RGB(255, 248, 240);
		hMainEditBrush = CreateSolidBrush(crMainEdit);
	} 
	else
	{
		crMainEdit = RGB(255, 255, 255);
		hMainEditBrush = CreateSolidBrush(crMainEdit);
	} 

}


//Display 'Open File' dialog-box
BOOL OpenFileDialog(LPSTR lpFilename)
{
	OPENFILENAME of;
	char szFilter[] = "��ִ���ļ� (*.exe)\0*.exe\0��̬���ӿ� (*.dll)\0*.dll\0ActiveX ģ�� (*.ocx)\0*.ocx\0�����ļ� (*.drv;*.vxd;*.sys;*.386)\0*.drv;*.vxd;*.sys;*.386\0�����ļ� (*.fon)\0*.fon\0�����ļ� (*.*)\0*.*\0\0";
	char szFile[MAX_PATH] = "\0";
	char szTitle[] = "ѡ��һ��PE�ļ�";
	char szExt[] = "exe";

	of.lStructSize = sizeof(of);
	of.hwndOwner = hMainDlg;
	of.lpstrFilter = szFilter;
	of.lpstrCustomFilter = NULL;
	of.nFilterIndex = 0;
	of.lpstrFile = szFile;
	of.nMaxFile = MAX_PATH;
	of.lpstrFileTitle = NULL;
	of.lpstrInitialDir = NULL;
	of.lpstrTitle = szTitle;
	of.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	of.lpstrDefExt = szExt;
	if (GetOpenFileName(&of))
	{
		strcpy(lpFilename, of.lpstrFile); 
		return TRUE;
	} else return FALSE;

}

void ClearMainText()
{
	SetDlgItemText(hMainDlg, IDC_MAIN_EDIT, "");
}

//Add text to the main edit control
void AddMainText(LPCSTR lpszText)
{
	char szTemp[MAINTEXT_BUFFER_SIZE];
	
	GetDlgItemText(hMainDlg, IDC_MAIN_EDIT, szTemp, MAINTEXT_BUFFER_SIZE);
	if (strlen(szTemp) + strlen(lpszText) + 1 < MAINTEXT_BUFFER_SIZE)
	{
		strcat(szTemp, lpszText);
		SetDlgItemText(hMainDlg, IDC_MAIN_EDIT, szTemp);
	}
 }

void AddTextLine(LPCSTR lpszCaption, LPCSTR lpszValue)
{
	char szLine[LINE_BUFFER_SIZE];

	sprintf(szLine, "%-26s : %s\r\n", lpszCaption, lpszValue);
	AddMainText(szLine);
}

//Get version information
BOOL GetInfoStr(char *pBuf, LPCSTR lpszLang1, LPCSTR lpszName, LPSTR lpszInfo)
{
	UINT size;
	char *str;
	char szTemp[255];
	strcpy(szTemp, "\\StringFileInfo\\");
	strcat(szTemp, lpszLang1);
	strcat(szTemp, "\\");
	strcat(szTemp, lpszName);

	BOOL bFound = VerQueryValue((LPVOID)pBuf, szTemp, (LPVOID *)&str, &size);
	if (bFound)
	{
		strcpy(lpszInfo, str);
		return TRUE;
	} else return FALSE;
}


LPSTR KeyValueToStr(KEY_VALUE *Items, DWORD dwNumOfItems, DWORD dwFindValue, LPSTR lpszValue, LPSTR lpszDefaultValue = NULL)
{
	for (DWORD dwIndex = 0; dwIndex < dwNumOfItems; dwIndex++)
		if (Items[dwIndex].dwKey == dwFindValue)
		{
			strcpy(lpszValue, Items[dwIndex].szValue);
			return lpszValue;
		}


	if (lpszDefaultValue != NULL)
		strcpy(lpszValue, lpszDefaultValue);
	else
		lpszValue[0] = '\0';

	return lpszValue;

}

LPSTR KeyValueBitsToStr(KEY_VALUE *Items, DWORD dwNumOfItems, DWORD dwFindValue, LPSTR lpszValue)
{
	
	lpszValue[0] = '\0';

	for (DWORD dwIndex = 0; dwIndex < dwNumOfItems; dwIndex++)
		if ((Items[dwIndex].dwKey & dwFindValue) != 0)
		{
			if (lpszValue[0] != '\0')
				strcat(lpszValue, ", ");

			strcat(lpszValue, Items[dwIndex].szValue);
		}
	return lpszValue;

}


LPSTR GetFileOSName(DWORD dwFileOS, LPSTR lpszOSName)
{
	const int NumOfOS = 9;
	KEY_VALUE Items[NumOfOS] = {
		{VOS_DOS, "MS-DOS"},
		{VOS_NT, "Windows NT"},
		{VOS__WINDOWS16, "16λ Windows"},
		{VOS__WINDOWS32, "32λ Windows"},
		{VOS_OS216, "16λ OS/2"},
		{VOS_OS232, "32λ OS/2"},
		{VOS__PM16, "16λ ���������"},
		{VOS__PM32, "32λ ���������"},
		{VOS_UNKNOWN, "δ֪"}};


	return KeyValueBitsToStr(Items, NumOfOS, dwFileOS, lpszOSName);
}

LPSTR GetFileType(DWORD dwFileType, LPSTR lpszFileType) 
{
	const int NumOfTypes = 7;
	KEY_VALUE FilesTypes[NumOfTypes] = {
		{VFT_UNKNOWN, "δ֪"},
		{VFT_APP, "Ӧ�ó���"},
		{VFT_DLL, "��̬���ӿ�"},
		{VFT_DRV, "�豸����"},
		{VFT_FONT, "�����ļ�"},
		{VFT_VXD, "�����豸"},
		{VFT_STATIC_LIB, "��̬���ӿ�"}};

	return KeyValueToStr(FilesTypes, NumOfTypes, dwFileType, lpszFileType);
}

LPSTR GetFileSubType(DWORD dwFileType, DWORD dwFileSubType, LPSTR lpszFileSubType)
{
	const int NumOfTypes = 11;
	DWORD dwFileSubTypesIndex[NumOfTypes] = {VFT2_UNKNOWN, VFT2_DRV_COMM, VFT2_DRV_PRINTER, VFT2_DRV_KEYBOARD, 
									VFT2_DRV_LANGUAGE, VFT2_DRV_DISPLAY, VFT2_DRV_MOUSE, VFT2_DRV_NETWORK, 
									VFT2_DRV_SYSTEM, VFT2_DRV_INSTALLABLE, VFT2_DRV_SOUND};

	LPSTR szFileSubTypes[NumOfTypes] = {"δ֪", "ͨ����������", "��ӡ������", "��������", "��������", "��ʾ������", "�������", "��������", "ϵͳ����", "�ɰ�װ������", "��Ƶ����"};
	
	strcpy(lpszFileSubType,"δ֪");
	if (dwFileType == VFT_DRV)
	{
		for (int nIndex = 0; nIndex < NumOfTypes;nIndex++)
			if (dwFileSubTypesIndex[nIndex] == dwFileSubType)
			{
				strcpy(lpszFileSubType, szFileSubTypes[nIndex]);
				break;
			}
	} else if (dwFileType == VFT_FONT)
	{
		if (dwFileSubType == VFT2_FONT_RASTER)
			strcpy(lpszFileSubType, "��դ����");
		else if (dwFileSubType == VFT2_FONT_VECTOR)
			strcpy(lpszFileSubType, "ʸ������");
		else if (dwFileSubType == VFT2_FONT_TRUETYPE)
			strcpy(lpszFileSubType, "TrueType ����");

	}

	return lpszFileSubType;
}

LPSTR GetExeType(DWORD dwExeType, LPSTR lpszExeType)
{
	const int NumOfTypes = 7;
	KEY_VALUE ExeTypes[NumOfTypes] = {
		{EXE_TYPE_UNKNOWN, "δ֪"},
		{EXE_TYPE_DOS, "MS-DOS"},
		{EXE_TYPE_NE, "�¿�ִ���ļ� (16λ)"},
		{EXE_TYPE_PE, "PE��ִ���ļ� (32λ)"},
		{EXE_TYPE_LE, "���Կ�ִ���ļ� (�����豸����)"},
		{EXE_TYPE_LX, "���Կ�ִ���ļ�: OS/2 2.x "},
		{EXE_TYPE_W3, "Windows WIN386.EXE �ļ�"}};
	
	return KeyValueToStr(ExeTypes, NumOfTypes, dwExeType, lpszExeType);
}


HANDLE OpenFileForRead(LPCSTR lpszFilename)
{
	return CreateFile(lpszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);	
}


LPSTR FormatDWordHex(DWORD dwValue, LPSTR lpszValue, DWORD dwMaxDigits = 4)
{
	char szTemp[50];

	_ultoa(dwValue, szTemp, 16);
	int nAdd = dwMaxDigits - strlen(szTemp);
	if (nAdd > 0)
	{
		memset(lpszValue, '0', nAdd);
		lpszValue[nAdd] = '\0';
	}
	else
		lpszValue[0] = '\0';

	strcat(lpszValue, szTemp);
	sprintf(szTemp, "0x%s", lpszValue );
	strcpy(lpszValue, szTemp);

	return lpszValue;
}

LPSTR FormatDWord(DWORD dwValue, LPSTR lpszValue, LPSTR lpszMore = NULL)
{
	char szTemp[30];
	NUMBERFMT nf;

	nf.NumDigits = 0;
	nf.LeadingZero = 0;
	nf.Grouping = 3;
	nf.lpDecimalSep = ".";
	nf.lpThousandSep = ",";
	nf.NegativeOrder = 0;

	_ultoa(dwValue, szTemp, 10);
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szTemp, &nf, lpszValue, LINE_BUFFER_SIZE);
	if (lpszMore != NULL)
		strcat(lpszValue, lpszMore);

	return lpszValue;
}

LPSTR FormatFileTime(FILETIME ft, LPSTR szDateTime)
{
	SYSTEMTIME st;
	char szTime[15];

	FileTimeToSystemTime(&ft, &st);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, szDateTime, LINE_BUFFER_SIZE);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, 15);
	strcat(szDateTime, " ");
	strcat(szDateTime, szTime);
	return szDateTime;
}

LPSTR GetCharacteristics(WORD wCharacteristics, LPSTR lpszCharacteristics)
{
	const int NumOfItems = 13;
	KEY_VALUE Items[NumOfItems] = {
		{IMAGE_FILE_DLL, "���Ƕ�̬���ӿ��ļ�"},
		//{IMAGE_FILE_EXECUTABLE_IMAGE, "The file is executable"}, 
		{IMAGE_FILE_RELOCS_STRIPPED, "�ļ��ض�λ"},
		{IMAGE_FILE_LINE_NUMS_STRIPPED,"�ļ������ض�λ"},
		{IMAGE_FILE_LOCAL_SYMS_STRIPPED, "���ط����ض�λ�ļ�"},
		{IMAGE_FILE_AGGRESIVE_WS_TRIM, "�ö����޼�������"}, 
		{IMAGE_FILE_LARGE_ADDRESS_AWARE, "Ӧ�ó�����Դ����ַ����2GB"},
		{IMAGE_FILE_BYTES_REVERSED_LO, "�ֽڵĻ��������෴��"},
		//{IMAGE_FILE_32BIT_MACHINE, "32 bit word machine"},
		{IMAGE_FILE_DEBUG_STRIPPED, ".dbg�ļ������ض�λ"},
		{IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "���ӳ���ǿ��ƶ�ý��,�ӽ����ļ����ƺ�����"},
		{IMAGE_FILE_NET_RUN_FROM_SWAP, "���ӳ���������ϵ��ļ�,�ӽ����ļ����ƺ�����"},
		{IMAGE_FILE_SYSTEM, "ϵͳ�ļ�"},
		{IMAGE_FILE_UP_SYSTEM_ONLY, "�ļ�Ӧ��ֻ������һ������"},
		{IMAGE_FILE_BYTES_REVERSED_HI, "�ֽڵĻ��������෴��"}};

	lpszCharacteristics[0] = '\0';

	for (DWORD dwIndex = 0; dwIndex < NumOfItems; dwIndex++)
		if ((wCharacteristics & Items[dwIndex].dwKey) != 0)
		{
			strcat(lpszCharacteristics, "\r\n * ");
			strcat(lpszCharacteristics, Items[dwIndex].szValue);
		}	

	return lpszCharacteristics;


}

LPSTR GetSubsystemName(WORD wSubsystem, LPSTR lpszSubsystemName)
{
	const int NumOfSubsystems = 8;
	KEY_VALUE Subsystems[NumOfSubsystems] = {
		{IMAGE_SUBSYSTEM_UNKNOWN, "δ֪��ϵͳ"},
		{IMAGE_SUBSYSTEM_NATIVE, "������Ҫ��ϵͳ"},
		{IMAGE_SUBSYSTEM_WINDOWS_GUI, "Windows ͼ���û�����"},
		{IMAGE_SUBSYSTEM_WINDOWS_CUI, "Windows ����̨����"},
		{IMAGE_SUBSYSTEM_OS2_CUI, "OS/2 ����̨��ϵͳ"},
		{IMAGE_SUBSYSTEM_POSIX_CUI, "Posix ����̨��ϵͳ"},
		{IMAGE_SUBSYSTEM_NATIVE_WINDOWS, "Win9x ��������"},
		{IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, "Windows CE ��ϵͳ"}};

	
	return KeyValueToStr(Subsystems, NumOfSubsystems, wSubsystem, lpszSubsystemName, UNKNOWN_STR);
}


LPSTR GetMachineName(USHORT Machine, LPSTR lpszMachineName)
{
	const int NumOfMachines = 18;
	KEY_VALUE Machines[NumOfMachines] = {
		{IMAGE_FILE_MACHINE_UNKNOWN, "δ֪"},           
		{IMAGE_FILE_MACHINE_I386, "Intel 386"},
		{IMAGE_FILE_MACHINE_R3000, "MIPS ��λ����, 0x160 ��λ����"},
		{IMAGE_FILE_MACHINE_R4000, "MIPS ��λ����"},
		{IMAGE_FILE_MACHINE_R10000,"MIPS ��λ����"},
		{IMAGE_FILE_MACHINE_WCEMIPSV2, "MIPS ��λ���� WCE v2"},
		{IMAGE_FILE_MACHINE_ALPHA, "Alpha_AXP"}, 
		{IMAGE_FILE_MACHINE_POWERPC, "IBM PowerPC ��λ����"},
		{IMAGE_FILE_MACHINE_SH3, "SH3 ��λ����"},
		{IMAGE_FILE_MACHINE_SH3E, "SH3E ��λ����"},
		{IMAGE_FILE_MACHINE_SH4, "SH4 ��λ����"},
		{IMAGE_FILE_MACHINE_ARM, "ARM ��λ����"},
		{IMAGE_FILE_MACHINE_THUMB, "?"},            
		{IMAGE_FILE_MACHINE_IA64, "Intel 64"},
		{IMAGE_FILE_MACHINE_MIPS16, "MIPS"},
		{IMAGE_FILE_MACHINE_MIPSFPU, "MIPS"},
		{IMAGE_FILE_MACHINE_MIPSFPU16, "MIPS"},
		{IMAGE_FILE_MACHINE_ALPHA64, "ALPHA64"}};
	
	

	return KeyValueToStr(Machines, NumOfMachines, Machine, lpszMachineName, UNKNOWN_STR);
}

BOOL GetDosCompress(LPCSTR lpszMZHeader, LPSTR szCompressName)
{
	if (strncmp(&lpszMZHeader[0x1e], "PKLITE", 6) == 0)
	{
		sprintf(szCompressName, "PKLITE v%d.%d", (lpszMZHeader[0x1d] & 0xf), lpszMZHeader[0x1c]);
		return TRUE;
	}

	if (strncmp(&lpszMZHeader[0x1c], "LZ", 2) == 0)
	{
		char szVer[10] = "";
		if (strncmp(&lpszMZHeader[0x1e], "09", 2) == 0)
			strcpy(szVer, "0.90");
		else if (strncmp(&lpszMZHeader[0x1e], "91", 2) == 0)
			strcpy(szVer, "0.91");

		sprintf(szCompressName, "LZEXE %s", szVer);

		return TRUE;
	}

	if (strncmp(&lpszMZHeader[0x25], "LHarc's SFX", 11) == 0)
	{
		strcpy(szCompressName, "LHarc 1.x");
		return TRUE;
	}

	if (strncmp(&lpszMZHeader[0x24], "LHa's SFX", 9) == 0 ||
		strncmp(&lpszMZHeader[0x24], "LHA's SFX", 9) == 0)
	{
		strcpy(szCompressName, "LHA 2.x");
		return TRUE;
	}
	

	return FALSE;
}


LPSTR GetNETargetOS(BYTE ExeType, LPSTR lpszOSName)
{
	const int NumOfItems = 8;
	KEY_VALUE Items[NumOfItems] = {
		{0x0, "δ֪"},	
		{0x1, "OS/2"},	
		{0x2, "Windows"},	
		{0x3, "European MS-DOS 4.x"},	
		{0x4, "Windows 386"},	
		{0x5, "BOSS (Borland ����ϵͳ����)"},	
		{0x81, "PharLap 286|DOS-��չ��, OS/2"},	
		{0x82, "PharLap 286|DOS-��չ��, Windows"}};
	
	return KeyValueToStr(Items, NumOfItems, ExeType, lpszOSName, UNKNOWN_STR);
}

void GetInfoFromSectionHeaders(IMAGE_SECTION_HEADER *ishs, USHORT uNumOfSections, DWORD *dwInfo1)
{
	*dwInfo1 = 0;
	for (USHORT uIndex = 0; uIndex < uNumOfSections; uIndex++)
	{
		if (strncmp((char *)ishs[uIndex].Name, ".pklstb", 7) == 0) *dwInfo1 |= SECTIONS_HEADER_INFO_PKLITE;
		if (strncmp((char *)ishs[uIndex].Name, "_winzip_", 8) == 0) *dwInfo1 |= SECTIONS_HEADER_INFO_WINZIP;
		if (strncmp((char *)ishs[uIndex].Name, ".WWP32", 6) == 0) *dwInfo1 |= SECTIONS_HEADER_INFO_WWPACK32;
		if (strncmp((char *)ishs[uIndex].Name, "UPX", 3) == 0) *dwInfo1 |= SECTIONS_HEADER_INFO_UPX;
	}
}


LPSTR GetSectionCharacteristics(DWORD dwCharacteristics, LPSTR lpszValue)
{
	const int NumOfItems = 9;
	KEY_VALUE Items[NumOfItems] = {
		{0x00000020, "�����"},
		{0x00000040, "��ʼ������"},
		{0x00000080, "δ��ʼ��������"},
		{0x04000000, "���ɻ���"},
		{0x08000000, "���ɷ�ҳ"},
		{0x10000000, "����"},
		{0x20000000, "��ִ��"},
		{0x40000000, "�ɶ�"},
		{0x80000000, "��д"}};
	
	return KeyValueBitsToStr(Items, NumOfItems, dwCharacteristics, lpszValue);
}


void ShowExeInfo(LPCSTR lpszFilename)
{
	DWORD dwTemp;
	UINT dwSize;
	HANDLE hFile;
	char szValue[LINE_BUFFER_SIZE], szTemp[LINE_BUFFER_SIZE];
	
	//Clear all text.
	ClearMainText();

	//Open file
	if ((hFile = OpenFileForRead(lpszFilename)) != INVALID_HANDLE_VALUE)
	{
		char szMZHeader[MZHEADER_SIZE];
		char szMewHeader[NEWHEADER_SIZE];

		DWORD dwRead, dwExeType, dwNewHeaderLoc;
		BOOL bResult;

		//Read MZ header. (MS-DOS Header)
		bResult = ReadFile(hFile, szMZHeader, MZHEADER_SIZE, &dwRead, NULL);
		if (bResult && dwRead == MZHEADER_SIZE)
		{
			AddTextLine("�ļ���", lpszFilename);
			if (szMZHeader[0] == 'M' && szMZHeader[1] == 'Z')
			{
				IMAGE_DOS_HEADER *idh;	
				idh = (IMAGE_DOS_HEADER *)szMZHeader;

				if (szMZHeader[0x18] != 0x40)
					dwExeType = EXE_TYPE_DOS;
				else
				{
					dwNewHeaderLoc = idh->e_lfanew;
					SetFilePointer(hFile, dwNewHeaderLoc, NULL, FILE_BEGIN);

					//Read next header. (PE, NE, or LE)
					bResult = ReadFile(hFile, szMewHeader, NEWHEADER_SIZE, &dwRead, NULL);
					if (bResult && dwRead >= 255) 
					{
						if (szMewHeader[0] == 'P' && szMewHeader[1] == 'E')
							dwExeType = EXE_TYPE_PE;
						else if (szMewHeader[0] == 'N' && szMewHeader[1] == 'E')
							dwExeType = EXE_TYPE_NE;
						else if (szMewHeader[0] == 'L' && szMewHeader[1] == 'E')
							dwExeType = EXE_TYPE_LE;
					} else
						dwExeType = EXE_TYPE_UNKNOWN;

				}

				DWORD dwFileSize = GetFileSize(hFile, NULL);
				FormatDWord(dwFileSize, szValue);
				strcat(szValue, szBytesWord);
				AddTextLine("�ļ���С", szValue);

				FILETIME ftCreation, ftModified;
				GetFileTime(hFile, &ftCreation, NULL, &ftModified);
				AddTextLine("��������", FormatFileTime(ftCreation, szValue));
				AddTextLine("�޸�����", FormatFileTime(ftModified, szValue));
				AddTextLine("��ִ���ļ���ʽ", GetExeType(dwExeType, szValue));
				
				AddMainText("\r\nMS-DOS ͷ��Ϣ\r\n=========================\r\n");
				AddTextLine("�����һҳ�ֽ�", FormatDWord(idh->e_cblp, szValue));
				AddTextLine("��ҳ����", FormatDWord(idh->e_cp, szValue));
				AddTextLine("�ض�λ���", FormatDWord(idh->e_crlc, szValue));
				AddTextLine("�ε�ͷ��С", FormatDWord(idh->e_cparhdr, szValue));
				AddTextLine("����� SS (��Ե�)", FormatDWordHex(idh->e_ss, szValue));
				AddTextLine("����� SP", FormatDWordHex(idh->e_sp, szValue));
				AddTextLine("����� CS (��Ե�)", FormatDWordHex(idh->e_cs, szValue));
				AddTextLine("����� IP", FormatDWordHex(idh->e_ip, szValue));
				AddTextLine("Checksum", FormatDWordHex(idh->e_csum, szValue));
				if (GetDosCompress(szMZHeader, szTemp))
				{
					sprintf(szValue, "\r\n*** �ļ��� %s ���� ***\r\n", szTemp);
					AddMainText(szValue);
				}

				if (dwExeType == EXE_TYPE_NE)
				{
					IMAGE_OS2_HEADER *iosh = (IMAGE_OS2_HEADER *)szMewHeader;
					AddMainText("\r\n�µĿ�ִ���ļ�ͷ��Ϣ\r\n===================================\r\n");
					AddTextLine("�汾��", itoa(iosh->ne_ver, szValue, 10));
					AddTextLine("�޶���", itoa(iosh->ne_rev, szValue, 10));
					AddTextLine("��ڱ�ƫ��", FormatDWordHex(iosh->ne_enttab, szValue));
					AddTextLine("��ڱ��С", FormatDWord(iosh->ne_cbenttab, szValue, szBytesWord));
					AddTextLine("Checksum", FormatDWordHex(iosh->ne_crc, szValue, 8));
					AddTextLine("��ģ��", ((iosh->ne_flags & 0x8000) ? "Yes" : "No"));
					AddTextLine("Ŀ��ϵͳ", GetNETargetOS(iosh->ne_exetyp, szValue));

					AddTextLine("��ʼ�ѷ���", FormatDWord(iosh->ne_heap, szValue, szBytesWord));
					AddTextLine("��ʼ��ջ����", FormatDWord(iosh->ne_heap, szValue, szBytesWord));
					sprintf(szValue, "%.4x:%.4x", (WORD)(((DWORD)iosh->ne_csip) >> 16), (WORD)(((DWORD)iosh->ne_csip) & 0xffff));
					AddTextLine("��ʼ CS:IP", szValue);

					sprintf(szValue, "%.4x:%.4x", (WORD)(((DWORD)iosh->ne_sssp) >> 16), (WORD)(((DWORD)iosh->ne_sssp) & 0xffff));
					AddTextLine("��ʼ SS:SP", szValue);

					AddTextLine("������Ŀ", FormatDWord(iosh->ne_cseg, szValue));
					AddTextLine("ģ����Ŀ", FormatDWord(iosh->ne_cmod, szValue));
					AddTextLine("��פ�����Ʊ�", FormatDWord(iosh->ne_cbnrestab, szValue, szBytesWord));
					
					AddTextLine("�α�ƫ��", FormatDWordHex(iosh->ne_segtab, szValue));
					AddTextLine("��Դ��ƫ��", FormatDWordHex(iosh->ne_rsrctab, szValue));
					AddTextLine("פ������ƫ��", FormatDWordHex(iosh->ne_restab, szValue));
					AddTextLine("ģ������ƫ��", FormatDWordHex(iosh->ne_modtab, szValue));
					AddTextLine("��������ƫ��", FormatDWordHex(iosh->ne_imptab, szValue));
					AddTextLine("��פ����ƫ��", FormatDWordHex(iosh->ne_nrestab, szValue, 8));
					
					AddTextLine("���ƶ�����Ŀ��", FormatDWord(iosh->ne_cmovent, szValue));
					AddTextLine("�ζ���", FormatDWord(iosh->ne_align, szValue));
					AddTextLine("��Դ�ζ���", FormatDWord(iosh->ne_cres, szValue));
				}


				if (dwExeType == EXE_TYPE_PE)
				{
					IMAGE_FILE_HEADER *ifh;
	
					AddMainText("\r\nPEͷ��Ϣ\r\n===================================\r\n");
					ifh = (IMAGE_FILE_HEADER *)&szMewHeader[4];
					AddTextLine("����", GetMachineName(ifh->Machine, szValue));
					AddTextLine("������", FormatDWord(ifh->NumberOfSections, szValue));
					AddTextLine("��������", FormatDWord(ifh->NumberOfSymbols, szValue));
					AddTextLine("��ѡͷ��С", FormatDWord(ifh->SizeOfOptionalHeader, szValue, szBytesWord));
					AddTextLine("ʱ���", FormatDWordHex(ifh->TimeDateStamp, szValue));
					AddTextLine("����", GetCharacteristics(ifh->Characteristics, szValue));
					
					AddMainText("\r\n��ѡͷ��Ϣ\r\n===================================\r\n");
					
					IMAGE_OPTIONAL_HEADER32 *ioh = (IMAGE_OPTIONAL_HEADER32 *)&szMewHeader[24];
					sprintf(szValue, "%d.%d", ioh->MajorLinkerVersion, ioh->MinorLinkerVersion);
					AddTextLine("�������汾", szValue);
					AddTextLine("�����С", FormatDWord(ioh->SizeOfCode, szValue, szBytesWord ));
					AddTextLine("��ʼ�����ݴ�С", FormatDWord(ioh->SizeOfInitializedData, szValue, szBytesWord));
					AddTextLine("δ��ʼ����С", FormatDWord(ioh->SizeOfUninitializedData, szValue, szBytesWord));

					if (ifh->Machine == IMAGE_FILE_MACHINE_I386 && ifh->SizeOfOptionalHeader >= 224)
					{
						AddTextLine("�ڶ���", FormatDWord(ioh->SectionAlignment, szValue, szBytesWord));
						AddTextLine("�ļ�����", FormatDWord(ioh->FileAlignment, szValue, szBytesWord));
						sprintf(szValue, "%d.%d", ioh->MajorOperatingSystemVersion, ioh->MinorOperatingSystemVersion);
						AddTextLine("ϵͳ�汾", szValue);
						sprintf(szValue, "%d.%d", ioh->MajorImageVersion, ioh->MinorImageVersion);
						AddTextLine("ӳ��汾", szValue);
						sprintf(szValue, "%d.%d", ioh->MajorSubsystemVersion, ioh->MinorSubsystemVersion);
						AddTextLine("��ϵͳ�汾", szValue);
						AddTextLine("�����С", FormatDWord(ioh->SizeOfImage, szValue, szBytesWord));
						AddTextLine("ͷ��С", FormatDWord(ioh->SizeOfHeaders, szValue, szBytesWord));
						AddTextLine("Checksum", FormatDWordHex(ioh->CheckSum, szValue, 8));
						AddTextLine("��ϵͳ", GetSubsystemName(ioh->Subsystem, szValue));
						AddTextLine("������ջ��С", FormatDWordHex(ioh->SizeOfStackReserve, szValue, 8));
						AddTextLine("�ύ��ջ��С", FormatDWordHex(ioh->SizeOfStackCommit, szValue, 8));
						AddTextLine("�����Ѵ�С", FormatDWordHex(ioh->SizeOfHeapReserve, szValue, 8));
						AddTextLine("�ύ�Ѵ�С", FormatDWordHex(ioh->SizeOfHeapCommit, szValue, 8));
						AddTextLine("��ַ", FormatDWordHex(ioh->ImageBase, szValue, 8));

					}
					AddTextLine("��ڵ�", FormatDWordHex(ioh->AddressOfEntryPoint, szValue, 8));
					AddTextLine("�������", FormatDWordHex(ioh->BaseOfCode, szValue, 8));
					AddTextLine("���ݻ���", FormatDWordHex(ioh->BaseOfData, szValue, 8));

					//Section Headers
					IMAGE_SECTION_HEADER *ishs;
					DWORD dwInfo1;
					USHORT uNumOfSections = ifh->NumberOfSections;
					if (uNumOfSections > MAX_NUM_OF_SECTIONS)
						uNumOfSections  = MAX_NUM_OF_SECTIONS;

					ishs = (IMAGE_SECTION_HEADER *)&szMewHeader[24 + ifh->SizeOfOptionalHeader]; 
					
					AddMainText("\r\n��ͷ����Ϣ\r\n===================================\r\n");
					sprintf(szValue, "����ļ��� %d ��:\r\n", ifh->NumberOfSections);
					AddMainText(szValue);
					for (USHORT uIndex = 0; uIndex < uNumOfSections; uIndex++)
					{
						if (uIndex == 0) 
							AddMainText("----------------------------------------\r\n");
						memcpy(szValue, ishs[uIndex].Name, 8);
						szValue[8] = '\0';
						AddTextLine("������", szValue);
						AddTextLine("�����ַ", FormatDWordHex(ishs[uIndex].VirtualAddress, szValue, 8));
						AddTextLine("ԭ���ݴ�С", FormatDWordHex(ishs[uIndex].SizeOfRawData, szValue, 8));
						AddTextLine("Ԫ����ָ��", FormatDWordHex(ishs[uIndex].PointerToRawData, szValue, 8));
						AddTextLine("����", GetSectionCharacteristics(ishs[uIndex].Characteristics, szValue));

						AddMainText("----------------------------------------\r\n");
					}

					GetInfoFromSectionHeaders(ishs, uNumOfSections, &dwInfo1);
					if ((dwInfo1 & SECTIONS_HEADER_INFO_PKLITE) != 0)
						AddMainText("\r\n*** �ļ���ѹ��ΪWindows��PKLITE ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_WINZIP) != 0)
						AddMainText("\r\n*** ���ļ��Խ��������WinZip�鵵 ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_WWPACK32) != 0)
						AddMainText("\r\n*** �ļ���WWPACK32ѹ�� ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_UPX) != 0)
						AddMainText("\r\n*** �ļ���UPX�ӿ� ***\r\n");
					
					//Compilers Detection
					char *pVBSignature = (char *)&szMewHeader[24 + ifh->SizeOfOptionalHeader + 40 * uNumOfSections + 0x10];
					char *pDelphiSignature = (char *)&szMewHeader[0x400 - idh->e_lfanew]; 

					BYTE VBVer = 0;
					char szCompiler[30] = "";
					//Visual Basic Detection
					if (memcmp(pVBSignature, SIGNATURE_VB4, strlen(SIGNATURE_VB4)) == 0) VBVer = 4;
					if (memcmp(pVBSignature, SIGNATURE_VB5, strlen(SIGNATURE_VB5)) == 0) VBVer = 5;
					if (memcmp(pVBSignature, SIGNATURE_VB6, strlen(SIGNATURE_VB6)) == 0) VBVer = 6;
					if (VBVer != 0)
						sprintf(szCompiler, "Visual Basic %d.0", VBVer);
					else
						//Delphi Detection
						if	(memcmp(pDelphiSignature, SIGNATURE_DELPHI_1, 13) == 0 ||
							 memcmp(pDelphiSignature, SIGNATURE_DELPHI_2, 13) == 0 || 
							 memcmp(pDelphiSignature, SIGNATURE_DELPHI_3, 11) == 0)
								strcpy(szCompiler, "Borland Delphi");

					if (szCompiler[0] != '\0')
					{
						sprintf(szValue, "\r\n*** ���ļ��� %s ���� ***\r\n", szCompiler);
						AddMainText(szValue);
					}
						
				}

			} else
			{
				AddMainText("\r\n����ʶ���ļ���ʽ\r\n");
				return;
			}

		} else
		{
			ShowError(lpszFilename, "���ܴ� %s ��ȡ\r\n���� %d:%s");
			return;
		}

	} else
	{
		
		ShowError(lpszFilename, "���ܴ� %s\r\n���� %d:%s");
		return;
	}

	AddMainText("\r\n�汾��Ϣ\r\n====================\r\n");
	dwSize = GetFileVersionInfoSize((LPTSTR)lpszFilename, &dwTemp);
	if (dwSize > 0)
	{
		char *buf = new char[dwSize];
		char *str;
		char szLang1[20];
		VS_FIXEDFILEINFO *pInfo;
		
		GetFileVersionInfo((LPTSTR)lpszFilename, 0, dwSize, buf);
		if (VerQueryValue((LPVOID)buf, "\\", (LPVOID *)&pInfo, &dwSize))
		{
			AddTextLine("����ϵͳ", GetFileOSName(pInfo->dwFileOS, szValue));
			AddTextLine("�ļ�����", GetFileType(pInfo->dwFileType, szValue));
			AddTextLine("�ļ�������", GetFileSubType(pInfo->dwFileType, pInfo->dwFileSubtype , szValue));
			
			sprintf(szValue, "%d,%d,%d,%d", (pInfo->dwFileVersionMS >> 16), (pInfo->dwFileVersionMS & 65535), (pInfo->dwFileVersionLS >> 16), pInfo->dwFileVersionLS & 65535);
			AddTextLine("�ļ��汾", szValue);

			sprintf(szValue, "%d,%d,%d,%d", (pInfo->dwProductVersionMS >> 16), (pInfo->dwProductVersionMS & 65535), (pInfo->dwProductVersionLS >> 16), pInfo->dwProductVersionLS & 65535);
			AddTextLine("��Ʒ�汾", szValue);

		}
		AddMainText("============================================================\r\n");
		VerQueryValue((LPVOID)buf, "\\VarFileInfo\\Translation", (LPVOID *)&str, &dwSize);
		sprintf(szLang1, "%4.4X%4.4X", *(unsigned short *)str, *(unsigned short *)(str + 2));

		if (GetInfoStr(buf, szLang1, "ProductName", szValue))
			AddTextLine("��Ʒ����", szValue);
		if (GetInfoStr(buf, szLang1, "FileDescription", szValue))
			AddTextLine("�ļ�����", szValue);
		if (GetInfoStr(buf, szLang1, "FileVersion", szValue))
			AddTextLine("�ļ��汾", szValue);
		if (GetInfoStr(buf, szLang1, "ProductVersion", szValue))
			AddTextLine("��Ʒ�汾", szValue);
		if (GetInfoStr(buf, szLang1, "CompanyName", szValue))
			AddTextLine("��˾��", szValue);
		if (GetInfoStr(buf, szLang1, "InternalName", szValue))
			AddTextLine("�ڲ�����", szValue);
		if (GetInfoStr(buf, szLang1, "LegalCopyright", szValue))
			AddTextLine("�Ϸ���Ȩ", szValue);
		if (GetInfoStr(buf, szLang1, "OriginalFileName", szValue))
			AddTextLine("ԭ�ļ���", szValue);


		delete []buf;
	} else
		AddMainText("����ļ��汾��Ϣ�����á�");
}

void ExeOpen(HWND hwndDlg)
{
	char szFilename[MAX_PATH];

	//Display open file dialog-box
	if (OpenFileDialog(szFilename))
	{
		//Load the file that the user selected.
		ShowExeInfo(szFilename);
	}

}

void CopyEditToClipboard(HWND hWnd)
{
	SendMessage(hWnd, EM_SETSEL, 0, 65535L);
	SendMessage(hWnd, WM_COPY, 0 , 0);
	SendMessage(hWnd, EM_SETSEL, 0, 0);
}


BOOL CALLBACK MainDlgProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	WORD wNotifyCode, wID;
	char szDropFile[MAX_PATH];
	HDROP hDrop = (HDROP) wParam; 
	HINSTANCE hInstance;
	switch(uMsg)
	{
		case WM_INITDIALOG:
			SetDlgItemText(hwndDlg, IDC_EDIT_MAIN, START_TEXT);

			//Allows to drag files into the main dialog-box.
			DragAcceptFiles(hwndDlg, TRUE);

			//Set main font.
			SetEditFont(GetDlgItem(hwndDlg, IDC_MAIN_EDIT));
			return TRUE;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			return TRUE;
		
		case WM_CTLCOLORSTATIC:
			if ((HWND)lParam == GetDlgItem(hwndDlg, IDC_EDIT_MAIN))
			{
				SetBkColor((HDC)wParam, crMainEdit);
				return (BOOL)hMainEditBrush;
			}
			return TRUE;
		
		case WM_DISPLAYCHANGE:
			//On display settings change, recreate the brush.
			PrepareEditBrush();		
			return TRUE;

		case WM_DROPFILES:
			//Load the file that the user dropped into ExeInfo.
			hDrop = (HDROP) wParam; 
			DragQueryFile(hDrop, 0, szDropFile, MAX_PATH);
			DragFinish(hDrop);
			ShowExeInfo(szDropFile);
			return TRUE;

		case WM_COMMAND:
			wNotifyCode = HIWORD(wParam); // notification code 
			wID = LOWORD(wParam);         // item, control, or accelerator identifier 
			
			if (wID == IDC_ABOUTB && wNotifyCode == BN_CLICKED)
			{
				DialogBox(hInstance,MAKEINTRESOURCE(IDD_ABOUTBOX),NULL,NULL);
				return TRUE;
			}

			if (wID == IDC_BUTTON_EXIT && wNotifyCode == BN_CLICKED)
			{
				PostQuitMessage(0);
				return TRUE;
			}
			else if (wID == IDC_BUTTON_OPEN && wNotifyCode == BN_CLICKED)
			{
				ExeOpen(hwndDlg);
				return TRUE;
			} 
			else if (wID == IDC_BUTTON_COPY && wNotifyCode == BN_CLICKED)
			{
				CopyEditToClipboard(GetDlgItem(hwndDlg, IDC_EDIT_MAIN));
				return TRUE;
			}
			return FALSE;


		default:
			return FALSE;
	}
}

/*BOOL WINAPI About(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;//��Ϣ�Ѵ�������Ҫ�ٵ����������ڲ��Ի�����Ϣ������
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd,1);
			return true;
		}
	}
	return FALSE;
}*/

void CenterWin(HWND hWnd)
{
	RECT rcWin;
	GetWindowRect(hWnd, &rcWin);
	SetWindowPos(hWnd, NULL, (GetSystemMetrics(SM_CXFULLSCREEN) - (rcWin.right - rcWin.left + 1)) / 2, 
							 (GetSystemMetrics(SM_CYFULLSCREEN) - (rcWin.bottom - rcWin.top + 1)) / 2, 
							 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
		
}


int WINAPI WinMain(
	HINSTANCE  hInstance,	// handle to current instance
    HINSTANCE  hPrevInstance,	// handle to previous instance
    LPSTR  lpCmdLine,	// pointer to command line
    int  nShowCmd 	// show state of window
   )
{
	MSG msg;
	char szCommand[1024];

	//Load small and large Icons from IDI_MAIN resource
	hMainIcon32 = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 32, 32, 0);
	hMainIcon16 = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 16, 16, 0);
	
	//Create brush
	PrepareEditBrush();
	
	if ((hMainDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDlgProc)) != NULL)
	{
		//Load icons into the dialog-box.
		SendMessage(hMainDlg, WM_SETICON, ICON_SMALL, (LPARAM)hMainIcon16);
		SendMessage(hMainDlg, WM_SETICON, ICON_BIG, (LPARAM)hMainIcon32);
		
		//Center dialog-box
		CenterWin(hMainDlg);
		SetMainText(START_TEXT);
		
		//Display the dialog-box
		ShowWindow(hMainDlg, SW_SHOW);

		StrTrim(lpCmdLine, szCommand);
		if (szCommand[0] != '\0')
		{
			//Load Exe specified in command-line
			RemoveQuotes(szCommand);
			ShowExeInfo(szCommand);
		}

		//Main message loop.
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!IsDialogMessage(hMainDlg, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		//Free the brush and font before exit.
		DeleteObject(hMainEditBrush);
		DeleteObject(hEditFont);

	}
	
	return 0;
}
