#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//////////////////////////////////////////////////////////////////////////
//#include "logger.h"
class Logger{
public:
    void WriteLog(const char *fmt, ...){}
};
static Logger  g_Logger;

//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_MISSILE_FIRST            2									//��������
#define ST_NORMAL                   3									//��ͨ
#define ST_SPECIAL                  4									//����

//////////////////////////////////////////////////////////////////////////
//��Ŀ����

#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ
#define GOOD_CARD_COUNT				38									//������Ŀ
#define BACK_COUNT					3									//������Ŀ
#define NORMAL_COUNT				17									//������Ŀ

//////////////////////////////////////////////////////////////////////////

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//�˿�����
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_LINE_TAKE_ONE		7									//����һ��
#define CT_THREE_LINE_TAKE_TWO		8									//����һ��
#define CT_FOUR_LINE_TAKE_ONE		9									//�Ĵ�����
#define CT_FOUR_LINE_TAKE_TWO		10									//�Ĵ�����

#define CT_BOMB_CARD				11									//ը������
#define CT_MISSILE_CARD				12									//�������
#define CT_JET                      13                                  //�ɻ�
#define CT_BOMB_CARD_5				14									//5����ͬ����ը������
#define CT_MISSILE_CARD_3		    15									//������� ����+2С��
#define CT_BOMB_CARD_6				16									//6����ͬ����ը������
#define CT_MISSILE_CARD_4			17									//������� 4��
#define CT_BOMB_CARD_7				18									//7����ͬ����ը������
#define CT_MISSILE_CARD_5			19									//������� 5��
#define CT_BOMB_CARD_8				20									//8����ͬ����ը������
#define CT_MISSILE_CARD_6			21									//������� 6��

#define CT_ALLTYPE_CARD             21                                  //����֧�ֵ�����

#define IS_BOMB_TYPE(TYPE)          ((TYPE & (CT_BOMB_CARD | CT_BOMB_CARD_5 | CT_BOMB_CARD_6 | CT_BOMB_CARD_7 | CT_BOMB_CARD_8)) == (CT_BOMB_CARD | CT_BOMB_CARD_5 | CT_BOMB_CARD_6 | CT_BOMB_CARD_7 | CT_BOMB_CARD_8))
#define IS_MISSILE_TYPE(TYPE)       ((TYPE & (CT_MISSILE_CARD | CT_MISSILE_CARD_3 | CT_MISSILE_CARD_4 | CT_MISSILE_CARD_5 | CT_MISSILE_CARD_6)) == (CT_MISSILE_CARD | CT_MISSILE_CARD_3 | CT_MISSILE_CARD_4 | CT_MISSILE_CARD_5 | CT_MISSILE_CARD_6))
//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
    BYTE 							cbEightCount;						//������Ŀ
    BYTE 							cbSenvenCount;						//������Ŀ
    BYTE 							cbSixCount;						    //������Ŀ
    BYTE 							cbFiveCount;						//������Ŀ
	BYTE 							cbFourCount;						//������Ŀ
	BYTE 							cbThreeCount;						//������Ŀ
	BYTE 							cbDoubleCount;						//������Ŀ
	BYTE							cbSignedCount;						//������Ŀ
    BYTE							cbEightCardData[MAX_COUNT];			//�����˿�
    BYTE							cbSenvenCardData[MAX_COUNT];		//�����˿�
    BYTE							cbSixCardData[MAX_COUNT];			//�����˿�
    BYTE							cbFiveCardData[MAX_COUNT];			//�����˿�
	BYTE							cbFourCardData[MAX_COUNT];			//�����˿�
	BYTE							cbThreeCardData[MAX_COUNT];			//�����˿�
	BYTE							cbDoubleCardData[MAX_COUNT];		//�����˿�
	BYTE							cbSignedCardData[MAX_COUNT];		//�����˿�
};

//��¼����
struct tagBomCardResult
{
    BYTE							cbCardType[10];             //����
    BYTE							cbCardCount[10];			//��Ŀ
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

#define MAX_TYPE_COUNT 254
struct tagOutCardTypeResult 
{
	BYTE							cbCardType;							//�˿�����
	BYTE							cbCardTypeCount;					//������Ŀ
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//ÿ�ָ���
	BYTE							cbCardData[MAX_TYPE_COUNT][MAX_COUNT];//�˿�����
//    BYTE							cbEachCardType[CT_ALLTYPE_CARD];   //����Upside�Ⱥ����ж��У������ը��ͬ��ը����һ�������Ҫ���ӱ�������ը������ add by cx
};


//�˿���Ϣ
struct tagHandCardInfo {
	BYTE						cbHandCardData[ MAX_COUNT ];				//�˿�����
	BYTE						cbHandCardCount;							//�˿���Ŀ
	tagOutCardTypeResult		CardTypeResult[ (12+1) ] ;					//��������

	//��ʼ����
	tagHandCardInfo( void ) {
		ZeroMemory( cbHandCardData, sizeof( cbHandCardData ) ) ;
		cbHandCardCount = 0;
		ZeroMemory( &CardTypeResult, sizeof( CardTypeResult ) );
	}
};

//���Ͷ���
typedef CArrayTemplate< tagHandCardInfo * > tagHandCardInfoArray;

//ջ�ṹ
class tagStackHandCardInfo {

	//��������
public:

	//���캯��
	tagStackHandCardInfo( void ) { 
		m_HandCardInfoFreeArray.RemoveAll(); 
		m_HandCardInfoArray.RemoveAll();
	}

	//��������
	~tagStackHandCardInfo( void ) {

		//���ջ
		ClearAll();
	}

	//Ԫ��ѹջ
	void Push( tagHandCardInfo * pHandCardInfo ) {

		//�Ƿ��пռ�
		if ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			//��ȡ�ռ�
			tagHandCardInfo * pHandCardInfoFree = m_HandCardInfoFreeArray[ 0 ];
			m_HandCardInfoFreeArray.RemoveAt( 0 );

			//Ԫ�ظ�ֵ
			CopyMemory( pHandCardInfoFree->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pHandCardInfoFree->cbHandCardData ) );
			pHandCardInfoFree->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pHandCardInfoFree->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//ѹ��ջ��
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pHandCardInfoFree );
		}
		else {
			//����ռ�
			tagHandCardInfo * pNewHandCardInfo = new tagHandCardInfo ;

			//Ԫ�ظ�ֵ
			CopyMemory( pNewHandCardInfo->cbHandCardData, pHandCardInfo->cbHandCardData, sizeof( pNewHandCardInfo->cbHandCardData ) );
			pNewHandCardInfo->cbHandCardCount = pHandCardInfo->cbHandCardCount;
			CopyMemory( pNewHandCardInfo->CardTypeResult, pHandCardInfo->CardTypeResult, sizeof( pHandCardInfo->CardTypeResult ) );

			//ѹ��ջ��
			INT_PTR nECount = m_HandCardInfoArray.GetCount() ; 
			m_HandCardInfoArray.InsertAt( nECount, pNewHandCardInfo );
		}
		
	}

	//����ջ��
	void Pop() {

		//�ǿ��ж�
		if ( IsEmpty() ) return ;

		//��ȡԪ��
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		tagHandCardInfo * pTopHandCardInfo = m_HandCardInfoArray.GetAt( nECount - 1 );

		//�Ƴ�Ԫ��
		m_HandCardInfoArray.RemoveAt( nECount - 1 );

		//����ռ�
		m_HandCardInfoFreeArray.Add( pTopHandCardInfo );		
	}

	//��ʼջ
	void InitStack() {

		//����ռ�
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			m_HandCardInfoArray.RemoveAt( 0 );
			m_HandCardInfoFreeArray.Add( pHandCardInfo );
		}
	}

	//���ջ
	void ClearAll() {

		//�ͷ��ڴ�
		while ( 0 < m_HandCardInfoArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoArray.RemoveAt( 0 );
		}

		//�ͷ��ڴ�
		while ( 0 < m_HandCardInfoFreeArray.GetCount() ) {
			tagHandCardInfo *pHandCardInfo = m_HandCardInfoFreeArray[ 0 ];
			delete pHandCardInfo;
			pHandCardInfo = NULL;
			m_HandCardInfoFreeArray.RemoveAt( 0 );
		}
	}

	//��ȡջ��
	void GetTop( tagHandCardInfo * & pHandCardInfo ) {

		//�ǿ��ж�
		if ( IsEmpty() ) {
			ASSERT( false );
			return;
		}

		//��ȡԪ��
		INT_PTR nECount = m_HandCardInfoArray.GetCount() ;
		pHandCardInfo = m_HandCardInfoArray[ nECount - 1 ];
	}

	//���ж�
	bool IsEmpty() {
		return m_HandCardInfoArray.IsEmpty();
	}

	//��Ա����
private:
	tagHandCardInfoArray			m_HandCardInfoFreeArray;					//�˿���Ϣ
	tagHandCardInfoArray			m_HandCardInfoArray;						//�˿���Ϣ

};


//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����
	static const BYTE				m_cbGoodcardData[GOOD_CARD_COUNT];	//��������
    static const BYTE               m_cbGoodcardData2[FULL_COUNT];      //ȥ��3~9 ������˫��
	
	//AI����
public:
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE							m_cbLandScoreCardData[MAX_COUNT];	//�����˿�
	BYTE							m_cbUserCardCount[GAME_PLAYER];		//�˿���Ŀ
	WORD							m_wBankerUser;						//�������
	LONG							m_lBankerOutCardCount ;				//���ƴ���
	tagStackHandCardInfo			m_StackHandCardInfo;				//ջ����

#ifdef DEBUG
public:
#else
private:
#endif
    BYTE                            m_bRandCardMode;                    //ϴ������

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
    //������ϴ��
    void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bMode);
	//�õ�����
	void GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT]) ;
	//ɾ������
	bool RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount) ;
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//����˿�
	BYTE GetRandomCard(void) ;

	//�߼�����
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//�ڲ�����
public:
	//�����˿�
	bool AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);

//////////////////////////////////////////////////////////////////////////
//AI����

	//���ú���
public:
	//�����˿�
	void SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount) ;
	//���õ���
	void SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount) ;
	//����ׯ��
	void SetBanker(WORD wBanker) ;
	//�����˿�
	void SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) ;
	//ɾ���˿�
	void RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) ;

	//��������
public:
	//����㷨
	void Combination(BYTE cbCombineCardData[], BYTE cbResComLen,  BYTE cbResultCardData[254][5], BYTE &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	//�����㷨
	void Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len) ;
	//����ը��
    void GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//����˳��
	void GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount);
	//��������
	void GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount);
	//��������
	void GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount);
	//��������
	void GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount);
	//���Ʋ���
	bool _TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, BYTE	cbAllCardData[GAME_PLAYER][MAX_COUNT], BYTE cbUserCardCount[GAME_PLAYER], bool bFirstOutCard) ;
	//���Ʋ���
	bool TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, bool bFirstOutCard) ;
	//�Ĵ�����
	bool AnalyseFourCardType( BYTE const cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbEnemyCardData[MAX_COUNT], BYTE cbEnemyCardCount, tagOutCardResult &CardResult ) ;
	//����ж�
	bool IsLargestCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount );
	//�Ƿ��ܳ�
	bool VerifyOutCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE const cbCurHandCardData[ MAX_COUNT ], BYTE const cbCurHandCardCount, bool bFirstOutCard ) ;
    //����Ƿ�ɻ�����
    bool IsJetCardType(const BYTE cbCardData[], BYTE cbCardCount);

    BYTE GetJetMaxLogicValue(const BYTE cbCardData[], BYTE cbCardCount);

    bool IsAllCouple(const BYTE cbCardData[], BYTE cbCardCount);

	//��Ҫ����
public:
	//�������ͣ�����Ƶ��ã�
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbTurnCardData[], BYTE const cbTurnCardCount, tagOutCardTypeResult CardTypeResult[12+1]);
	//�������ƣ��ȳ��Ƶ��ã�
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[12+1]);
	//���Ƹ���
	BYTE AnalyseSinleCardCount(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE cbSingleCardData[]=NULL);

	//���ƺ���
public:
	//�������ƣ��ȳ��ƣ�
	void BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult & OutCardResult) ;
	//�������ƣ�����ƣ�
	void BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//�����ϼң��ȳ��ƣ�
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) ;
	//�����ϼң�����ƣ�
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser,  const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//�����¼ң��ȳ��ƣ�
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) ;
	//�����¼ң�����ƣ�
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) ;
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult);

    bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult);

	//�зֺ���
public:
	//�з��ж�
	BYTE LandScore(WORD wMeChairID, BYTE cbCurrentLandScore, BYTE cbCallScorePhase) ;

private:
    bool SearchFiveBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchSixBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchSevenBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    bool SearchEightBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard);
    int CountJokers(const BYTE cbCardData[], BYTE cbCardCount);
    int CountBigJoker(const BYTE cbCardData[], BYTE cbCardCount);
    BYTE SequentialThreeDataCount(BYTE bCardData[], BYTE bCardCount);
public:
    void GetAllTarBomCardByType(BYTE const BomType, BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
    //����ը�� ��ԭ���Ļ��������Ӵ�С����
    //cbSortTypeΪST_ORDER��ʾ�ɴ�С
    //cbSortTypeΪST_KINGFIRST��ʾ����ǰ��������˿��ɴ�С
    void GetAllBomCardOrder(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagBomCardResult &BomCardResult, BYTE cbSortType = ST_ORDER);
    void GetAllDoubleCardEx(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, BYTE cbSortType = ST_NORMAL);
    //������Ƹ�ǰ17��
    void HeavenCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bMode);
    void SwapCardList(BYTE cbFirstCardData[], BYTE cFirstCardCount, BYTE cbSecCardData[], BYTE cSecCardCount);

//////////////////////////////////////////////////////////////////////////
};


#endif