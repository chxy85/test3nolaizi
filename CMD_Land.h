#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

#pragma pack(push)
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����궨��
#ifdef VIDEO_GAME
#define KIND_ID						2									//��Ϸ I D
#else
#define KIND_ID						2									//��Ϸ I D
#endif

#define GAME_PLAYER					3									//��Ϸ����
#define GAME_NAME					TEXT("��Ӷ�����")						//��Ϸ����
#define INVALID_BYTE				((BYTE)(0xFF))						//��Ч��ֵ
#define INVALID_WORD				((WORD)(0xFFFF))					//��Ч��ֵ
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//��Ч��ֵ

//////////////////////////////////////////////////////////////////////////
//���ó���

//��Ч��ֵ
#define INVALID_TABLE				INVALID_WORD						//��Ч����
#define INVALID_CHAIR				INVALID_WORD						//��Ч����

//////////////////////////////////////////////////////////////////////////

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//��Ч��ַ
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////

//�洢����
#ifdef _UNICODE
#define CountStringBuffer CountStringBufferW
#else
#define CountStringBuffer CountStringBufferA
#endif

//�洢����
#define CountStringBufferA(String) ((UINT)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((UINT)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////

//�ӿ��ͷ�
#define  SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//ɾ��ָ��
#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//ɾ������

//////////////////////////////////////////////////////////////////////////
//��Ŀ����

#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ
#define BACK_CARD_COUNT             3


//��Ϸ״̬
#define GS_WK_FREE					GS_FREE								//�ȴ���ʼ
#define GS_WK_SCORE					GS_PLAYING							//�з�״̬
#define GS_WK_DOUBLE_SCORE			GS_PLAYING+1						//�ӱ�״̬
#define GS_WK_PLAYING				GS_PLAYING+2						//��Ϸ����

//�׶α���
#define CSD_NORMAL		0			//�е���
#define CSD_SNATCHLAND	1           //������
#define CSD_BRIGHTCARD	2			//����
#define CSD_GAMESTART	3			//��ʼ

//��������
#define BRIGHT_START_TIME				5									//���ƿ�ʼ
#define BRIGHT_CARD_TIME				5									//���Ʊ���
#define SPRING_CARD_TIME                4                                   /* spring */
#define DECLARE_LAND_TIME               3
#define SNATCH_LAND_TIME                2
#define ADD_TIME                        2
#define VIP_ADD_TIME                    5
#define BOMB_TIME                       2

#define GOODS_TYPE_NUM                  10                                  //��������

typedef enum enRandCardType{
    enRandCard_Normal,
    enRandCard_DoubleGoodCard,
    enRandCard_MAX
};

//////////////////////////////////////////////////////////////////////////
//����������ṹ

#define SUB_S_SEND_CARD				100									//��������
#define SUB_S_LAND_SCORE			101									//�з�����
#define SUB_S_GAME_START			102									//��Ϸ��ʼ
#define SUB_S_OUT_CARD				103									//�û�����
#define SUB_S_PASS_CARD				104									//��������
#define SUB_S_GAME_END				105									//��Ϸ����
#define SUB_S_BRIGHT_START			106									//���ƿ�ʼ
#define SUB_S_BRIGHT_CARD			107									//�������
#define SUB_S_DOUBLE_SCORE			108									//�ӱ�����
#define SUB_S_USER_DOUBLE			109									//�ӱ�����
#define SUB_S_RECORDCARD            110                                 //����
#define SUB_S_QUERY_TIME            111                                 //����ʱ��
#define SUB_S_TASK_INFO             112                                 //������Ϣ
#define SUB_S_NOTIFY_TASK_TIMES     113                                 //֪ͨ�ͻ���������
#define SUB_S_BET_BACK_CARD         114                                 //Ѻ������Ӧ��Ϣ
#define SUB_S_BET_BACK_CARD_RESULT  115                                 //Ѻ���ƽ��
#define SUB_S_DOUBLE_LANDER_BACK_CARD  116                              //�����ƺ��Ƶ�����õĵ���֪ͨ

#define SUB_S_EXPRESSION   			127									//���͵��� expression

//����
#define SUB_S_GOODS_TYPE                200                             //��������
#define SUB_S_SEND_GOODS                201                             //���͵���

//ActionType���Ӻͼ���(�����͸���)  DepleteType���������ģ�����Ϊ������

//���ߵ�����
enum enGoodsType
{
    enGoodsType_Null,               //ɶ��û��
    enGoodsType_YuJi,               //�¼�
    enGoodsType_MeiGui,             //õ��
    enGoodsType_NiuSi,              //ţʺ
    enGoodsType_XiNiuSi,            //Ϭţʺ
    enGoodsType_JiDan,              //����
    enGoodsType_TangGuo,            //�ǹ�
    enGoodsType_PiJiu,              //ơ��
    enGoodsType_FeiWen,             //����
    enGoodsType_Bugle,              //����
};

//���͵õ�����
enum enGoodsActionType
{
    enGoodsActionType_Null,             //ɶ��û��
    enGoodsActionType_Loveliness,       //����
    enGoodsActionType_Ingon,            //Ԫ��
    enGoodsActionType_Jetton,           //����
    enGoodsActionType_Power,            //������
    enGoodsActionType_Exp,              //����
};

//��������
enum enGoodsDepleteType
{
    enGoodsDepleteType_Null,            //ɶ��û��
    enGoodsDepleteType_Loveliness,      //����
    enGoodsDepleteType_Ingon,           //Ԫ��
    enGoodsDepleteType_Jetton,          //����
    enGoodsDepleteType_Power,           //������
    enGoodsDepleteType_Exp,             //����
};

//��������
enum enGoodsLimitType
{
    enGoodsLimitType_Null,              //ɶ��û��
    enGoodsLimitType_Loveliness,        //����
    enGoodsLimitType_Ingon,             //Ԫ��
    enGoodsLimitType_Jetton,            //����
    enGoodsLimitType_Power,             //������
    enGoodsLimitType_Exp,               //����
};

enum enTaskType{
    enTaskType_LastA,                   //���һ�ų�A
    enTaskType_Single,                  //���� 2
    enTaskType_Couple,                  //���� 4
    enTaskType_Triple,                  //���� 8
    enTaskType_Sequence,                //˳�� 16
    enTaskType_SequenceCouple,          //���� 20
    enTaskType_Jet,                     //�ɻ� 32
    enTaskType_Over8Sequence,           //��������˳�� 40 
    enTaskType_FourWithTwo,             //�Ĵ��� 64
    enTaskType_MAX
};

enum enTaskItemType{
    enTaskItemOperateType_Add,
    enTaskItemOperateType_Multiply
};

struct CMD_S_DoubleLanderBackCard 
{
    BYTE    bIndex;                         //����
    BYTE    bCardData;                      //��������
};

struct CMD_S_BetCard_Result
{
    BYTE    bResult;                    //Ѻ�еĴ�Сֵ
    LONG    lScore;                     //Ӯ��Ǯ
};

struct CMD_S_RecordCards
{
    BYTE    cbOutCards[FULL_COUNT];         //��������
    BYTE    cbCounts;                       //���Ƶ�����
};

//ͨ����Ӧ��Ϣ�ṹ
struct CMD_S_CommonResp
{
    BYTE    errCD;                          //ͨ����Ϣ��Ӧ������ 0�ɹ� ��0ʧ��
};

//��������Ϣ
struct CMD_S_TaskTimes
{
    WORD    wTaskAddTimes;
    WORD    wTaskMultiTimes;
};

//���߲���
struct CMD_S_GoodsType
{
    BYTE            bGoodsType;         //��������

    BYTE            bActionType;        //���ͻ������
    LONG            nActionNum;         //���ͻ������

    BYTE            bLimitType;         //����ʹ������
    LONG            nLimitNum;          //����ʹ������

    BYTE            bDepleteType;       //��������
    LONG            nDepleteNum;        //��������
};

//���߲���
struct CMD_S_SendGoods
{
    BYTE            bGoodsType;         //��������
    LONG            nGoosTypeNum;       //���͵�������
    LONG            nSendUserID;        //�͵���ID
    DWORD           wSendIndex;         //�͵���
    LONG            nRecvUserID;        //�յ���ID
    DWORD           wRecvIndex;         //�յ���
};

//����
struct CMD_S_Expression
{
    BYTE								cbChairId;							// ���Ӻ�
    BYTE								cbType;								// �ڼ��ױ���
    BYTE								cbIndex;							// �ڼ�������
};

//��Ϸ״̬
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//��������
	bool							bBrightStart[GAME_PLAYER];			//�������
	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ״̬
struct CMD_S_StatusScore
{
	WORD							bLandScore;							//��������
	LONG							lBaseScore;							//��������
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							bScoreInfo[3];						//�з���Ϣ
	BYTE							bCardData[3][20];					//�����˿�
	bool							bUserTrustee[GAME_PLAYER];			//����й�
	BYTE							bCallScorePhase;					//���ƽ׶�
	BYTE							bBrightTime;						//���Ʊ���
	bool							bUserBrightCard[GAME_PLAYER];		//�������
    BYTE							bBackCard[BACK_CARD_COUNT];			//�����˿�
	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
    WORD							wLandUser;							//�������
};

//�ӱ�״̬
struct CMD_S_StatusDoubleScore
{
	WORD							wLandUser;							//�������
	LONG							lBaseScore;							//��������
	WORD							bLandScore;							//��������
	BYTE							bBackCard[3];						//�����˿�
	BYTE							bCardData[3][20];					//�����˿�
	BYTE							bCardCount[3];						//�˿���Ŀ
	bool							bUserTrustee[GAME_PLAYER];			//����й�
	bool							bAllowDouble;						//����ӱ�
	bool							bDoubleUser[GAME_PLAYER];			//�ӱ����
	bool							bUserBrightCard[GAME_PLAYER];		//�������
	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//��Ϸ״̬
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//�������
	WORD							wBombTime;							//ը������
	LONG							lBaseScore;							//��������
	WORD							bLandScore;							//��������
	WORD							wLastOutUser;						//���Ƶ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							bBackCard[3];						//�����˿�
	BYTE							bCardData[3][20];					//�����˿�
	BYTE							bCardCount[3];						//�˿���Ŀ
	BYTE							bTurnCardCount;						//��������
	BYTE							bTurnCardData[20];					//�����б�
	bool							bUserTrustee[GAME_PLAYER];			//����й�
	BYTE							bBrightTime;						//���Ʊ���
	bool							bUserBrightCard[GAME_PLAYER];		//�������
	//��ʷ����
	LONGLONG						lTurnScore[GAME_PLAYER];			//������Ϣ
	LONGLONG						lCollectScore[GAME_PLAYER];			//������Ϣ
};

//�����˿�
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							bCardData[17];						//�˿��б�
};

//�����˿�
struct CMD_S_SendAllCard
{
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							bCardData[GAME_PLAYER][20];			//�˿��б�
	BYTE							bBackCardData[3];					//�����˿�
	bool							bUserBrightCard[GAME_PLAYER];		//�������
    BYTE							bLaiZiCard;							//������
};


//�û��з�
struct CMD_S_LandScore
{
	WORD							bLandUser;							//�з����
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							bLandScore;							//�ϴνз�
	WORD							bCurrentScore;						//��ǰ�з�
	BYTE							bPreCallScorePhase;					//֮ǰ�׶�
	BYTE							bCallScorePhase;					//���ƽ׶�
	bool							bBrightCardUser[GAME_PLAYER];		//�������
	BYTE							bBackCardData[3];					//�����˿�
    WORD                            wBankerUser;                        /*�������*/
};

//��Ϸ��ʼ
struct CMD_S_GameStart
{
	WORD				 			wLandUser;							//�������
	WORD							bLandScore;							//��������
	WORD				 			wCurrentUser;						//��ǰ���	
	BYTE							bBrightCard;						//���Ʊ�־
	bool							bUserBrightCard[GAME_PLAYER];		//�������
	BYTE							bBackCardData[3];		        	//�����˿�
};

//�û�����
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//������Ŀ
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wOutCardUser;						//�������
	BYTE							bCardData[20];						//�˿��б�
};

//��������
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//һ�ֿ�ʼ
	WORD				 			wPassUser;							//�������
	WORD				 			wCurrentUser;						//��ǰ���
};

//��Ϸ����
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//��Ϸ˰��
	LONG							lGameScore[3];						//��Ϸ����
	BYTE							bCardCount[3];						//�˿���Ŀ
	BYTE							bCardData[54];						//�˿��б� 
};

//���ƿ�ʼ
struct CMD_S_BrightStart
{
	WORD							wBrightUser;						//�������
	bool							bBright;							//���Ʊ�ʶ
};

//�������
struct CMD_S_BrightCard
{
	WORD							wBrightUser;						//�������
	BYTE							cbBrightTime;						//���Ʊ���
	BYTE							cbCurrenBrightTime;					//��ǰ����
	BYTE							cbCallScorePhase;					//�зֽ׶�
	BYTE							bCardData[20];						//�˿��б�  ���ƺ�Ż���Ʒ����û�
};

//�ӱ��ṹ
struct CMD_S_DoubleScore
{
	bool							bAllowDouble;						//����ӱ�
	BYTE							bBackCard[3];						//�����˿�
	WORD				 			wLandUser;							//�������
	WORD							bCurrentScore;						//��ǰ�з�
};

//�ӱ��ṹ
struct CMD_S_UserDouble
{
	WORD							wDoubleUser;						//�ӱ����
	bool							bDoubleScore;						//�Ƿ�ӱ�
	WORD							bCurrentScore;						//��ǰ�з�
};

struct tagTaskItem{
    BYTE    bOperateType;                               //�� ��
    BYTE    bTaskType;                                  //��������
    BYTE    bTaskValue;                                 //������
    BYTE    bTaskReserved;                              //�����ֶ� ���һ�ų������� ��ֵ
};

struct CMD_S_TaskInfo{
    BYTE            bCount;
    tagTaskItem     TaskInfo[enTaskType_MAX];
};

//////////////////////////////////////////////////////////////////////////
//�ͻ�������ṹ

#define SUB_C_LAND_SCORE			1									//�û��з�
#define SUB_C_OUT_CART				2									//�û�����
#define SUB_C_PASS_CARD				3									//��������
#define SUB_C_TRUSTEE				4									//�й���Ϣ
#define SUB_C_BRIGHT_START			5									//���ƿ�ʼ
#define SUB_C_BRIGHT				6									//�������
#define SUB_C_DOUBLE_SCORE			7									//�ӱ�����
#define SUB_C_YUYAN_SELECT			8									//����ѡ��
#define SUB_CHANGE_DESK             9                                   //����

#define SUB_C_SENDGOODS             10                               //���͵���
#define SUB_C_GOODTIME_TYPE         11                               //���ߺ�VIP����

#define SUB_C_EXPRESSION   			13									//����

#define SUB_C_RECORDCARD            14                                  //�������
#define SUB_C_QUERY_TIME            15                                  //���������ʱ��

#define SUB_C_BET_BACK_CARD         16                                  //�µ���

#define SUB_C_CHANGE_TASK           17                                  //��������

#define SUB_C_DOUBLE_GOOD_CARD_START 18                                 //�Ƿ������ƿ�ʼ


enum enBetBackCardType{
    enBetCardType_Small = 1,
    enBetCardType_Even,
    EnbetCardType_Big
};

struct CMD_C_BetBackCard
{
    BYTE    bType;                                                      //С 1 ƽ 2 �� 3
    BYTE    bTimes;                                                     //��ע�ı���
};

//���ߵ�����
enum enGoodTimeType
{
    //������ʹ�õ�Time�ĵ�һ���ֶ�  ���ð��µ����
    GoodTimeType_BuyHistory_day=1,      //������ʷ��¼ 1��      2��Ԫ��
    GoodTimeType_BuyHistory_week,       //������ʷ��¼ 7��      12��Ԫ��
    GoodTimeType_BuyHistory_month,      //������ʷ��¼ 30��     48��Ԫ��
    //-----------------------------------------------------------------
};

struct CMD_C_GoodTimeType
{
    WORD wGoodType;
};

//���߲���
struct CMD_C_SendGoods
{
    BYTE            bGoodsType;         //��������
    LONG            nGoosTypeNum;       //���͵�������
    WORD            wSendIndex;        //�͵���
    WORD            wRecvIndex;        //�յ���
};

//����
struct CMD_C_Expression
{
    BYTE								cbType;							//��һ�ױ���
    BYTE								cbIndex;						//�ڼ�������
};

//�йܽṹ
struct CMD_C_UserTrustee {
	WORD							wUserChairID;						//�������
	bool							bTrustee;							//�йܱ�ʶ
};
//����
struct CMD_C_ChangeDesk
{
	WORD                             wChangeUserID;
};

//�û��з�
struct CMD_C_LandScore
{
	WORD							bLandScore;							//��������
};

//�������ݰ�
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//������Ŀ
	BYTE							bCardData[20];						//�˿��б�
};

//����ѡ�����ݱ�
struct CMD_C_YuYanSelect
{
	BYTE							cbSelectIndex;						//ѡ������
	WORD							wSelectUser;						//�����û�
};

//�������
struct CMD_C_BrightCard
{
	BYTE							cbBrightCardTime;					//���Ʊ���
};

//��Ҽӱ�
struct CMD_C_DoubleScore
{
	bool							bDoubleScore;						//��Ҽӱ�
	BYTE                            cbDoubleScore;                      //��Ҽӱ��ı���
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif