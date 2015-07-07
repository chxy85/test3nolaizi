#ifndef CMD_LAND_HEAD_FILE
#define CMD_LAND_HEAD_FILE

#pragma pack(push)
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义
#ifdef VIDEO_GAME
#define KIND_ID						2									//游戏 I D
#else
#define KIND_ID						2									//游戏 I D
#endif

#define GAME_PLAYER					3									//游戏人数
#define GAME_NAME					TEXT("癞子斗地主")						//游戏名字
#define INVALID_BYTE				((BYTE)(0xFF))						//无效数值
#define INVALID_WORD				((WORD)(0xFFFF))					//无效数值
#define INVALID_DWORD				((DWORD)(0xFFFFFFFF))				//无效数值

//////////////////////////////////////////////////////////////////////////
//常用常量

//无效数值
#define INVALID_TABLE				INVALID_WORD						//无效桌子
#define INVALID_CHAIR				INVALID_WORD						//无效椅子

//////////////////////////////////////////////////////////////////////////

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//无效地址
#define INVALID_IP_ADDRESS(IPAddress) (((IPAddress==0L)||(IPAddress==INADDR_NONE)))

//////////////////////////////////////////////////////////////////////////

//存储长度
#ifdef _UNICODE
#define CountStringBuffer CountStringBufferW
#else
#define CountStringBuffer CountStringBufferA
#endif

//存储长度
#define CountStringBufferA(String) ((UINT)((lstrlenA(String)+1)*sizeof(CHAR)))
#define CountStringBufferW(String) ((UINT)((lstrlenW(String)+1)*sizeof(WCHAR)))

//////////////////////////////////////////////////////////////////////////

//接口释放
#define  SafeRelease(pObject) { if (pObject!=NULL) { pObject->Release(); pObject=NULL; } }

//删除指针
#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

//删除数组

//////////////////////////////////////////////////////////////////////////
//数目定义

#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目
#define BACK_CARD_COUNT             3


//游戏状态
#define GS_WK_FREE					GS_FREE								//等待开始
#define GS_WK_SCORE					GS_PLAYING							//叫分状态
#define GS_WK_DOUBLE_SCORE			GS_PLAYING+1						//加倍状态
#define GS_WK_PLAYING				GS_PLAYING+2						//游戏进行

//阶段变量
#define CSD_NORMAL		0			//叫地主
#define CSD_SNATCHLAND	1           //抢地主
#define CSD_BRIGHTCARD	2			//明牌
#define CSD_GAMESTART	3			//开始

//倍数变量
#define BRIGHT_START_TIME				5									//明牌开始
#define BRIGHT_CARD_TIME				5									//明牌倍数
#define SPRING_CARD_TIME                4                                   /* spring */
#define DECLARE_LAND_TIME               3
#define SNATCH_LAND_TIME                2
#define ADD_TIME                        2
#define VIP_ADD_TIME                    5
#define BOMB_TIME                       2

#define GOODS_TYPE_NUM                  10                                  //道具数量

typedef enum enRandCardType{
    enRandCard_Normal,
    enRandCard_DoubleGoodCard,
    enRandCard_MAX
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_SEND_CARD				100									//发牌命令
#define SUB_S_LAND_SCORE			101									//叫分命令
#define SUB_S_GAME_START			102									//游戏开始
#define SUB_S_OUT_CARD				103									//用户出牌
#define SUB_S_PASS_CARD				104									//放弃出牌
#define SUB_S_GAME_END				105									//游戏结束
#define SUB_S_BRIGHT_START			106									//明牌开始
#define SUB_S_BRIGHT_CARD			107									//玩家明牌
#define SUB_S_DOUBLE_SCORE			108									//加倍命令
#define SUB_S_USER_DOUBLE			109									//加倍命令
#define SUB_S_RECORDCARD            110                                 //记牌
#define SUB_S_QUERY_TIME            111                                 //返回时间
#define SUB_S_TASK_INFO             112                                 //任务信息
#define SUB_S_NOTIFY_TASK_TIMES     113                                 //通知客户端任务倍数
#define SUB_S_BET_BACK_CARD         114                                 //押底牌响应消息
#define SUB_S_BET_BACK_CARD_RESULT  115                                 //押底牌结果
#define SUB_S_DOUBLE_LANDER_BACK_CARD  116                              //两副牌好牌地主获得的底牌通知

#define SUB_S_EXPRESSION   			127									//赠送道具 expression

//道具
#define SUB_S_GOODS_TYPE                200                             //道具类型
#define SUB_S_SEND_GOODS                201                             //赠送道具

//ActionType增加和减少(正数和负数)  DepleteType必须是消耗（必须为正数）

//道具的类型
enum enGoodsType
{
    enGoodsType_Null,               //啥都没有
    enGoodsType_YuJi,               //月季
    enGoodsType_MeiGui,             //玫瑰
    enGoodsType_NiuSi,              //牛屎
    enGoodsType_XiNiuSi,            //犀牛屎
    enGoodsType_JiDan,              //鸡蛋
    enGoodsType_TangGuo,            //糖果
    enGoodsType_PiJiu,              //啤酒
    enGoodsType_FeiWen,             //飞吻
    enGoodsType_Bugle,              //喇叭
};

//赠送得到类型
enum enGoodsActionType
{
    enGoodsActionType_Null,             //啥都没有
    enGoodsActionType_Loveliness,       //魅力
    enGoodsActionType_Ingon,            //元宝
    enGoodsActionType_Jetton,           //筹码
    enGoodsActionType_Power,            //能力槽
    enGoodsActionType_Exp,              //经验
};

//消耗类型
enum enGoodsDepleteType
{
    enGoodsDepleteType_Null,            //啥都没有
    enGoodsDepleteType_Loveliness,      //魅力
    enGoodsDepleteType_Ingon,           //元宝
    enGoodsDepleteType_Jetton,          //筹码
    enGoodsDepleteType_Power,           //能力槽
    enGoodsDepleteType_Exp,             //经验
};

//限制类型
enum enGoodsLimitType
{
    enGoodsLimitType_Null,              //啥都没有
    enGoodsLimitType_Loveliness,        //魅力
    enGoodsLimitType_Ingon,             //元宝
    enGoodsLimitType_Jetton,            //筹码
    enGoodsLimitType_Power,             //能力槽
    enGoodsLimitType_Exp,               //经验
};

enum enTaskType{
    enTaskType_LastA,                   //最后一张出A
    enTaskType_Single,                  //单张 2
    enTaskType_Couple,                  //对子 4
    enTaskType_Triple,                  //三张 8
    enTaskType_Sequence,                //顺子 16
    enTaskType_SequenceCouple,          //连对 20
    enTaskType_Jet,                     //飞机 32
    enTaskType_Over8Sequence,           //八张以上顺子 40 
    enTaskType_FourWithTwo,             //四带二 64
    enTaskType_MAX
};

enum enTaskItemType{
    enTaskItemOperateType_Add,
    enTaskItemOperateType_Multiply
};

struct CMD_S_DoubleLanderBackCard 
{
    BYTE    bIndex;                         //索引
    BYTE    bCardData;                      //底牌数据
};

struct CMD_S_BetCard_Result
{
    BYTE    bResult;                    //押中的大小值
    LONG    lScore;                     //赢的钱
};

struct CMD_S_RecordCards
{
    BYTE    cbOutCards[FULL_COUNT];         //出掉的牌
    BYTE    cbCounts;                       //出牌的总数
};

//通用响应消息结构
struct CMD_S_CommonResp
{
    BYTE    errCD;                          //通用消息响应错误码 0成功 非0失败
};

//任务倍数信息
struct CMD_S_TaskTimes
{
    WORD    wTaskAddTimes;
    WORD    wTaskMultiTimes;
};

//道具参数
struct CMD_S_GoodsType
{
    BYTE            bGoodsType;         //道具类型

    BYTE            bActionType;        //赠送获得类型
    LONG            nActionNum;         //赠送获得数量

    BYTE            bLimitType;         //限制使用类型
    LONG            nLimitNum;          //限制使用数量

    BYTE            bDepleteType;       //消耗类型
    LONG            nDepleteNum;        //消耗数量
};

//道具参数
struct CMD_S_SendGoods
{
    BYTE            bGoodsType;         //道具类型
    LONG            nGoosTypeNum;       //赠送道具数量
    LONG            nSendUserID;        //送的人ID
    DWORD           wSendIndex;         //送的人
    LONG            nRecvUserID;        //收的人ID
    DWORD           wRecvIndex;         //收的人
};

//表情
struct CMD_S_Expression
{
    BYTE								cbChairId;							// 椅子号
    BYTE								cbType;								// 第几套表情
    BYTE								cbIndex;							// 第几个表情
};

//游戏状态
struct CMD_S_StatusFree
{
	LONG							lBaseScore;							//基础积分
	bool							bBrightStart[GAME_PLAYER];			//玩家明牌
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusScore
{
	WORD							bLandScore;							//地主分数
	LONG							lBaseScore;							//基础积分
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bScoreInfo[3];						//叫分信息
	BYTE							bCardData[3][20];					//手上扑克
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bCallScorePhase;					//叫牌阶段
	BYTE							bBrightTime;						//明牌倍数
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
    BYTE							bBackCard[BACK_CARD_COUNT];			//底牌扑克
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
    WORD							wLandUser;							//坑主玩家
};

//加倍状态
struct CMD_S_StatusDoubleScore
{
	WORD							wLandUser;							//坑主玩家
	LONG							lBaseScore;							//基础积分
	WORD							bLandScore;							//地主分数
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[3][20];					//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	bool							bAllowDouble;						//允许加倍
	bool							bDoubleUser[GAME_PLAYER];			//加倍玩家
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//游戏状态
struct CMD_S_StatusPlay
{
	WORD							wLandUser;							//坑主玩家
	WORD							wBombTime;							//炸弹倍数
	LONG							lBaseScore;							//基础积分
	WORD							bLandScore;							//地主分数
	WORD							wLastOutUser;						//出牌的人
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bBackCard[3];						//底牌扑克
	BYTE							bCardData[3][20];					//手上扑克
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bTurnCardCount;						//基础出牌
	BYTE							bTurnCardData[20];					//出牌列表
	bool							bUserTrustee[GAME_PLAYER];			//玩家托管
	BYTE							bBrightTime;						//明牌倍数
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
	//历史积分
	LONGLONG						lTurnScore[GAME_PLAYER];			//积分信息
	LONGLONG						lCollectScore[GAME_PLAYER];			//积分信息
};

//发送扑克
struct CMD_S_SendCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[17];						//扑克列表
};

//发送扑克
struct CMD_S_SendAllCard
{
	WORD				 			wCurrentUser;						//当前玩家
	BYTE							bCardData[GAME_PLAYER][20];			//扑克列表
	BYTE							bBackCardData[3];					//底牌扑克
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
    BYTE							bLaiZiCard;							//赖子牌
};


//用户叫分
struct CMD_S_LandScore
{
	WORD							bLandUser;							//叫分玩家
	WORD				 			wCurrentUser;						//当前玩家
	WORD							bLandScore;							//上次叫分
	WORD							bCurrentScore;						//当前叫分
	BYTE							bPreCallScorePhase;					//之前阶段
	BYTE							bCallScorePhase;					//叫牌阶段
	bool							bBrightCardUser[GAME_PLAYER];		//明牌玩家
	BYTE							bBackCardData[3];					//底牌扑克
    WORD                            wBankerUser;                        /*地主玩家*/
};

//游戏开始
struct CMD_S_GameStart
{
	WORD				 			wLandUser;							//地主玩家
	WORD							bLandScore;							//地主分数
	WORD				 			wCurrentUser;						//当前玩家	
	BYTE							bBrightCard;						//明牌标志
	bool							bUserBrightCard[GAME_PLAYER];		//玩家亮牌
	BYTE							bBackCardData[3];		        	//底牌扑克
};

//用户出牌
struct CMD_S_OutCard
{
	BYTE							bCardCount;							//出牌数目
	WORD				 			wCurrentUser;						//当前玩家
	WORD							wOutCardUser;						//出牌玩家
	BYTE							bCardData[20];						//扑克列表
};

//放弃出牌
struct CMD_S_PassCard
{
	BYTE							bNewTurn;							//一轮开始
	WORD				 			wPassUser;							//放弃玩家
	WORD				 			wCurrentUser;						//当前玩家
};

//游戏结束
struct CMD_S_GameEnd
{
	LONG							lGameTax;							//游戏税收
	LONG							lGameScore[3];						//游戏积分
	BYTE							bCardCount[3];						//扑克数目
	BYTE							bCardData[54];						//扑克列表 
};

//明牌开始
struct CMD_S_BrightStart
{
	WORD							wBrightUser;						//明牌玩家
	bool							bBright;							//明牌标识
};

//玩家明牌
struct CMD_S_BrightCard
{
	WORD							wBrightUser;						//明牌玩家
	BYTE							cbBrightTime;						//明牌倍数
	BYTE							cbCurrenBrightTime;					//当前倍数
	BYTE							cbCallScorePhase;					//叫分阶段
	BYTE							bCardData[20];						//扑克列表  明牌后才会把牌发给用户
};

//加倍结构
struct CMD_S_DoubleScore
{
	bool							bAllowDouble;						//允许加倍
	BYTE							bBackCard[3];						//底牌扑克
	WORD				 			wLandUser;							//地主玩家
	WORD							bCurrentScore;						//当前叫分
};

//加倍结构
struct CMD_S_UserDouble
{
	WORD							wDoubleUser;						//加倍玩家
	bool							bDoubleScore;						//是否加倍
	WORD							bCurrentScore;						//当前叫分
};

struct tagTaskItem{
    BYTE    bOperateType;                               //加 乘
    BYTE    bTaskType;                                  //任务类型
    BYTE    bTaskValue;                                 //任务倍数
    BYTE    bTaskReserved;                              //保留字段 最后一张出牌任务 牌值
};

struct CMD_S_TaskInfo{
    BYTE            bCount;
    tagTaskItem     TaskInfo[enTaskType_MAX];
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_LAND_SCORE			1									//用户叫分
#define SUB_C_OUT_CART				2									//用户出牌
#define SUB_C_PASS_CARD				3									//放弃出牌
#define SUB_C_TRUSTEE				4									//托管消息
#define SUB_C_BRIGHT_START			5									//明牌开始
#define SUB_C_BRIGHT				6									//玩家明牌
#define SUB_C_DOUBLE_SCORE			7									//加倍命令
#define SUB_C_YUYAN_SELECT			8									//语言选择
#define SUB_CHANGE_DESK             9                                   //换桌

#define SUB_C_SENDGOODS             10                               //赠送道具
#define SUB_C_GOODTIME_TYPE         11                               //道具和VIP购买

#define SUB_C_EXPRESSION   			13									//表情

#define SUB_C_RECORDCARD            14                                  //请求记牌
#define SUB_C_QUERY_TIME            15                                  //请求服务器时间

#define SUB_C_BET_BACK_CARD         16                                  //猜底牌

#define SUB_C_CHANGE_TASK           17                                  //更换任务

#define SUB_C_DOUBLE_GOOD_CARD_START 18                                 //是否两幅牌开始


enum enBetBackCardType{
    enBetCardType_Small = 1,
    enBetCardType_Even,
    EnbetCardType_Big
};

struct CMD_C_BetBackCard
{
    BYTE    bType;                                                      //小 1 平 2 大 3
    BYTE    bTimes;                                                     //底注的倍数
};

//道具的类型
enum enGoodTimeType
{
    //这三个使用的Time的第一个字段  采用包月的情况
    GoodTimeType_BuyHistory_day=1,      //购买历史记录 1天      2个元宝
    GoodTimeType_BuyHistory_week,       //购买历史记录 7天      12个元宝
    GoodTimeType_BuyHistory_month,      //购买历史记录 30天     48个元宝
    //-----------------------------------------------------------------
};

struct CMD_C_GoodTimeType
{
    WORD wGoodType;
};

//道具参数
struct CMD_C_SendGoods
{
    BYTE            bGoodsType;         //道具类型
    LONG            nGoosTypeNum;       //赠送道具数量
    WORD            wSendIndex;        //送的人
    WORD            wRecvIndex;        //收的人
};

//表情
struct CMD_C_Expression
{
    BYTE								cbType;							//第一套表情
    BYTE								cbIndex;						//第几个表情
};

//托管结构
struct CMD_C_UserTrustee {
	WORD							wUserChairID;						//玩家椅子
	bool							bTrustee;							//托管标识
};
//换桌
struct CMD_C_ChangeDesk
{
	WORD                             wChangeUserID;
};

//用户叫分
struct CMD_C_LandScore
{
	WORD							bLandScore;							//地主分数
};

//出牌数据包
struct CMD_C_OutCard
{
	BYTE							bCardCount;							//出牌数目
	BYTE							bCardData[20];						//扑克列表
};

//语言选择数据表
struct CMD_C_YuYanSelect
{
	BYTE							cbSelectIndex;						//选择索引
	WORD							wSelectUser;						//发言用户
};

//玩家明牌
struct CMD_C_BrightCard
{
	BYTE							cbBrightCardTime;					//明牌倍数
};

//玩家加倍
struct CMD_C_DoubleScore
{
	bool							bDoubleScore;						//玩家加倍
	BYTE                            cbDoubleScore;                      //玩家加倍的倍数
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif