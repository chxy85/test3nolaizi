#ifndef GAME_LOGICOLD_HEAD_FILE
#define GAME_LOGICOLD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "gamelogic.h"
//////////////////////////////////////////////////////////////////////////
#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目
#define GOOD_CARD_COUNT				38									//好牌数目
#define BACK_COUNT					3									//底牌数目
#define NORMAL_COUNT				17									//常规数目
/*
//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序

//////////////////////////////////////////////////////////////////////////
//数目定义


//////////////////////////////////////////////////////////////////////////

//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINE				4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_LINE_TAKE_ONE		7									//三带一单
#define CT_THREE_LINE_TAKE_TWO		8									//三带一对
#define CT_FOUR_LINE_TAKE_ONE		9									//四带两单
#define CT_FOUR_LINE_TAKE_TWO		10									//四带两对

#define CT_BOMB_CARD				11									//炸弹类型
#define CT_MISSILE_CARD				12									//火箭类型
#define CT_JET                      13                                  //飞机
#define CT_BOMB_CARD_5				14									//5个相同牌型炸弹类型
#define CT_MISSILE_CARD_3		    15									//火箭类型 大王+2小王
#define CT_BOMB_CARD_6				16									//6个相同牌型炸弹类型
#define CT_MISSILE_CARD_4			17									//火箭类型 4王
#define CT_BOMB_CARD_7				18									//7个相同牌型炸弹类型
#define CT_MISSILE_CARD_5			19									//火箭类型 5王
#define CT_BOMB_CARD_8				20									//8个相同牌型炸弹类型
#define CT_MISSILE_CARD_6			21									//火箭类型 6王

//////////////////////////////////////////////////////////////////////////

//分析结构
struct tagAnalyseResult
{
    BYTE 							cbEightCount;						//八张数目
    BYTE 							cbSenvenCount;						//七张数目
    BYTE 							cbSixCount;						    //六张数目
    BYTE 							cbFiveCount;						//五张数目
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
    BYTE							cbEightCardData[MAX_COUNT];			//四张扑克
    BYTE							cbSenvenCardData[MAX_COUNT];		//八张扑克
    BYTE							cbSixCardData[MAX_COUNT];			//六张扑克
    BYTE							cbFiveCardData[MAX_COUNT];			//五张扑克
	BYTE							cbFourCardData[MAX_COUNT];			//四张扑克
	BYTE							cbThreeCardData[MAX_COUNT];			//三张扑克
	BYTE							cbDoubleCardData[MAX_COUNT];		//两张扑克
	BYTE							cbSignedCardData[MAX_COUNT];		//单张扑克
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

#define MAX_TYPE_COUNT 254
struct tagOutCardTypeResult 
{
	BYTE							cbCardType;							//扑克类型
	BYTE							cbCardTypeCount;					//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//每手个数
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//扑克数据
};


//扑克信息
struct tagHandCardInfo {
	BYTE						cbHandCardData[ MAX_COUNT ];				//扑克数据
	BYTE						cbHandCardCount;							//扑克数目
	tagOutCardTypeResult		CardTypeResult[ 12 + 1 ] ;					//分析数据

	//初始数据
	tagHandCardInfo( void ) {
		ZeroMemory( cbHandCardData, sizeof( cbHandCardData ) ) ;
		cbHandCardCount = 0;
		ZeroMemory( &CardTypeResult, sizeof( CardTypeResult ) );
	}
};

//类型定义
typedef CArrayTemplate< tagHandCardInfo * > tagHandCardInfoArray;

//栈结构
class tagStackHandCardInfo {

	//内联函数
public:

	//构造函数
	tagStackHandCardInfo( void ) { 
		m_HandCardInfoFreeArray.RemoveAll(); 
		m_HandCardInfoArray.RemoveAll();
	}

	//析构函数
	~tagStackHandCardInfo( void ) {

		//清空栈
		ClearAll();
	}

	//元素压栈
	void Push( tagHandCardInfo * pHandCardInfo ) {

		//是否还有空间
		if ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			//获取空间
			tagHandCardInfo * pHandCardInfoFree = m_HandCardInfoFreeArray[ 0 ];
			m_HandCardInfoFreeArray.RemoveAt( 0 );

			//元素赋值
			CopyMemory( pHandCardInfoFree->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pHandCardInfoFree->cbHandCardData ) );
			pHandCardInfoFree->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pHandCardInfoFree->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//压入栈顶
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pHandCardInfoFree );
		}
		else {
			//申请空间
			tagHandCardInfo * pNewHandCardInfo = new tagHandCardInfo ;

			//元素赋值
			CopyMemory( pNewHandCardInfo->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pNewHandCardInfo->cbHandCardData ) );
			pNewHandCardInfo->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pNewHandCardInfo->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//压入栈顶
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pNewHandCardInfo );
		}
		
	}

	//弹出栈顶
	void Pop() {

		//非空判断
		if ( IsEmpty() ) return ;

		//获取元素
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		tagHandCardInfo * pTopHandCardInfo = m_HandCardInfoArray.GetAt( nECount - 1 );

		//移除元素
		m_HandCardInfoArray.RemoveAt( nECount - 1 );

		//保存空间
		m_HandCardInfoFreeArray.Add( pTopHandCardInfo );		
	}

	//初始栈
	void InitStack() {

		//保存空间
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			m_HandCardInfoArray.RemoveAt( 0 );
			m_HandCardInfoFreeArray.Add( pHandCardInfo );
		}
	}

	//清空栈
	void ClearAll() {

		//释放内存
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoArray.RemoveAt( 0 );
		}

		//释放内存
		while ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoFreeArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoFreeArray.RemoveAt( 0 );
		}
	}

	//获取栈顶
	void GetTop( tagHandCardInfo * & pHandCardInfo ) {

		//非空判断
		if ( IsEmpty() ) {
			ASSERT( false );
			return;
		}

		//获取元素
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		pHandCardInfo = m_HandCardInfoArray[ nECount - 1 ];
	}

	//空判断
	bool IsEmpty() {
		return m_HandCardInfoArray.IsEmpty();
	}

	//成员变量
private:
	tagHandCardInfoArray			m_HandCardInfoFreeArray;					//扑克信息
	tagHandCardInfoArray			m_HandCardInfoArray;						//扑克信息

};

*/
//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogicold
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据
	static const BYTE				m_cbGoodcardData[GOOD_CARD_COUNT];	//好牌数据
    static const BYTE               m_cbGoodcardData2[FULL_COUNT];      //去掉3~9 其他牌双份
	
	//AI变量
public:
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//所有扑克
	BYTE							m_cbLandScoreCardData[MAX_COUNT];	//叫牌扑克
	BYTE							m_cbUserCardCount[GAME_PLAYER];		//扑克数目
	WORD							m_wBankerUser;						//地主玩家
	LONG							m_lBankerOutCardCount ;				//出牌次数
	tagStackHandCardInfo			m_StackHandCardInfo;				//栈变量

private:
    BYTE                            m_bRandCardMode;                    //洗牌类型

	//函数定义
public:
	//构造函数
	CGameLogicold();
	//析构函数
	virtual ~CGameLogicold();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
    //按类型洗牌
    void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bMode);
	//得到好牌
	void GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT]) ;
	//删除好牌
	bool RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount) ;
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//随机扑克
	BYTE GetRandomCard(void) ;

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//内部函数
public:
	//分析扑克
	bool AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);

//////////////////////////////////////////////////////////////////////////
//AI函数

	//设置函数
public:
	//设置扑克
	void SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount) ;
	//设置底牌
	void SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//设置庄家
	void SetBanker(WORD wBanker) ;
	//叫牌扑克
	void SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//删除扑克
	void RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;

	//辅助函数
public:
	//组合算法
	void Combination(BYTE cbCombineCardData[], BYTE cbResComLen,  BYTE cbResultCardData[254][5], BYTE &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	//排列算法
	void Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len) ;
	//分析炸弹
	void GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//分析顺子
	void GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount);
	//分析三条
	void GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount);
	//分析对子
	void GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount);
	//分析单牌
	void GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount);
	//出牌测试
	bool _TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, BYTE	cbAllCardData[GAME_PLAYER][MAX_COUNT], BYTE cbUserCardCount[GAME_PLAYER], bool bFirstOutCard) ;
	//出牌测试
	bool TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, bool bFirstOutCard) ;
	//四带牌型
	bool AnalyseFourCardType( BYTE const cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbEnemyCardData[MAX_COUNT], BYTE cbEnemyCardCount, tagOutCardResult &CardResult ) ;
	//最大判断
	bool IsLargestCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount );
	//是否能出
	bool VerifyOutCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE const cbCurHandCardData[ MAX_COUNT ], BYTE const cbCurHandCardCount, bool bFirstOutCard ) ;
    //检查是否飞机牌型
    bool IsJetCardType(const BYTE cbCardData[], BYTE cbCardCount);

    BYTE GetJetMaxLogicValue(const BYTE cbCardData[], BYTE cbCardCount);

    bool IsAllCouple(const BYTE cbCardData[], BYTE cbCardCount);

	//主要函数
protected:
	//分析牌型（后出牌调用）
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbTurnCardData[], BYTE const cbTurnCardCount, tagOutCardTypeResult CardTypeResult[12+1]);
	//分析牌牌（先出牌调用）
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[12+1]);
	//单牌个数
	BYTE AnalyseSinleCardCount(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE cbSingleCardData[]=NULL);

	//出牌函数
public:
	//地主出牌（先出牌）
	void BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult & OutCardResult) ;
	//地主出牌（后出牌）
	void BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//地主上家（先出牌）
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) ;
	//地主上家（后出牌）
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser,  const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//地主下家（先出牌）
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) ;
	//地主下家（后出牌）
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult);

    bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);

	//叫分函数
public:
	//叫分判断
	BYTE LandScore(WORD wMeChairID, BYTE cbCurrentLandScore, BYTE cbCallScorePhase) ;

private:
    bool SearchFiveBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchSixBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchSevenBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchEightBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    int CountJokers(const BYTE cbCardData[], BYTE cbCardCount);
    int CountBigJoker(const BYTE cbCardData[], BYTE cbCardCount);
    BYTE SequentialThreeDataCount(BYTE bCardData[], BYTE bCardCount);
//////////////////////////////////////////////////////////////////////////
};


#endif