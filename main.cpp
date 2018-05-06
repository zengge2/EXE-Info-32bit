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

char szBytesWord[] = " 字节";

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

	FillFontStruct(&lf, "宋体", 18, FALSE);
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
	char szFilter[] = "可执行文件 (*.exe)\0*.exe\0动态链接库 (*.dll)\0*.dll\0ActiveX 模块 (*.ocx)\0*.ocx\0驱动文件 (*.drv;*.vxd;*.sys;*.386)\0*.drv;*.vxd;*.sys;*.386\0字体文件 (*.fon)\0*.fon\0所有文件 (*.*)\0*.*\0\0";
	char szFile[MAX_PATH] = "\0";
	char szTitle[] = "选择一个PE文件";
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
		{VOS__WINDOWS16, "16位 Windows"},
		{VOS__WINDOWS32, "32位 Windows"},
		{VOS_OS216, "16位 OS/2"},
		{VOS_OS232, "32位 OS/2"},
		{VOS__PM16, "16位 报告管理器"},
		{VOS__PM32, "32位 报告管理器"},
		{VOS_UNKNOWN, "未知"}};


	return KeyValueBitsToStr(Items, NumOfOS, dwFileOS, lpszOSName);
}

LPSTR GetFileType(DWORD dwFileType, LPSTR lpszFileType) 
{
	const int NumOfTypes = 7;
	KEY_VALUE FilesTypes[NumOfTypes] = {
		{VFT_UNKNOWN, "未知"},
		{VFT_APP, "应用程序"},
		{VFT_DLL, "动态链接库"},
		{VFT_DRV, "设备驱动"},
		{VFT_FONT, "字体文件"},
		{VFT_VXD, "虚拟设备"},
		{VFT_STATIC_LIB, "静态链接库"}};

	return KeyValueToStr(FilesTypes, NumOfTypes, dwFileType, lpszFileType);
}

LPSTR GetFileSubType(DWORD dwFileType, DWORD dwFileSubType, LPSTR lpszFileSubType)
{
	const int NumOfTypes = 11;
	DWORD dwFileSubTypesIndex[NumOfTypes] = {VFT2_UNKNOWN, VFT2_DRV_COMM, VFT2_DRV_PRINTER, VFT2_DRV_KEYBOARD, 
									VFT2_DRV_LANGUAGE, VFT2_DRV_DISPLAY, VFT2_DRV_MOUSE, VFT2_DRV_NETWORK, 
									VFT2_DRV_SYSTEM, VFT2_DRV_INSTALLABLE, VFT2_DRV_SOUND};

	LPSTR szFileSubTypes[NumOfTypes] = {"未知", "通信驱动程序", "打印机驱动", "键盘驱动", "语言驱动", "显示器驱动", "鼠标驱动", "网络驱动", "系统驱动", "可安装的驱动", "音频驱动"};
	
	strcpy(lpszFileSubType,"未知");
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
			strcpy(lpszFileSubType, "光栅字体");
		else if (dwFileSubType == VFT2_FONT_VECTOR)
			strcpy(lpszFileSubType, "矢量字体");
		else if (dwFileSubType == VFT2_FONT_TRUETYPE)
			strcpy(lpszFileSubType, "TrueType 字体");

	}

	return lpszFileSubType;
}

LPSTR GetExeType(DWORD dwExeType, LPSTR lpszExeType)
{
	const int NumOfTypes = 7;
	KEY_VALUE ExeTypes[NumOfTypes] = {
		{EXE_TYPE_UNKNOWN, "未知"},
		{EXE_TYPE_DOS, "MS-DOS"},
		{EXE_TYPE_NE, "新可执行文件 (16位)"},
		{EXE_TYPE_PE, "PE可执行文件 (32位)"},
		{EXE_TYPE_LE, "线性可执行文件 (虚拟设备驱动)"},
		{EXE_TYPE_LX, "线性可执行文件: OS/2 2.x "},
		{EXE_TYPE_W3, "Windows WIN386.EXE 文件"}};
	
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
		{IMAGE_FILE_DLL, "这是动态链接库文件"},
		//{IMAGE_FILE_EXECUTABLE_IMAGE, "The file is executable"}, 
		{IMAGE_FILE_RELOCS_STRIPPED, "文件重定位"},
		{IMAGE_FILE_LINE_NUMS_STRIPPED,"文件线性重定位"},
		{IMAGE_FILE_LOCAL_SYMS_STRIPPED, "本地符号重定位文件"},
		{IMAGE_FILE_AGGRESIVE_WS_TRIM, "好斗的修剪工作集"}, 
		{IMAGE_FILE_LARGE_ADDRESS_AWARE, "应用程序可以处理地址大于2GB"},
		{IMAGE_FILE_BYTES_REVERSED_LO, "字节的机器码是相反的"},
		//{IMAGE_FILE_32BIT_MACHINE, "32 bit word machine"},
		{IMAGE_FILE_DEBUG_STRIPPED, ".dbg文件调试重定位"},
		{IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "如果映像是可移动媒体,从交换文件复制和运行"},
		{IMAGE_FILE_NET_RUN_FROM_SWAP, "如果映像是网络上的文件,从交换文件复制和运行"},
		{IMAGE_FILE_SYSTEM, "系统文件"},
		{IMAGE_FILE_UP_SYSTEM_ONLY, "文件应该只运行在一个机器"},
		{IMAGE_FILE_BYTES_REVERSED_HI, "字节的机器字是相反的"}};

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
		{IMAGE_SUBSYSTEM_UNKNOWN, "未知子系统"},
		{IMAGE_SUBSYSTEM_NATIVE, "镜像不需要子系统"},
		{IMAGE_SUBSYSTEM_WINDOWS_GUI, "Windows 图形用户界面"},
		{IMAGE_SUBSYSTEM_WINDOWS_CUI, "Windows 控制台程序"},
		{IMAGE_SUBSYSTEM_OS2_CUI, "OS/2 控制台子系统"},
		{IMAGE_SUBSYSTEM_POSIX_CUI, "Posix 控制台子系统"},
		{IMAGE_SUBSYSTEM_NATIVE_WINDOWS, "Win9x 本地驱动"},
		{IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, "Windows CE 子系统"}};

	
	return KeyValueToStr(Subsystems, NumOfSubsystems, wSubsystem, lpszSubsystemName, UNKNOWN_STR);
}


LPSTR GetMachineName(USHORT Machine, LPSTR lpszMachineName)
{
	const int NumOfMachines = 18;
	KEY_VALUE Machines[NumOfMachines] = {
		{IMAGE_FILE_MACHINE_UNKNOWN, "未知"},           
		{IMAGE_FILE_MACHINE_I386, "Intel 386"},
		{IMAGE_FILE_MACHINE_R3000, "MIPS 低位优先, 0x160 高位优先"},
		{IMAGE_FILE_MACHINE_R4000, "MIPS 低位优先"},
		{IMAGE_FILE_MACHINE_R10000,"MIPS 低位优先"},
		{IMAGE_FILE_MACHINE_WCEMIPSV2, "MIPS 低位优先 WCE v2"},
		{IMAGE_FILE_MACHINE_ALPHA, "Alpha_AXP"}, 
		{IMAGE_FILE_MACHINE_POWERPC, "IBM PowerPC 低位优先"},
		{IMAGE_FILE_MACHINE_SH3, "SH3 低位优先"},
		{IMAGE_FILE_MACHINE_SH3E, "SH3E 低位优先"},
		{IMAGE_FILE_MACHINE_SH4, "SH4 低位优先"},
		{IMAGE_FILE_MACHINE_ARM, "ARM 低位优先"},
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
		{0x0, "未知"},	
		{0x1, "OS/2"},	
		{0x2, "Windows"},	
		{0x3, "European MS-DOS 4.x"},	
		{0x4, "Windows 386"},	
		{0x5, "BOSS (Borland 操作系统服务)"},	
		{0x81, "PharLap 286|DOS-扩展器, OS/2"},	
		{0x82, "PharLap 286|DOS-扩展器, Windows"}};
	
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
		{0x00000020, "代码段"},
		{0x00000040, "初始化数据"},
		{0x00000080, "未初始化的数据"},
		{0x04000000, "不可缓存"},
		{0x08000000, "不可分页"},
		{0x10000000, "共享"},
		{0x20000000, "可执行"},
		{0x40000000, "可读"},
		{0x80000000, "可写"}};
	
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
			AddTextLine("文件名", lpszFilename);
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
				AddTextLine("文件大小", szValue);

				FILETIME ftCreation, ftModified;
				GetFileTime(hFile, &ftCreation, NULL, &ftModified);
				AddTextLine("创建日期", FormatFileTime(ftCreation, szValue));
				AddTextLine("修改日期", FormatFileTime(ftModified, szValue));
				AddTextLine("可执行文件格式", GetExeType(dwExeType, szValue));
				
				AddMainText("\r\nMS-DOS 头信息\r\n=========================\r\n");
				AddTextLine("在最后一页字节", FormatDWord(idh->e_cblp, szValue));
				AddTextLine("分页数量", FormatDWord(idh->e_cp, szValue));
				AddTextLine("重定位入口", FormatDWord(idh->e_crlc, szValue));
				AddTextLine("段的头大小", FormatDWord(idh->e_cparhdr, szValue));
				AddTextLine("最初的 SS (相对的)", FormatDWordHex(idh->e_ss, szValue));
				AddTextLine("最初的 SP", FormatDWordHex(idh->e_sp, szValue));
				AddTextLine("最初的 CS (相对的)", FormatDWordHex(idh->e_cs, szValue));
				AddTextLine("最初的 IP", FormatDWordHex(idh->e_ip, szValue));
				AddTextLine("Checksum", FormatDWordHex(idh->e_csum, szValue));
				if (GetDosCompress(szMZHeader, szTemp))
				{
					sprintf(szValue, "\r\n*** 文件由 %s 编译 ***\r\n", szTemp);
					AddMainText(szValue);
				}

				if (dwExeType == EXE_TYPE_NE)
				{
					IMAGE_OS2_HEADER *iosh = (IMAGE_OS2_HEADER *)szMewHeader;
					AddMainText("\r\n新的可执行文件头信息\r\n===================================\r\n");
					AddTextLine("版本号", itoa(iosh->ne_ver, szValue, 10));
					AddTextLine("修订号", itoa(iosh->ne_rev, szValue, 10));
					AddTextLine("入口表偏移", FormatDWordHex(iosh->ne_enttab, szValue));
					AddTextLine("入口表大小", FormatDWord(iosh->ne_cbenttab, szValue, szBytesWord));
					AddTextLine("Checksum", FormatDWordHex(iosh->ne_crc, szValue, 8));
					AddTextLine("库模块", ((iosh->ne_flags & 0x8000) ? "Yes" : "No"));
					AddTextLine("目标系统", GetNETargetOS(iosh->ne_exetyp, szValue));

					AddTextLine("初始堆分配", FormatDWord(iosh->ne_heap, szValue, szBytesWord));
					AddTextLine("初始堆栈分配", FormatDWord(iosh->ne_heap, szValue, szBytesWord));
					sprintf(szValue, "%.4x:%.4x", (WORD)(((DWORD)iosh->ne_csip) >> 16), (WORD)(((DWORD)iosh->ne_csip) & 0xffff));
					AddTextLine("初始 CS:IP", szValue);

					sprintf(szValue, "%.4x:%.4x", (WORD)(((DWORD)iosh->ne_sssp) >> 16), (WORD)(((DWORD)iosh->ne_sssp) & 0xffff));
					AddTextLine("初始 SS:SP", szValue);

					AddTextLine("部分条目", FormatDWord(iosh->ne_cseg, szValue));
					AddTextLine("模块条目", FormatDWord(iosh->ne_cmod, szValue));
					AddTextLine("非驻留名称表", FormatDWord(iosh->ne_cbnrestab, szValue, szBytesWord));
					
					AddTextLine("段表偏移", FormatDWordHex(iosh->ne_segtab, szValue));
					AddTextLine("资源表偏移", FormatDWordHex(iosh->ne_rsrctab, szValue));
					AddTextLine("驻留名称偏移", FormatDWordHex(iosh->ne_restab, szValue));
					AddTextLine("模块引用偏移", FormatDWordHex(iosh->ne_modtab, szValue));
					AddTextLine("导入名称偏移", FormatDWordHex(iosh->ne_imptab, szValue));
					AddTextLine("非驻留名偏移", FormatDWordHex(iosh->ne_nrestab, szValue, 8));
					
					AddTextLine("可移动的条目数", FormatDWord(iosh->ne_cmovent, szValue));
					AddTextLine("段对齐", FormatDWord(iosh->ne_align, szValue));
					AddTextLine("资源段对齐", FormatDWord(iosh->ne_cres, szValue));
				}


				if (dwExeType == EXE_TYPE_PE)
				{
					IMAGE_FILE_HEADER *ifh;
	
					AddMainText("\r\nPE头信息\r\n===================================\r\n");
					ifh = (IMAGE_FILE_HEADER *)&szMewHeader[4];
					AddTextLine("机器", GetMachineName(ifh->Machine, szValue));
					AddTextLine("节数量", FormatDWord(ifh->NumberOfSections, szValue));
					AddTextLine("符号数量", FormatDWord(ifh->NumberOfSymbols, szValue));
					AddTextLine("可选头大小", FormatDWord(ifh->SizeOfOptionalHeader, szValue, szBytesWord));
					AddTextLine("时间戳", FormatDWordHex(ifh->TimeDateStamp, szValue));
					AddTextLine("特征", GetCharacteristics(ifh->Characteristics, szValue));
					
					AddMainText("\r\n可选头信息\r\n===================================\r\n");
					
					IMAGE_OPTIONAL_HEADER32 *ioh = (IMAGE_OPTIONAL_HEADER32 *)&szMewHeader[24];
					sprintf(szValue, "%d.%d", ioh->MajorLinkerVersion, ioh->MinorLinkerVersion);
					AddTextLine("链接器版本", szValue);
					AddTextLine("代码大小", FormatDWord(ioh->SizeOfCode, szValue, szBytesWord ));
					AddTextLine("初始化数据大小", FormatDWord(ioh->SizeOfInitializedData, szValue, szBytesWord));
					AddTextLine("未初始化大小", FormatDWord(ioh->SizeOfUninitializedData, szValue, szBytesWord));

					if (ifh->Machine == IMAGE_FILE_MACHINE_I386 && ifh->SizeOfOptionalHeader >= 224)
					{
						AddTextLine("节对齐", FormatDWord(ioh->SectionAlignment, szValue, szBytesWord));
						AddTextLine("文件对齐", FormatDWord(ioh->FileAlignment, szValue, szBytesWord));
						sprintf(szValue, "%d.%d", ioh->MajorOperatingSystemVersion, ioh->MinorOperatingSystemVersion);
						AddTextLine("系统版本", szValue);
						sprintf(szValue, "%d.%d", ioh->MajorImageVersion, ioh->MinorImageVersion);
						AddTextLine("映像版本", szValue);
						sprintf(szValue, "%d.%d", ioh->MajorSubsystemVersion, ioh->MinorSubsystemVersion);
						AddTextLine("子系统版本", szValue);
						AddTextLine("镜像大小", FormatDWord(ioh->SizeOfImage, szValue, szBytesWord));
						AddTextLine("头大小", FormatDWord(ioh->SizeOfHeaders, szValue, szBytesWord));
						AddTextLine("Checksum", FormatDWordHex(ioh->CheckSum, szValue, 8));
						AddTextLine("子系统", GetSubsystemName(ioh->Subsystem, szValue));
						AddTextLine("储备堆栈大小", FormatDWordHex(ioh->SizeOfStackReserve, szValue, 8));
						AddTextLine("提交堆栈大小", FormatDWordHex(ioh->SizeOfStackCommit, szValue, 8));
						AddTextLine("储备堆大小", FormatDWordHex(ioh->SizeOfHeapReserve, szValue, 8));
						AddTextLine("提交堆大小", FormatDWordHex(ioh->SizeOfHeapCommit, szValue, 8));
						AddTextLine("基址", FormatDWordHex(ioh->ImageBase, szValue, 8));

					}
					AddTextLine("入口点", FormatDWordHex(ioh->AddressOfEntryPoint, szValue, 8));
					AddTextLine("代码基段", FormatDWordHex(ioh->BaseOfCode, szValue, 8));
					AddTextLine("数据基段", FormatDWordHex(ioh->BaseOfData, szValue, 8));

					//Section Headers
					IMAGE_SECTION_HEADER *ishs;
					DWORD dwInfo1;
					USHORT uNumOfSections = ifh->NumberOfSections;
					if (uNumOfSections > MAX_NUM_OF_SECTIONS)
						uNumOfSections  = MAX_NUM_OF_SECTIONS;

					ishs = (IMAGE_SECTION_HEADER *)&szMewHeader[24 + ifh->SizeOfOptionalHeader]; 
					
					AddMainText("\r\n节头部信息\r\n===================================\r\n");
					sprintf(szValue, "这个文件有 %d 节:\r\n", ifh->NumberOfSections);
					AddMainText(szValue);
					for (USHORT uIndex = 0; uIndex < uNumOfSections; uIndex++)
					{
						if (uIndex == 0) 
							AddMainText("----------------------------------------\r\n");
						memcpy(szValue, ishs[uIndex].Name, 8);
						szValue[8] = '\0';
						AddTextLine("节名称", szValue);
						AddTextLine("虚拟地址", FormatDWordHex(ishs[uIndex].VirtualAddress, szValue, 8));
						AddTextLine("原数据大小", FormatDWordHex(ishs[uIndex].SizeOfRawData, szValue, 8));
						AddTextLine("元数据指针", FormatDWordHex(ishs[uIndex].PointerToRawData, szValue, 8));
						AddTextLine("特征", GetSectionCharacteristics(ishs[uIndex].Characteristics, szValue));

						AddMainText("----------------------------------------\r\n");
					}

					GetInfoFromSectionHeaders(ishs, uNumOfSections, &dwInfo1);
					if ((dwInfo1 & SECTIONS_HEADER_INFO_PKLITE) != 0)
						AddMainText("\r\n*** 文件被压缩为Windows的PKLITE ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_WINZIP) != 0)
						AddMainText("\r\n*** 该文件自解包程序由WinZip归档 ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_WWPACK32) != 0)
						AddMainText("\r\n*** 文件由WWPACK32压缩 ***\r\n");

					if ((dwInfo1 & SECTIONS_HEADER_INFO_UPX) != 0)
						AddMainText("\r\n*** 文件被UPX加壳 ***\r\n");
					
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
						sprintf(szValue, "\r\n*** 此文件由 %s 编译 ***\r\n", szCompiler);
						AddMainText(szValue);
					}
						
				}

			} else
			{
				AddMainText("\r\n不认识的文件格式\r\n");
				return;
			}

		} else
		{
			ShowError(lpszFilename, "不能从 %s 读取\r\n错误 %d:%s");
			return;
		}

	} else
	{
		
		ShowError(lpszFilename, "不能打开 %s\r\n错误 %d:%s");
		return;
	}

	AddMainText("\r\n版本信息\r\n====================\r\n");
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
			AddTextLine("操作系统", GetFileOSName(pInfo->dwFileOS, szValue));
			AddTextLine("文件类型", GetFileType(pInfo->dwFileType, szValue));
			AddTextLine("文件子类型", GetFileSubType(pInfo->dwFileType, pInfo->dwFileSubtype , szValue));
			
			sprintf(szValue, "%d,%d,%d,%d", (pInfo->dwFileVersionMS >> 16), (pInfo->dwFileVersionMS & 65535), (pInfo->dwFileVersionLS >> 16), pInfo->dwFileVersionLS & 65535);
			AddTextLine("文件版本", szValue);

			sprintf(szValue, "%d,%d,%d,%d", (pInfo->dwProductVersionMS >> 16), (pInfo->dwProductVersionMS & 65535), (pInfo->dwProductVersionLS >> 16), pInfo->dwProductVersionLS & 65535);
			AddTextLine("产品版本", szValue);

		}
		AddMainText("============================================================\r\n");
		VerQueryValue((LPVOID)buf, "\\VarFileInfo\\Translation", (LPVOID *)&str, &dwSize);
		sprintf(szLang1, "%4.4X%4.4X", *(unsigned short *)str, *(unsigned short *)(str + 2));

		if (GetInfoStr(buf, szLang1, "ProductName", szValue))
			AddTextLine("产品名称", szValue);
		if (GetInfoStr(buf, szLang1, "FileDescription", szValue))
			AddTextLine("文件描述", szValue);
		if (GetInfoStr(buf, szLang1, "FileVersion", szValue))
			AddTextLine("文件版本", szValue);
		if (GetInfoStr(buf, szLang1, "ProductVersion", szValue))
			AddTextLine("产品版本", szValue);
		if (GetInfoStr(buf, szLang1, "CompanyName", szValue))
			AddTextLine("公司名", szValue);
		if (GetInfoStr(buf, szLang1, "InternalName", szValue))
			AddTextLine("内部名称", szValue);
		if (GetInfoStr(buf, szLang1, "LegalCopyright", szValue))
			AddTextLine("合法版权", szValue);
		if (GetInfoStr(buf, szLang1, "OriginalFileName", szValue))
			AddTextLine("原文件名", szValue);


		delete []buf;
	} else
		AddMainText("这个文件版本信息不可用。");
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
		return TRUE;//消息已处理，不需要再调用真正的内部对话框消息处理函数
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
