// LEScanWinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LEScanWinDlg.h"
#include "GattPublic.h"
#include "trace.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

extern "C"{
#include "LEGap.h"
}

#pragma comment(lib, "LEGap.lib")

extern unsigned long long BDAddress;
extern CString BDAddrStr;
extern CString BTDevNameStr;

CLEScanWinDlg* CLEScanWinDlg::pThis = NULL;

// CLEScanWinDlg 对话框

IMPLEMENT_DYNAMIC(CLEScanWinDlg, CDialog)

CLEScanWinDlg::CLEScanWinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLEScanWinDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CLEScanWinDlg::pThis = this;
	m_OTAMScan = 0;
}

CLEScanWinDlg::CLEScanWinDlg(int ScanDeviceInOTAM, ULONGLONG NormalAddress, CWnd* pParent /*=NULL*/)
	: CDialog(CLEScanWinDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CLEScanWinDlg::pThis = this;
	m_OTAMScan = ScanDeviceInOTAM;
	m_NormalAddress = NormalAddress;
}

CLEScanWinDlg::~CLEScanWinDlg()
{
	OnBnClickedScanStop();
}

BOOL CLEScanWinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CListCtrl* lst = (CListCtrl*)GetDlgItem(IDC_DEVICE_LIST);
	lst->ModifyStyle( 0, LVS_REPORT );               // 报表模式  
	lst->SetExtendedStyle(lst->GetExtendedStyle() 
		| LVS_EX_GRIDLINES 
		| LVS_EX_FULLROWSELECT);  

	lst->InsertColumn(0,_T("Address")); 
	lst->InsertColumn(1,_T("Device Name")); 
	lst->InsertColumn(2,_T("Adv Type")); 
	lst->InsertColumn(3,_T("RSSI"));

	CRect rect;
	lst->GetClientRect(rect); //获得当前客户区信息
	lst->SetColumnWidth(0, rect.Width() / 4); //设置列的宽度。  
	lst->SetColumnWidth(1, rect.Width() / 4); 
	lst->SetColumnWidth(2, rect.Width() / 4); 
	lst->SetColumnWidth(3, rect.Width() / 4); 

	m_AdvType[ADV_IND] = CString(L"ADV_IND");
	m_AdvType[ADV_DIRECT_IND] = CString(L"ADV_DIRECT_IND");
	m_AdvType[ADV_SCAN_IND] = CString(L"ADV_SCAN_IND");
	m_AdvType[ADV_NONCONN_IND] = CString(L"ADV_NONCONN_IND");

	m_AdvTypeR[CString(L"ADV_IND")] = ADV_IND;
	m_AdvTypeR[CString(L"ADV_DIRECT_IND")] = ADV_DIRECT_IND;
	m_AdvTypeR[CString(L"ADV_SCAN_IND")] = ADV_SCAN_IND;
	m_AdvTypeR[CString(L"ADV_NONCONN_IND")] = ADV_NONCONN_IND;

	if(m_OTAMScan)
		SetWindowText(L"Scan devices in OTA mode");

	m_ScanStop.EnableWindow(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLEScanWinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLEScanWinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLEScanWinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_SCAN_START, m_ScanEnable);
	DDX_Control(pDX, ID_SCAN_STOP, m_ScanStop);
}


BEGIN_MESSAGE_MAP(CLEScanWinDlg, CDialog)
	ON_BN_CLICKED(ID_SCAN_START, &CLEScanWinDlg::OnBnClickedScanStart)
	ON_NOTIFY(NM_DBLCLK, IDC_DEVICE_LIST, &CLEScanWinDlg::OnNMDblclkDeviceList)
	ON_BN_CLICKED(ID_SCAN_STOP, &CLEScanWinDlg::OnBnClickedScanStop)
END_MESSAGE_MAP()

void CLEScanWinDlg::OnBnClickedScanStart()
{
	// TODO: 在此添加控件通知处理程序代码
	signed char s;
	// TODO: 在此添加控件通知处理程序代码
	BleRegisterScanCallback(LeGapScanCallBack);

	// set scan interval, if not call, using default value
	// Range: 0x0004 to 0x4000
	// Default: 0x0010 (10 ms)
	// Time = N * 0.625 msec
	// Time Range: 2.5 msec to 10.24 seconds
	// BleSetScanningInterval( 0x0010 ); //default value: 10ms

	// start scanning and check status
	BleIsScanning(&s);
	if(s)
	{
		printf("Is Scanning, stop scan and restart again");
		BleEnableScan(false,0x0);            
	}
	BleSetScanMode(ACTIVE);		//set active mode
	BleEnableScan(true, 0x1);
	printf("Set scan enable\n");
	BleIsScanning(&s);
	printf("Is Scanning: %d\n", s);
	m_ScanEnable.EnableWindow(FALSE);
	m_ScanStop.EnableWindow(TRUE);
}

void CLEScanWinDlg::LeGapScanCallBack(
									  unsigned char* pAddress, 
									  const int nSizeAddress, 
									  unsigned char* pPkt, 
									  const int nSizePkt, 
									  void* pUserData)
{
	unsigned char RSSI;
	PADV_DATA advData = NULL;
	int advLen = 0;
	unsigned char nameArr[31] = {0};
	CString address;
	CString nameStr;
	UCHAR AdvType = (UCHAR)pUserData;

	address.Format(_T("%02x%02x%02x%02x%02x%02x"),pAddress[5], pAddress[4], pAddress[3], pAddress[2], pAddress[1],pAddress[0]);
	if(AdvType == ADV_IND && nSizePkt > 1)
	{
		advLen = nSizePkt - 1;
		RSSI = *(pPkt + advLen);
		advData = (PADV_DATA)(pPkt);
		/**
		typedef struct _ADV_DATA{
			unsigned char Length;
			unsigned char AdType;
			unsigned char AdData[0];
		}ADV_DATA, *PADV_DATA;
		*/
		for(int i=advLen, templen = 0; i>0; )
		{
			templen = advData->Length;
			switch(advData->AdType)
			{
			case AT_FLAGS:
				{
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("AT_FLAGS:   \n")));
					DEBUG_DATA(TRACE_LEVEL_INFORMATION, advData->AdData, templen - 1);
				}
				break;
			case AT_MORE_16UUID:                  	
			case AT_COMPLETE_16UUID:             	
			case AT_MORE_32UUID:
			case AT_COMPLETE_32UUID:              	
			case AT_MORE_128UUID:
			case AT_COMPLETE_128UUID:            	
				break;
			case AT_SHORTEN_LOCAL_NAME:          	
			case AT_COMPLETE_LOCAL_NAME:
				{
					memcpy(nameArr, advData->AdData, templen-1);
					size_t nu = templen-1;
					size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)nameArr,int(nu),NULL,0);
					wchar_t* buffer;
					buffer = new wchar_t[n+1];
					::MultiByteToWideChar(CP_ACP,0,(const char *)nameArr,int(nu),buffer,int(n)); 
					buffer[n] = '\0';
					nameStr.Format(_T("%s"), buffer);
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Local Name:   %s\n"), buffer));
					DEBUG_DATA(TRACE_LEVEL_INFORMATION, advData->AdData, templen - 1);
				}
				break;
			case AT_TX_POWER_LEVEL:
			case AT_CLASS_OF_DEVICE:
			case AT_SIMPLE_PAIR_HASH_C:
			case AT_SIMPLE_PAIR_RAND_R:
			case AT_TK_VALUE:
			case AT_OOB_FLAGS:
			case AT_SLAVE_CONNECTION_INTERVAL_RANGE:
			case AT_SERVICE_SOLICATION_UUID16:
			case AT_SERVICE_SOLICATION_UUID128:
			case AT_SERVICE_DATA:
			case AT_SERVICE_DATA_32:
			case AT_SERVICE_DATA_128:
			case AT_PUBLIC_TARGET_ADDRESS:
			case AT_RANDOM_TARGET_ADDRESS:
			case AT_APPEARANCE:
			case AT_ADVERTISING_INTERVAL:
			case AT_LE_BT_ADDRESS:
			case AT_LE_ROLE:
			case AT_SIMPLE_PAIR_HASH:
			case AT_SIMPLE_PAIR_Randomizer:
			case AT_3D_INROMATION_DATA:
			case AT_MANUFACTURE_SPEC_DATA:
				{
					DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("MFS:   \n")));
					DEBUG_DATA(TRACE_LEVEL_INFORMATION, advData->AdData, templen-1);
					if(pThis->m_OTAMScan && templen >= 8) // OTA Mode Scan
					{
						int CompID = 0;
						ULONGLONG OriginalBDAdress = 0;
						UCHAR* pOriginalBDAddress = (UCHAR*)&OriginalBDAdress;
						pThis->m_OriginalAddress = 0;
						memcpy(&CompID, advData->AdData, 2);
						UCHAR* pAdvAddress = (UCHAR*)(advData->AdData+2);
						//memcpy(&OriginalBDAdress, advData->AdData+2, 6);
						pOriginalBDAddress[5] = *(pAdvAddress + 0);
						pOriginalBDAddress[4] = *(pAdvAddress + 1);
						pOriginalBDAddress[3] = *(pAdvAddress + 2);
						pOriginalBDAddress[2] = *(pAdvAddress + 3);
						pOriginalBDAddress[1] = *(pAdvAddress + 4);
						pOriginalBDAddress[0] = *(pAdvAddress + 5);
						DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Company ID:   %x\n"), CompID));
						DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Original BDAddress:   %x\n"), OriginalBDAdress));
						if(CompID == 0x005d)
						{
							pThis->m_OriginalAddress = OriginalBDAdress;
						}
					}
				}
				break;
				// More User defined AT type
				//
			default:
				break;
			}
			i -= templen + 1;
			advData = (PADV_DATA)((char *)advData + templen + 1);
		}
	}
	else if( AdvType == ADV_DIRECT_IND && nSizePkt >= 1 )
	{
		ULONGLONG BDADdress;
		advLen = nSizePkt - 1;
		RSSI = *(pPkt + advLen);

		memcpy(&BDADdress, pAddress, nSizeAddress);
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("AdvType(%x), Address(%x)"), AdvType, BDADdress));
	}

	if(pThis->m_OTAMScan && pThis->m_OriginalAddress != pThis->m_NormalAddress)
	{
		DEBUG_TRACE(TRACE_LEVEL_INFORMATION, RTK_LE_PROFILE_RCU, (_T("Original Address(%x), Normal Address(%x)"), pThis->m_OriginalAddress,pThis->m_NormalAddress));
		return;
	}

	CString RSSIStr;
	if( RSSI & 0x80) 
		RSSIStr.Format(L"-%d dBm", RSSI&0x7F);	// -127 - 0
	else
		RSSIStr.Format(L"+%d dBm", RSSI);		// 0 - 20

	int Index;
	LVFINDINFO info;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = address.AllocSysString();
	CListCtrl* lst = (CListCtrl*)pThis->GetDlgItem(IDC_DEVICE_LIST);
	if( NULL == lst )
		return;
	if((Index = lst->FindItem(&info)) != -1)
	{// change the exist item
		if(pThis->m_scanList[address].IsEmpty() && !nameStr.IsEmpty())
		{
			lst->SetItemText(Index, 1, nameStr.AllocSysString());
			lst->SetItemText(Index, 2, pThis->m_AdvType[AdvType].AllocSysString());
			lst->SetItemText(Index, 3, RSSIStr.AllocSysString());
			pThis->m_scanList[address] = nameStr;	// only when str is empty, change it
		}
	}
	else
	{// add new item
		Index = lst->GetItemCount();
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = Index; 
		lvItem.iSubItem = 0;
		lvItem.pszText = address.AllocSysString();
		lst->InsertItem(&lvItem); 
		if(!nameStr.IsEmpty() || pThis->m_scanList[address].IsEmpty())
		{// cache is empty
			pThis->m_scanList[address] = nameStr;
			lst->SetItemText(Index, 1, nameStr.AllocSysString());
			lst->SetItemText(Index, 2, pThis->m_AdvType[AdvType].AllocSysString());
		}
		else
		{// cache is not empty
			lst->SetItemText(Index, 1, pThis->m_scanList[address].AllocSysString());
			lst->SetItemText(Index, 2, pThis->m_AdvType[AdvType].AllocSysString());
		}
		lst->SetItemText(Index, 3, RSSIStr.AllocSysString());
	}
}


char *w2c(char *pcstr,const wchar_t *pwstr, size_t len)
{

	int nlength=wcslen(pwstr);

	//获取转换后的长度

	size_t nbytes = WideCharToMultiByte( 0, // specify the code page used to perform the conversion
		0,         // no special flags to handle unmapped characters
		pwstr,     // wide character string to convert
		nlength,   // the number of wide characters in that string
		NULL,      // no output buffer given, we just want to know how long it needs to be
		0,
		NULL,      // no replacement character given
		NULL );    // we don't want to know if a character didn't make it through the translation

	// make sure the buffer is big enough for this, making it larger if necessary

	if(nbytes>len)   nbytes=len;

	WideCharToMultiByte( 0, // specify the code page used to perform the conversion
		0,         // no special flags to handle unmapped characters
		pwstr,   // wide character string to convert
		nlength,   // the number of wide characters in that string
		pcstr, // put the output ascii characters at the end of the buffer
		nbytes,                           // there is at least this much space there
		NULL,      // no replacement character given
		NULL );

	return pcstr ;

}

void CLEScanWinDlg::OnNMDblclkDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* lst = (CListCtrl*)pThis->GetDlgItem(IDC_DEVICE_LIST);

	POSITION pos = lst->GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		return;
	}
	else
	{
		int tIndex = lst->GetNextSelectedItem(pos);

		TCHAR szBuf[1024];
		LVITEM lvi;
		lvi.iItem = tIndex;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szBuf;
		lvi.cchTextMax = 1024;
		lst->GetItem(&lvi);

		char addrBuf[13];
		w2c(addrBuf, szBuf, 12);
		addrBuf[12] = '\0';

		stringstream ss(addrBuf);
		ss>> hex >> m_BDAddress;

		BDAddress = m_BDAddress;

		m_BDAddrStr = lst->GetItemText(tIndex, 0);
		m_BTDevNameStr = lst->GetItemText(tIndex, 1);
		m_AdvTypeStr = lst->GetItemText(tIndex, 2);
		m_RSSIStr = lst->GetItemText(tIndex, 3);

		if(m_AdvTypeR[m_AdvTypeStr] != ADV_IND && m_AdvTypeR[m_AdvTypeStr] != ADV_DIRECT_IND)
		{
			CString Info(L"This device cannot pair, adv type is ");
			MessageBox(Info + m_AdvTypeStr);
			return;
		}

		OnBnClickedScanStop();
		//CDialog::EndDialog(0);	//return value == parameter
		CDialog::OnOK();
	}
}

void CLEScanWinDlg::OnBnClickedScanStop()
{
	signed char s;
	// TODO: 在此添加控件通知处理程序代码
	// unregister callback
	// stop scanning and check status
	BleEnableScan(false, 0x1);
	printf("Set scan disable\n");
	BleIsScanning(&s);
	printf("Is Scanning: %d\n", s);
	printf("Test Scan Callback OK");
	BleUnregisterScanCallback();

	CListCtrl* lst = (CListCtrl*)pThis->GetDlgItem(IDC_DEVICE_LIST);
	int nIndex = lst->GetItemCount();
	for (int i=nIndex-1; i>=0; i--)
		lst->DeleteItem(i);
	m_ScanStop.EnableWindow(FALSE);
	m_ScanEnable.EnableWindow(TRUE);
}
