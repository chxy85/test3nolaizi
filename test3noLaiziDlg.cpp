// test3noLaiziDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test3noLaizi.h"
#include "test3noLaiziDlg.h"
#include ".\test3nolaizidlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ctest3noLaiziDlg 对话框



Ctest3noLaiziDlg::Ctest3noLaiziDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ctest3noLaiziDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest3noLaiziDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ctest3noLaiziDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
    ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
END_MESSAGE_MAP()


// Ctest3noLaiziDlg 消息处理程序

BOOL Ctest3noLaiziDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void Ctest3noLaiziDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest3noLaiziDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR Ctest3noLaiziDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "GameLogic.h"
CGameLogic gamelogic;

void Ctest3noLaiziDlg::OnBnClickedButton1()
{
    const BYTE cbHandCardData[] = {1,0xa,0xa,0xa,0xa};
    gamelogic.m_cbUserCardCount[2] = sizeof(cbHandCardData);
    memcpy(gamelogic.m_cbAllCardData[2], cbHandCardData, sizeof(cbHandCardData));

    const BYTE cbTurnCardData[] = {5,5,5,5};
    tagOutCardResult OutCardResult = {0};
    WORD wOutCardUser = 1;

    BYTE cbBankerCardData[] = {9,9,9,9};
    gamelogic.m_cbUserCardCount[0] = sizeof(cbBankerCardData);
    memcpy(gamelogic.m_cbAllCardData[0], cbBankerCardData, sizeof(cbBankerCardData));

    gamelogic.m_cbUserCardCount[1] = 1;
     gamelogic.m_cbAllCardData[1][0] = 0x1;

//     gamelogic.m_cbAllCardData[0][0] = 0xb;
//     gamelogic.m_cbAllCardData[0][1] = 0xb;
//     gamelogic.m_cbAllCardData[0][2] = 0xb;
//      gamelogic.m_cbAllCardData[0][3] = 0xb;
//     gamelogic.m_cbAllCardData[0][4] = 0x1;
//     gamelogic.m_cbAllCardData[0][5] = 0x1;
//     gamelogic.m_cbAllCardData[0][6] = 0x1;
//     gamelogic.m_cbAllCardData[0][7] = 0x1;
//     gamelogic.m_cbAllCardData[0][8] = 0x1;
//     gamelogic.m_cbAllCardData[0][9] = 0x1;
//     gamelogic.m_cbAllCardData[0][10] = 0x1;
//     gamelogic.m_cbAllCardData[0][11] = 0x4e;
//     gamelogic.m_cbAllCardData[0][12] = 0x4e;
//     gamelogic.m_cbAllCardData[0][13] = 0x4e;
//     gamelogic.m_cbAllCardData[0][14] = 0x4f;
//     gamelogic.m_cbAllCardData[0][15] = 0x1;

///     gamelogic.m_cbAllCardData[1][0] = 0x09;
//     gamelogic.m_cbAllCardData[1][1] = 0x19;
//     gamelogic.m_cbAllCardData[1][2] = 0x29;
//     gamelogic.m_cbAllCardData[1][3] = 0x07;
//     gamelogic.m_cbAllCardData[1][4] = 0x17;
//     gamelogic.m_cbAllCardData[1][5] = 0x27;
//     gamelogic.m_cbAllCardData[1][6] = 0x03;
//     gamelogic.m_cbAllCardData[1][7] = 0x13;
//     gamelogic.m_cbAllCardData[1][8] = 0x24;
//     gamelogic.m_cbAllCardData[1][9] = 0x14 ;
//     gamelogic.m_cbAllCardData[1][10] = 0x32;
//     gamelogic.m_cbAllCardData[1][11] = 0x02;
//     gamelogic.m_cbAllCardData[1][12] = 0x12;
//     gamelogic.m_cbAllCardData[1][13] = 0x22;
//     gamelogic.m_cbAllCardData[1][14] = 0x3B;
//     gamelogic.m_cbAllCardData[1][15] = 0x2b;
//     gamelogic.m_cbAllCardData[1][16] = 0x1b;
// 
//     gamelogic.m_cbAllCardData[2][0] = 0x21;
//     gamelogic.m_cbAllCardData[2][1] = 0x24;
//     gamelogic.m_cbAllCardData[2][2] = 0x09;
//     gamelogic.m_cbAllCardData[2][3] = 0x26;
//     gamelogic.m_cbAllCardData[2][4] = 0x27;
//     gamelogic.m_cbAllCardData[2][5] = 0x28;
//     gamelogic.m_cbAllCardData[2][6] = 0x29;
//     gamelogic.m_cbAllCardData[2][7] = 0x2A;
//     gamelogic.m_cbAllCardData[2][8] = 0x2B;
//     gamelogic.m_cbAllCardData[2][9] = 0x14;
//     gamelogic.m_cbAllCardData[2][10] = 0x16;
//     gamelogic.m_cbAllCardData[2][11] = 0x17;
//     gamelogic.m_cbAllCardData[2][12] = 0x18;
//     gamelogic.m_cbAllCardData[2][13] = 0x1A;
//     gamelogic.m_cbAllCardData[2][14] = 0x1B;
//     gamelogic.m_cbAllCardData[2][15] = 0x1C;
//     gamelogic.m_cbAllCardData[2][16] = 0x1D;

    gamelogic.m_wBankerUser = 0;
    gamelogic.UpsideOfBankerOutCard(cbHandCardData,sizeof(cbHandCardData), wOutCardUser, cbTurnCardData, sizeof(cbTurnCardData), OutCardResult) ;

}
#include "log.h"
void Ctest3noLaiziDlg::OnBnClickedButton2()
{
    BYTE test[] = {1,2,0x4f,0x4f,0x4f,0x4e,0x4e};
gamelogic.SortCardList(test, 7,ST_ORDER);

 BYTE cbCardData = 0x4e;

//扑克属性
BYTE cbCardColor=cbCardData&0xf0;
BYTE cbCardValue=cbCardData&0xf;



int a = 0;
//转换数值
if (cbCardColor==0x40) a =  cbCardValue+2;
a =  (cbCardValue<=2)?(cbCardValue+13):cbCardValue;



    //////////////////////////////////////////////////////////////////////////
    BYTE cbHandCardData[] = {0x4f,0x4f,0x4e,0x32,0x02,0x31,0x21,0x3d,0x2d,0x2d,0x1d,0x3c,0x3b,0x3b,0x1b,0x3a,0x2a,0xa};

    tagBomCardResult BomCardResult;
    BYTE cbBomCardData[MAX_COUNT];
    BYTE cbBomCardCount;
    tagOutCardTypeResult CardTypeResult[12+1] = {0};
    ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

//    gamelogic.AnalyseOutCardType(cbHandCardData, sizeof(cbHandCardData), CardTypeResult) ;

}

void Ctest3noLaiziDlg::OnBnClickedButton3()
{
    BYTE cbHandCardData[] = {0x4e,2,2,2,2,1,1,1,0xd,0xd,0xd,0xd,0xc,0xc,0xb,0xb,0xb,0xa,0xa,0xa};
    BYTE m_cbTurnCardData[20];
    BYTE cbWantOutCardData[] = {0x4f,0x4f,0x4f,0x4e};
    tagOutCardResult OutCardResult = {0};


    gamelogic.m_cbAllCardData[0][0] = 0xb;
    gamelogic.m_cbAllCardData[0][1] = 0xb;
    gamelogic.m_cbAllCardData[0][2] = 0xb;
    gamelogic.m_cbAllCardData[0][3] = 0xb;
    gamelogic.m_cbAllCardData[0][4] = 0x1;
    gamelogic.m_cbAllCardData[0][5] = 0x1;
    gamelogic.m_cbAllCardData[0][6] = 0x1;
    gamelogic.m_cbAllCardData[0][7] = 0x1;
    gamelogic.m_cbAllCardData[0][8] = 0x1;
    gamelogic.m_cbAllCardData[0][9] = 0x1;
    gamelogic.m_cbAllCardData[0][10] = 0x1;
    gamelogic.m_cbAllCardData[0][11] = 0x4e;
    gamelogic.m_cbAllCardData[0][12] = 0x4e;
    gamelogic.m_cbAllCardData[0][13] = 0x4e;
    gamelogic.m_cbAllCardData[0][14] = 0x4f;
    gamelogic.m_cbAllCardData[0][15] = 0x1;

    gamelogic.m_cbAllCardData[1][0] = 0x09;
    gamelogic.m_cbAllCardData[1][1] = 0x19;
    gamelogic.m_cbAllCardData[1][2] = 0x29;
    gamelogic.m_cbAllCardData[1][3] = 0x07;
    gamelogic.m_cbAllCardData[1][4] = 0x17;
    gamelogic.m_cbAllCardData[1][5] = 0x27;
    gamelogic.m_cbAllCardData[1][6] = 0x03;
    gamelogic.m_cbAllCardData[1][7] = 0x13;
    gamelogic.m_cbAllCardData[1][8] = 0x24;
    gamelogic.m_cbAllCardData[1][9] = 0x14 ;
    gamelogic.m_cbAllCardData[1][10] = 0x32;
    gamelogic.m_cbAllCardData[1][11] = 0x02;
    gamelogic.m_cbAllCardData[1][12] = 0x12;
    gamelogic.m_cbAllCardData[1][13] = 0x22;
    gamelogic.m_cbAllCardData[1][14] = 0x3B;
    gamelogic.m_cbAllCardData[1][15] = 0x2b;
    gamelogic.m_cbAllCardData[1][16] = 0x1b;

    gamelogic.m_cbAllCardData[2][0] = 0x4f;
    gamelogic.m_cbAllCardData[2][1] = 0x4f;
    gamelogic.m_cbAllCardData[2][2] = 0x4f;
    gamelogic.m_cbAllCardData[2][3] = 0x4e;
    gamelogic.m_cbAllCardData[2][4] = 0x32;
    gamelogic.m_cbAllCardData[2][5] = 0x12;
    gamelogic.m_cbAllCardData[2][6] = 0x2;
    gamelogic.m_cbAllCardData[2][7] = 0x31;
    gamelogic.m_cbAllCardData[2][8] = 0x21;
    gamelogic.m_cbAllCardData[2][9] = 0x1d;
    gamelogic.m_cbAllCardData[2][10] = 0x0d;
    gamelogic.m_cbAllCardData[2][11] = 0x3b;
    gamelogic.m_cbAllCardData[2][12] = 0x2b;
    gamelogic.m_cbAllCardData[2][13] = 0x1b;
    gamelogic.m_cbAllCardData[2][14] = 0x3a;
    gamelogic.m_cbAllCardData[2][15] = 0x3a;
    gamelogic.m_cbAllCardData[2][16] = 0x2a;
gamelogic.m_cbUserCardCount[2] = 17;
//     gamelogic.m_cbAllCardData[2][0] = 0x21;
//     gamelogic.m_cbAllCardData[2][1] = 0x24;
//     gamelogic.m_cbAllCardData[2][2] = 0x09;
//     gamelogic.m_cbAllCardData[2][3] = 0x26;
//     gamelogic.m_cbAllCardData[2][4] = 0x27;
//     gamelogic.m_cbAllCardData[2][5] = 0x28;
//     gamelogic.m_cbAllCardData[2][6] = 0x29;
//     gamelogic.m_cbAllCardData[2][7] = 0x2A;
//     gamelogic.m_cbAllCardData[2][8] = 0x2B;
//     gamelogic.m_cbAllCardData[2][9] = 0x14;
//     gamelogic.m_cbAllCardData[2][10] = 0x16;
//     gamelogic.m_cbAllCardData[2][11] = 0x17;
//     gamelogic.m_cbAllCardData[2][12] = 0x18;
//     gamelogic.m_cbAllCardData[2][13] = 0x1A;
//     gamelogic.m_cbAllCardData[2][14] = 0x1B;
//     gamelogic.m_cbAllCardData[2][15] = 0x1C;
//     gamelogic.m_cbAllCardData[2][16] = 0x1D;





    gamelogic.TestOutAllCard( 2, cbWantOutCardData, 4, false );
}

void Ctest3noLaiziDlg::OnBnClickedButton4()
{

    BYTE cbtemp1[] = {0x4f,0x4e,0x3d,0x2d,0x1d,0x0d,0x2c,0x2c,0x1c,0x0c};
    BYTE cbtemp1count = sizeof(cbtemp1);
    BYTE cbtemp2[] = {0x32,0x12,0x02,0x21,0x11,0x01,0x3d,0x1d,0x0d,0x3c,0x3c,0x0c,0x2b,0x0b,0x2a,0x0a,0x0a};
    BYTE cbtemp2count = sizeof(cbtemp2);
    BYTE cbtemp3[] = {0x32,0x22,0x12,0x02,0x31,0x21,0x11,0x01,0x3b,0x3b,0x2b,0x1b};
    BYTE cbtemp3count = sizeof(cbtemp3);

    memcpy(&gamelogic.m_cbAllCardData[0][0], &cbtemp1[0],cbtemp1count);
    gamelogic.m_cbUserCardCount[0] = cbtemp1count;
    memcpy(&gamelogic.m_cbAllCardData[1][0], &cbtemp2[0],cbtemp2count);
    gamelogic.m_cbUserCardCount[1] = cbtemp2count;
    memcpy(&gamelogic.m_cbAllCardData[2][0], &cbtemp3[0],cbtemp3count);
    gamelogic.m_cbUserCardCount[2] = cbtemp3count;



    gamelogic.m_wBankerUser = 2;
    gamelogic.m_lBankerOutCardCount =3;
    BYTE cbTurnCardData[] = {0x1b,0x0b,0x3a,0x2a,0x1a};
    BYTE cbHandCardData[] = {0x32,0x22,0x12,0x02,0x31,0x21,0x11,0x01,0x3b,0x3b,0x2b,0x1b};
    //---------m_cbUserCardCount[1][14],m_cbAllCardData[1][0x12,0x31,0x21,0x11,0x2c,0x2c,0x1c,0x3b,0x2b,0x1b,0x1b,0x0b,0x3a,0x2a]

    tagOutCardResult OutCardResult = {0};
    //gamelogic.BankerOutCard(cbHandCardData, sizeof(cbHandCardData), 1, cbTurnCardData, sizeof(cbTurnCardData), OutCardResult);
    //gamelogic.BankerOutCard(cbHandCardData, sizeof(cbHandCardData), OutCardResult);
    gamelogic.SearchOutCard(cbHandCardData, sizeof(cbHandCardData),cbTurnCardData,sizeof(cbTurnCardData),0,2,OutCardResult);
    
    tagOutCardTypeResult CardTypeResult[12+1] = {0};
    BYTE cbTurnCardData2[] = {0x3c,0x2c,0x1c,1};
    BYTE cbHandCardData2[] = {0x4f,0x4e,0x3a,0x2a,0x1a,0x1a};
    //gamelogic.AnalyseOutCardType(cbHandCardData2, sizeof(cbHandCardData2),cbTurnCardData2,sizeof(cbTurnCardData2),CardTypeResult);
}

void Ctest3noLaiziDlg::OnBnClickedButton5()
{
    tagOutCardTypeResult CardTypeResult[ (12+1) ] ;
    int a= sizeof(CardTypeResult);
    a= sizeof(tagOutCardTypeResult);
    a= sizeof(tagOutCardTypeResult)*13;
    ZeroMemory( CardTypeResult, sizeof( CardTypeResult ) ) ;


    bool b = 0;
    BYTE cbTurnCardData[] = {1,1,1,1,1,1,0xc,0xc,0xc,0xb,0xb,0xb,0xa,0xa,0xa};
    b = gamelogic.GetCardType(cbTurnCardData,sizeof(cbTurnCardData));

    BYTE cbTurnCardData2[] = {1,1,1,1,0xd,0xd,0xd,0xc,0xc,0xc,0xb,0xb,0xb,0xa,0xa,0xa};
    b = gamelogic.GetCardType(cbTurnCardData2,sizeof(cbTurnCardData2));

    BYTE cbTurnCardData3[] = {1,1,1,0xd,0xd,0xd,0xb,0xb,0xb,0xa,0xa,0xa};
    b = gamelogic.GetCardType(cbTurnCardData3,sizeof(cbTurnCardData3));


    BYTE cbHandCardData[] = {0x4f,0x4e,0x22,0x22,0x31,0x31,0x11,0x01,0x1d,0x2c,0x2c,0x1c,0x1c,0x0c,0x2b,0x2a,0x2a,0x1a};
    BYTE cbWantOutCardData[] = {0x2a,0x2a};

    BYTE count = gamelogic.AnalyseSinleCardCount(cbHandCardData,sizeof(cbHandCardData), cbWantOutCardData, sizeof(cbWantOutCardData));

}

#include "GameLogicold.h"
CGameLogicold gamelogicold;

void Ctest3noLaiziDlg::OnBnClickedButton6()
{
    /*
    [2015-06-29 15:24:06]: ------------SearchOutCard2 ----start----m_wBankerUser[2],wUndersideOfBanker[0],wUpsideOfBanker[1],cbTurnCardCount[2],wOutCardUser[2],wMeChairID[0],m_lBankerOutCardCount[0]
    [2015-06-29 15:24:06]: ------------SearchOutCard2 -------------m_cbUserCardCount[0][13],m_cbAllCardData[0][0x4f,0x32,0x22,0x02,0x31,0x21,0x01,0x3d,0x2d,0x3c,0x0c,0x3a,0x1a,0x0b,0x3a,0x1a,0x0a,0x2a,0x00,0x00]
    [2015-06-29 15:24:06]: ------------SearchOutCard2 -------------m_cbUserCardCount[1][17],m_cbAllCardData[1][0x4e,0x12,0x31,0x11,0x01,0x0d,0x3c,0x2c,0x1c,0x1c,0x0c,0x3b,0x2b,0x1b,0x1b,0x3a,0x0a,0x00,0x00,0x00]
    [2015-06-29 15:24:06]: ------------SearchOutCard2 -------------m_cbUserCardCount[2][8],m_cbAllCardData[2][0x4f,0x4f,0x4e,0x32,0x22,0x12,0x02,0x2c,0x11,0x2c,0x2d,0x1d,0x1d,0x2c,0x2b,0x2a,0x2a,0x1a,0x00,0x00]
    [2015-06-29 15:24:06]: ---------------UndersideOfBankerOutCard2 ----start----cbHandCardCount[13],cbHandCardData[0x4f,0x32,0x22,0x02,0x31,0x21,0x01,0x3d,0x2d,0x3c,0x0c,0x3a,0x1a,0x0b,0x3a,0x1a,0x0a,0x00,0x00,0x00]
    [2015-06-29 15:24:06]: ---------------UndersideOfBankerOutCard2 -------------cbTurnCardCount[2],cbTurnCardData[0x21,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    [2015-06-29 15:24:06]: ---------------UndersideOfBankerOutCard2 line[7688]-------------OutCardResult.cbCardCount[2],OutCardResult.cbResultCard[0x32,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    [2015-06-29 15:24:06]: ------------SearchOutCard2 -------end---OutCardResult[2][0x32,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3a0e18c,0x3a0dfe0,0x43cd90,0xcccccccc,0xcccccccc,0xcccccccc,0xcccccccc,0xcccccccc,0xcccccccc]
    */

    BYTE cbtemp1[] = {0x4f,0x4e,0x3d,0x2d,0x1d,0x0d,0x2c,0x2c,0x1c,0x0c};
    BYTE cbtemp1count = sizeof(cbtemp1);
    BYTE cbtemp2[] = {0x32,0x12,0x02,0x21,0x11,0x01,0x3d,0x1d,0x0d,0x3c,0x3c,0x0c,0x2b,0x0b,0x2a,0x0a,0x0a};
    BYTE cbtemp2count = sizeof(cbtemp2);
    BYTE cbtemp3[] = {0x32,0x22,0x12,0x02,0x31,0x21,0x11,0x01,0x3b,0x3b,0x2b,0x1b};
    BYTE cbtemp3count = sizeof(cbtemp3);

    memcpy(&gamelogicold.m_cbAllCardData[0][0], &cbtemp1[0],cbtemp1count);
    gamelogicold.m_cbUserCardCount[0] = cbtemp1count;
    memcpy(&gamelogicold.m_cbAllCardData[1][0], &cbtemp2[0],cbtemp2count);
    gamelogicold.m_cbUserCardCount[1] = cbtemp2count;
    memcpy(&gamelogicold.m_cbAllCardData[2][0], &cbtemp3[0],cbtemp3count);
    gamelogicold.m_cbUserCardCount[2] = cbtemp3count;



    gamelogic.m_wBankerUser = 2;
    gamelogic.m_lBankerOutCardCount =3;
    BYTE cbTurnCardData[] = {0x1b,0x0b,0x3a,0x2a,0x1a};
    BYTE cbHandCardData[] = {0x32,0x22,0x12,0x02,0x31,0x21,0x11,0x01,0x3b,0x3b,0x2b,0x1b};
    //---------m_cbUserCardCount[1][14],m_cbAllCardData[1][0x12,0x31,0x21,0x11,0x2c,0x2c,0x1c,0x3b,0x2b,0x1b,0x1b,0x0b,0x3a,0x2a]

    tagOutCardResult OutCardResult = {0};
    //gamelogic.BankerOutCard(cbHandCardData, sizeof(cbHandCardData), 1, cbTurnCardData, sizeof(cbTurnCardData), OutCardResult);
    //gamelogic.BankerOutCard(cbHandCardData, sizeof(cbHandCardData), OutCardResult);
    gamelogicold.SearchOutCard(cbHandCardData, sizeof(cbHandCardData),cbTurnCardData,0/*sizeof(cbTurnCardData)*/,1,1,OutCardResult);
}
