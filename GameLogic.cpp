#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

//�˿�����
const BYTE	CGameLogic::m_cbCardData[FULL_COUNT]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x4E,0x4F,
};

const BYTE	CGameLogic::m_cbGoodcardData[GOOD_CARD_COUNT]=
{
0x01,0x02,
0x11,0x12,
0x21,0x22,
0x31,0x32,
0x4E,0x4F,
0x07,0x08,0x09,
0x17,0x18,0x19,
0x27,0x28,0x29,
0x37,0x38,0x39,
0x0A,0x0B,0x0C,0x0D,
0x1A,0x1B,0x1C,0x1D,
0x2A,0x2B,0x2C,0x2D,
0x3A,0x3B,0x3C,0x3D
};

//ȥ��3~9 ����double
const BYTE	CGameLogic::m_cbGoodcardData2[FULL_COUNT]=
{
        0x01,0x01,0x02,0x02,0x0A,0x0A,0x0B,0x0B,0x0C,0x0C,0x0D,0x0D,	//���� A - K
        0x11,0x11,0x12,0x12,0x1A,0x1A,0x1B,0x1B,0x1C,0x1C,0x1D,0x1D,	//÷�� A - K
        0x21,0x21,0x22,0x22,0x2A,0x2A,0x2B,0x2B,0x2C,0x2C,0x2D,0x2D,	//���� A - K
        0x31,0x31,0x32,0x32,0x3A,0x3A,0x3B,0x3B,0x3C,0x3C,0x3D,0x3D,    //���� A - K
        0x4E,0x4F,0x4E,0x4F,0x4E,0x4F
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
	//AI����
	m_lBankerOutCardCount = 0 ;	
    memset(m_cbAllCardData, 0x00, sizeof(m_cbAllCardData));
    memset(m_cbLandScoreCardData, 0x00, sizeof(m_cbLandScoreCardData));
    
    m_bRandCardMode = enRandCard_DoubleGoodCard;      //ϴ��ģʽ
}

//��������
CGameLogic::~CGameLogic()
{
#ifdef _CHECK_DUG
	file.Close() ;
#endif
}

//��ȡ����
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//������
	switch (cbCardCount)
	{
	case 0:	//����
		{
			return CT_ERROR;
		}
	case 1: //����
		{
			return CT_SINGLE;
		}
	case 2:	//���ƻ��
		{
			//�����ж�
			if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4E)) return CT_MISSILE_CARD;
            if ((cbCardData[0]==0x4F)&&(cbCardData[1]==0x4F))
            {
                //������
                return CT_MISSILE_CARD;
            }
            if ((cbCardData[0]==0x4E)&&(cbCardData[1]==0x4E))
            {
                return CT_MISSILE_CARD;
            }
			if (GetCardLogicValue(cbCardData[0])==GetCardLogicValue(cbCardData[1])) return CT_DOUBLE;

			return CT_ERROR;
		}
    case 3:
    case 4:
    case 5:
    case 6:
        {
            //��С��ը���жϣ���return ����break��������
            BYTE i = 0;
            for (i = 0; i < cbCardCount; i++)
            {
                if ((0x4F == cbCardData[i]) || (0x4E == cbCardData[i]))
                {
                    continue;
                }
                else
                {
                    break;
                }
            }

            if (i == cbCardCount)
            {
                //�����ƶ�����
                switch (cbCardCount)
                {
                case 3:
                    return CT_MISSILE_CARD_3;
                case 4:
                    return CT_MISSILE_CARD_4;
                case 5:
                    return CT_MISSILE_CARD_5;
                case 6:
                    return CT_MISSILE_CARD_6;
                default:
                	return CT_ERROR;
                }
            }
            break;
        }
    default:
        break;
	}

	//�����˿�
	tagAnalyseResult AnalyseResult;
	if(!AnalysebCardData(cbCardData,cbCardCount,AnalyseResult)) 
		return CT_ERROR ;

    /************************************************************************/
    /* �ɻ���������ж���                                                   */
    /* �������ĸ� ��AAAABBBB ������3A 3B ��AB �ɻ�                          */
    /* ������5�� Ҳ����                                                     */
    /************************************************************************/
    if(IsJetCardType(cbCardData, cbCardCount))
    {
        return CT_JET;
    }

    //��������������
    if (AnalyseResult.cbFiveCount > 0 && cbCardCount == 5)
    {
        return CT_BOMB_CARD_5;
    }

    if (AnalyseResult.cbSixCount > 0 && cbCardCount == 6)
    {
        return CT_BOMB_CARD_6;
    }

    if (AnalyseResult.cbSenvenCount > 0 && cbCardCount == 7)
    {
        return CT_BOMB_CARD_7;
    }

    if (AnalyseResult.cbEightCount > 0 && cbCardCount == 8)
    {
        return CT_BOMB_CARD_8;
    }

    //end of ��������

	//�����ж�
	if (AnalyseResult.cbFourCount>0)
	{
		//�����ж�
		if ((AnalyseResult.cbFourCount==1)&&(cbCardCount==4)) return CT_BOMB_CARD;
        /* ���ֳ�û���Ĵ���
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbSignedCount==2)&&(cbCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
        if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbDoubleCount==1)&&(cbCardCount==6)) return CT_FOUR_LINE_TAKE_ONE;
		if ((AnalyseResult.cbFourCount==1)&&(AnalyseResult.cbDoubleCount==2)&&(cbCardCount==8)) return CT_FOUR_LINE_TAKE_TWO;
        */
		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.cbThreeCount>0)
	{
		//��������
		if(AnalyseResult.cbThreeCount==1 && cbCardCount==3) return CT_THREE ;

		//�����ж�
		if (AnalyseResult.cbThreeCount>1)
		{
			//��������
			BYTE cbCardData=AnalyseResult.cbThreeCardData[0];
			BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue>=15) return CT_ERROR;

			//�����ж�
			for (BYTE i=1;i<AnalyseResult.cbThreeCount;i++)
			{
				BYTE cbCardData=AnalyseResult.cbThreeCardData[i*3];
				if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
			}
		}

		//�����ж�
		if (AnalyseResult.cbThreeCount*3==cbCardCount) return CT_THREE_LINE;
		if ((AnalyseResult.cbThreeCount*4==cbCardCount) && (AnalyseResult.cbThreeCount == 1)) return CT_THREE_LINE_TAKE_ONE;
		if ((AnalyseResult.cbThreeCount*5==cbCardCount)&&(AnalyseResult.cbDoubleCount==AnalyseResult.cbThreeCount)) return CT_THREE_LINE_TAKE_TWO;

        if (((AnalyseResult.cbThreeCount*4==cbCardCount) && (AnalyseResult.cbThreeCount > 1))
            || ((AnalyseResult.cbThreeCount > 1)
                 && (AnalyseResult.cbDoubleCount > 1) 
                 && (AnalyseResult.cbThreeCount == AnalyseResult.cbDoubleCount)
                 && (AnalyseResult.cbThreeCount * 5 == cbCardCount)))
        {
            return CT_JET;
        }

		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.cbDoubleCount>=3)
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbDoubleCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbDoubleCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbDoubleCardData[i*2];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.cbDoubleCount*2)==cbCardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.cbSignedCount>=5)&&(AnalyseResult.cbSignedCount==cbCardCount))
	{
		//��������
		BYTE cbCardData=AnalyseResult.cbSignedCardData[0];
		BYTE cbFirstLogicValue=GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue>=15) return CT_ERROR;

		//�����ж�
		for (BYTE i=1;i<AnalyseResult.cbSignedCount;i++)
		{
			BYTE cbCardData=AnalyseResult.cbSignedCardData[i];
			if (cbFirstLogicValue!=(GetCardLogicValue(cbCardData)+i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

//�����˿�
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType)
{
	//��Ŀ����
	if (cbCardCount==0) return;

	//ת����ֵ
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardLogicValue(cbCardData[i]);	

	//�������
	bool bSorted=true;
	BYTE cbThreeCount,cbLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//����λ��
				cbThreeCount=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbThreeCount;
				cbThreeCount=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=cbThreeCount;
				bSorted=false;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	//��Ŀ����
	if (cbSortType==ST_COUNT)
	{
		//�����˿�
		BYTE cbIndex=0;
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

		//��������
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbFourCardData,sizeof(BYTE)*AnalyseResult.cbFourCount*4);
		cbIndex+=AnalyseResult.cbFourCount*4;

		//��������
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbThreeCardData,sizeof(BYTE)*AnalyseResult.cbThreeCount*3);
		cbIndex+=AnalyseResult.cbThreeCount*3;

		//��������
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbDoubleCardData,sizeof(BYTE)*AnalyseResult.cbDoubleCount*2);
		cbIndex+=AnalyseResult.cbDoubleCount*2;

		//��������
		CopyMemory(&cbCardData[cbIndex],AnalyseResult.cbSignedCardData,sizeof(BYTE)*AnalyseResult.cbSignedCount);
		cbIndex+=AnalyseResult.cbSignedCount;
	}

	return;
}
//�õ�����
void CGameLogic::GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT])
{
	//����׼��
    BYTE cbCardData[CountArray(m_cbGoodcardData)] = {0};
    BYTE cbCardBuffer[CountArray(m_cbGoodcardData)] = {0};
	CopyMemory(cbCardData,m_cbGoodcardData,sizeof(m_cbGoodcardData));

	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	BYTE cbBufferCount=CountArray(m_cbGoodcardData);
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	//���ƺ���
	CopyMemory(cbGoodCardData, cbCardBuffer, NORMAL_COUNT) ;
}

//ɾ������
bool CGameLogic::RemoveGoodCardData(BYTE cbGoodcardData[], BYTE cbGoodCardCount, BYTE cbCardData[], BYTE cbCardCount) 
{
	//��������
	ASSERT(cbGoodCardCount<=cbCardCount);
	if(cbGoodCardCount>cbCardCount)
		return false ;

	//�������
	BYTE cbDeleteCount=0;
    BYTE cbTempCardData[FULL_COUNT] = {0};
	if (cbCardCount>CountArray(cbTempCardData)) 
    {    
        return false;
    }

	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbGoodCardCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbGoodcardData[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	ASSERT(cbDeleteCount==cbGoodCardCount) ;
	if (cbDeleteCount!=cbGoodCardCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//�����˿�
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
    BYTE cbCardData[CountArray(m_cbCardData)] = {0};
	CopyMemory(cbCardData,m_cbCardData,sizeof(m_cbCardData));

    m_bRandCardMode = enRandCard_Normal;
	//�����˿�
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);
    g_Logger.WriteLog("------------RandCardList1 -------------m_bRandCardMode[%d]",m_bRandCardMode);

	return;
}

void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bMode)
{
    BYTE cbCardData[CountArray(m_cbCardData)] = {0};
    BYTE cbRandCount = 0;
    BYTE cbPosition = 0;

    m_bRandCardMode = bMode;

    if (enRandCard_DoubleGoodCard == bMode)
    {
        CopyMemory(cbCardData, m_cbGoodcardData2,sizeof(cbCardData));

        do
        {
            cbPosition=rand()%(cbBufferCount-cbRandCount);
            cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
            cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
        } while (cbRandCount<cbBufferCount);
    }
    g_Logger.WriteLog("------------RandCardList2 -------------m_bRandCardMode[%d]",m_bRandCardMode);
    return ;
}

void CGameLogic::SwapCardList(BYTE cbFirstCardData[], BYTE cFirstCardCount, BYTE cbSecCardData[], BYTE cSecCardCount)
{
    //��������
    ASSERT(cFirstCardCount<=MAX_COUNT);
    ASSERT(cSecCardCount<=MAX_COUNT);

    BYTE cbTempFirst[20] = {0};
    CopyMemory(cbTempFirst, cbFirstCardData, cFirstCardCount);
    BYTE cbTempSec[20] = {0};
    CopyMemory(cbTempSec, cbSecCardData, cSecCardCount);

    //������
    BYTE cbFirstSingleCardData[MAX_COUNT] = {0}; BYTE cbFirstSingleCardCount = 0;
    GetAllSingleCard(cbFirstCardData, cFirstCardCount, cbFirstSingleCardData, cbFirstSingleCardCount);
    BYTE cbSecSingleCardData[MAX_COUNT] = {0}; BYTE cbSecSingleCardCount = 0;
    GetAllSingleCard(cbSecCardData, cSecCardCount, cbSecSingleCardData, cbSecSingleCardCount);

    BYTE cSwapCountSingle = cbFirstSingleCardCount >= cbSecSingleCardCount?cbSecSingleCardCount:cbFirstSingleCardCount;
    RemoveCard(cbFirstSingleCardData, cSwapCountSingle, cbFirstCardData, cFirstCardCount);
    CopyMemory(cbFirstCardData+cFirstCardCount-cSwapCountSingle, cbSecSingleCardData, cSwapCountSingle);
    RemoveCard(cbSecSingleCardData, cSwapCountSingle, cbSecCardData, cSecCardCount);
    CopyMemory(cbSecCardData+cSecCardCount-cSwapCountSingle, cbFirstSingleCardData, cSwapCountSingle);

    //��˫��
    BYTE cbFirstDoubleCardData[MAX_COUNT] = {0}; BYTE cbFirstDoubleCardCount = 0;
    GetAllDoubleCardEx(cbFirstCardData, cFirstCardCount, cbFirstDoubleCardData, cbFirstDoubleCardCount, ST_SPECIAL);
    BYTE cbSecDoubleCardData[MAX_COUNT] = {0}; BYTE cbSecDoubleCardCount = 0;
    GetAllDoubleCardEx(cbSecCardData, cSecCardCount, cbSecDoubleCardData, cbSecDoubleCardCount, ST_SPECIAL);

    BYTE cSwapCountDouble = cbFirstDoubleCardCount >= cbSecDoubleCardCount?cbSecDoubleCardCount:cbFirstDoubleCardCount;
    RemoveCard(cbFirstDoubleCardData, cSwapCountDouble, cbFirstCardData, cFirstCardCount);
    CopyMemory(cbFirstCardData+cFirstCardCount-cSwapCountDouble, cbSecDoubleCardData, cSwapCountDouble);
    RemoveCard(cbSecDoubleCardData, cSwapCountDouble, cbSecCardData, cSecCardCount);
    CopyMemory(cbSecCardData+cSecCardCount-cSwapCountDouble, cbFirstDoubleCardData, cSwapCountDouble);

    return ;
}

//�ϵ��˿�
void CGameLogic::HeavenCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bMode)
{
    BYTE cbCardData[CountArray(m_cbCardData)] = {0};
    BYTE cbRandCount = 0;
    BYTE cbPosition = 0;

    m_bRandCardMode = bMode;

    if (enRandCard_DoubleGoodCard == bMode)
    {
        CopyMemory(cbCardData, m_cbGoodcardData2,sizeof(cbCardData));

        do
        {
            cbPosition=rand()%(cbBufferCount-cbRandCount);
            cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
            cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
        } while (cbRandCount<cbBufferCount);
    }
    g_Logger.WriteLog("------------HeavenCardList -------------m_bRandCardMode[%d]",m_bRandCardMode);
    return ;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//��������
	ASSERT(cbRemoveCount<=cbCardCount);
	if(cbRemoveCount>cbCardCount)
		return false ;

	//�������
	BYTE cbDeleteCount = 0;
    BYTE cbTempCardData[MAX_COUNT] = {0};
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//�����˿�
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//��ȡ����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//��Ч�ж�
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//�߼���ֵ
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

#ifdef _DEBUG
	if(cbCardValue<=0 || cbCardValue>(MASK_VALUE&0x4f))
		return 0 ;
#endif

	ASSERT(cbCardValue>0 && cbCardValue<=(MASK_VALUE&0x4f)) ;


	//ת����ֵ
	if (cbCardColor==0x40) return cbCardValue+2;
	return (cbCardValue<=2)?(cbCardValue+13):cbCardValue;
}

//�Ա��˿�
bool CGameLogic::CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount)
{
	//��ȡ����
	BYTE cbNextType=GetCardType(cbNextCard,cbNextCount);
	BYTE cbFirstType=GetCardType(cbFirstCard,cbFirstCount);

	//�����ж�
	if (cbNextType==CT_ERROR) return false;
    if (m_bRandCardMode == enRandCard_DoubleGoodCard)
    {
        if (cbNextType == CT_MISSILE_CARD_6)
        {
            return true;
        }

        if (cbFirstType == CT_MISSILE_CARD_6)
        {
            return false;
        }
    }
    else
    {
	    if (cbNextType==CT_MISSILE_CARD) return true;
	    if (cbFirstType==CT_MISSILE_CARD) return false ;
    }

	//ը���ж�
	if (((cbFirstType < CT_BOMB_CARD) || (cbFirstType == CT_JET))&&((cbNextType >=CT_BOMB_CARD) && (cbNextType != CT_JET))) return true;
	if (((cbFirstType >= CT_BOMB_CARD) && (cbFirstType != CT_JET))&&((cbNextType < CT_BOMB_CARD) || (cbNextType == CT_JET))) 
    {
        return false;
    }

    if ((cbNextType >= CT_BOMB_CARD) && (cbNextType != CT_JET)
        && (cbFirstType >= CT_BOMB_CARD) && (cbFirstType != CT_JET))
    {
        if (cbNextType > cbFirstType)
        {
            return true;
        }
        else if (cbNextType < cbFirstType)
        {
              return false;
        }
    }
	//�����ж�
	if ((cbFirstType!=cbNextType)||(cbFirstCount!=cbNextCount)) 
    {
        return false;
    }

	//��ʼ�Ա�
	switch (cbNextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
    case CT_BOMB_CARD_5:
    case CT_BOMB_CARD_6:
    case CT_BOMB_CARD_7:
    case CT_BOMB_CARD_8:
		{
			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(cbNextCard[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_THREE_LINE_TAKE_ONE:
	case CT_THREE_LINE_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbThreeCardData[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbThreeCardData[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
	case CT_FOUR_LINE_TAKE_ONE:
	case CT_FOUR_LINE_TAKE_TWO:
		{
			//�����˿�
			tagAnalyseResult NextResult;
			tagAnalyseResult FirstResult;
			AnalysebCardData(cbNextCard,cbNextCount,NextResult);
			AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

			//��ȡ��ֵ
			BYTE cbNextLogicValue=GetCardLogicValue(NextResult.cbFourCardData[0]);
			BYTE cbFirstLogicValue=GetCardLogicValue(FirstResult.cbFourCardData[0]);

			//�Ա��˿�
			return cbNextLogicValue>cbFirstLogicValue;
		}
    case CT_JET:
        {
            //tagAnalyseResult NextResult;
            //tagAnalyseResult FirstResult;
            //AnalysebCardData(cbNextCard,cbNextCount,NextResult);
            //AnalysebCardData(cbFirstCard,cbFirstCount,FirstResult);

            //��ȡ��ֵ
            BYTE cbNextLogicValue=GetJetMaxLogicValue(cbNextCard, cbNextCount);
            BYTE cbFirstLogicValue=GetJetMaxLogicValue(cbFirstCard, cbFirstCount);

            //�Ա��˿�
            return cbNextLogicValue > cbFirstLogicValue;
        }
    case CT_MISSILE_CARD_3:
    case CT_MISSILE_CARD_4:
    case CT_MISSILE_CARD_5:
    case CT_MISSILE_CARD_6:
    case CT_MISSILE_CARD:
        {
            BYTE i = 0;
            BYTE cbNextLogicValue= INVALID_BYTE;
            BYTE cbFirstLogicValue= INVALID_BYTE;

            for (i = 0; i < cbNextCount; i++)
            {
                cbNextLogicValue = GetCardLogicValue(cbNextCard[i]);
                cbFirstLogicValue = GetCardLogicValue(cbFirstCard[i]);
                if (cbNextLogicValue == cbFirstLogicValue)
                {
                    continue;
                }
                else
                {
                    return cbNextLogicValue > cbFirstLogicValue;  
                }
            }
        
        }
	}

	return false;
}

//�����˿�
bool CGameLogic::AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//���ý��
	ZeroMemory(&AnalyseResult,sizeof(tagAnalyseResult));

	//�˿˷���
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbCardData[i]);
		if(cbLogicValue<=0) 
			return false;

		//����ͬ��
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		//���ý��
		switch (cbSameCount)
		{
		case 1:		//����
			{
				BYTE cbIndex=AnalyseResult.cbSignedCount++;
				AnalyseResult.cbSignedCardData[cbIndex*cbSameCount]=cbCardData[i];
				break;
			}
		case 2:		//����
			{
				BYTE cbIndex=AnalyseResult.cbDoubleCount++;
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbDoubleCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				break;
			}
		case 3:		//����
			{
				BYTE cbIndex=AnalyseResult.cbThreeCount++;
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbThreeCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				break;
			}
		case 4:		//����
			{
				BYTE cbIndex=AnalyseResult.cbFourCount++;
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount]=cbCardData[i];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
				AnalyseResult.cbFourCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
				break;
			}
        case 5:		//����
            {
                BYTE cbIndex=AnalyseResult.cbFiveCount++;
                AnalyseResult.cbFiveCardData[cbIndex*cbSameCount]=cbCardData[i];
                AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
                AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
                AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
                AnalyseResult.cbFiveCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
                break;
            }
        case 6:		//����
            {
                BYTE cbIndex=AnalyseResult.cbSixCount++;
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount]=cbCardData[i];
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
                AnalyseResult.cbSixCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
                break;
            }
        case 7:		//����
            {
                BYTE cbIndex=AnalyseResult.cbSenvenCount++;
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount]=cbCardData[i];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
                AnalyseResult.cbSenvenCardData[cbIndex*cbSameCount+6]=cbCardData[i+6];
                break;
            }
        case 8:		//����
            {
                BYTE cbIndex=AnalyseResult.cbEightCount++;
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount]=cbCardData[i];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+1]=cbCardData[i+1];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+2]=cbCardData[i+2];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+3]=cbCardData[i+3];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+4]=cbCardData[i+4];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+5]=cbCardData[i+5];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+6]=cbCardData[i+6];
                AnalyseResult.cbEightCardData[cbIndex*cbSameCount+7]=cbCardData[i+7];
                break;
            }
		}

		//��������
		i+=cbSameCount-1;
	}

	return true;
}
//����˿�
BYTE CGameLogic::GetRandomCard(void)
{
	size_t cbIndex = rand()%(sizeof(m_cbCardData)) ;
	return m_cbCardData[cbIndex] ;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////����ΪAI����

//��������
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, WORD wOutCardUser, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//����ж�
	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//�����¼�
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//�����ϼ�

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;
    g_Logger.WriteLog("------------SearchOutCard2 ----start----m_wBankerUser[%d],wUndersideOfBanker[%d],wUpsideOfBanker[%d],cbTurnCardCount[%d],wOutCardUser[%d],wMeChairID[%d],m_lBankerOutCardCount[%d]", m_wBankerUser, wUndersideOfBanker, wUpsideOfBanker,cbTurnCardCount,wOutCardUser,wMeChairID,m_lBankerOutCardCount);
    g_Logger.WriteLog("-----------------------------------------------------------------------------------[1   ,2   ,3   ,4   ,5   ,6   ,7   ,8   ,9   ,10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20]");
    g_Logger.WriteLog("----------------------------------------m_cbUserCardCount[0][%d],m_cbAllCardData[0][0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",m_cbUserCardCount[0],m_cbAllCardData[0][0],m_cbAllCardData[0][1],m_cbAllCardData[0][2],m_cbAllCardData[0][3],m_cbAllCardData[0][4],m_cbAllCardData[0][5],m_cbAllCardData[0][6],m_cbAllCardData[0][7],m_cbAllCardData[0][8],m_cbAllCardData[0][9],m_cbAllCardData[0][10],m_cbAllCardData[0][11],m_cbAllCardData[0][12],m_cbAllCardData[0][13],m_cbAllCardData[0][14],m_cbAllCardData[0][15],m_cbAllCardData[0][16],m_cbAllCardData[0][17],m_cbAllCardData[0][18],m_cbAllCardData[0][19]);
    g_Logger.WriteLog("----------------------------------------m_cbUserCardCount[1][%d],m_cbAllCardData[1][0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",m_cbUserCardCount[1],m_cbAllCardData[1][0],m_cbAllCardData[1][1],m_cbAllCardData[1][2],m_cbAllCardData[1][3],m_cbAllCardData[1][4],m_cbAllCardData[1][5],m_cbAllCardData[1][6],m_cbAllCardData[1][7],m_cbAllCardData[1][8],m_cbAllCardData[1][9],m_cbAllCardData[1][10],m_cbAllCardData[1][11],m_cbAllCardData[1][12],m_cbAllCardData[1][13],m_cbAllCardData[1][14],m_cbAllCardData[1][15],m_cbAllCardData[1][16],m_cbAllCardData[1][17],m_cbAllCardData[1][18],m_cbAllCardData[1][19]);
    g_Logger.WriteLog("----------------------------------------m_cbUserCardCount[2][%d],m_cbAllCardData[2][0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",m_cbUserCardCount[2],m_cbAllCardData[2][0],m_cbAllCardData[2][1],m_cbAllCardData[2][2],m_cbAllCardData[2][3],m_cbAllCardData[2][4],m_cbAllCardData[2][5],m_cbAllCardData[2][6],m_cbAllCardData[2][7],m_cbAllCardData[2][8],m_cbAllCardData[2][9],m_cbAllCardData[2][10],m_cbAllCardData[2][11],m_cbAllCardData[2][12],m_cbAllCardData[2][13],m_cbAllCardData[2][14],m_cbAllCardData[2][15],m_cbAllCardData[2][16],m_cbAllCardData[2][17],m_cbAllCardData[2][18],m_cbAllCardData[2][19]);

	//�ȳ���
	if(cbTurnCardCount==0)
	{
		//��������
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, OutCardResult) ;
		//�����¼�
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount,wMeChairID,  OutCardResult) ;
		//�����ϼ�
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wMeChairID, OutCardResult) ;
		//���� ID
		else ASSERT(false) ;
	}
	//ѹ��
	else
	{	
		//��������
		if(wMeChairID==m_wBankerUser) BankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//�����¼�
		else if(wMeChairID==wUndersideOfBanker) UndersideOfBankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//�����ϼ�
		else if(wMeChairID==wUpsideOfBanker) UpsideOfBankerOutCard(cbHandCardData, cbHandCardCount, wOutCardUser, cbTurnCardData, cbTurnCardCount, OutCardResult) ;
		//���� ID
		else ASSERT(false) ;

		//�������ж�
		if (GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) != CT_ERROR && m_lBankerOutCardCount == 1 && (OutCardResult.cbCardCount <= 0 || 
			!CompareCard(cbTurnCardData, OutCardResult.cbResultCard , cbTurnCardCount , OutCardResult.cbCardCount)))
		{
            g_Logger.WriteLog("------------SearchOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			//���±�û��
			tagOutCardTypeResult CardTypeResult[12+1] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//��������
			AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

			if (cbOutCardType <= CT_BOMB_CARD/*�ݲ�֧�ַɻ�*/ && CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType != CT_THREE)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex] ;

				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			//��ը��
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			/*else if (CardTypeResult[CT_MISSILE_CARD_6].cbCardTypeCount > 0)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
			}*/
		}

		//�����ж�
		if (GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) != CT_ERROR && (OutCardResult.cbCardCount <= 0 || 
			!CompareCard(cbTurnCardData, OutCardResult.cbResultCard , cbTurnCardCount , OutCardResult.cbCardCount)) &&
			m_cbUserCardCount[(1 + m_wBankerUser) % GAME_PLAYER] == NORMAL_COUNT && m_cbUserCardCount[(2 + m_wBankerUser) % GAME_PLAYER] == NORMAL_COUNT)
		{
			//���±�û��
			tagOutCardTypeResult CardTypeResult[12+1] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//��������
			AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

			BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

			if (cbOutCardType <= CT_BOMB_CARD/*�ݲ�֧�ַɻ�*/ && CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType != CT_THREE)
			{
				BYTE cbIndex = CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbIndex] ;

				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			//��ը��
			else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
			}
			/*else if (CardTypeResult[CT_MISSILE_CARD_6].cbCardTypeCount > 0)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
			}*/
		}

	}

	//���ƴ���
	if (m_wBankerUser == wMeChairID && OutCardResult.cbCardCount > 0) ++m_lBankerOutCardCount ;

    g_Logger.WriteLog("------------SearchOutCard2 -------end---OutCardResult[%d][0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10]);

    return true ;
}

//����ը��
void CGameLogic::GetAllTarBomCardByType(BYTE const BomType, BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount)
{
    BYTE cbTmpCardData[MAX_COUNT] ;
    CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

    //��С����
    SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

    cbBomCardCount = 0 ;

    if(cbHandCardCount<2) return ;

    if (IS_MISSILE_TYPE(BomType))
    {
        //��ը��
        if((0x4F==cbTmpCardData[0] || 0x4E==cbTmpCardData[0]) && (0x4E==cbTmpCardData[1] || 0x4F==cbTmpCardData[1]))
        {
            BYTE i = 0;
            while(0x4F==cbTmpCardData[i] || 0x4E==cbTmpCardData[i])
            {
                cbBomCardData[cbBomCardCount++] = cbTmpCardData[i++] ;
            }
            switch(cbBomCardCount)
            {
                case 2:
                    if (CT_MISSILE_CARD != BomType)
                        cbBomCardCount = 0 ;
                    break;
                case 3:
                    if (CT_MISSILE_CARD_3 != BomType)
                        cbBomCardCount = 0 ;
                    break;
                case 4:
                    if (CT_MISSILE_CARD_4 != BomType)
                        cbBomCardCount = 0 ;
                    break;
                case 5:
                    if (CT_MISSILE_CARD_5 != BomType)
                        cbBomCardCount = 0 ;
                    break;
                case 6:
                    if (CT_MISSILE_CARD_6 != BomType)
                        cbBomCardCount = 0 ;
                    break;
                default:
                    ASSERT(false);
                    break;
            }
            return;
        }
    }

    //�˿˷���
    for (BYTE i=0;i<cbHandCardCount;i++)
    {
        //��������
        BYTE cbSameCount=1;
        BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

        //����ͬ��
        for (BYTE j=i+1;j<cbHandCardCount;j++)
        {
            //��ȡ�˿�
            if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

            //���ñ���
            cbSameCount++;
        }
        
        switch(BomType)
        {
            case CT_BOMB_CARD:
                if(4==cbSameCount)
                {
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
                }
                break;
            case CT_BOMB_CARD_5:
                if(5==cbSameCount)
                {
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
                }
                break;
            case CT_BOMB_CARD_6:
                if(6==cbSameCount)
                {
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
                }
                break;
            case CT_BOMB_CARD_7:
                if(7==cbSameCount)
                {
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+6] ;
                }
                break;
            case CT_BOMB_CARD_8:
                if(8==cbSameCount)
                {
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+6] ;
                    cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+7] ;
                }
                break;
            default:
                break;
        }
        //��������
        i+=cbSameCount-1;
    }
}

//����ı����ǴӴ�С���е� ��������ǰ��
void CGameLogic::GetAllBomCardOrder(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount, tagBomCardResult &BomCardResult, BYTE cbSortType)
{
    BYTE cbTmpCardData[MAX_COUNT] = {0}; 
    BYTE cbTmpCardData2[MAX_COUNT] = {0}; 
    BYTE cbTmpCardCount2 ;
    BYTE cbEachBomIndex = 0;
    CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;
    ZeroMemory(&BomCardResult, sizeof(tagBomCardResult));

    //��С����
    SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);
    BYTE *pTmpCardData = &cbTmpCardData[0];
    BYTE cTempHandCardCount = cbHandCardCount;
    cbBomCardCount = 0 ;

    if(cbHandCardCount<2) return ;

    if (ST_MISSILE_FIRST == cbSortType)
    {
        //��ը��
        if((0x4F==pTmpCardData[0] || 0x4E==pTmpCardData[0]) && (0x4E==pTmpCardData[1] || 0x4F==pTmpCardData[1]))
        {
            BYTE i = 0;
            while((0x4F==pTmpCardData[i] || 0x4E==pTmpCardData[i]) && (i < cTempHandCardCount))
            {
                cbBomCardData[cbBomCardCount++] = pTmpCardData[i++] ;
            }
            switch(i)
            {
            case 2:
                BomCardResult.cbCardCount[cbEachBomIndex] = 2;
                BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD;
                break;
            case 3:
                BomCardResult.cbCardCount[cbEachBomIndex] = 3;
                BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_3;
                break;
            case 4:
                BomCardResult.cbCardCount[cbEachBomIndex] = 4;
                BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_4;
                break;
            case 5:
                BomCardResult.cbCardCount[cbEachBomIndex] = 5;
                BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_5;
                break;
            case 6:
                BomCardResult.cbCardCount[cbEachBomIndex] = 6;
                BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_6;
                break;
            default:
                ASSERT(false);
                break;
            }
            pTmpCardData += i;
            cTempHandCardCount -= i;
        }
    }
    //6��ը��
    if(cTempHandCardCount > 5 && pTmpCardData[5] == 0x4e)
    {
        cbBomCardData[cbBomCardCount++] = pTmpCardData[0] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[1] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[2] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[3] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[4] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[5] ;
        BomCardResult.cbCardCount[cbEachBomIndex] = 6;
        BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_6;
    }
    //8����ͬ����ը������
    GetAllTarBomCardByType(CT_BOMB_CARD_8, pTmpCardData, cTempHandCardCount, cbTmpCardData2, cbTmpCardCount2);
    if (cbTmpCardCount2 > 0)
    {
        for (BYTE i = 0; i < cbTmpCardCount2; i+=8)
        {
            CopyMemory(&cbBomCardData[cbBomCardCount], cbTmpCardData2+i, 8);
            BomCardResult.cbCardCount[cbEachBomIndex] = 8;
            BomCardResult.cbCardType[cbEachBomIndex++] = CT_BOMB_CARD_8;
            cbBomCardCount += 8;
        }
    }
    //5��ը��
    if((cTempHandCardCount == 5 && pTmpCardData[4] == 0x4e) || (cTempHandCardCount >5 && pTmpCardData[5] != 0x4e && pTmpCardData[4] == 0x4e))
    {
        cbBomCardData[cbBomCardCount++] = pTmpCardData[0] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[1] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[2] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[3] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[4] ;
        BomCardResult.cbCardCount[cbEachBomIndex] = 5;
        BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_5;
    }
    //7����ͬ����ը������
    GetAllTarBomCardByType(CT_BOMB_CARD_7, pTmpCardData, cTempHandCardCount, cbTmpCardData2, cbTmpCardCount2);
    if (cbTmpCardCount2 > 0)
    {
        for (BYTE i = 0; i < cbTmpCardCount2; i+=7)
        {
            CopyMemory(&cbBomCardData[cbBomCardCount], cbTmpCardData2+i, 7);
            BomCardResult.cbCardCount[cbEachBomIndex] = 7;
            BomCardResult.cbCardType[cbEachBomIndex++] = CT_BOMB_CARD_7;
            cbBomCardCount += 7;
        }
    }
    //4��ը��
    if((cTempHandCardCount == 4 && pTmpCardData[3] == 0x4e) || (cTempHandCardCount > 4 && pTmpCardData[4] != 0x4e && pTmpCardData[3] == 0x4e))
    {
        cbBomCardData[cbBomCardCount++] = pTmpCardData[0] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[1] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[2] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[3] ;
        BomCardResult.cbCardCount[cbEachBomIndex] = 4;
        BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_4;
    }
    //6����ͬ����ը������
    GetAllTarBomCardByType(CT_BOMB_CARD_6, pTmpCardData, cTempHandCardCount, cbTmpCardData2, cbTmpCardCount2);
    if (cbTmpCardCount2 > 0)
    {
        for (BYTE i = 0; i < cbTmpCardCount2; i+=6)
        {
            CopyMemory(&cbBomCardData[cbBomCardCount], cbTmpCardData2+i, 6);
            BomCardResult.cbCardCount[cbEachBomIndex] = 6;
            BomCardResult.cbCardType[cbEachBomIndex++] = CT_BOMB_CARD_6;
            cbBomCardCount += 6;
        }
    }
    //3��ը��
    if((cTempHandCardCount == 3 && (pTmpCardData[2] == 0x4f || pTmpCardData[2] == 0x4e)) 
        || (cTempHandCardCount > 3 && (pTmpCardData[3] != 0x4e && pTmpCardData[3] != 0x4f) && (pTmpCardData[2] == 0x4f || pTmpCardData[2] == 0x4e)))
    {
        cbBomCardData[cbBomCardCount++] = pTmpCardData[0] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[1] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[2] ;
        BomCardResult.cbCardCount[cbEachBomIndex] = 3;
        BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD_3;
    }
    //5����ͬ����ը������
    GetAllTarBomCardByType(CT_BOMB_CARD_5, pTmpCardData, cTempHandCardCount, cbTmpCardData2, cbTmpCardCount2);
    if (cbTmpCardCount2 > 0)
    {
        for (BYTE i = 0; i < cbTmpCardCount2; i+=5)
        {
            CopyMemory(&cbBomCardData[cbBomCardCount], cbTmpCardData2+i, 5);
            BomCardResult.cbCardCount[cbEachBomIndex] = 5;
            BomCardResult.cbCardType[cbEachBomIndex++] = CT_BOMB_CARD_5;
            cbBomCardCount += 5;
        }
    }
    //2��ը��
    if((cTempHandCardCount == 2 && (pTmpCardData[1] == 0x4f || pTmpCardData[1] == 0x4e)) 
        || (cTempHandCardCount > 2 && (pTmpCardData[2] != 0x4e && pTmpCardData[2] != 0x4f) && (pTmpCardData[1] == 0x4f || pTmpCardData[1] == 0x4e)))
    {
        cbBomCardData[cbBomCardCount++] = pTmpCardData[0] ;
        cbBomCardData[cbBomCardCount++] = pTmpCardData[1] ;
        BomCardResult.cbCardCount[cbEachBomIndex] = 2;
        BomCardResult.cbCardType[cbEachBomIndex++] = CT_MISSILE_CARD;
    }
    //4����ͬ����ը������
    GetAllTarBomCardByType(CT_BOMB_CARD, pTmpCardData, cTempHandCardCount, cbTmpCardData2, cbTmpCardCount2);
    if (cbTmpCardCount2 > 0)
    {
        for (BYTE i = 0; i < cbTmpCardCount2; i+=4)
        {
            CopyMemory(&cbBomCardData[cbBomCardCount], cbTmpCardData2+i, 4);
            BomCardResult.cbCardCount[cbEachBomIndex] = 4;
            BomCardResult.cbCardType[cbEachBomIndex++] = CT_BOMB_CARD;
            cbBomCardCount += 4;
        }
    }

    return;
}

void CGameLogic::GetAllBomCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbBomCardCount = 0 ;

	if(cbHandCardCount<2) return ;

    //��ը��
    if((0x4F==cbTmpCardData[0] || 0x4E==cbTmpCardData[0]) && (0x4E==cbTmpCardData[1] || 0x4F==cbTmpCardData[1]))
    {
        BYTE i = 0;
        while(0x4F==cbTmpCardData[i] || 0x4E==cbTmpCardData[i])
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i++] ;
        }
    }

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

        if(4==cbSameCount)
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
        }
        if(5==cbSameCount)
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
        }
        if(6==cbSameCount)
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
        }
        if(7==cbSameCount)
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+6] ;
        }
        if(8==cbSameCount)
        {
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+1] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+2] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+3] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+4] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+5] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+6] ;
            cbBomCardData[cbBomCardCount++] = cbTmpCardData[i+7] ;
        }

		//��������
		i+=cbSameCount-1;
	}
}

//����˳��
void CGameLogic::GetAllLineCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount)
{
	BYTE cbTmpCard[MAX_COUNT] ;
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	//��С����
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;

	cbLineCardCount = 0 ;

	//����У��
	if(cbHandCardCount<5) return ;

	BYTE cbFirstCard = 0 ;
	//ȥ��2����
    for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

	BYTE cbSingleLineCard[12] ;
	BYTE cbSingleLineCount=0 ;
	BYTE cbLeftCardCount = cbHandCardCount ;
	bool bFindSingleLine = true ;

	//�����ж�
	while (cbLeftCardCount>=5 && bFindSingleLine)
	{
		cbSingleLineCount=1 ;
		bFindSingleLine = false ;
		BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
		cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
		for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
		{
			BYTE cbCardData=cbTmpCard[i];

			//�����ж�
			if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
			{
				cbLastCard = cbTmpCard[i] ;
				if(cbSingleLineCount<5) 
				{
					cbSingleLineCount = 1 ;
					cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
					continue ;
				}
				else break ;
			}
			//ͬ���ж�
			else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
			{
				cbLastCard = cbCardData ;
				cbSingleLineCard[cbSingleLineCount] = cbCardData ;
				++cbSingleLineCount ;
			}					
		}

		//��������
		if(cbSingleLineCount>=5)
		{
			RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
			memcpy(cbLineCardData+cbLineCardCount , cbSingleLineCard, sizeof(BYTE)*cbSingleLineCount) ;
			cbLineCardCount += cbSingleLineCount ;
			cbLeftCardCount -= cbSingleLineCount ;
			bFindSingleLine = true ;
		}
	}
}

//��������
void CGameLogic::GetAllThreeCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbThreeCardCount = 0 ;

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount>=3)
		{
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+1] ;
			cbThreeCardData[cbThreeCardCount++] = cbTmpCardData[i+2] ;	
		}

		//��������
		i+=cbSameCount-1;
	}
}

//��������
void CGameLogic::GetAllDoubleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount)
{
	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	cbDoubleCardCount = 0 ;

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount>=2)
		{
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i] ;
			cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[i+1] ;
		}

		//��������
		i+=cbSameCount-1;
	}
}
void CGameLogic::GetAllDoubleCardEx(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount, BYTE cbSortType)
{
    BYTE cbTmpCardData[MAX_COUNT] ;
    CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

    //��С����
    SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

    cbDoubleCardCount = 0 ;

    //�˿˷���
    for (BYTE i=0;i<cbHandCardCount;i++)
    {
        //��������
        BYTE cbSameCount=1;
        BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

        //����ͬ��
        for (BYTE j=i+1;j<cbHandCardCount;j++)
        {
            //��ȡ�˿�
            if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

            //���ñ���
            cbSameCount++;
        }

		INT nCount = cbSameCount;
        BYTE cIndex = i;
        if (cbSortType == ST_NORMAL)
        {
            while( nCount >= 2)
            {
                cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[cIndex++] ;
                cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[cIndex++] ;
                nCount -= 2;
            }
        }
        else if (cbSortType == ST_SPECIAL)
        {
            if( nCount == 2)
            {
                cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[cIndex++] ;
                cbDoubleCardData[cbDoubleCardCount++] = cbTmpCardData[cIndex++] ;
            }
        }
      
        //��������
        i+=cbSameCount-1;
    }
}
//��������
void CGameLogic::GetAllSingleCard(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount)
{
	cbSingleCardCount =0 ;

	BYTE cbTmpCardData[MAX_COUNT] ;
	CopyMemory(cbTmpCardData, cbHandCardData, cbHandCardCount) ;

	//��С����
	SortCardList(cbTmpCardData, cbHandCardCount, ST_ORDER);

	//�˿˷���
	for (BYTE i=0;i<cbHandCardCount;i++)
	{
		//��������
		BYTE cbSameCount=1;
		BYTE cbLogicValue=GetCardLogicValue(cbTmpCardData[i]);

		//����ͬ��
		for (BYTE j=i+1;j<cbHandCardCount;j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(cbTmpCardData[j])!=cbLogicValue) break;

			//���ñ���
			cbSameCount++;
		}

		if(cbSameCount==1)
		{
			cbSingleCardData[cbSingleCardCount++] = cbTmpCardData[i] ;
		}

		//��������
		i+=cbSameCount-1;
	}
}

//��������
void CGameLogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[12+1])
{
	ZeroMemory(CardTypeResult, sizeof(tagOutCardTypeResult)*(12+1)) ;
	BYTE cbTmpCardData[MAX_COUNT] = {0}; 
	//�����˿ˣ���ֹ����ʱ�ı��˿�
	BYTE cbReserveCardData[MAX_COUNT] = {0}; 
	CopyMemory(cbReserveCardData, cbHandCardData, cbHandCardCount) ;	
	SortCardList(cbReserveCardData, cbHandCardCount, ST_ORDER) ;
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	//��������
	for(BYTE i=0; i<cbHandCardCount; ++i)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount ;
		CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
		CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCardData[i] ;
		CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1 ;
		CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;	

		ASSERT(CardTypeResult[CT_SINGLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}

	//��������
	{
		BYTE cbDoubleCardData[MAX_COUNT] ;
		BYTE cbDoubleCardcount=0; 
		GetAllDoubleCard(cbTmpCardData, cbHandCardCount, cbDoubleCardData, cbDoubleCardcount) ;
		for(BYTE i=0; i<cbDoubleCardcount; i+=2)
		{
			BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount ;
			CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbDoubleCardData[i] ;
			CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbDoubleCardData[i+1] ;
			CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2 ;
			CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;	

			ASSERT(CardTypeResult[CT_DOUBLE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//��������
	{
		BYTE cbThreeCardData[MAX_COUNT];
		BYTE cbThreeCardCount=0 ;
		GetAllThreeCard(cbTmpCardData, cbHandCardCount, cbThreeCardData, cbThreeCardCount) ;
		for(BYTE i=0; i<cbThreeCardCount; i+=3)
		{
			BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount ;
			CardTypeResult[CT_THREE].cbCardType = CT_THREE ;
			CardTypeResult[CT_THREE].cbCardData[Index][0] = cbThreeCardData[i] ;
			CardTypeResult[CT_THREE].cbCardData[Index][1] = cbThreeCardData[i+1] ;
			CardTypeResult[CT_THREE].cbCardData[Index][2] = cbThreeCardData[i+2] ;
			CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3 ;
			CardTypeResult[CT_THREE].cbCardTypeCount++ ;	

			ASSERT(CardTypeResult[CT_THREE].cbCardTypeCount<MAX_TYPE_COUNT) ;
		}
	}

	//ը������
    {
        BYTE cbFourCardData[MAX_COUNT] = {0}; 
        BYTE cbFourCardCount=0 ;
        tagBomCardResult BomCardResult = {0};
        GetAllBomCardOrder(cbTmpCardData, cbHandCardCount, cbFourCardData, cbFourCardCount, BomCardResult) ;
        
        BYTE EachIndex = 0;
        BYTE i=0;
        while(BomCardResult.cbCardCount[i] != 0)
        {
            BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
            CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
            CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbFourCardData+EachIndex, BomCardResult.cbCardCount[i]) ;
            EachIndex += BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;
            i++;

            ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
        }
/*		for (BYTE i=0; i<cbFourCardCount; i++)
        {
            BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
            CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
            CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbFourCardData+EachIndex, BomCardResult.cbCardCount[i]) ;
            EachIndex += BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

            ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
        }*/
    }
	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbFirstCard = 0 ;
		//ȥ��2����
        for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCardData[i])<15) {break;}}

		BYTE cbSingleLineCard[12] ;
		BYTE cbSingleLineCount=1 ;
		BYTE cbLeftCardCount = cbHandCardCount ;
		bool bFindSingleLine = true ;

		//�����ж�
		while (cbLeftCardCount>=5 && bFindSingleLine)
		{
			cbSingleLineCount=1 ;
			bFindSingleLine = false ;
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[cbFirstCard] ;
			for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
			{
				BYTE cbCardData=cbTmpCardData[i];

				//�����ж�
				if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
				{
					cbLastCard = cbTmpCardData[i] ;
					//�Ƿ�Ϸ�
					if(cbSingleLineCount<5) 
					{
						cbSingleLineCount = 1 ;
						cbSingleLineCard[cbSingleLineCount-1] = cbTmpCardData[i] ;
						continue ;
					}
					else break ;
				}
				//ͬ���ж�
				else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
				{
					cbLastCard = cbCardData ;
					cbSingleLineCard[cbSingleLineCount] = cbCardData ;
					++cbSingleLineCount ;
				}
			}

			//��������
			if(cbSingleLineCount>=5)
			{
				BYTE Index ;
				//��������

				int nStart = 0 ;
				
				//�Ӵ�С
				nStart = cbSingleLineCount - 5 ;
				while ( 0 < nStart )
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-nStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					nStart-- ;
				}

				//��С����
				nStart = cbSingleLineCount - 5;
				while ( 0 <= nStart )
				{
					Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
					BYTE cbThisLineCount = cbSingleLineCount-nStart ;
					CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+nStart, sizeof(BYTE)*(cbThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					nStart-- ;
				}

				RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCardData, cbLeftCardCount) ;
				cbLeftCardCount -= cbSingleLineCount ;
				bFindSingleLine = true ;
			}
		}

	}

	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//�����ж�
		BYTE cbFirstCard = 0 ;
		//ȥ��2����
        for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCardData[i])<15) {break;}}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindDoubleLine = true ;
		BYTE cbDoubleLineCount = 0 ;
		BYTE cbDoubleLineCard[24] ;
		//��ʼ�ж�
		while (cbLeftCardCount>=6 && bFindDoubleLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbDoubleLineCount = 0 ;
			bFindDoubleLine=false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//����ͬ��
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastDoubleCardValue ;
				if(cbDoubleLineCount>0) cbLastDoubleCardValue = GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) ;
				//���¿�ʼ
				if((cbSameCount<2 || (cbDoubleLineCount>0 && (cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbDoubleLineCount>=6) break ;
					//����
					if(cbSameCount>=2) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbDoubleLineCount = 0 ;
				}
				//��������
				else if(cbSameCount>=2)
				{
					cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbDoubleLineCount += 2 ;

					//��β�ж�
					if(i==(cbLeftCardCount+cbFirstCard-2))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])))
						{
							cbDoubleLineCard[cbDoubleLineCount] = cbTmpCardData[i] ;
							cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCardData[i+1] ;
							cbDoubleLineCount += 2 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//��������
			if(cbDoubleLineCount>=6)
			{
				BYTE Index ;

				//��������
				BYTE cbCurrentDoubleLineCount = 6 ;
				while ( cbCurrentDoubleLineCount < cbDoubleLineCount )
				{
					Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
					CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
					CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard, sizeof(BYTE)*cbCurrentDoubleLineCount);
					CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbCurrentDoubleLineCount;
					CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;

					cbCurrentDoubleLineCount += 2 ;

					ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
				}

				//��С����
				if ( cbDoubleLineCount >= 6 )
				{
					//��������
					int cbLeftLen = cbDoubleLineCount - 6 ;
					while ( cbLeftLen >= 0 )
					{
						Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
						CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard + cbLeftLen, sizeof( BYTE ) * ( cbDoubleLineCount - cbLeftLen ));
						CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbDoubleLineCount - cbLeftLen;
						CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;

						cbLeftLen -= 2 ;

						ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}
				}

				RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;				
				bFindDoubleLine=true ;
				cbLeftCardCount -= cbDoubleLineCount ;
			}
		}
	}

	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		//�����ж�
		BYTE cbFirstCard = 0 ;
		//ȥ��2����
        for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCardData[i])<15) {break;}}

		BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
		bool bFindThreeLine = true ;
		BYTE cbThreeLineCount = 0 ;
		BYTE cbThreeLineCard[20] ;
		//��ʼ�ж�
		while (cbLeftCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastCard = cbTmpCardData[cbFirstCard] ;
			BYTE cbSameCount = 1 ;
			cbThreeLineCount = 0 ;
			bFindThreeLine = false ;
			for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
			{
				//����ͬ��
				while (GetCardLogicValue(cbLastCard)==GetCardLogicValue(cbTmpCardData[i]) && i<cbLeftCardCount+cbFirstCard)
				{
					++cbSameCount;
					++i ;
				}

				BYTE cbLastThreeCardValue ;
				if(cbThreeLineCount>0) cbLastThreeCardValue = GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) ;

				//���¿�ʼ
				if((cbSameCount<3 || (cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
				{
					if(cbThreeLineCount>=6) break ;

					if(cbSameCount>=3) i-=cbSameCount ;
					cbLastCard = cbTmpCardData[i] ;
					cbThreeLineCount = 0 ;
				}
				//��������
				else if(cbSameCount>=3)
				{
					cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i-cbSameCount] ;
					cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i-cbSameCount+1] ;
					cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i-cbSameCount+2] ;
					cbThreeLineCount += 3 ;

					//��β�ж�
					if(i==(cbLeftCardCount+cbFirstCard-3))
						if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCardData[i]))==1 && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+1])) && (GetCardLogicValue(cbTmpCardData[i])==GetCardLogicValue(cbTmpCardData[i+2])))
						{
							cbThreeLineCard[cbThreeLineCount] = cbTmpCardData[i] ;
							cbThreeLineCard[cbThreeLineCount+1] = cbTmpCardData[i+1] ;
							cbThreeLineCard[cbThreeLineCount+2] = cbTmpCardData[i+2] ;
							cbThreeLineCount += 3 ;
							break ;
						}

				}

				cbLastCard = cbTmpCardData[i] ;
				cbSameCount = 1 ;
			}

			//��������
			if(cbThreeLineCount>=6)
			{
				BYTE Index ;

				Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount ;
				CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE ;
				CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbThreeLineCount);
				CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = cbThreeLineCount;
				CardTypeResult[CT_THREE_LINE].cbCardTypeCount++ ;

				ASSERT(CardTypeResult[CT_THREE_LINE].cbCardTypeCount<MAX_TYPE_COUNT) ;

				RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTmpCardData, cbFirstCard+cbLeftCardCount) ;
				bFindThreeLine=true ;
				cbLeftCardCount -= cbThreeLineCount ;
			}
		}

	}
	//����һ��
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbHandThreeCard[MAX_COUNT] = {0}; 
		BYTE cbHandThreeCount=0 ;

		//�Ƴ�ը��
		BYTE cbAllBomCardData[MAX_COUNT] = {0}; 
		BYTE cbAllBomCardCount=0 ;
		GetAllBomCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
		RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount) ;

		GetAllThreeCard(cbTmpCardData, cbHandCardCount-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

		{
			BYTE Index ;
			//ȥ������
			BYTE cbRemainCardData[MAX_COUNT] ;
			CopyMemory(cbRemainCardData, cbTmpCardData, cbHandCardCount-cbAllBomCardCount) ;
			BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;
			RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCardData, cbHandCardCount-cbAllBomCardCount) ;
			//������һ��
			for(BYTE i=0; i<cbHandThreeCount; i+=3)
			{
				//������һ��
				for(BYTE j=0; j<cbRemainCardCount; ++j)
				{
					Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][0] = cbHandThreeCard[i] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][1] = cbHandThreeCard[i+1] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][2] = cbHandThreeCard[i+2] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index][3] = cbRemainCardData[j] ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 4 ;
					CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;
				}			
			}
		}

		//��������
		BYTE cbLeftThreeCardCount=cbHandThreeCount ;
		bool bFindThreeLine=true ;
		BYTE cbLastIndex=0 ;
		if(GetCardLogicValue(cbHandThreeCard[0])==15) cbLastIndex=3 ;
		while (cbLeftThreeCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
			BYTE cbThreeLineCard[MAX_COUNT] = {0}; 
			BYTE cbThreeLineCardCount=3;
			cbThreeLineCard[0]=cbHandThreeCard[cbLastIndex];
			cbThreeLineCard[1]=cbHandThreeCard[cbLastIndex+1];
			cbThreeLineCard[2]=cbHandThreeCard[cbLastIndex+2];

			bFindThreeLine = false ;
			for(BYTE j=3+cbLastIndex; j<cbLeftThreeCardCount; j+=3)
			{
				//�����ж�
				if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))))
				{
					cbLastIndex = j ;
					if(cbLeftThreeCardCount-j>=6) bFindThreeLine = true ;

					break;
				}

				cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
				cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
				cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
				cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
				cbThreeLineCardCount += 3;
			}
			if(cbThreeLineCardCount>3)
			{
				BYTE Index ;

				BYTE cbRemainCard[MAX_COUNT] = {0}; 
				BYTE cbRemainCardCount=cbHandCardCount-cbAllBomCardCount-cbHandThreeCount ;


				//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
				CopyMemory(cbRemainCard, cbTmpCardData, (cbHandCardCount-cbAllBomCardCount)*sizeof(BYTE));
				RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCard, cbHandCardCount-cbAllBomCardCount) ;

				for(BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
				{
					//��˳��Ŀ
					BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start ;
					//���Ƹ���
					BYTE cbSingleCardCount=(cbThisTreeLineCardCount)/3;

					//���Ʋ���
					if(cbRemainCardCount<cbSingleCardCount) continue ;

					//�������
					BYTE cbComCard[5];
					BYTE cbComResCard[254][5] ;
					BYTE cbComResLen=0 ;

					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, cbRemainCardCount, cbSingleCardCount);
					for(BYTE i=0; i<cbComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE ;
						//��������
						CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//���浥��
						CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index]+cbThisTreeLineCardCount, cbComResCard[i], cbSingleCardCount) ;


						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = cbThisTreeLineCardCount+cbSingleCardCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;

						ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}

				}

				//�Ƴ�����
				bFindThreeLine = true ;
				RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount) ;
				cbLeftThreeCardCount -= cbThreeLineCardCount ;
			}
		}
	}

	//����һ��
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

		BYTE cbHandThreeCard[MAX_COUNT] = {0}; 
		BYTE cbHandThreeCount=0 ;
		BYTE cbRemainCarData[MAX_COUNT] = {0}; 
		BYTE cbRemainCardCount=0 ;

        //�Ƴ�ը��
        BYTE cbAllBomCardData[MAX_COUNT] = {0}; 
        BYTE cbAllBomCardCount=0 ;
        GetAllBomCard(cbTmpCardData, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
        RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCardData, cbHandCardCount) ;

        //��ȡ����
		GetAllThreeCard(cbTmpCardData, cbHandCardCount-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

        //�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
        CopyMemory(cbRemainCarData, cbTmpCardData, cbHandCardCount-cbAllBomCardCount) ;
        RemoveCard(cbHandThreeCard, cbHandThreeCount, cbRemainCarData, cbHandCardCount-cbAllBomCardCount) ;
        cbRemainCardCount = cbHandCardCount-cbHandThreeCount-cbAllBomCardCount ;

        //��ȡ����
		BYTE cbAllDoubleCardData[MAX_COUNT] ;
		BYTE cbAllDoubleCardCount=0 ;
		GetAllDoubleCard(cbRemainCarData, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;

		//������һ��
		for(BYTE i=0; i<cbHandThreeCount; i+=3)
		{
			BYTE Index ;

			//������һ��
			for(BYTE j=0; j<cbAllDoubleCardCount; j+=2)
			{
				Index = CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardType = CT_THREE_LINE_TAKE_TWO ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][0] = cbHandThreeCard[i] ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][1] = cbHandThreeCard[i+1] ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][2] = cbHandThreeCard[i+2] ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][3] = cbAllDoubleCardData[j] ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][4] = cbAllDoubleCardData[j+1] ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbEachHandCardCount[Index] = 5 ;
				CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount++ ;
			}	
		}

		//��������
		BYTE cbLeftThreeCardCount=cbHandThreeCount ;
		bool bFindThreeLine=true ;
		BYTE cbLastIndex=0 ;
		if(GetCardLogicValue(cbHandThreeCard[0])==15) cbLastIndex=3 ;
		while (cbLeftThreeCardCount>=6 && bFindThreeLine)
		{
			BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[cbLastIndex]);
			BYTE cbThreeLineCard[MAX_COUNT] = {0}; 
			BYTE cbThreeLineCardCount=3;
			cbThreeLineCard[0]=cbHandThreeCard[cbLastIndex];
			cbThreeLineCard[1]=cbHandThreeCard[cbLastIndex+1];
			cbThreeLineCard[2]=cbHandThreeCard[cbLastIndex+2];

			bFindThreeLine=false ;
			for(BYTE j=3+cbLastIndex; j<cbLeftThreeCardCount; j+=3)
			{
				//�����ж�
				if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))))
				{
					cbLastIndex = j ;
					if(cbLeftThreeCardCount-j>=6) bFindThreeLine = true ;

					break;
				}

				cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
				cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
				cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
				cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
				cbThreeLineCardCount += 3;
			}
			if(cbThreeLineCardCount>3)
			{
				BYTE Index ;

				for(BYTE start=0; start<cbThreeLineCardCount-3; start+=3)
				{
					//��˳��Ŀ
					BYTE cbThisTreeLineCardCount = cbThreeLineCardCount-start ;
					//��������
					BYTE cbDoubleCardCount=((cbThisTreeLineCardCount)/3);

					//���Ʋ���
					if(cbRemainCardCount<cbDoubleCardCount) continue ;

					BYTE cbDoubleCardIndex[10]; //�����±�
					for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
						cbDoubleCardIndex[j]=i ;

					//�������
					BYTE cbComCard[5];
					BYTE cbComResCard[254][5] ;
					BYTE cbComResLen=0 ;

					//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
					Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, cbDoubleCardCount, cbAllDoubleCardCount/2, cbDoubleCardCount);

					ASSERT(cbComResLen<=254) ;

					for(BYTE i=0; i<cbComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardType = CT_THREE_LINE_TAKE_TWO ;
						//��������
						CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index], cbThreeLineCard+start, sizeof(BYTE)*cbThisTreeLineCardCount);
						//�������
						for(BYTE j=0, k=0; j<cbDoubleCardCount; ++j, k+=2)
						{
							CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][cbThisTreeLineCardCount+k] = cbAllDoubleCardData[cbComResCard[i][j]];
							CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][cbThisTreeLineCardCount+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
						}

						CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbEachHandCardCount[Index] = cbThisTreeLineCardCount+2*cbDoubleCardCount ;
						CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount++ ;

						ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount<MAX_TYPE_COUNT) ;
					}

				}
				//�Ƴ�����
				bFindThreeLine = true ;				
				RemoveCard(cbThreeLineCard, cbThreeLineCardCount, cbHandThreeCard, cbLeftThreeCardCount) ;
				cbLeftThreeCardCount -= cbThreeLineCardCount ;
			}
		}
	}
	//�Ĵ�����
	/*
	{
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	BYTE cbFirstCard = 0 ;
	//ȥ������
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCardData[i])!=0x40)	{cbFirstCard = i ; break ;}

	BYTE cbHandAllFourCardData[MAX_COUNT] ;
	BYTE cbHandAllFourCardCount=0;
	//��ȡ����
	GetAllBomCard(cbTmpCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;

	//�Ƴ�����
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount=cbHandCardCount-cbHandAllFourCardCount ;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount) ;

	for(BYTE Start=0; Start<cbHandAllFourCardCount; Start += 4)
	{
	BYTE Index ;
	//�������
	BYTE cbComCard[5];
	BYTE cbComResCard[254][5] ;
	BYTE cbComResLen=0 ;
	//�������
	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbRemainCardCount, 2);
	for(BYTE i=0; i<cbComResLen; ++i)
	{
	//���ܴ���
	if(GetCardValue(cbComResCard[i][0])==GetCardValue(cbComResCard[i][1])) continue ;

	Index=CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardType = CT_FOUR_LINE_TAKE_ONE ;
	CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index], cbHandAllFourCardData+Start, 4) ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4] = cbComResCard[i][0] ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4+1] = cbComResCard[i][1] ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 6 ;
	CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount++ ;

	ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}
	}
	}*/


	//�Ĵ�����
	/*
	{
	//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
	CopyMemory(cbTmpCardData, cbReserveCardData, cbHandCardCount) ;

	BYTE cbFirstCard = 0 ;
	//ȥ������
	for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCardData[i])!=0x40)	{cbFirstCard = i ; break ;}

	BYTE cbHandAllFourCardData[MAX_COUNT] ;
	BYTE cbHandAllFourCardCount=0;

	//��ȡ����
	GetAllBomCard(cbTmpCardData+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;

	//�Ƴ�����
	BYTE cbRemainCard[MAX_COUNT];
	BYTE cbRemainCardCount=cbHandCardCount-cbHandAllFourCardCount ;
	CopyMemory(cbRemainCard, cbTmpCardData, cbHandCardCount*sizeof(BYTE));
	RemoveCard(cbHandAllFourCardData, cbHandAllFourCardCount, cbRemainCard, cbHandCardCount) ;

	for(BYTE Start=0; Start<cbHandAllFourCardCount; Start += 4)
	{
	//��ȡ����
	BYTE cbAllDoubleCardData[MAX_COUNT] ;
	BYTE cbAllDoubleCardCount=0 ;
	GetAllDoubleCard(cbRemainCard, cbRemainCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;

	BYTE cbDoubleCardIndex[10]; //�����±�
	for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
	cbDoubleCardIndex[j]=i ;

	//�������
	BYTE cbComCard[5];
	BYTE cbComResCard[255][5] ;
	BYTE cbComResLen=0 ;

	//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
	Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount/2, 2);
	for(BYTE i=0; i<cbComResLen; ++i)
	{
	BYTE Index = CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount ;
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardType = CT_FOUR_LINE_TAKE_TWO ;
	CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index], cbHandAllFourCardData+Start, 4) ;

	//�������
	for(BYTE j=0, k=0; j<4; ++j, k+=2)
	{
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k] = cbAllDoubleCardData[cbComResCard[i][j]];
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
	}

	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbEachHandCardCount[Index] = 8 ;
	CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount++ ;

	ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount<MAX_TYPE_COUNT) ;
	}
	}
	}*/

}

//��������
void CGameLogic::AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbTurnCardData[], BYTE const cbTurnCardCount,tagOutCardTypeResult CardTypeResult[12+1])
{
	ZeroMemory(CardTypeResult, sizeof(tagOutCardTypeResult)*(12+1)) ;

    BYTE cbTmpCard[MAX_COUNT] = {0};
	CopyMemory(cbTmpCard, cbHandCardData, cbHandCardCount) ;
	SortCardList(cbTmpCard, cbHandCardCount, ST_ORDER) ;
	//	SortCardList(cbTurnCardData, cbTurnCardCount, ST_ORDER) ;

	BYTE cbTurnCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

	ASSERT(cbTurnCardType!=CT_ERROR) ;
#ifdef	_DEBUG
	if(cbTurnCardType==CT_ERROR)
	{
		CString str = "\nAnalyseOutCardType:  ";
		for(BYTE i=0; i<cbTurnCardCount; ++i)
		{
			CString strTmp ;
			strTmp.Format("%d,", GetCardValue(cbTurnCardData[i])) ;
			str += strTmp ;
		}
		str += "\n\n" ;
		OutputDebugString(str) ;
	}
#endif
	if(cbTurnCardType==CT_ERROR)
		return ;

    if(cbTurnCardType < CT_BOMB_CARD || cbTurnCardType == CT_JET)
    {
        BYTE cbFourCardData[MAX_COUNT] = {0};
        BYTE cbFourCardCount=0 ;
        tagBomCardResult BomCardResult = {0};
        GetAllBomCardOrder(cbTmpCard, cbHandCardCount, cbFourCardData, cbFourCardCount, BomCardResult) ;

        BYTE EachIndex = 0;
        BYTE i=0;
        while(BomCardResult.cbCardCount[i] != 0)
        {
            BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount;
            CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
            CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbFourCardData+EachIndex, BomCardResult.cbCardCount[i]) ;
            EachIndex += BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = BomCardResult.cbCardCount[i];
            CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;
            i++;

            ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
        }
    }

	switch(cbTurnCardType)
	{
	case CT_SINGLE:				//��������
		{			
			for(BYTE i=0; i<cbHandCardCount; ++i) 
				if(GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0])) 
				{
					BYTE Index = CardTypeResult[CT_SINGLE].cbCardTypeCount ;
					CardTypeResult[CT_SINGLE].cbCardType = CT_SINGLE ;
					CardTypeResult[CT_SINGLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] = 1;
					CardTypeResult[CT_SINGLE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_SINGLE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				break ;
		}
	case CT_DOUBLE:				//��������
		{
			//�˿˷���
			for (BYTE i=0;i<cbHandCardCount;i++)
			{
				//��������
				BYTE cbSameCount=1;
				BYTE cbLogicValue=GetCardLogicValue(cbTmpCard[i]);

				//����ͬ��
				for (BYTE j=i+1;j<cbHandCardCount;j++)
				{
					//��ȡ�˿�
					if (GetCardLogicValue(cbTmpCard[j])!=cbLogicValue) break;

					//���ñ���
					cbSameCount++;
				}

				if(cbSameCount>=2 && GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0]) && GetCardColor(cbTmpCard[i]) != 0x40)
				{
					BYTE Index = CardTypeResult[CT_DOUBLE].cbCardTypeCount ;
					CardTypeResult[CT_DOUBLE].cbCardType = CT_DOUBLE ;
					CardTypeResult[CT_DOUBLE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_DOUBLE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_DOUBLE].cbEachHandCardCount[Index] = 2;
					CardTypeResult[CT_DOUBLE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_DOUBLE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				//��������
				i+=cbSameCount-1;
			}
			break ;
		}
	case CT_THREE:				//��������
		{
			//�˿˷���
			for (BYTE i=0;i<cbHandCardCount;i++)
			{
				//��������
				BYTE cbSameCount=1;
				BYTE cbLogicValue=GetCardLogicValue(cbTmpCard[i]);

				//����ͬ��
				for (BYTE j=i+1;j<cbHandCardCount;j++)
				{
					//��ȡ�˿�
					if (GetCardLogicValue(cbTmpCard[j])!=cbLogicValue) break;

					//���ñ���
					cbSameCount++;
				}

				if(cbSameCount>=3 && GetCardLogicValue(cbTmpCard[i])>GetCardLogicValue(cbTurnCardData[0]) && GetCardColor(cbTmpCard[i]) != 0x40)
				{
					BYTE Index = CardTypeResult[CT_THREE].cbCardTypeCount ;
					CardTypeResult[CT_THREE].cbCardType = CT_THREE ;
					CardTypeResult[CT_THREE].cbCardData[Index][0] = cbTmpCard[i];
					CardTypeResult[CT_THREE].cbCardData[Index][1] = cbTmpCard[i+1];
					CardTypeResult[CT_THREE].cbCardData[Index][2] = cbTmpCard[i+2];
					CardTypeResult[CT_THREE].cbEachHandCardCount[Index] = 3;
					CardTypeResult[CT_THREE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_THREE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
				//��������
				i+=cbSameCount-1;
			}
			break ;
		}
	case CT_SINGLE_LINE:		//��������
		{
			BYTE cbFirstCard = 0 ;
			//ȥ��2����
            for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

            BYTE cbSingleLineCard[12] = {0};
			BYTE cbSingleLineCount=1 ;
			BYTE cbLeftCardCount = cbHandCardCount ;
			bool bFindSingleLine = true ;

			//�����ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindSingleLine)
			{
				cbSingleLineCount=1 ;
				bFindSingleLine = false ;
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[cbFirstCard] ;
				for (BYTE i=cbFirstCard+1; i<cbLeftCardCount; i++)
				{
					BYTE cbCardData=cbTmpCard[i];

					//�����ж�
					if (1!=(GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbCardData)) && GetCardValue(cbLastCard)!=GetCardValue(cbCardData)) 
					{
						cbLastCard = cbTmpCard[i] ;
						//�Ƿ�Ϸ�
						if(cbSingleLineCount<cbTurnCardCount) 
						{
							cbSingleLineCount = 1 ;
							cbSingleLineCard[cbSingleLineCount-1] = cbTmpCard[i] ;
							continue ;
						}
						else break ;
					}
					//ͬ���ж�
					else if(GetCardValue(cbLastCard)!=GetCardValue(cbCardData))
					{
						cbLastCard = cbCardData ;
						cbSingleLineCard[cbSingleLineCount] = cbCardData ;
						++cbSingleLineCount ;
					}
				}

				//��������
				if(cbSingleLineCount>=cbTurnCardCount && GetCardLogicValue(cbSingleLineCard[0])>GetCardLogicValue(cbTurnCardData[0]))
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//��������
					while (GetCardLogicValue(cbSingleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbSingleLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_SINGLE_LINE].cbCardType = CT_SINGLE_LINE ;
						CopyMemory(CardTypeResult[CT_SINGLE_LINE].cbCardData[Index], cbSingleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount++ ;
						cbStart++;

						ASSERT(CardTypeResult[CT_SINGLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}

					RemoveCard(cbSingleLineCard, cbSingleLineCount, cbTmpCard, cbLeftCardCount) ;
					cbLeftCardCount -= cbSingleLineCount ;
					bFindSingleLine = true ;
				}
			}

			break ;
		}
	case CT_DOUBLE_LINE:		//��������
		{
			//�����ж�
			BYTE cbFirstCard = 0 ;
			//ȥ��2����
            for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

			BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
			bool bFindDoubleLine = true ;
			BYTE cbDoubleLineCount = 0 ;
			BYTE cbDoubleLineCard[24] ;
			//��ʼ�ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindDoubleLine)
			{
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				BYTE cbSameCount = 1 ;
				cbDoubleLineCount = 0 ;
				bFindDoubleLine=false ;
				for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
				{
					//����ͬ��
					while (GetCardValue(cbLastCard)==GetCardValue(cbTmpCard[i]) && i<cbLeftCardCount+cbFirstCard)
					{
						++cbSameCount;
						++i ;
					}

					BYTE cbLastDoubleCardValue ;
					if(cbDoubleLineCount>0) cbLastDoubleCardValue = GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) ;
					//���¿�ʼ
					if((cbSameCount<2 || (cbDoubleLineCount>0 && (cbLastDoubleCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
					{
						if(cbDoubleLineCount>=cbTurnCardCount) break ;

						if(cbSameCount>=2) i-=cbSameCount ;

						cbLastCard = cbTmpCard[i] ;
						cbDoubleLineCount = 0 ;
					}
					//��������
					else if(cbSameCount>=2)
					{
						cbDoubleLineCard[cbDoubleLineCount] = cbTmpCard[i-cbSameCount] ;
						cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCard[i-cbSameCount+1] ;
						cbDoubleLineCount += 2 ;

						//��β�ж�
						if(i==(cbLeftCardCount+cbFirstCard-2))
							if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCard[i]))==1 && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+1])))
							{
								cbDoubleLineCard[cbDoubleLineCount] = cbTmpCard[i] ;
								cbDoubleLineCard[cbDoubleLineCount+1] = cbTmpCard[i+1] ;
								cbDoubleLineCount += 2 ;
								break ;
							}

					}

					cbLastCard = cbTmpCard[i] ;
					cbSameCount = 1 ;
				}

				//��������
				if(cbDoubleLineCount>=cbTurnCardCount)
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//��������
					while (GetCardLogicValue(cbDoubleLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbDoubleLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_DOUBLE_LINE].cbCardType = CT_DOUBLE_LINE ;
						CopyMemory(CardTypeResult[CT_DOUBLE_LINE].cbCardData[Index], cbDoubleLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_DOUBLE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount++ ;
						cbStart += 2;

						ASSERT(CardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}
					RemoveCard(cbDoubleLineCard, cbDoubleLineCount, cbTmpCard, cbFirstCard+cbLeftCardCount) ;				
					bFindDoubleLine=true ;
					cbLeftCardCount -= cbDoubleLineCount ;
				}
			}

			break;
		}
	case CT_THREE_LINE:			//��������
		{
			//�����ж�
			BYTE cbFirstCard = 0 ;
			//ȥ��2����
            for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

			BYTE cbLeftCardCount = cbHandCardCount-cbFirstCard ;
			bool bFindThreeLine = true ;
			BYTE cbThreeLineCount = 0 ;
			BYTE cbThreeLineCard[20] ;
			//��ʼ�ж�
			while (cbLeftCardCount>=cbTurnCardCount && bFindThreeLine)
			{
				BYTE cbLastCard = cbTmpCard[cbFirstCard] ;
				BYTE cbSameCount = 1 ;
				cbThreeLineCount = 0 ;
				bFindThreeLine = false ;
				for(BYTE i=cbFirstCard+1 ; i<cbLeftCardCount+cbFirstCard ; ++i)
				{
					//����ͬ��
					while (GetCardValue(cbLastCard)==GetCardValue(cbTmpCard[i]) && i<cbLeftCardCount+cbFirstCard)
					{
						++cbSameCount;
						++i ;
					}

					BYTE cbLastThreeCardValue ;
					if(cbThreeLineCount>0) cbLastThreeCardValue = GetCardLogicValue(cbThreeLineCard[cbThreeLineCount-1]) ;

					//���¿�ʼ
					if((cbSameCount<3 || (cbThreeLineCount>0&&(cbLastThreeCardValue-GetCardLogicValue(cbLastCard))!=1)) && i<=cbLeftCardCount+cbFirstCard)
					{
						if(cbThreeLineCount>=cbTurnCardCount) break ;

						if(cbSameCount>=3) i-= 3 ;
						cbLastCard = cbTmpCard[i] ;
						cbThreeLineCount = 0 ;
					}
					//��������
					else if(cbSameCount>=3)
					{
						cbThreeLineCard[cbThreeLineCount] = cbTmpCard[i-cbSameCount] ;
						cbThreeLineCard[cbThreeLineCount+1] = cbTmpCard[i-cbSameCount+1] ;
						cbThreeLineCard[cbThreeLineCount+2] = cbTmpCard[i-cbSameCount+2] ;
						cbThreeLineCount += 3 ;

						//��β�ж�
						if(i==(cbLeftCardCount+cbFirstCard-3))
							if((GetCardLogicValue(cbLastCard)-GetCardLogicValue(cbTmpCard[i]))==1 && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+1])) && (GetCardLogicValue(cbTmpCard[i])==GetCardLogicValue(cbTmpCard[i+2])))
							{
								cbThreeLineCard[cbThreeLineCount] = cbTmpCard[i] ;
								cbThreeLineCard[cbThreeLineCount+1] = cbTmpCard[i+1] ;
								cbThreeLineCard[cbThreeLineCount+2] = cbTmpCard[i+2] ;
								cbThreeLineCount += 3 ;
								break ;
							}

					}

					cbLastCard = cbTmpCard[i] ;
					cbSameCount = 1 ;
				}

				//��������
				if(cbThreeLineCount>=cbTurnCardCount)
				{
					BYTE Index ;
					BYTE cbStart=0 ;
					//��������
					while (GetCardLogicValue(cbThreeLineCard[cbStart])>GetCardLogicValue(cbTurnCardData[0]) && ((cbThreeLineCount-cbStart)>=cbTurnCardCount))
					{
						Index = CardTypeResult[CT_THREE_LINE].cbCardTypeCount ;
						CardTypeResult[CT_THREE_LINE].cbCardType = CT_THREE_LINE ;
						CopyMemory(CardTypeResult[CT_THREE_LINE].cbCardData[Index], cbThreeLineCard+cbStart, sizeof(BYTE)*cbTurnCardCount);
						CardTypeResult[CT_THREE_LINE].cbEachHandCardCount[Index] = cbTurnCardCount;
						CardTypeResult[CT_THREE_LINE].cbCardTypeCount++ ;
						cbStart += 3;

						ASSERT(CardTypeResult[CT_THREE_LINE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					}

					RemoveCard(cbThreeLineCard, cbThreeLineCount, cbTmpCard, cbFirstCard+cbLeftCardCount) ;
					bFindThreeLine=true ;
					cbLeftCardCount -= cbThreeLineCount ;
				}
			}

			break;
		}
	case CT_THREE_LINE_TAKE_ONE://����һ��
		{
            BYTE cbTurnThreeCard[MAX_COUNT] = {0}; 
			BYTE cbTurnThreeCount=0;
			BYTE cbHandThreeCard[MAX_COUNT] = {0}; 
			BYTE cbHandThreeCount=0 ;
			BYTE cbSingleCardCount=cbTurnCardCount/4;

			//�Ƴ�ը��
			BYTE cbAllBomCardData[MAX_COUNT] = {0}; 
			BYTE cbAllBomCardCount=0 ;
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
            RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCard, cbHandCardCount) ;

			//�����˿�
			GetAllThreeCard(cbTurnCardData, cbTurnCardCount, cbTurnThreeCard, cbTurnThreeCount) ;

			BYTE cbFirstCard = 0 ;

			//ȥ��2����
			if(cbTurnThreeCount>3)
                for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

					GetAllThreeCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard-cbAllBomCardCount, cbHandThreeCard, cbHandThreeCount) ;

					if(cbHandThreeCount<cbTurnThreeCount || (cbHandThreeCount>0&&GetCardLogicValue(cbHandThreeCard[0])<GetCardLogicValue(cbTurnThreeCard[0]))) return ;

					for(BYTE i=0; i<cbHandThreeCount; i+=3)
					{
						BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[i]);
						BYTE cbThreeLineCard[MAX_COUNT];
						BYTE cbThreeLineCardCount=3;
						cbThreeLineCard[0]=cbHandThreeCard[i];
						cbThreeLineCard[1]=cbHandThreeCard[i+1];
						cbThreeLineCard[2]=cbHandThreeCard[i+2];
						for(BYTE j=i+3; j<cbHandThreeCount; j+=3)
						{
							//�����ж�
							if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))) || cbThreeLineCardCount==cbTurnThreeCount) break;

							cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
							cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
							cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
							cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
							cbThreeLineCardCount += 3;
						}
						if(cbThreeLineCardCount==cbTurnThreeCount && GetCardLogicValue(cbThreeLineCard[0])>GetCardLogicValue(cbTurnThreeCard[0]))
						{
							BYTE Index ;

							BYTE cbRemainCard[MAX_COUNT];
							CopyMemory(cbRemainCard, cbTmpCard, (cbHandCardCount-cbAllBomCardCount)*sizeof(BYTE));
							RemoveCard(cbThreeLineCard, cbTurnThreeCount, cbRemainCard, (cbHandCardCount-cbAllBomCardCount)) ;

							//�������
							BYTE cbComCard[5];
							BYTE cbComResCard[254][5] ;
							BYTE cbComResLen=0 ;
							Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, cbSingleCardCount, (cbHandCardCount-cbAllBomCardCount)-cbTurnThreeCount, cbSingleCardCount);
							for(BYTE i=0; i<cbComResLen; ++i)
							{
								Index = CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardType = CT_THREE_LINE_TAKE_ONE;
								//��������
								CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbTurnThreeCount);
								//���浥��
								CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardData[Index]+cbTurnThreeCount, cbComResCard[i], cbSingleCardCount) ;

								ASSERT(cbTurnThreeCount+cbSingleCardCount==cbTurnCardCount) ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbEachHandCardCount[Index] = cbTurnCardCount ;
								CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount++ ;

								ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
							}

						}
					}

					break;
		}
	case CT_THREE_LINE_TAKE_TWO://����һ��
		{
			BYTE cbTurnThreeCard[MAX_COUNT] = {0}; 
			BYTE cbTurnThreeCount=0;
			BYTE cbHandThreeCard[MAX_COUNT] = {0}; 
			BYTE cbHandThreeCount=0 ;
			BYTE cbDoubleCardCount=cbTurnCardCount/5;

/*            //�Ƴ�ը��
            BYTE cbAllBomCardData[MAX_COUNT] ;
            BYTE cbAllBomCardCount=0 ;
            GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
            RemoveCard(cbAllBomCardData, cbAllBomCardCount, cbTmpCard, cbHandCardCount) ;
*/
			//�����˿�
			GetAllThreeCard(cbTurnCardData, cbTurnCardCount, cbTurnThreeCard, cbTurnThreeCount) ;

			BYTE cbFirstCard = 0 ;

			//ȥ��2����
			if(cbTurnThreeCount>3)
                for(BYTE i=0 ; i<cbHandCardCount ; ++i, cbFirstCard = i) {if(GetCardLogicValue(cbTmpCard[i])<15) {break;}}

				GetAllThreeCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandThreeCard, cbHandThreeCount) ;

				if(cbHandThreeCount<cbTurnThreeCount || (cbHandThreeCount>0&&GetCardLogicValue(cbHandThreeCard[0])<GetCardLogicValue(cbTurnThreeCard[0]))) return ;

				for(BYTE i=0; i<cbHandThreeCount; i+=3)
				{
					BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[i]);
					BYTE cbThreeLineCard[MAX_COUNT];
					BYTE cbThreeLineCardCount=3;
					cbThreeLineCard[0]=cbHandThreeCard[i];
					cbThreeLineCard[1]=cbHandThreeCard[i+1];
					cbThreeLineCard[2]=cbHandThreeCard[i+2];
					for(BYTE j=i+3; j<cbHandThreeCount; j+=3)
					{
						//�����ж�
						if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[j]))) || cbThreeLineCardCount==cbTurnThreeCount) break;

						cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[j]);
						cbThreeLineCard[cbThreeLineCardCount]=cbHandThreeCard[j];
						cbThreeLineCard[cbThreeLineCardCount+1]=cbHandThreeCard[j+1];
						cbThreeLineCard[cbThreeLineCardCount+2]=cbHandThreeCard[j+2];
						cbThreeLineCardCount += 3;
					}
					if(cbThreeLineCardCount==cbTurnThreeCount && GetCardLogicValue(cbThreeLineCard[0])>GetCardLogicValue(cbTurnThreeCard[0]))
					{
						BYTE Index ;

						BYTE cbRemainCard[MAX_COUNT];
						CopyMemory(cbRemainCard, cbTmpCard, cbHandCardCount*sizeof(BYTE));
						RemoveCard(cbThreeLineCard, cbTurnThreeCount, cbRemainCard, cbHandCardCount) ;

						BYTE cbAllDoubleCardData[MAX_COUNT] ;
						BYTE cbAllDoubleCardCount=0 ;
						GetAllDoubleCard(cbRemainCard, cbHandCardCount-cbTurnThreeCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;


						BYTE cbDoubleCardIndex[10]; //�����±�
						for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
							cbDoubleCardIndex[j]=i ;

						//�������
						BYTE cbComCard[5];
						BYTE cbComResCard[254][5] ;
						BYTE cbComResLen=0 ;

						//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
						Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, cbDoubleCardCount, cbAllDoubleCardCount/2, cbDoubleCardCount);
						for(BYTE i=0; i<cbComResLen; ++i)
						{
							Index = CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount ;
							CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardType = CT_THREE_LINE_TAKE_TWO ;
							//��������
							CopyMemory(CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index], cbThreeLineCard, sizeof(BYTE)*cbTurnThreeCount);
							//�������
							for(BYTE j=0, k=0; j<cbDoubleCardCount; ++j, k+=2)
							{
								CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][cbTurnThreeCount+k] = cbAllDoubleCardData[cbComResCard[i][j]];
								CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardData[Index][cbTurnThreeCount+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
							}

							ASSERT(cbTurnThreeCount+cbDoubleCardCount*2==cbTurnCardCount) ;
							CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbEachHandCardCount[Index] = cbTurnCardCount ;

							CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount++ ;

							ASSERT(CardTypeResult[CT_THREE_LINE_TAKE_TWO].cbCardTypeCount<=MAX_TYPE_COUNT) ;
						}		

					}
				}

				break;
		}
	/* //���ֳ�û���Ĵ�����
    case CT_FOUR_LINE_TAKE_ONE://�Ĵ�����
		{
			BYTE cbFirstCard = 0 ;
			//ȥ������
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCard[i])!=0x40)	{cbFirstCard = i ; break ;}

			BYTE cbHandAllFourCardData[MAX_COUNT] ;
			BYTE cbHandAllFourCardCount=0;
			BYTE cbTurnAllFourCardData[MAX_COUNT];
			BYTE cbTurnAllFourCardCount=0;
			//��ȡ����
			GetAllBomCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;
			GetAllBomCard(cbTurnCardData, cbTurnCardCount, cbTurnAllFourCardData, cbTurnAllFourCardCount) ;

			if(cbHandAllFourCardCount>0 && GetCardLogicValue(cbHandAllFourCardData[0])<GetCardLogicValue(cbTurnAllFourCardData[0])) return ;


			BYTE cbCanOutFourCardData[MAX_COUNT] ;
			BYTE cbCanOutFourCardCount=0 ;
			//�ɳ�����
			for(BYTE i=0; i<cbHandAllFourCardCount; i+=4)
			{
				if(GetCardLogicValue(cbHandAllFourCardData[i])>GetCardLogicValue(cbTurnAllFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandAllFourCardData[i] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandAllFourCardData[i+1] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandAllFourCardData[i+2] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandAllFourCardData[i+3] ;
					cbCanOutFourCardCount += 4 ;
				}
			}

			if((cbHandCardCount-cbCanOutFourCardCount) < (cbTurnCardCount-cbTurnAllFourCardCount)) return ;

			BYTE cbRemainCard[MAX_COUNT];
			CopyMemory(cbRemainCard, cbTmpCard, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCard, cbHandCardCount) ;
			for(BYTE Start=0; Start<cbCanOutFourCardCount; Start += 4)
			{
				BYTE Index ;
				//�������
				BYTE cbComCard[5];
				BYTE cbComResCard[254][5] ;
				BYTE cbComResLen=0 ;
				//�������
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbRemainCard, 2, cbHandCardCount-cbCanOutFourCardCount, 2);
				for(BYTE i=0; i<cbComResLen; ++i)
				{
					//���ܴ���
					if(GetCardValue(cbComResCard[i][0])==GetCardValue(cbComResCard[i][1])) continue ;

					Index=CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardType = CT_FOUR_LINE_TAKE_ONE ;
					CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index], cbCanOutFourCardData+Start, 4) ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4] = cbComResCard[i][0] ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardData[Index][4+1] = cbComResCard[i][1] ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbEachHandCardCount[Index] = 6 ;
					CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_ONE].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}


			break;
		}
	case CT_FOUR_LINE_TAKE_TWO://�Ĵ�����
		{
			BYTE cbFirstCard = 0 ;
			//ȥ������
			for(BYTE i=0 ; i<cbHandCardCount ; ++i)	if(GetCardColor(cbTmpCard[i])!=0x40)	{cbFirstCard = i ; break ;}

			BYTE cbHandAllFourCardData[MAX_COUNT] ;
			BYTE cbHandAllFourCardCount=0;
			BYTE cbTurnAllFourCardData[MAX_COUNT];
			BYTE cbTurnAllFourCardCount=0;
			//��ȡ����
			GetAllBomCard(cbTmpCard+cbFirstCard, cbHandCardCount-cbFirstCard, cbHandAllFourCardData, cbHandAllFourCardCount) ;
			GetAllBomCard(cbTurnCardData, cbTurnCardCount, cbTurnAllFourCardData, cbTurnAllFourCardCount) ;

			if(cbHandAllFourCardCount>0 && GetCardLogicValue(cbHandAllFourCardData[0])<GetCardLogicValue(cbTurnAllFourCardData[0])) return ;


			BYTE cbCanOutFourCardData[MAX_COUNT] ;
			BYTE cbCanOutFourCardCount=0 ;
			//�ɳ�����
			for(BYTE i=0; i<cbHandAllFourCardCount; i+=4)
			{
				if(GetCardLogicValue(cbHandAllFourCardData[i])>GetCardLogicValue(cbTurnAllFourCardData[0]))
				{
					cbCanOutFourCardData[cbCanOutFourCardCount] = cbHandAllFourCardData[i] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+1] = cbHandAllFourCardData[i+1] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+2] = cbHandAllFourCardData[i+2] ;
					cbCanOutFourCardData[cbCanOutFourCardCount+3] = cbHandAllFourCardData[i+3] ;
					cbCanOutFourCardCount += 4 ;
				}
			}

			if((cbHandCardCount-cbCanOutFourCardCount) < (cbTurnCardCount-cbTurnAllFourCardCount)) return ;

			BYTE cbRemainCard[MAX_COUNT];
			CopyMemory(cbRemainCard, cbTmpCard, cbHandCardCount*sizeof(BYTE));
			RemoveCard(cbCanOutFourCardData, cbCanOutFourCardCount, cbRemainCard, cbHandCardCount) ;
			for(BYTE Start=0; Start<cbCanOutFourCardCount; Start += 4)
			{
				BYTE cbAllDoubleCardData[MAX_COUNT] ;
				BYTE cbAllDoubleCardCount=0 ;
				GetAllDoubleCard(cbRemainCard, cbHandCardCount-cbCanOutFourCardCount, cbAllDoubleCardData, cbAllDoubleCardCount) ;


				BYTE cbDoubleCardIndex[10]; //�����±�
				for(BYTE i=0, j=0; i<cbAllDoubleCardCount; i+=2, ++j)
					cbDoubleCardIndex[j]=i ;

				//�������
				BYTE cbComCard[5];
				BYTE cbComResCard[254][5] ;
				BYTE cbComResLen=0 ;

				//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
				Combination(cbComCard, 0, cbComResCard, cbComResLen, cbDoubleCardIndex, 2, cbAllDoubleCardCount/2, 2);
				for(BYTE i=0; i<cbComResLen; ++i)
				{
					BYTE Index = CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount ;
					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardType = CT_FOUR_LINE_TAKE_TWO ;
					CopyMemory(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index], cbCanOutFourCardData+Start, 4) ;

					//�������
					for(BYTE j=0, k=0; j<4; ++j, k+=2)
					{
						CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k] = cbAllDoubleCardData[cbComResCard[i][j]];
						CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardData[Index][4+k+1] = cbAllDoubleCardData[cbComResCard[i][j]+1];
					}

					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbEachHandCardCount[Index] = 8 ;
					CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_FOUR_LINE_TAKE_TWO].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}
			break;
		}
        */
	case CT_BOMB_CARD:			//ը������
	case CT_BOMB_CARD_5:			//ը������
	case CT_BOMB_CARD_6:			//ը������
	case CT_BOMB_CARD_7:			//ը������
	case CT_BOMB_CARD_8:			//ը������
	case CT_MISSILE_CARD:			//ը������
	case CT_MISSILE_CARD_3:			//ը������
	case CT_MISSILE_CARD_4:			//ը������
	case CT_MISSILE_CARD_5:			//ը������
		{
            BYTE cbAllBomCardData[MAX_COUNT]  = {0};
			BYTE cbAllBomCardCount=0 ; 
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
			BYTE j = 0;
			BYTE cbFirstBom=0 ;
			BYTE Index ;
			if(cbHandCardCount>=2 && (0x4F==cbTmpCard[0] || 0x4E==cbTmpCard[0]) && (0x4E==cbTmpCard[1] || 0x4F==cbTmpCard[1]))
			{
				for (j = 0 ; j < cbHandCardCount; j++)
				{
					if (cbTmpCard[j] == 0x4E || cbTmpCard[j] == 0x4F)
					{
						cbAllBomCardData[j] = cbTmpCard[j] ;
					}
					else
						break;
				}
				if (CompareCard(cbTurnCardData, cbAllBomCardData, cbTurnCardCount, j))
				{
					Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
					CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
					CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbAllBomCardData, j);
					CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = j ;
					CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
					cbFirstBom=j;
				}
            }

			BYTE bTempLogicCardData = cbAllBomCardData[j];
			BYTE EachIndex = 0;
			for (BYTE i=j; i<cbAllBomCardCount; i++)
			{
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++;

				BYTE Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1 ;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CardTypeResult[CT_BOMB_CARD].cbCardData[Index][EachIndex++] = cbAllBomCardData[i] ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = EachIndex ;
				if (GetCardLogicValue(bTempLogicCardData) != GetCardLogicValue(cbAllBomCardData[i+1]))
				{

                    if (CompareCard(cbTurnCardData, CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbTurnCardCount, EachIndex)) //�������ͱ���
                    {
                        bTempLogicCardData = cbAllBomCardData[i+1];
                        EachIndex = 0;
                    }
                    else
                    {
                        bTempLogicCardData = cbAllBomCardData[i+1];
                        EachIndex = 0;
                        CardTypeResult[CT_BOMB_CARD].cbCardTypeCount--;
                    }

				}
				else
					CardTypeResult[CT_BOMB_CARD].cbCardTypeCount--;

				ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<MAX_TYPE_COUNT) ;
			}


			//////////////////////////////////////////////////////////////////////////
/*			BYTE cbAllBomCardData[MAX_COUNT] ;
			BYTE cbAllBomCardCount=0 ; 
			GetAllBomCard(cbTmpCard, cbHandCardCount, cbAllBomCardData, cbAllBomCardCount) ;
			BYTE cbFirstBom=0 ;
			BYTE Index ;
			if(cbAllBomCardCount>0 && cbAllBomCardData[0]==0x4F)
			{
				Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
				CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
				CardTypeResult[CT_BOMB_CARD].cbCardData[Index][0] = 0x4F ;
				CardTypeResult[CT_BOMB_CARD].cbCardData[Index][1] = 0x4E ;
				CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 2 ;
				CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;

				ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				cbFirstBom=2;
			}
			for(BYTE i=cbFirstBom; i<cbAllBomCardCount; i+=4)
			{
				if(GetCardLogicValue(cbAllBomCardData[i])>GetCardLogicValue(cbTurnCardData[0]))
				{
					Index = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount ;
					CardTypeResult[CT_BOMB_CARD].cbCardType = CT_BOMB_CARD ;
					CopyMemory(CardTypeResult[CT_BOMB_CARD].cbCardData[Index], cbAllBomCardData+i, 4) ;
					CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[Index] = 4 ;
					CardTypeResult[CT_BOMB_CARD].cbCardTypeCount++ ;

					ASSERT(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount<=MAX_TYPE_COUNT) ;
				}
			}*/
			break;
		}
	case CT_MISSILE_CARD_6:		//������� 6��
		{
			//û�б�6��������������
			break;
		}
    case CT_JET:
        {
            break;
        }
	default:
		ASSERT(false) ;
		break;
	}

}


/********************************************************************
��������Combination
������
cbCombineCardData���洢��������Ͻ��
cbResComLen���ѵõ�����ϳ��ȣ���ʼ����ʱ�˲���Ϊ0
cbResultCardData���洢���е���Ͻ��
cbResCardLen��cbResultCardData�ĵ�һ�±�ĳ��ȣ���Ͻ���ĸ���
cbSrcCardData����Ҫ����ϵ�����
cbSrcLen��cbSrcCardData��������Ŀ
cbCombineLen2��cbCombineLen1����ϵĳ��ȣ���ʼ����ʱ������ȡ�
*********************************************************************/
//����㷨
void CGameLogic::Combination(BYTE cbCombineCardData[], BYTE cbResComLen,  BYTE cbResultCardData[100][5], BYTE &cbResCardLen,BYTE cbSrcCardData[] , BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2)
{

	if( cbResComLen == cbCombineLen2 )
	{
		CopyMemory(&cbResultCardData[cbResCardLen], cbCombineCardData, cbResComLen) ;
		++cbResCardLen ;

		ASSERT(cbResCardLen<255) ;

	}
	else
	{ 
		if(cbCombineLen1 >= 1 && cbSrcLen > 0 && (cbSrcLen+1) >= 0 ){
			cbCombineCardData[cbCombineLen2-cbCombineLen1] =  cbSrcCardData[0];
			++cbResComLen ;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1-1, cbSrcLen-1, cbCombineLen2);

			--cbResComLen;
			Combination(cbCombineCardData,cbResComLen, cbResultCardData, cbResCardLen, cbSrcCardData+1,cbCombineLen1, cbSrcLen-1, cbCombineLen2);
		}
	}
}

//�����㷨
void CGameLogic::Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len)
{ 
	int j,temp; 
	if(m == n){ 
		for(j = 0; j < n; j++) 
			result[len][j]=list[j]; 
		len++ ;
	} 
	else{ 
		for(j = m; j < n; j++){ 
			temp = list[m] ;
			list[m] = list[j];
			list[j] = temp ;
			Permutation(list,m+1,n,result,len); 
			temp = list[m] ;
			list[m] = list[j];
			list[j] = temp ;
		} 
	} 
} 

//���Ƹ���
BYTE CGameLogic::AnalyseSinleCardCount(BYTE const cbHandCardData[], BYTE const cbHandCardCount, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE cbSingleCardData[])
{
	//�����ж�
	ASSERT(cbHandCardCount>0) ;
	if(cbHandCardCount<=0) return MAX_COUNT + 5 ;

	BYTE cbRemainCard[MAX_COUNT] ;
	BYTE cbRemainCardCount=0 ;
	CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
	SortCardList(cbRemainCard, cbHandCardCount, ST_ORDER) ;

	//�Ƴ��˿�
	if(cbWantOutCardCount!=0) RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbHandCardCount) ;
	cbRemainCardCount = cbHandCardCount-cbWantOutCardCount ;

	//����ָ��
	typedef void (CGameLogic::*pGetAllCardFun)(BYTE const [], BYTE const , BYTE[], BYTE &); 

	//ָ������
	pGetAllCardFun GetAllCardFunArray[4] ;
	GetAllCardFunArray[0] = &CGameLogic::GetAllBomCard ;		//ը������
	GetAllCardFunArray[1] = &CGameLogic::GetAllLineCard ;	//˳�Ӻ���
	GetAllCardFunArray[2] = &CGameLogic::GetAllThreeCard ;	//��������
	GetAllCardFunArray[3] = &CGameLogic::GetAllDoubleCard ;	//���Ӻ���

	//ָ�������±�
	BYTE cbIndexArray[4] = {0,1,2,3} ;
	//���н��
	BYTE cbPermutationRes[24][4] ;
	BYTE cbLen=0 ;
	//��������
	Permutation(cbIndexArray, 0, 4, cbPermutationRes, cbLen) ;
	ASSERT(cbLen==24) ;
	if(cbLen!=24) return MAX_COUNT + 5 ;

	//������Ŀ
	BYTE cbMinSingleCardCount = MAX_COUNT + 5 ;
	//������Сֵ
	for(BYTE i=0; i<24; ++i)
	{
		//��������
		BYTE cbTmpCardData[MAX_COUNT] ;
		BYTE cbTmpCardCount = cbRemainCardCount ;
		CopyMemory(cbTmpCardData, cbRemainCard, cbRemainCardCount) ;

		for(BYTE j=0; j<4; ++j)
		{
			BYTE Index = cbPermutationRes[i][j] ;

			//У���±�
			ASSERT(Index>=0 && Index<4) ;
			if(Index<0 || Index>=4) return MAX_COUNT + 5 ;

			pGetAllCardFun pTmpGetAllCardFun = GetAllCardFunArray[Index] ;

			//��ȡ�˿�
            BYTE cbGetCardData[MAX_COUNT] = {0};
			BYTE cbGetCardCount=0 ;
			//��Ա����
            if (pTmpGetAllCardFun ==  &CGameLogic::GetAllLineCard)
                continue;
			((*this).*pTmpGetAllCardFun)(cbTmpCardData, cbTmpCardCount, cbGetCardData, cbGetCardCount) ;

			//ɾ���˿�
			if(cbGetCardCount!=0) RemoveCard(cbGetCardData, cbGetCardCount, cbTmpCardData, cbTmpCardCount) ;
			cbTmpCardCount -= cbGetCardCount ;
		}

		//���㵥��
		BYTE cbSingleCard[MAX_COUNT] ;
		BYTE cbSingleCardCount=0 ;
		GetAllSingleCard(cbTmpCardData, cbTmpCardCount, cbSingleCard, cbSingleCardCount) ;
		if(cbMinSingleCardCount> cbSingleCardCount)
		{
			cbMinSingleCardCount = cbSingleCardCount ;

			//���浥��
			if(cbSingleCardData!=NULL)
			{
				CopyMemory(cbSingleCardData, cbSingleCard, cbSingleCardCount) ;
			}
		}
	}

	//�������ж�
	if (cbWantOutCardCount > 0)
	{
		//��������
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;
		if (cbCardType == CT_THREE_LINE_TAKE_ONE || cbCardType == CT_THREE_LINE_TAKE_TWO)

		for (BYTE i = 3; i < cbWantOutCardCount; ++i)
		{
			if (GetCardLogicValue(cbWantOutCardData[i]) >= 14) cbMinSingleCardCount += 3 ;
		}
	}

	//�������ж�
	if ( GetCardType(cbWantOutCardData, cbWantOutCardCount) == CT_DOUBLE )
	{
		BYTE cbAllThreeCardData[MAX_COUNT], cbAllThreeCount ;
		BYTE cbAllLineCardData[MAX_COUNT], cbAllLineCount ;

		GetAllThreeCard(cbHandCardData, cbHandCardCount, cbAllThreeCardData, cbAllThreeCount) ;
		GetAllLineCard( cbHandCardData, cbHandCardCount, cbAllLineCardData, cbAllLineCount ) ;

		BYTE cbThreeCardValue = 0 ;
		for ( BYTE i = 0; i < cbAllThreeCount; ++i)
			for ( BYTE j = 0; j < cbWantOutCardCount; ++j)
				if ( GetCardLogicValue(cbWantOutCardData[j]) == GetCardLogicValue(cbAllThreeCardData[i]) )
				{
					cbThreeCardValue = GetCardValue( cbWantOutCardData[j] ) ;
					break ;
				}

		//�Ƿ�������
		bool bInLineCard = false ;
		for ( BYTE cbLineCardIndex = 0; cbLineCardIndex < cbAllLineCount; ++cbLineCardIndex )
			if ( GetCardValue( cbAllLineCardData[cbLineCardIndex] ) == cbThreeCardValue )
			{
				bInLineCard = true ;
				break ;
			}
		if ( !bInLineCard && cbThreeCardValue != 0 ) cbMinSingleCardCount += 2 ;
	}

	//��ը�ж�
	if(cbWantOutCardCount!=0)
	{
		//ը���˿�
		BYTE cbBombCard[20] ;
		BYTE cbBombCardCount=0;
        //GetAllBomCard(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCardCount) ;
        tagBomCardResult BomCardResult = {0};
        GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCardCount, BomCardResult, ST_MISSILE_FIRST);

		//��������
		BYTE cbCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;

		if(cbBombCardCount>0 && cbCardType<CT_BOMB_CARD)
		{
            BYTE m = GetCardColor(cbBombCard[0]) == 0x40 ? 1 : 0;
            BYTE k = GetCardColor(cbBombCard[0]) == 0x40 ? BomCardResult.cbCardCount[0] : 0;
            BYTE i = k;
            //Ѱ����ͬ
            for( ; i<cbBombCardCount; i += k)
            {
                for(BYTE j=0; j<cbWantOutCardCount; ++j)
                {
                    if(GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] ) /*&& GetCardLogicValue(cbWantOutCardData[j])<=15*/ && 
                        cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE) 
                        return MAX_COUNT + 5 ; //����ը��

                    else if ( GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] )/* && GetCardLogicValue(cbWantOutCardData[j])<=15*/ && 
                        ( cbCardType == CT_SINGLE_LINE || cbCardType == CT_DOUBLE_LINE) ) 
                        cbMinSingleCardCount += 2 ; //����ը��
                }
                k = BomCardResult.cbCardCount[m++];
            }

			//���ը���ж�
			if (cbCardType == CT_SINGLE_LINE)
			{
				BYTE cbBombCount = 0 ;

                i = GetCardColor(cbBombCard[0]) == 0x40 ? BomCardResult.cbCardCount[0] : 0;
                k = GetCardColor(cbBombCard[0]) == 0x40 ? 1 : 0;
				for ( ; i < cbBombCardCount; i += BomCardResult.cbCardCount[k])
					for (BYTE j=0; j < cbWantOutCardCount; ++j)
						if ( GetCardValue( cbBombCard[i] ) == GetCardValue( cbWantOutCardData[j] ) ) ++cbBombCount ;

				if (cbBombCount >= 2) return  MAX_COUNT ; //����ը��

				//��������
				tagAnalyseResult AnalyseResult;
				AnalysebCardData(cbHandCardData, cbHandCardCount,AnalyseResult) ;

                BYTE cbThreeCount = 0 ;

				for (BYTE i = 0; i < AnalyseResult.cbThreeCount; ++i)
					for (BYTE j = 0; j < cbWantOutCardCount; ++j)
						if (GetCardValue(cbWantOutCardData[j]) == GetCardValue(AnalyseResult.cbThreeCardData[3 * i])) ++cbThreeCount ;

				if (cbThreeCount + cbBombCount >= 2) return  MAX_COUNT + 5 ;

			}
		}
	}

	return cbMinSingleCardCount ;
}

//�����˿�
void CGameLogic::SetUserCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	CopyMemory(m_cbAllCardData[wChairID], cbCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] = cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], cbCardCount, ST_ORDER) ;
}

//���õ���
void CGameLogic::SetBackCard(WORD wChairID, BYTE cbBackCardData[], BYTE cbCardCount)
{
	BYTE cbTmpCount = m_cbUserCardCount[wChairID] ;
	CopyMemory(m_cbAllCardData[wChairID]+cbTmpCount, cbBackCardData, cbCardCount*sizeof(BYTE)) ;
	m_cbUserCardCount[wChairID] += cbCardCount ;

	//�����˿�
	SortCardList(m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID], ST_ORDER) ;
}

//����ׯ��
void CGameLogic::SetBanker(WORD wBanker) 
{
	m_wBankerUser = wBanker ;
	m_lBankerOutCardCount = 0 ;
}
//�����˿�
void CGameLogic::SetLandScoreCardData(BYTE cbCardData[], BYTE cbCardCount) 
{
    if (m_bRandCardMode == enRandCard_DoubleGoodCard)
    {
        ASSERT(cbCardCount==MAX_COUNT-2) ;
        if(cbCardCount!=MAX_COUNT-2) return ;
    }
    else
    {
        ASSERT(cbCardCount==MAX_COUNT) ;
        if(cbCardCount!=MAX_COUNT) return ;
    }

	CopyMemory(m_cbLandScoreCardData, cbCardData, cbCardCount*sizeof(BYTE)) ;
	//�����˿�
	SortCardList(m_cbLandScoreCardData, cbCardCount, ST_ORDER) ;
};
//ɾ���˿�
void CGameLogic::RemoveUserCardData(WORD wChairID, BYTE cbRemoveCardData[], BYTE cbRemoveCardCount) 
{
	bool bSuccess = RemoveCard(cbRemoveCardData, cbRemoveCardCount, m_cbAllCardData[wChairID], m_cbUserCardCount[wChairID]) ;
	ASSERT(bSuccess) ;
	m_cbUserCardCount[wChairID] -= cbRemoveCardCount ;

}

//�������ƣ��ȳ��ƣ�
void CGameLogic::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult & OutCardResult) 
{
	//���±�û��CT_JET
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;

    g_Logger.WriteLog("---------------BankerOutCard1 ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);

    //��ʣ��ը��
    BYTE cbBomCardData[MAX_COUNT] = {0};
    BYTE cbBomCardCount = 0;
    tagBomCardResult BomCardResult = {0};
    AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
    GetAllBomCardOrder(cbHandCardData,cbHandCardCount, cbBomCardData, cbBomCardCount, BomCardResult);
    if (cbBomCardCount == cbHandCardCount) 
    {
        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
        return ;
    }

	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//�����¼�
	WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//�����ϼ�

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				if(TestOutAllCard(m_wBankerUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD|| cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 || cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 ||
                       cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					if (cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}

				}
			}
		}
		//ֱ�ӷ���
		if (bFindBestCard)
        {
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

	//����������
	if ( cbHandCardCount == 8 && GetCardLogicValue(cbHandCardData[5]) == 16 && m_cbUserCardCount[wUndersideOfBanker] == 1 && 
		GetCardLogicValue(cbHandCardData[6]) < GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[6] ;
        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//�Ĵ������ж� ���ֳ���֧��
// 	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], OutCardResult ) )
// 	{
//         g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 		return ;
// 	}

	//����������
	if ( cbHandCardCount == 8 && GetCardLogicValue(cbHandCardData[5]) == 16 && m_cbUserCardCount[wUpsideOfBanker] == 1 && 
		GetCardLogicValue(cbHandCardData[6]) < GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[6] ;
        g_Logger.WriteLog("---------------UndersideOBankerOutCard1fBankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

    //�Ĵ������ж� ���ֳ���֧��
// 	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], OutCardResult ) )
// 	{
//         g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 		return ;
// 	}

	//���ֻʣ˳�ƺ͵�ֻ�����ȳ�˳
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//˫��ը����һ��
		else if(cbHandCardCount>6 && cbHandCardData[5]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
			GetCardType(cbHandCardData+6, cbHandCardCount-6) !=CT_FOUR_LINE_TAKE_ONE && 
			GetCardType(cbHandCardData+6, cbHandCardCount-6) !=CT_FOUR_LINE_TAKE_TWO)
		{
			OutCardResult.cbCardCount = 6 ;
            OutCardResult.cbResultCard[0] = 0x4f ;
            OutCardResult.cbResultCard[1] = 0x4f ;
            OutCardResult.cbResultCard[2] = 0x4f ;
            OutCardResult.cbResultCard[3] = 0x4e ;
            OutCardResult.cbResultCard[4] = 0x4e ;
            OutCardResult.cbResultCard[5] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
	}

	//������һֻ
	if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}
	//����
	else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}
	//ֻʣһ����
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//ֻʣһ�ź�һ��
	if(cbHandCardCount>=2)
	{
		//�ϼ��˿�
		tagOutCardTypeResult UpsideCanOutCardType1[(12+1)] ;
		ZeroMemory(UpsideCanOutCardType1, sizeof(UpsideCanOutCardType1)) ;
		tagOutCardTypeResult UpsideCanOutCardType2[(12+1)] ;
		ZeroMemory(UpsideCanOutCardType2, sizeof(UpsideCanOutCardType2)) ;

		//�¼��˿�
		tagOutCardTypeResult UndersideCanOutCardType1[(12+1)] ;
		ZeroMemory(UndersideCanOutCardType1, sizeof(UndersideCanOutCardType1)) ;
		tagOutCardTypeResult UndersideCanOutCardType2[(12+1)] ;
		ZeroMemory(UndersideCanOutCardType2, sizeof(UndersideCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//�Ƿ���ը
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//û��ը��
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE && cbFirstHandCardType!=CT_THREE_LINE_TAKE_TWO)
		{
		if(CT_ERROR!=cbFirstHandCardType && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO)
		{
			AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], cbHandCardData, cbHandCardCount-1, UpsideCanOutCardType1) ;
			AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], cbHandCardData, cbHandCardCount-1, UndersideCanOutCardType1) ;
		}
		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO)
		{
			AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], cbHandCardData+1, cbHandCardCount-1, UpsideCanOutCardType2) ;
			AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], cbHandCardData+1, cbHandCardCount-1, UndersideCanOutCardType2) ;
		}

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && UpsideCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && UndersideCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && 
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO && UpsideCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && UndersideCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0]) &&
			GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0 && UndersideCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
		}
		//����ը��
		else
		{
			//��ȥը�����
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
                g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
		}
	}

	{
		{
			//�����˿�
			tagOutCardTypeResult MeOutCardTypeResult[(12+1)] ;
			ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

			//���㵥��
			BYTE cbMinSingleCardCount[4] ;
			cbMinSingleCardCount[0]=MAX_COUNT ;
			cbMinSingleCardCount[1]=MAX_COUNT ;
			cbMinSingleCardCount[2]=MAX_COUNT ;
			cbMinSingleCardCount[3]=MAX_COUNT ;
			BYTE cbIndex[4]={0} ;
			BYTE cbOutcardType[4]={CT_ERROR} ;
			BYTE cbMinValue=MAX_COUNT ; 
			BYTE cbMinSingleCountInFour=MAX_COUNT ;
			BYTE cbMinCardType=CT_ERROR ;
			BYTE cbMinIndex=0 ;

			//��ը�������
			for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
			{

				tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

				//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
				BYTE cbThisHandCardCount = MAX_COUNT ;

				//�ϼ��˿�
				tagOutCardTypeResult UpsideOutCardTypeResult[(12+1)] ;
				ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult)) ;

				//�¼��˿�
				tagOutCardTypeResult UndersideOutCardTypeResult[(12+1)] ;
				ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult)) ;


				for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
				{
					//�������ж�
					if ( cbCardType == CT_DOUBLE )
					{
						tagAnalyseResult AnalyseResult ;
						ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
						AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
						if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
						{
							bool bContinue = false ;
							for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
								if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
								{
									bContinue = true ;
									break ;
								}
							if ( bContinue ) continue ;
						}
					}

					BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

					//���·���
					if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
					{
						cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
						AnalyseOutCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker], 
							tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,UpsideOutCardTypeResult) ;
						AnalyseOutCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker], 
							tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,UndersideOutCardTypeResult) ;
					}
					BYTE cbMaxValue=0 ; 
					BYTE Index = 0 ;

					//���˳�ӣ������Ĵ���
					BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
					if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
						( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ) )
					{
						BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
						CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
						cbRemainCardCount = cbHandCardCount ;

						//�Ƴ��˿�
						RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
						cbRemainCardCount -= cbThisHandCardCount ;

						//����˿�
						BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

						if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
							cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
							cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
							cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
						{
							//��ʱ����
							if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
								GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
								continue ;

							if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
								GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
								continue ;
						}
					}

					//��Դ�ԣ������ȳ���
					if (cbCardType == CT_DOUBLE && GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11)
					{
						BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCount ;
						cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;
						if (cbAllSingleCount >= 2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2]) < 10) continue ;
					}

					//�з�����ѹס��
					if(UpsideOutCardTypeResult[cbCardType].cbCardTypeCount>0 || UndersideOutCardTypeResult[cbCardType].cbCardTypeCount>0)
					{
						//�ϼ��ܵ�
						if(UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[wUpsideOfBanker]<=UpsideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
							continue ;

						//�¼��ܵ�
						if(UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[wUndersideOfBanker]<=UndersideOutCardTypeResult[cbCardType].cbEachHandCardCount[0]+1)
							continue ;

						//�Լ����������û���Ȩ
						//if(UpsideOutCardTypeResult[cbCardType].cbCardTypeCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(UpsideOutCardTypeResult[cbCardType].cbCardData[0][0]) ||
						//	UpsideOutCardTypeResult[cbCardType].cbCardTypeCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(UpsideOutCardTypeResult[cbCardType].cbCardData[0][0])) 
						//	continue ;
					}
					//�Ƿ��д��� 
					if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
					{
						bool bHaveLargeCard=false ;
						for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						{
							if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;
							if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE  && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 
						}

						if(bHaveLargeCard)
							continue ;
					}

					//����cbMinSingleCardCount[4]�����ֵ
					for(BYTE j=0; j<4; ++j)
					{
						if(cbMinSingleCardCount[j]>=cbTmpCount)
						{
							cbMinSingleCardCount[j] = cbTmpCount ;
							cbIndex[j] = i ;
							cbOutcardType[j] = cbCardType ;
							break ;
						}
					}

					//������Сֵ
					if(cbMinSingleCountInFour>=cbTmpCount)
					{
						//��С����
						cbMinCardType = cbCardType ;
						//��С�����е���С����
						cbMinSingleCountInFour=cbTmpCount ;						
						//��С�����е���С��
						cbMinIndex=i ;
					}
				}
			}

			if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
				(m_cbUserCardCount[wUndersideOfBanker]>=4 && m_cbUserCardCount[wUpsideOfBanker]>=4))
				cbMinSingleCountInFour=MAX_COUNT ;

			if(cbMinSingleCountInFour!=MAX_COUNT)
			{
				BYTE Index = cbMinIndex ;

				//ѡ����С��
				for(BYTE i=0; i<4; ++i)
				{
					if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
						GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
						Index = cbIndex[i] ;
				}

				//������һֻ
				if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
				{
                    OutCardResult.cbCardCount = 6 ;
                    OutCardResult.cbResultCard[0] = 0x4f ;
                    OutCardResult.cbResultCard[1] = 0x4f ;
                    OutCardResult.cbResultCard[2] = 0x4f ;
                    OutCardResult.cbResultCard[3] = 0x4e ;
                    OutCardResult.cbResultCard[4] = 0x4e ;
                    OutCardResult.cbResultCard[5] = 0x4e ;
                    g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
				//����
				else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
				{
                    OutCardResult.cbCardCount = 6 ;
                    OutCardResult.cbResultCard[0] = 0x4f ;
                    OutCardResult.cbResultCard[1] = 0x4f ;
                    OutCardResult.cbResultCard[2] = 0x4f ;
                    OutCardResult.cbResultCard[3] = 0x4e ;
                    OutCardResult.cbResultCard[4] = 0x4e ;
                    OutCardResult.cbResultCard[5] = 0x4e ;
                    g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
				else
				{
					//���ñ���
					OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
					CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                    g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}

				ASSERT(OutCardResult.cbCardCount>0) ;

                g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			//��û���ҵ��ʺϵ�����Ӵ����С
			if(OutCardResult.cbCardCount<=0 && (m_cbUserCardCount[wUndersideOfBanker] == 1 || m_cbUserCardCount[wUpsideOfBanker] == 1))
			{
				//ֻ��һ����ʱ���ܷ���
				if(MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
				{
					BYTE Index=MAX_COUNT ;
					for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
					{
						if((m_cbUserCardCount[wUndersideOfBanker] == 1 && GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[wUndersideOfBanker][0])) ||
							(m_cbUserCardCount[wUpsideOfBanker] == 1 && GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[wUpsideOfBanker][0])))
						{
							Index=i ;
						}
						else break ;
					}

					if(MAX_COUNT!=Index)
					{
						OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
						CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
                        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}
			}
		}
	}
	BYTE cbFirstCard=0 ;
	//��������2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//���㵥��
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//��ը�������
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//�мҿ����ߵ�
				if ( CompareCard(tmpCardResult.cbCardData[i], m_cbAllCardData[wUndersideOfBanker], tmpCardResult.cbEachHandCardCount[i], m_cbUserCardCount[wUndersideOfBanker]) ||
					CompareCard(tmpCardResult.cbCardData[i],  m_cbAllCardData[wUpsideOfBanker], tmpCardResult.cbEachHandCardCount[i],m_cbUserCardCount[wUpsideOfBanker]))
					continue ;

				//���˳�ӣ������Ĵ���
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//�Ƴ��˿�
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//��ʱ����
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//����cbMinSingleCardCount[4]�����ֵ
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//������Сֵ
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//��С����
					cbMinCardType = cbCardType ;
					//��С�����е���С����
					cbMinSingleCountInFour=cbTmpCount ;						
					//��С�����е���С��
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//������һֻ
			if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			//����
			else if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			else
			{
				//���ñ���
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
		//���ֻʣը��
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{
			//BYTE Index=0 ;
			//BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			////��Сը��
			//for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
			//	if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
			//	{
			//		cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
			//		Index = i ;
			//	}

            //���ñ���
            OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
            CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));

            return ;
		}

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//�����û���������ͳ���С��һ��
			if ( ( 1 == m_cbUserCardCount[wUndersideOfBanker] || 1 == m_cbUserCardCount[wUpsideOfBanker] ) && cbAllSingleCardCount >= 2 ) 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-2] ;
			}
			else 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;
			}
            g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		//�����û���������ͳ���С��һ��
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;

        g_Logger.WriteLog("---------------BankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
}

//�������ƣ�����ƣ�
void CGameLogic::BankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) 
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;

	//���±�û��
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;
    g_Logger.WriteLog("---------------BankerOutCard2 ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);
    g_Logger.WriteLog("---------------BankerOutCard2 -------------cbTurnCardCount[%d],cbTurnCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbTurnCardCount,cbTurnCardData[0],cbTurnCardData[1],cbTurnCardData[2],cbTurnCardData[3],cbTurnCardData[4],cbTurnCardData[5],cbTurnCardData[6],cbTurnCardData[7],cbTurnCardData[8],cbTurnCardData[9],cbTurnCardData[10],cbTurnCardData[11],cbTurnCardData[12],cbTurnCardData[13],cbTurnCardData[14],cbTurnCardData[15],cbTurnCardData[16],cbTurnCardData[17],cbTurnCardData[18],cbTurnCardData[19]);

	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	WORD wUndersideUser = (m_wBankerUser+1)%GAME_PLAYER ;
	WORD wUpsideUser = (wUndersideUser+1)%GAME_PLAYER ;

	//ֻʣը��
    if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && (cbOutCardType<CT_BOMB_CARD || 
        CompareCard(cbTurnCardData,CardTypeResult[CT_BOMB_CARD].cbCardData[0],cbTurnCardCount,CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;
        g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],cbTurnCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, cbTurnCardCount,cbTurnCardData[0],cbTurnCardData[1],cbTurnCardData[2],cbTurnCardData[3],cbTurnCardData[4],cbTurnCardData[5],cbTurnCardData[6],cbTurnCardData[7],cbTurnCardData[8],cbTurnCardData[9],cbTurnCardData[10],cbTurnCardData[11],cbTurnCardData[12],cbTurnCardData[13],cbTurnCardData[14],cbTurnCardData[15],cbTurnCardData[16],cbTurnCardData[17],cbTurnCardData[18],cbTurnCardData[19]);

		return ;
	}
	//����ը����һ��
	else if(cbHandCardCount>6 && cbHandCardData[5]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+6, cbHandCardCount-6) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+6, cbHandCardCount-6))
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
		return  ;
	}

	//ը����һ��
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
    tagBomCardResult BomCardResult = {0};
	GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount, BomCardResult, ST_MISSILE_FIRST) ;

	//��ը�ж� ֻʣһ��ը
	if(cbAllBombCount>0 && CardTypeResult[CT_BOMB_CARD].cbCardType == 1)
	{
		//ʣ���˿�
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;

		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) &&
			GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[wUpsideUser][0])))
		{
            BYTE cbTempAllBombCard[MAX_COUNT], cbTempAllBombCount=0 ;
            tagBomCardResult TempBomCardResult = {0};
            GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbTempAllBombCard, cbTempAllBombCount, TempBomCardResult) ;

            if((cbOutCardType<CT_BOMB_CARD || CompareCard(cbTurnCardData, cbTempAllBombCard, cbTurnCardCount, TempBomCardResult.cbCardCount[0])/*GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])*/) &&
				 ( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) ) )
			{
				//����ը��
				if(GetCardColor(cbAllBombCard[5])==0x40)
				{
                    OutCardResult.cbCardCount = 6 ;
                    OutCardResult.cbResultCard[0] = 0x4f ;
                    OutCardResult.cbResultCard[1] = 0x4f ;
                    OutCardResult.cbResultCard[2] = 0x4f ;
                    OutCardResult.cbResultCard[3] = 0x4e ;
                    OutCardResult.cbResultCard[4] = 0x4e ;
                    OutCardResult.cbResultCard[5] = 0x4e ;
					return ;
				}
				else
				{
					//�����м���
					BYTE cbUnderSideBankerAllBombCard[MAX_COUNT], cbUnderSideBankerAllBombCardCount ;
                    tagBomCardResult UnderSideBomCardResult = {0};
					GetAllBomCardOrder( m_cbAllCardData[wUndersideUser], m_cbUserCardCount[wUndersideUser], cbUnderSideBankerAllBombCard, cbUnderSideBankerAllBombCardCount, UnderSideBomCardResult) ;

                    BYTE cbUpSideBankerAllBombCard[MAX_COUNT], cbUpSideBankerAllBombCardCount ;
                    tagBomCardResult UpSideBomCardResult = {0};
                    GetAllBomCardOrder( m_cbAllCardData[wUpsideUser], m_cbUserCardCount[wUpsideUser], cbUpSideBankerAllBombCard, cbUpSideBankerAllBombCardCount, UpSideBomCardResult) ;

                    BYTE cbBombCard[MAX_COUNT], cbBombCount=0 ;
                    tagBomCardResult CardResult = {0};
                    GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCount, CardResult) ;
					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  ( cbUnderSideBankerAllBombCardCount <= 0 && cbUpSideBankerAllBombCardCount )|| 
                        CompareCard(cbUnderSideBankerAllBombCard, cbBombCard, UnderSideBomCardResult.cbCardCount[0], CardResult.cbCardCount[0]) &&
                        CompareCard(cbUpSideBankerAllBombCard, cbBombCard, UpSideBomCardResult.cbCardCount[0], CardResult.cbCardCount[0]))
// 						( GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbUnderSideBankerAllBombCard[0] ) && 
// 						GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbUpSideBankerAllBombCard[0] )) )
					{
						OutCardResult.cbCardCount = CardResult.cbCardCount[0] ;
						CopyMemory(OutCardResult.cbResultCard, cbBombCard, CardResult.cbCardCount[0]) ;
                        g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}
			}
		}
	}

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				if(TestOutAllCard(m_wBankerUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
                    if (cbBombCardType == CT_BOMB_CARD|| cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 || cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 ||
                        cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;

					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					//����ը��
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;

					if (cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}
				}
			}
		}
		//ֱ�ӷ���
        if (bFindBestCard) 
        {       
            g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

	//ȡ���ĸ���С����
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
    tagOutCardTypeResult CanOutCard = {0};
    if (cbOutCardType < CT_BOMB_CARD)
    {
        CopyMemory(&CanOutCard, &CardTypeResult[cbOutCardType], sizeof(tagOutCardTypeResult));
    }

	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//����cbMinSingleCardCount[4]�����ֵ
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	if(CanOutCard.cbCardTypeCount>0 && cbMinSingleCountInFour < MAX_COUNT)
	{
		BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
		bool bFindCard = false ;
		BYTE cbCanOutIndex=0 ;
		for(BYTE i=0; i<4; ++i)
		{
			BYTE Index = cbIndex[i] ;

			if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_TWO ) &&
				cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
			{
				//��Դ���
				bool bNoLargeCard = true ;

				//���������������������4�����ҳ�����С��K���ƶ��Ҳ��ǳ������ϵ������ʱ�����ܳ�2ȥ��
				if(m_cbUserCardCount[wOutCardUser]>=4 && cbHandCardCount>=5  && CanOutCard.cbEachHandCardCount[Index]>=2 && 
					GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
					GetCardLogicValue(cbTurnCardData[0])<13 &&
					(wOutCardUser==wUndersideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) || wOutCardUser==wUpsideUser&&GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[wUpsideUser][0])) && 
					CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
					bNoLargeCard=false ;

				//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
				for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
				{
					if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						bNoLargeCard = false ;
				}
				if(bNoLargeCard)
				{
					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}
		}

		if(bFindCard)
		{
			//�����
			BYTE cbLargestLogicCard ;
			if(wOutCardUser==wUndersideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) ;
			else if(wOutCardUser==wUpsideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]) ;
			bool bCanOut=true ;

			//��ֻѹ2
			if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
			{
				if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
				{
					bCanOut = false ;
				}
			}

			//˫���ж�
			if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=6 && cbHandCardData[5]==0x4E)
			{
				bool bOutMissileCard = false ;
				//һ���ƺ�һ��ը��
				BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
				CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
				RemoveCard(cbRemainCardData, 2, cbRemainCardData, cbRemainCardCount) ;
				cbRemainCardCount -= 2 ;
				if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

				//ֻʣ������
				BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
				if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

				//ը������
				BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
					CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
				if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

				//����С��
				if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
					(GetCardLogicValue(m_cbAllCardData[(1+m_wBankerUser) % GAME_PLAYER][0]) > 10 || GetCardLogicValue(m_cbAllCardData[(2+m_wBankerUser) % GAME_PLAYER][0]) > 10))
					bOutMissileCard = false ;

				//����˿�
				if(bOutMissileCard)
				{
					//��������ը��
					BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

					OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
					CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
                    g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			if(bCanOut)
			{
				//���ñ���
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

                g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
		}

		if(cbOutCardType==CT_SINGLE)
		{
			//�мҵ������
			BYTE cbLargestLogicCard ;
			if(wOutCardUser==wUndersideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) ;
			else if(wOutCardUser==wUpsideUser) cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]) ;

			if(GetCardLogicValue(cbTurnCardData[0])==14 || 
				GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
				(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
				(wOutCardUser==wUndersideUser&&m_cbUserCardCount[wUndersideUser]<=5 || wOutCardUser==wUpsideUser&&m_cbUserCardCount[wUpsideUser]<=5))
			{
				//ȡһ�Ŵ��ڵ���2����Ҫ���мҳ����ƴ���ƣ�
				BYTE cbIndex=MAX_COUNT ;
				for(BYTE i=0; i<cbHandCardCount; ++i)
					if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&
						GetCardLogicValue(cbHandCardData[i])>=15)
					{
						cbIndex = i ;
					}
					if(cbIndex!=MAX_COUNT)
					{
						//���ñ���
						OutCardResult.cbCardCount=1;
						OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;
                        g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

						return ;
					}
			}
		}


		BYTE cbMinSingleCount=MAX_COUNT ;
		BYTE Index=0 ;
		for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
			if(cbMinSingleCount>=cbTmpCount)
			{
				cbMinSingleCount = cbTmpCount ;
				Index = i ;
			}
		}
		//���ñ���
		OutCardResult.cbCardCount=CardTypeResult[cbOutCardType].cbEachHandCardCount[Index];
		CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;
        g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

		return ;
	}

	//��Ҫ����ը��
	if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0)
	{
		tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
        BYTE Index = 0 ;
		/*Ѱ����Сը��
        BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
		for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
		{
			if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
			{
				cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
				Index = i ;
			}
		}*/
        Index = BomCard.cbCardTypeCount-1;

		bool bOutBomb=false ;

		//��һ�м�
		//WORD wOtherUser=INVALID_CHAIR ;
		//for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser)
		//	if(wUser!=wOutCardUser && wUser!=m_wBankerUser) wOtherUser=wOtherUser ;
		
		//һ���ƺ�һ��ը��
		BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
		CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
		RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
		cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
		if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

		//ը������
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
		if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > 
			GetCardLogicValue(m_cbAllCardData[wUpsideUser == wOutCardUser ? wUndersideUser : wUpsideUser][0])) bOutBomb = true ;

		//ֻʣһ��
		BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
		if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
			cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[wUndersideUser][0]) && 
			GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[wUpsideUser][0]))) bOutBomb = true ;

		//������
		if (cbRemainCardType != CT_ERROR && m_lBankerOutCardCount == 1) bOutBomb = true ;

		//ֻʣ������
		BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
		if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
			cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

        //ը����������
        /*cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
        if(cbSingleCardCount <= 1)
        {
            bOutBomb = true ;
        }*/

		//�����˿�
		tagAnalyseResult AnalyseResult ;
		AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

		if (m_cbUserCardCount[wOutCardUser] == 1 && (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + 1 >= cbRemainCardCount)) bOutBomb = true ;


		//���ñ���
		if(bOutBomb)
		{
			OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
            CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
            g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		}
        g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
//		return ;
	}

    //������ֻʣ��һ�Ż���һ�֣�һ��Ҫѹ
    if (OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[wOutCardUser] == 1 || (GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) != CT_ERROR)))
    {
        //ը��ѹ
        if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
        {
            OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
            CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
            g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
        //��ͨѹ
        if (cbOutCardType < CT_BOMB_CARD && CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
        {
            BYTE bMinCardCount = 0;
            BYTE bCurMinNum = MAX_COUNT;
            for (BYTE i = 0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; i++)
            {
                BYTE bMinNum = 0;
                bMinNum = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i],CardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
                if (bCurMinNum > bMinNum)
                {
                    bCurMinNum = bMinNum;
                    bMinCardCount = i;
                }
            }
            OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[bMinCardCount];
            CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[bMinCardCount], OutCardResult.cbCardCount*sizeof(BYTE));
            g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
    }

    g_Logger.WriteLog("---------------BankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

	return ;
}

//�����ϼң��ȳ��ƣ�
void CGameLogic::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID, tagOutCardResult & OutCardResult)
{
	//���±�û��
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;
    g_Logger.WriteLog("---------------UpsideOfBankerOutCard ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);

    //��ʣ��ը��
    BYTE cbBomCardData[MAX_COUNT] = {0};
    BYTE cbBomCardCount = 0;
    tagBomCardResult BomCardResult = {0};
    AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
    GetAllBomCardOrder(cbHandCardData,cbHandCardCount, cbBomCardData, cbBomCardCount, BomCardResult);
    if (cbBomCardCount == cbHandCardCount) 
    {
        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
        return ;
    }
	//����ֻ��һ�ţ��Ӵ�С����
	if (m_cbUserCardCount[m_wBankerUser] == 1)
	{
		BYTE cbSingleCardData[MAX_COUNT] ;
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbSingleCardData) ;

		//ֻʣ����
		if (cbSingleCardCount == cbHandCardCount) 
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;

			return ;
		}
	}


	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+2)%GAME_PLAYER ;
				
				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
                    if (cbBombCardType == CT_BOMB_CARD || cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 ||cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 || 
                        cbBombCardType == CT_MISSILE_CARD || cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

						bFindBestCard = true ;
					}
				}
			}
		}
		//ֱ�ӷ���
		if (bFindBestCard) return ;

		//����������
		if ( cbHandCardCount == 8 && GetCardLogicValue(cbHandCardData[5]) == 16 && m_cbUserCardCount[m_wBankerUser] == 1 && 
			GetCardLogicValue(cbHandCardData[6]) < GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[6] ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

        //�Ĵ������ж� ���ֳ���֧��
// 		if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], OutCardResult ) )
// 		{
//             g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 			return ;
// 		}

	//�����˳�ƺ͵�ֻ��һ�ԣ����ҵ�ֻ��Աȵ�����С�����ȳ�˳
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//����ը����һ��
		else if(cbHandCardCount>6 && cbHandCardData[5]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
			CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+6, cbHandCardCount-6) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+6, cbHandCardCount-6))
		{
			OutCardResult.cbCardCount = 6 ;
            OutCardResult.cbResultCard[0] = 0x4f ;
            OutCardResult.cbResultCard[1] = 0x4f ;
            OutCardResult.cbResultCard[2] = 0x4f ;
            OutCardResult.cbResultCard[3] = 0x4e ;
            OutCardResult.cbResultCard[4] = 0x4e ;
            OutCardResult.cbResultCard[5] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
	}
	//������һֻ
	if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
	}
	//����
	else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
	}
	//ֻʣһ����
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//ֻʣһ�ź�һ��
	if(cbHandCardCount>=2)
	{
		//�����˿�
		tagOutCardTypeResult BankerCanOutCardType1[(12+1)] ;
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1)) ;
		tagOutCardTypeResult BankerCanOutCardType2[(12+1)] ;
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//�Ƿ���ը
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//û��ը��
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE && cbFirstHandCardType!=CT_THREE_LINE_TAKE_TWO)
		{
		//�������Գ�����
		if(cbFirstHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData, cbHandCardCount-1, BankerCanOutCardType1) ;
		if(cbSecondHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData+1, cbHandCardCount-1, BankerCanOutCardType2) ;

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && 
			BankerCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
		}
		//����ը��
		else
		{
			//��ȥը�����
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
		}
	}

	//���ƽ����ж�
	WORD wFriendID = (m_wBankerUser + 1) % GAME_PLAYER ;
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && m_cbUserCardCount[wFriendID] >= 2)
	{
		tagAnalyseResult MeAnalyseResult ;
		ZeroMemory(&MeAnalyseResult, sizeof(MeAnalyseResult)) ;

		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult) ;

		tagAnalyseResult FriendAnalyseResult ;
		ZeroMemory(&FriendAnalyseResult, sizeof(FriendAnalyseResult)) ;

		//�����˿�
		AnalysebCardData(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], FriendAnalyseResult) ;

		//�����ж�
		if ((m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
			m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1)) &&
			MeAnalyseResult.cbDoubleCount > 0 && FriendAnalyseResult.cbDoubleCount > 0)
		{
			//��С����
			BYTE cbMeLeastDoubleCardLogic = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]) ;
			//������
			BYTE cbFriendLargestDoublecardLogic = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]) ;

			//�����ж�
			if (cbMeLeastDoubleCardLogic < 14 && cbMeLeastDoubleCardLogic < cbFriendLargestDoublecardLogic)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2] ;
				OutCardResult.cbResultCard[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1] ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
          			
		}
	}

	//�¼�Ϊ���������ҵ����˿�����5��
	//	if(m_cbUserCardCount[m_wBankerUser]<=5)
	{
		//�����˿�
		tagOutCardTypeResult MeOutCardTypeResult[(12+1)] ;
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
		AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

		//�Լ��˿�
		WORD wFriendID ;
		for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID) 
			if(wChairID!=m_wBankerUser && wMeChairID!=wChairID) wFriendID = wChairID ;

		//���㵥��
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//������������
		BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
		GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

		//��ը�������
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE cbThisHandCardCount = MAX_COUNT ;

			//�����˿�
			tagOutCardTypeResult BankerCanOutCard[(12+1)] ;
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard)) ;

			tagOutCardTypeResult FriendOutCardTypeResult[(12+1)] ;
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult)) ;

			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//�������ж�
				if ( cbCardType == CT_DOUBLE )
				{
					tagAnalyseResult AnalyseResult ;
					ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
					AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
					if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
					{
						bool bContinue = false ;
						for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
							if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
							{
								bContinue = true ;
								break ;
							}
						if ( bContinue ) continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				//���ض���
				if (cbCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
					GetCardLogicValue(tmpCardResult.cbCardData[i][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tmpCardResult.cbCardData[0][0]) >= 10 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < 14) continue ;

				//���·���
				if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
				{
					cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
					AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], 
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,BankerCanOutCard) ;
					AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID],
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,FriendOutCardTypeResult) ;
				}

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//���˳�ӣ������Ĵ���
				BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
				if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//�Ƴ��˿�
					RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= cbThisHandCardCount ;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
						cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
					{
						//��ʱ����
						if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				//��������ѹ�ƣ����Ҷ��Ѳ�����ѹ����
				if((BankerCanOutCard[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) || (BankerCanOutCard[cbCardType].cbCardTypeCount>0 && FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0 &&
					GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbCardData[0][0])<=GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0])))
				{
					//�����ܵ�
					if( BankerCanOutCard[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[m_wBankerUser]<=BankerCanOutCard[cbCardType].cbEachHandCardCount[0]+1)
						continue ;

					//�Լ����������û���Ȩ
					if(GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0]) &&
						BankerCanOutCard[cbCardType].cbCardTypeCount > 0) 
						continue ;
				}
				//�Ƿ��д���
				if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard=false ;
					for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;
					if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE  && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 

					if(bHaveLargeCard) continue ;
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if(tmpCardResult.cbEachHandCardCount[i]==m_cbUserCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])>GetCardLogicValue(tmpCardResult.cbCardData[i][0])) continue ;

				//����cbMinSingleCardCount[4]�����ֵ
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//������Сֵ
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//��С����
					cbMinCardType = cbCardType ;
					//��С�����е���С����
					cbMinSingleCountInFour=cbTmpCount ;						
					//��С�����е���С��
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
			m_cbUserCardCount[m_wBankerUser]>4)
			cbMinSingleCountInFour=MAX_COUNT ;

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//������һֻ
			if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			//����
			else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			else
			{
				//���ñ���
				OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		//��������˿�����5����û���ҵ��ʺϵ�����Ӵ����С
		if(OutCardResult.cbCardCount<=0 && m_cbUserCardCount[m_wBankerUser]<=5)
		{
			//ֻ��һ����ʱ���ܷŵ�����
			if(m_cbUserCardCount[m_wBankerUser]==1 && MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
			{
				BYTE Index=MAX_COUNT ;
				for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
				{
					if(GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
					{
						Index=i ;
					}
					else break ;
				}

				if(MAX_COUNT!=Index)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
					CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}
		}
	}

	BYTE cbFirstCard=0 ;
	//��������2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//���㵥��
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//������������
		BYTE cbBankerSingleCardData[MAX_COUNT] ;
		BYTE cbBankerSingleCardCount=AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;
		BYTE cbBankerSingleCardLogic = 0 ;
		if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) ;
		else if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) ;
		else if(cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[0]) ;

		//������������
		BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
		GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

		//��ը�������
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//���ܷ��ߵ���С��
				if(cbCardType==CT_SINGLE &&  cbBankerSingleCardCount > 0 && GetCardLogicValue(tmpCardResult.cbCardData[i][0])<cbBankerSingleCardLogic) continue ;

				//���ض���
				if (cbCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
					GetCardLogicValue(tmpCardResult.cbCardData[i][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
					GetCardLogicValue(tmpCardResult.cbCardData[0][0]) >= 10 && GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < 14) continue ;

				//���˳�ӣ������Ĵ���
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//�Ƴ��˿�
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//��ʱ����
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//����cbMinSingleCardCount[4]�����ֵ
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//������Сֵ
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//��С����
					cbMinCardType = cbCardType ;
					//��С�����е���С����
					cbMinSingleCountInFour=cbTmpCount ;						
					//��С�����е���С��
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//������һֻ
			if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
				OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			//����
			else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                return ;
			}
			else
			{
				//���ñ���
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

			return ;
		}
		//���ֻʣը��
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{	
            /*BYTE Index=0 ;
            BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
            //��Сը��
            for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
            {
	            if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
	            {
		            cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
		            Index = i ;
	            }
            }*/
	        //���ñ���
	        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
	        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));

	        return ;
		}

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//�����û���������ͳ���С��һ��
			if ( 1 == m_cbUserCardCount[m_wBankerUser] ) 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[0] ;
			}
			else 
			{
				OutCardResult.cbCardCount = 1 ;
				OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;
			}
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		//�����û���������ͳ���С��һ��
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
}

//�����ϼң�����ƣ�
void CGameLogic::UpsideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult) 
{
	//���±�û��
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;
    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);
    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 -------------cbTurnCardCount[%d],cbTurnCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbTurnCardCount,cbTurnCardData[0],cbTurnCardData[1],cbTurnCardData[2],cbTurnCardData[3],cbTurnCardData[4],cbTurnCardData[5],cbTurnCardData[6],cbTurnCardData[7],cbTurnCardData[8],cbTurnCardData[9],cbTurnCardData[10],cbTurnCardData[11],cbTurnCardData[12],cbTurnCardData[13],cbTurnCardData[14],cbTurnCardData[15],cbTurnCardData[16],cbTurnCardData[17],cbTurnCardData[18],cbTurnCardData[19]);

	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData, cbTurnCardCount) ;

	//����ֻ��һ�ţ��Ӵ�С����
	if (m_cbUserCardCount[m_wBankerUser] == 1 && wOutCardUser != m_wBankerUser && CT_SINGLE == cbOutCardType)
	{
		BYTE cbSingleCardData[MAX_COUNT] ;
		BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbSingleCardData) ;

		WORD wFriendID = (m_wBankerUser+1) % GAME_PLAYER ;
		WORD wMeID = (wFriendID+1) % GAME_PLAYER ;

		BYTE cbFriendLargestLogicCard = GetCardLogicValue(m_cbAllCardData[wFriendID][0]) ;
		BYTE cbMeLargestLogicCard = GetCardLogicValue(cbHandCardData[0]) ;
		BYTE cbTurnLogicCard = GetCardLogicValue(cbTurnCardData[0]) ;

		//ֻʣ����(���Ի�����)
		if (cbSingleCardCount == cbHandCardCount && cbFriendLargestLogicCard > cbTurnLogicCard && cbMeLargestLogicCard > cbTurnLogicCard) 
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

			return ;
		}
	}

	//�����ɳ���
	tagOutCardTypeResult BankerOutCardTypeResult[(12+1)] ;
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult)) ;

	AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], BankerOutCardTypeResult) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	//ֻʣը��
	if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && 
       ((cbOutCardType < CardTypeResult[CT_BOMB_CARD].cbCardType) || CompareCard(cbTurnCardData, CardTypeResult[CT_BOMB_CARD].cbCardData[0], cbTurnCardCount, CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}
	//����ը����һ��
	else if((cbHandCardCount > 6) && (GetCardType(cbHandCardData, 6) == CT_MISSILE_CARD_6) && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+6, cbHandCardCount-6) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+6, cbHandCardCount-6))
	{
		OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//ը����һ��
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
    tagBomCardResult HandBomCardResult = {0};
    //GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount) ;
    GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount, HandBomCardResult, ST_MISSILE_FIRST) ;

	//��ը�ж�
	if(cbAllBombCount>0 && m_wBankerUser==wOutCardUser)
	{
		//ʣ���˿�
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;

		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
		{
            BYTE cbTempAllBombCard[MAX_COUNT], cbTempAllBombCount=0 ;
            tagBomCardResult TempBomCardResult = {0};
            GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbTempAllBombCard, cbTempAllBombCount, TempBomCardResult) ;

            if((cbOutCardType<CT_BOMB_CARD || CompareCard(cbTurnCardData, cbTempAllBombCard, cbTurnCardCount, TempBomCardResult.cbCardCount[0])/*GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])*/) &&
				( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount)) )
			{
				//����ը��
                //if(GetCardColor(cbAllBombCard[0])==0x40)
                if(cbAllBombCount >5 && GetCardColor(cbAllBombCard[5])==0x40)
				{
                    OutCardResult.cbCardCount = 6 ;
                    OutCardResult.cbResultCard[0] = 0x4f ;
                    OutCardResult.cbResultCard[1] = 0x4f ;
                    OutCardResult.cbResultCard[2] = 0x4f ;
                    OutCardResult.cbResultCard[3] = 0x4e ;
                    OutCardResult.cbResultCard[4] = 0x4e ;
                    OutCardResult.cbResultCard[5] = 0x4e ;
                    return ;
				}
				else
				{
					//����������
					BYTE cbBankerAllBombCard[MAX_COUNT], cbBankerAllBombCardCount ;
                    tagBomCardResult BankerBomCardResult = {0};
					GetAllBomCardOrder( m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerAllBombCard, cbBankerAllBombCardCount, BankerBomCardResult) ;

                    //��������
                    BYTE cbBombCard[MAX_COUNT], cbBombCardCount ;
                    tagBomCardResult BomCardResult = {0};
                    GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCardCount, BomCardResult) ;
					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  cbBankerAllBombCardCount <= 0
						// || GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbBankerAllBombCard[0] )
                        //CompareCard( cbBankerAllBombCard, cbBombCard, BankerBomCardResult.cbCardCount[0], BomCardResult.cbCardCount[0])
                        )
					{
						/*OutCardResult.cbCardCount = BomCardResult.cbCardCount[0] ;
						CopyMemory(OutCardResult.cbResultCard, cbBombCard, BomCardResult.cbCardCount[0]) ;
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;*/
                        //���ñ���
                        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
                        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                        return ;

					}
				}
			}
		}
	}

	//ֻ��һ���� //���ֳ�û���Ĵ��������߲���
	if ( GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_ONE && GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_TWO &&
		CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount) )
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

		return ;
	}

	//���ƽ����ж�
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && cbOutCardType == CT_DOUBLE)
	{
		tagAnalyseResult AnalyseResult ;
		ZeroMemory(&AnalyseResult, sizeof(AnalyseResult)) ;

		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

		//�����ж�
		if (cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSixCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8) || 
			cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSixCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8 + 1))
		{
			//�����ж�
			for (int nIndex = AnalyseResult.cbDoubleCount-1; nIndex>=0 ; --nIndex)
			{
				if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[nIndex*2]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = AnalyseResult.cbDoubleCardData[nIndex*2] ;
					OutCardResult.cbResultCard[1] = AnalyseResult.cbDoubleCardData[nIndex*2+1] ;
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			//��ը�ж� ��Сը��
			if (AnalyseResult.cbFourCount > 0) 
			{
				//��Сը��
				BYTE cbLestBombIndex = AnalyseResult.cbFourCount-1 ;

				OutCardResult.cbCardCount = 4 ;
				OutCardResult.cbResultCard[0] = AnalyseResult.cbFourCardData[cbLestBombIndex*4] ;
				OutCardResult.cbResultCard[1] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+1] ;
				OutCardResult.cbResultCard[2] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+2] ;
				OutCardResult.cbResultCard[3] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+3] ;

				return ;
			}
            else if (AnalyseResult.cbFiveCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbFiveCount-1 ;

                OutCardResult.cbCardCount = 5 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+4] ;

                return ;
            }
            else if (AnalyseResult.cbSixCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbSixCount-1 ;

                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbSixCardData[cbLestBombIndex*6] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+5] ;

                return ;
            }
            else if (AnalyseResult.cbSenvenCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbSenvenCount-1 ;

                OutCardResult.cbCardCount = 7 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+5] ;
                OutCardResult.cbResultCard[6] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+6] ;

                return ;
            }
            else if (AnalyseResult.cbEightCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbEightCount-1 ;

                OutCardResult.cbCardCount = 8 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbEightCardData[cbLestBombIndex*8] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+5] ;
                OutCardResult.cbResultCard[6] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+6] ;
                OutCardResult.cbResultCard[7] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+7] ;

                return ;
            }
            			
		}

		//�����Լ�
		if ( wOutCardUser != m_wBankerUser )
		{
			tagAnalyseResult FriendAnalyseResult ;
			ZeroMemory( &FriendAnalyseResult, sizeof( FriendAnalyseResult ) ) ;

			//�����˿�
			AnalysebCardData( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendAnalyseResult ) ;

			//�����ж�
            if (m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
                m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
			{
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
			}

			//���±�û��
			tagOutCardTypeResult FriendCardTypeResult[(12+1)] ;
			ZeroMemory( FriendCardTypeResult, sizeof( FriendCardTypeResult ) ) ;

			AnalyseOutCardType( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendCardTypeResult ) ;

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//ʣ���˿�
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//�����˿�
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//�����ж�
                if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
                    cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
				{
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//ʣ���˿�
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//�����˿�
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//�����ж�
                if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
                    cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
				{
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}


		}
	}

	//�Լҿɷ����
	if( m_wBankerUser != wOutCardUser && ! CompareCard( cbTurnCardData, m_cbAllCardData[ m_wBankerUser ], cbTurnCardCount, m_cbUserCardCount[ m_wBankerUser ] ) )
	{
		//ׯ���˿�
		bool bBankerCanOut = false ;
		tagOutCardTypeResult BankerOutCardResult[(12+1)] ;
		ZeroMemory(BankerOutCardResult, sizeof(BankerOutCardResult)) ;

        //�����˿�
		AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbTurnCardData, cbTurnCardCount, BankerOutCardResult) ;
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType) if(BankerOutCardResult[cbCardType].cbCardTypeCount>0) bBankerCanOut = true ; 

		if(!bBankerCanOut)
		{
		//�Լ�ID
		WORD wFriendChairID = (m_wBankerUser+1)%GAME_PLAYER ;

		//�����˿�
		tagOutCardTypeResult FriendCardTypeResult[(12+1)] ;
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
		AnalyseOutCardType(m_cbAllCardData[wFriendChairID], m_cbUserCardCount[wFriendChairID], FriendCardTypeResult) ;

		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(FriendCardTypeResult[cbCardType].cbCardTypeCount>0)
			{
				for(LONG lIndex=0; lIndex<FriendCardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{
					if(TestOutAllCard(wFriendChairID, FriendCardTypeResult[cbCardType].cbCardData[lIndex], FriendCardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
					{
						//��ѹ�Լ�
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}
			}
		}
	}

	//���߶Լ�
/*	if (GetCardType(m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) == GetCardType(cbTurnCardData, cbTurnCardCount) &&
		CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) &&
		!CompareCard(cbTurnCardData, m_cbAllCardData[m_wBankerUser], cbTurnCardCount, m_cbUserCardCount[m_wBankerUser]))
		return ;

	if (CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 1) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 1) % GAME_PLAYER]) &&
		!CompareCard(cbTurnCardData, m_cbAllCardData[m_wBankerUser], cbTurnCardCount, m_cbUserCardCount[m_wBankerUser])) return ;*/

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+2)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					if (cbBombCardType == CT_BOMB_CARD || cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 ||cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 || 
                        cbBombCardType == CT_MISSILE_CARD || cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;

					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					//����ը��
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;


					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

						bFindBestCard = true ;
					}
				}
			}
		}
		//ֱ�ӷ���
        if (bFindBestCard) 
        {
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

	//���ׯ��û�д���������ѹ�Լ���
	if( m_cbUserCardCount[m_wBankerUser]<=5 && wOutCardUser!=m_wBankerUser &&
		(BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0 || GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0])<=GetCardLogicValue(cbTurnCardData[0])) &&
		CardTypeResult[cbOutCardType].cbEachHandCardCount[0]!=cbHandCardCount &&
        cbOutCardType < CT_BOMB_CARD)//����һ�γ���
	{
		//��������
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//�¼�Ϊ���������ҵ����˿�����5��
	if(cbOutCardType <= CT_BOMB_CARD && m_cbUserCardCount[m_wBankerUser]<=5 && CardTypeResult[cbOutCardType].cbCardTypeCount>0  && 
		((GetCardLogicValue(cbTurnCardData[0])<12 && wOutCardUser!=m_wBankerUser && BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0) ||//�Լҳ���
		(wOutCardUser==m_wBankerUser)))//��������
	{
		//��ֹ�����ȴ����Ƴ�ȥ
		BYTE Index = cbOutCardType == CT_SINGLE ? 0 : CardTypeResult[cbOutCardType].cbCardTypeCount - 1 ;
		//Ѱ�ҿ���ѹס��������Сһ����
		BYTE cbThisOutTypeMinSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[0], CardTypeResult[cbOutCardType].cbEachHandCardCount[0]) ;
		BYTE cbBestIndex = 255 ;
		for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
		{
			BYTE cbTmpSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
			if((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0 && 
				GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0]) ||
				BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0) && 
				cbTmpSingleCardCount<=cbThisOutTypeMinSingleCount)
			{
				cbBestIndex = i ;
				cbThisOutTypeMinSingleCount = cbTmpSingleCardCount ;
			}

			if((BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount>0 && 
				GetCardLogicValue(CardTypeResult[cbOutCardType].cbCardData[i][0])>=GetCardLogicValue(BankerOutCardTypeResult[cbOutCardType].cbCardData[0][0]) ||
				BankerOutCardTypeResult[cbOutCardType].cbCardTypeCount==0))
				Index = i ;
			else break ;
		}

		if(cbBestIndex!=255)
		{
			OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[cbBestIndex] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[cbBestIndex], OutCardResult.cbCardCount) ;
		}
		else
		{
			OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[Index] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;
		}
        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

		return ;
	}

	//���ƶ���
	if (CT_SINGLE == cbOutCardType && CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
	{
		BYTE cbMeSingleCardData[MAX_COUNT], cbMeSingleCardCount ;
		BYTE cbBankerSingleCardData[MAX_COUNT], cbBankerSingleCardCount ;

		//��ȡ����
		cbMeSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbMeSingleCardData) ;
		cbBankerSingleCardCount = AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;

		//��������С��
		if (cbBankerSingleCardCount > 0 && cbMeSingleCardCount > 0 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) <= 10)
		{
			//��������
			if (cbBankerSingleCardCount >= 2 && GetCardValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) <= 10)
			{
				for (int nMeIndex = cbMeSingleCardCount-1; nMeIndex >=0 ; --nMeIndex)
					if (GetCardLogicValue(cbMeSingleCardData[nMeIndex]) > GetCardLogicValue(cbTurnCardData[0]) && 
						GetCardLogicValue(cbMeSingleCardData[nMeIndex]) >= GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) &&
						GetCardLogicValue(cbMeSingleCardData[nMeIndex]) <= 15)
					{
						OutCardResult.cbCardCount = 1 ;
						OutCardResult.cbResultCard[0] = cbMeSingleCardData[nMeIndex] ;
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

						return ;
					}
			}	

			//����һ��
			for (int nMeIndex = cbMeSingleCardCount-1; nMeIndex >=0 ; --nMeIndex)
				if (GetCardLogicValue(cbMeSingleCardData[nMeIndex]) > GetCardLogicValue(cbTurnCardData[0]) && 
					GetCardLogicValue(cbMeSingleCardData[nMeIndex]) >= GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) &&
					GetCardLogicValue(cbMeSingleCardData[nMeIndex]) <= 15)
				{
					OutCardResult.cbCardCount = 1 ;
					OutCardResult.cbResultCard[0] = cbMeSingleCardData[nMeIndex] ;
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

					return ;
				}
		}
	}

	//ȡ���ĸ���С����
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
    tagOutCardTypeResult CanOutCard = {0};

    if (cbOutCardType < CT_BOMB_CARD)
    {
        CopyMemory(&CanOutCard, &CardTypeResult[cbOutCardType], sizeof(tagOutCardTypeResult));
    }
	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//����cbMinSingleCardCount[4]�����ֵ
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	//������������
	BYTE cbBankerDoubleCardData[MAX_COUNT], cbBankerDoubleCardCount ;
	GetAllDoubleCard(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerDoubleCardData, cbBankerDoubleCardCount) ;

	//���ѳ���
	bool bFriendOut = m_wBankerUser!=wOutCardUser ;
	if(bFriendOut)
	{
		//�������������һ����
		if (GetCardType(m_cbAllCardData[(1 + m_wBankerUser) % GAME_PLAYER], m_cbUserCardCount[(1 + m_wBankerUser) % GAME_PLAYER]) != CT_ERROR) return ;

		//�������TestOutAllCard���ѶԿɳ�ը�����������
		if(CanOutCard.cbCardTypeCount>0 && CanOutCard.cbCardType < CT_BOMB_CARD)
		{
			//������������
			BYTE cbBankerSingleCardData[MAX_COUNT] ;
			BYTE cbBankerSingleCardCount=AnalyseSinleCardCount(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], NULL, 0, cbBankerSingleCardData) ;
			BYTE cbBankerSingleCardLogic = 0 ;

			if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-2]) ;
			else if(cbBankerSingleCardCount>=2 && GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[cbBankerSingleCardCount-1]) ;
			else if(cbBankerSingleCardCount>0 && GetCardLogicValue(cbBankerSingleCardData[0])<=10) cbBankerSingleCardLogic = GetCardLogicValue(cbBankerSingleCardData[0]) ;

			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				//�����������Ʋ��ӶԼ���
				if ( CanOutCard.cbCardType >= CT_THREE &&  CanOutCard.cbCardType <= CT_MISSILE_CARD && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= 7 && 
					CanOutCard.cbEachHandCardCount[Index] <=5 )
					continue ;

				//��������
				bool bCanOut = false ;
				if(cbOutCardType==CT_SINGLE && cbBankerSingleCardCount > 0 && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= cbBankerSingleCardLogic &&
					GetCardLogicValue(cbTurnCardData[0]) < 14 && cbMinSingleCardCount[i] < cbOriginSingleCardCount &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > GetCardLogicValue(cbTurnCardData[0]))
					bCanOut = true ;

				//���ض���
				//if (cbOutCardType == CT_DOUBLE && cbBankerDoubleCardCount > 0 && GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) < 10 &&
				//	GetCardLogicValue(CanOutCard.cbCardData[Index][0]) < GetCardLogicValue(cbBankerDoubleCardData[cbBankerDoubleCardCount-1]) &&
				//	GetCardLogicValue(CanOutCard.cbCardData[0][0]) >= 10 && GetCardLogicValue(CanOutCard.cbCardData[0][0]) < 14) continue ;

				//С��J���ƣ�����С��K������ɢ��
				if(bCanOut ||
					((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3 &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_TWO ) /*&& 
					(GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=11 || (cbMinSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=13)*/) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]) && cbHandCardCount>5))
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true ;
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount) bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
				else if(cbHandCardCount<5 && cbMinSingleCardCount[i]<cbOriginSingleCardCount+4 && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//�ܳ������Լҵ�2
					if ( GetCardLogicValue( CanOutCard.cbCardData[Index][0] ) >= 16 && GetCardLogicValue( cbTurnCardData[0] ) >= 15 )
						continue ;

					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}

			if(bFindCard)
			{

				//���ñ���
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

				return ;
			}
			//��������������
			else if(cbHandCardCount<=5)
			{
				BYTE cbMinLogicCard = GetCardLogicValue(0x4f)+1 ;
				BYTE cbCanOutIndex = 0 ;
				for(BYTE i=0; i<4; ++i)
					if(cbMinSingleCardCount[i]<MAX_COUNT && cbMinSingleCardCount[i]<=cbMinSingleCountInFour && cbMinLogicCard>GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) && 
						GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) ;
						cbCanOutIndex = cbIndex[i] ;
					}

					if(cbMinLogicCard != (GetCardLogicValue(0x4f)+1))
					{
						//���ñ���
						OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
						CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

						return ;
					}
			}
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

			return ;
		}
		else
		{
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

	}
	//��������
	else
	{
		if(CanOutCard.cbCardTypeCount>0)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+4)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_TWO ) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//��Դ���
					bool bNoLargeCard = true ;

					//������������������4�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if(m_cbUserCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5  && CanOutCard.cbEachHandCardCount[Index]>=2 && 
						GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
						GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						//bNoLargeCard=false ;

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
							bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
			}

			if(bFindCard)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;
				bool bCanOut=true ;

				//��ֻѹ2
				if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
				{
					if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false ;
					}
				}
				 //˫���ж�
				if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=6 && cbHandCardData[5]==0x4E)
				{
					bool bOutMissileCard = false ;
					//һ���ƺ�һ��ը��
					BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					RemoveCard(cbRemainCardData, 6, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= 6 ;
					if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

					//ֻʣ������
					BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
					if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

					//ը������
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
						CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
					if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

					//����С��
					if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
						GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > 10)
						bOutMissileCard = false ;

					//����˿�
					if(bOutMissileCard)
					{
						//��������ը��
						BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

						OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
                        g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}

				if(bCanOut)
				{
					//���ñ���
					OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
                    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

					return ;
				}
			}

			if(cbOutCardType==CT_SINGLE)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;

				if(GetCardLogicValue(cbTurnCardData[0])==14 || 
					GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
					(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
					m_cbUserCardCount[m_wBankerUser]<=5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE cbIndex=MAX_COUNT ;
					for(BYTE i=0; i<cbHandCardCount; ++i)
						if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&	GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbIndex = i ;
						}
						if(cbIndex!=MAX_COUNT)
						{
							//���ñ���
							OutCardResult.cbCardCount=1;
							OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;

                            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
							return ;
						}
				}
			}
		}

		//��Ҫ����ը��
		if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0 /*&& 
			(NORMAL_COUNT  == cbHandCardCount&& NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+1)%GAME_PLAYER])*/)
		{
			tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
            BYTE Index = 0 ;
            /*Ѱ����Сը��
			BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
			for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
			{
				if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
					Index = i ;
				}
			}*/
            Index = BomCard.cbCardTypeCount-1;
			bool bOutBomb = false ;

			//�����ж�
			if (NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+1)%GAME_PLAYER] && CT_ERROR != GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]))
				bOutBomb = true ;

			//һ���ƺ�һ��ը��
			BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
			CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
			RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
			cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
			if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

			//ը������
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
			if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])) 
				bOutBomb = true ;

			//ֻʣһ��
			BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
			if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
				bOutBomb = true ;

			//ֻʣ������
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
			if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

            //ը����������
            cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
            if(cbSingleCardCount <= 1)
            {
                bOutBomb = true ;
            }

            //�����˿�
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

			if (m_cbUserCardCount[m_wBankerUser] ==1 &&  (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSignedCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8 + 1 >= cbRemainCardCount)) bOutBomb = true ;

			//���ñ���
			if(bOutBomb)
			{
				OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
			}

            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

//         g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 		return ;
	}
    //������ֻʣ��һ�Ż���һ�֣�һ��Ҫѹ
    if (OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[m_wBankerUser] == 1 || (GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) != CT_ERROR)))
    {
        //ը��ѹ
        if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
        {
            OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
            CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
        //��ͨѹ ѡʣ�µ������ٵķ���
        if (cbOutCardType < CT_BOMB_CARD && CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
        {
            BYTE bMinCardCount = 0;
            BYTE bCurMinNum = MAX_COUNT;
            for (BYTE i = 0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; i++)
            {
                BYTE bMinNum = 0;
                bMinNum = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i],CardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
                if (bCurMinNum > bMinNum)
                {
                    bCurMinNum = bMinNum;
                    bMinCardCount = i;
                }
            }
            OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[bMinCardCount];
            CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[bMinCardCount], OutCardResult.cbCardCount*sizeof(BYTE));
            g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
    }
    g_Logger.WriteLog("---------------UpsideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
	return ;
}

//�����¼ң��ȳ��ƣ�
void CGameLogic::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wMeChairID,tagOutCardResult & OutCardResult) 
{
	//���±�û��
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;
    g_Logger.WriteLog("---------------UndersideOfBankerOutCard ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);

    //��ʣ��ը��
    BYTE cbBomCardData[MAX_COUNT] = {0};
    BYTE cbBomCardCount = 0;
    tagBomCardResult BomCardResult = {0};
    AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;
    GetAllBomCardOrder(cbHandCardData,cbHandCardCount, cbBomCardData, cbBomCardCount, BomCardResult);
    if (cbBomCardCount == cbHandCardCount) 
    {
        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
        return ;
    }

	BYTE cbLineCard[MAX_COUNT] ;
	BYTE cbThreeLineCard[MAX_COUNT] ;
	BYTE cbDoubleLineCard[MAX_COUNT] ;
	BYTE cbLineCardCount;
	BYTE cbThreeLineCardCount ;
	BYTE cbDoubleLineCount ;
	GetAllLineCard(cbHandCardData, cbHandCardCount, cbLineCard, cbLineCardCount) ;
	GetAllThreeCard(cbHandCardData, cbHandCardCount, cbThreeLineCard, cbThreeLineCardCount) ;
	GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbDoubleLineCard, cbDoubleLineCount) ;

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
                    if (cbBombCardType == CT_BOMB_CARD|| cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 || cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 ||
                        cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;
					else if ( 15 == GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 2;
					else if ( 15 < GetCardLogicValue( CardTypeResult[ cbCardType ].cbCardData[ lIndex ][ 0 ] ) ) cbTmpSingleCount += 3;


					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;
					
					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;

                        bFindBestCard = true ;
					}
				}
			}
		}
	//ֱ�ӷ���
        if (bFindBestCard) {
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

	//����������
	if ( cbHandCardCount == 8 && GetCardLogicValue(cbHandCardData[5]) == 16 && m_cbUserCardCount[m_wBankerUser] == 1 && 
		GetCardLogicValue(cbHandCardData[5]) < GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
	{
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[5] ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

    //�Ĵ������ж� ���ֳ���֧��
// 	if ( AnalyseFourCardType(cbHandCardData, cbHandCardCount, m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], OutCardResult ) )
// 	{
//         g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 		return ;
// 	}


	//�����˳�ƺ͵�ֻ��һ�ԣ����ҵ�ֻ��Աȵ�����С�����ȳ�˳
	{
		if(cbLineCardCount+1==cbHandCardCount && CT_SINGLE==GetCardType(cbLineCard, cbLineCardCount))
		{
			OutCardResult.cbCardCount = cbLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbLineCard, cbLineCardCount) ;
		}
		else if(cbThreeLineCardCount+1==cbHandCardCount && CT_THREE_LINE==GetCardType(cbThreeLineCard, cbThreeLineCardCount))
		{
			OutCardResult.cbCardCount = cbThreeLineCardCount ;
			CopyMemory(OutCardResult.cbResultCard, cbThreeLineCard, cbThreeLineCardCount) ;
		}
		else if(cbDoubleLineCount+1==cbHandCardCount && CT_DOUBLE_LINE==GetCardType(cbDoubleLineCard, cbDoubleLineCount))
		{
			OutCardResult.cbCardCount = cbDoubleLineCount ;
			CopyMemory(OutCardResult.cbResultCard, cbDoubleLineCard, cbDoubleLineCount) ;
		}
		//˫��ը����һ��
		else if(cbHandCardCount>6 && cbHandCardData[5]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
			CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+6, cbHandCardCount-6) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+6, cbHandCardCount-6))
		{
			OutCardResult.cbCardCount = 6 ;
            OutCardResult.cbResultCard[0] = 0x4f ;
            OutCardResult.cbResultCard[1] = 0x4f ;
            OutCardResult.cbResultCard[2] = 0x4f ;
            OutCardResult.cbResultCard[3] = 0x4e ;
            OutCardResult.cbResultCard[4] = 0x4e ;
            OutCardResult.cbResultCard[5] = 0x4e ;
		}

		if(OutCardResult.cbCardCount>0)
		{
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
	}
	//������һֻ
	if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}
	//����
	else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
	{
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
        return ;
	}
	//ֻʣһ����
	else if(CT_ERROR!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_ONE!=GetCardType(cbHandCardData, cbHandCardCount) && CT_FOUR_LINE_TAKE_TWO!=GetCardType(cbHandCardData, cbHandCardCount))
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
	}

	//ֻʣһ�ź�һ��
	if(cbHandCardCount>=2)
	{
		//�����˿�
		tagOutCardTypeResult BankerCanOutCardType1[(12+1)] ;
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1)) ;
		tagOutCardTypeResult BankerCanOutCardType2[(12+1)] ;
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2)) ;

		BYTE cbFirstHandCardType = GetCardType(cbHandCardData, cbHandCardCount-1) ;
		BYTE cbSecondHandCardType = GetCardType(cbHandCardData+1, cbHandCardCount-1) ;

		//�Ƿ���ը
		BYTE cbAllBombCardData[MAX_COUNT], cbAllBombCount=0 ;
		GetAllBomCard(cbHandCardData, cbHandCardCount, cbAllBombCardData, cbAllBombCount) ;

		//û��ը��
		if (cbAllBombCount <= 0 && cbFirstHandCardType!=CT_THREE_LINE_TAKE_ONE && cbFirstHandCardType!=CT_THREE_LINE_TAKE_TWO)
		{
		//�������Գ�����
		if(cbFirstHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData, cbHandCardCount-1, BankerCanOutCardType1) ;
		if(cbSecondHandCardType!=CT_ERROR)
			AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbHandCardData+1, cbHandCardCount-1, BankerCanOutCardType2) ;

		if(cbSecondHandCardType!=CT_ERROR && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO && 
			BankerCanOutCardType2[cbSecondHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(cbFirstHandCardType!=CT_ERROR && cbFirstHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbFirstHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType1[cbFirstHandCardType].cbCardTypeCount==0 && BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(GetCardLogicValue(cbHandCardData[0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) &&
			CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[0] ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		if(CT_ERROR!=cbSecondHandCardType && cbSecondHandCardType!=CT_FOUR_LINE_TAKE_ONE && cbSecondHandCardType!= CT_FOUR_LINE_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].cbCardTypeCount==0)
		{
			OutCardResult.cbCardCount = cbHandCardCount-1 ;
			CopyMemory(OutCardResult.cbResultCard, cbHandCardData+1, cbHandCardCount-1) ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
		}
		//����ը��
		else
		{
			//��ȥը�����
			BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0 ;
			CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
			cbRemainCount = cbHandCardCount ;
			RemoveCard(cbAllBombCardData, cbAllBombCount, cbRemainCard, cbRemainCount) ;
			cbRemainCount -= cbAllBombCount ;
			if (GetCardType(cbRemainCard, cbRemainCount) != CT_ERROR)
			{
				OutCardResult.cbCardCount = cbRemainCount ;
				CopyMemory(OutCardResult.cbResultCard, cbRemainCard, cbRemainCount) ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
		}
	}

	//���߶���
	WORD wFriendID = (m_wBankerUser+2)%GAME_PLAYER ;
	BYTE cbFriendCardType = GetCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID]) ;
	
	//�����˿�
	if(CT_SINGLE==cbFriendCardType)
	{
		//�Ϸ��ж�
		ASSERT(m_cbUserCardCount[wFriendID]==1) ;
		if(m_cbUserCardCount[wFriendID]==1 && GetCardLogicValue(cbHandCardData[cbHandCardCount-1]) < GetCardLogicValue(m_cbAllCardData[wFriendID][0]))
		{
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
	}
	//һ���˿�
	else if(CT_DOUBLE==cbFriendCardType && cbDoubleLineCount>=2)
	{
		if(GetCardLogicValue(cbDoubleLineCard[cbDoubleLineCount-1]) < GetCardLogicValue(m_cbAllCardData[wFriendID][0]))
		{
			OutCardResult.cbCardCount = 2 ;
			OutCardResult.cbResultCard[0] = cbDoubleLineCard[cbDoubleLineCount-2] ;
			OutCardResult.cbResultCard[1] = cbDoubleLineCard[cbDoubleLineCount-1] ;

            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
	}

	//���ƽ����ж�
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && m_cbUserCardCount[wFriendID] >= 2)
	{
		tagAnalyseResult MeAnalyseResult ;
		ZeroMemory(&MeAnalyseResult, sizeof(MeAnalyseResult)) ;

		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, MeAnalyseResult) ;

		tagAnalyseResult FriendAnalyseResult ;
		ZeroMemory(&FriendAnalyseResult, sizeof(FriendAnalyseResult)) ;

		//�����˿�
		AnalysebCardData(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], FriendAnalyseResult) ;

		//�����ж�
		if ((m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
			m_cbUserCardCount[wFriendID] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1)) &&
			MeAnalyseResult.cbDoubleCount > 0 && FriendAnalyseResult.cbDoubleCount > 0)
		{
			//��С����
			BYTE cbMeLeastDoubleCardLogic = GetCardLogicValue(MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2]) ;
			//������
			BYTE cbFriendLargestDoublecardLogic = GetCardLogicValue(FriendAnalyseResult.cbDoubleCardData[0]) ;

			//�����ж�
			if (cbMeLeastDoubleCardLogic < 14 && cbMeLeastDoubleCardLogic < cbFriendLargestDoublecardLogic)
			{
				OutCardResult.cbCardCount = 2 ;
				OutCardResult.cbResultCard[0] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-2] ;
				OutCardResult.cbResultCard[1] = MeAnalyseResult.cbDoubleCardData[MeAnalyseResult.cbDoubleCount*2-1] ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
          			
		}
	}

	//�¼�Ϊ���������ҵ����˿�����5��
	//	if(m_cbUserCardCount[m_wBankerUser]<=5)
	{
		//�����˿�
		tagOutCardTypeResult MeOutCardTypeResult[(12+1)] ;
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult)) ;
		AnalyseOutCardType(cbHandCardData, cbHandCardCount, MeOutCardTypeResult) ;

		//�Լ��˿�
		WORD wFriendID ;
		for(WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID) 
			if(wChairID!=m_wBankerUser && wMeChairID!=wChairID) wFriendID = wChairID ;

		//���㵥��
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//��ը�������
		for(BYTE cbCardType=CT_DOUBLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[cbCardType] ;

			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE cbThisHandCardCount = MAX_COUNT ;

			//�����˿�
			tagOutCardTypeResult BankerCanOutCard[(12+1)] ;
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard)) ;

			tagOutCardTypeResult FriendOutCardTypeResult[(12+1)] ;
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult)) ;

			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//�������ж�
				if ( cbCardType == CT_DOUBLE )
				{
					tagAnalyseResult AnalyseResult ;
					ZeroMemory( &AnalyseResult,  sizeof( AnalyseResult ) ) ;
					AnalysebCardData( cbHandCardData, cbHandCardCount, AnalyseResult ) ;
					if ( AnalyseResult.cbSignedCount + AnalyseResult.cbThreeCount * 3 == cbHandCardCount )
					{
						bool bContinue = false ;
						for ( BYTE cbThreeIndex = 0; cbThreeIndex < AnalyseResult.cbThreeCount; ++cbThreeIndex )
							if ( GetCardValue(  tmpCardResult.cbCardData[i][0] ) == GetCardValue( AnalyseResult.cbThreeCardData[3 * cbThreeIndex] ) )
							{
								bContinue = true ;
								break ;
							}
						if ( bContinue ) continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				//���·���
				if(tmpCardResult.cbEachHandCardCount[i]!=cbThisHandCardCount)
				{
					cbThisHandCardCount = tmpCardResult.cbEachHandCardCount[i] ;
					AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], 
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,BankerCanOutCard) ;
					AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID],
						tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i] ,FriendOutCardTypeResult) ;
				}

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				
				//���˳�ӣ������Ĵ���
				BYTE cbCurrentCardType = GetCardType(tmpCardResult.cbCardData[i], cbThisHandCardCount) ;
				if (cbThisHandCardCount != cbHandCardCount && cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//�Ƴ��˿�
					RemoveCard(tmpCardResult.cbCardData[i], cbThisHandCardCount, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= cbThisHandCardCount ;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && cbThisHandCardCount <=5 || 
						cbCurrentCardType == CT_SINGLE_LINE && cbThisHandCardCount <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCurrentCardType >= CT_DOUBLE_LINE && cbCurrentCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && cbThisHandCardCount <= 8))
					{
						//��ʱ����
						if ( cbCurrentCardType >= CT_SINGLE_LINE && cbCurrentCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][cbThisHandCardCount - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCurrentCardType >= CT_THREE_LINE_TAKE_ONE && cbCurrentCardType <= CT_THREE_LINE_TAKE_TWO &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				//��Դ�ԣ������ȳ���
				if (cbCardType == CT_DOUBLE && GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11)
				{
					BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCount ;
					cbAllSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;
					if (cbAllSingleCount >= 2 && GetCardLogicValue(cbAllSingleCardData[cbAllSingleCount-2]) < 10) continue ;
				}

				//��������ѹ�ƣ����Ҷ��Ѳ�����ѹ����
				if((BankerCanOutCard[cbCardType].cbCardTypeCount>0&&FriendOutCardTypeResult[cbCardType].cbCardTypeCount==0) || (BankerCanOutCard[cbCardType].cbCardTypeCount>0 && FriendOutCardTypeResult[cbCardType].cbCardTypeCount>0 &&
					GetCardLogicValue(FriendOutCardTypeResult[cbCardType].cbCardData[0][0])<=GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0])))
				{
					//�����ܵ�
					if( BankerCanOutCard[cbCardType].cbEachHandCardCount[0] > 0 && m_cbUserCardCount[m_wBankerUser]<=BankerCanOutCard[cbCardType].cbEachHandCardCount[0]+1)
						continue ;

					//�Լ����������û���Ȩ
					//if(GetCardLogicValue(tmpCardResult.cbCardData[0][0]) < GetCardLogicValue(BankerCanOutCard[cbCardType].cbCardData[0][0]) && 
					//	BankerCanOutCard[cbCardType].cbCardTypeCount > 0) 
					//	continue ;
				}
				//�Ƿ��д���
				if(tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount)
				{
					bool bHaveLargeCard=false ;
					for(BYTE j=0; j<tmpCardResult.cbEachHandCardCount[i]; ++j)
						if(GetCardLogicValue(tmpCardResult.cbCardData[i][j])>=15) bHaveLargeCard=true ;

					if(cbCardType!=CT_SINGLE_LINE && cbCardType!=CT_DOUBLE_LINE && GetCardLogicValue(tmpCardResult.cbCardData[i][0])==14) bHaveLargeCard=true ; 

					if(bHaveLargeCard) continue ;
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if(tmpCardResult.cbEachHandCardCount[i]==m_cbUserCardCount[m_wBankerUser] && cbCardType==GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) &&
					GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])>GetCardLogicValue(tmpCardResult.cbCardData[i][0])) continue ;

				//����cbMinSingleCardCount[4]�����ֵ
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//������Сֵ
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//��С����
					cbMinCardType = cbCardType ;
					//��С�����е���С����
					cbMinSingleCountInFour=cbTmpCount ;						
					//��С�����е���С��
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour>=AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0)+3 && 
			m_cbUserCardCount[m_wBankerUser]>4)
			cbMinSingleCountInFour=MAX_COUNT ;

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//������һֻ
			if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                return ;
			}
			//����
			else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                return ;
			}
			else
			{
				//���ñ���
				OutCardResult.cbCardCount=MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,MeOutCardTypeResult[cbMinCardType].cbCardData[Index],MeOutCardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		//��������˿�����5����û���ҵ��ʺϵ�����Ӵ����С
		if(OutCardResult.cbCardCount<=0 && m_cbUserCardCount[m_wBankerUser]<=5)
		{
			//ֻ��һ����ʱ���ܷŵ�����
			if(m_cbUserCardCount[m_wBankerUser]==1 && MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount>0)
			{
				//��Сһ��
				BYTE Index=MAX_COUNT ;
				for(BYTE i=0; i<MeOutCardTypeResult[CT_SINGLE].cbCardTypeCount; ++i)
				{
					if(GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].cbCardData[i][0])>=GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]))
					{
						Index=i ;
					}
					else break ;
				}

				if(MAX_COUNT!=Index)
				{
					OutCardResult.cbCardCount = MeOutCardTypeResult[CT_SINGLE].cbEachHandCardCount[Index] ;
					CopyMemory(OutCardResult.cbResultCard, MeOutCardTypeResult[CT_SINGLE].cbCardData[Index], OutCardResult.cbCardCount) ;
                    g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}
		}
	}

	BYTE cbFirstCard=0 ;
	//��������2
	for(BYTE i=0; i<cbHandCardCount; ++i) 
		if(GetCardLogicValue(cbHandCardData[i])<15)
		{
			cbFirstCard = i ;
			break ;
		}

		if(cbFirstCard<cbHandCardCount-1)
			AnalyseOutCardType(cbHandCardData+cbFirstCard, cbHandCardCount-cbFirstCard, CardTypeResult) ;
		else
			AnalyseOutCardType(cbHandCardData, cbHandCardCount, CardTypeResult) ;

		//���㵥��
		BYTE cbMinSingleCardCount[4] ;
		cbMinSingleCardCount[0]=MAX_COUNT ;
		cbMinSingleCardCount[1]=MAX_COUNT ;
		cbMinSingleCardCount[2]=MAX_COUNT ;
		cbMinSingleCardCount[3]=MAX_COUNT ;
		BYTE cbIndex[4]={0} ;
		BYTE cbOutcardType[4]={CT_ERROR} ;
		BYTE cbMinValue=MAX_COUNT ; 
		BYTE cbMinSingleCountInFour=MAX_COUNT ;
		BYTE cbMinCardType=CT_ERROR ;
		BYTE cbMinIndex=0 ;

		//��ը�������
		for(BYTE cbCardType=CT_SINGLE; cbCardType<CT_BOMB_CARD; ++cbCardType)
		{
			tagOutCardTypeResult const &tmpCardResult = CardTypeResult[cbCardType] ;
			for(BYTE i=0; i<tmpCardResult.cbCardTypeCount; ++i)
			{
				//ׯ�ҿ����ߵ�
				if ( CompareCard(tmpCardResult.cbCardData[i], m_cbAllCardData[m_wBankerUser], tmpCardResult.cbEachHandCardCount[i], m_cbUserCardCount[m_wBankerUser]) )
					continue ;

				//���˳�ӣ������Ĵ���
				if ( tmpCardResult.cbEachHandCardCount[i] != cbHandCardCount && cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
					( GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i]-1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-2]) || 
						GetCardLogicValue(tmpCardResult.cbCardData[i][0]) >= 11 ))
				{
					BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					cbRemainCardCount = cbHandCardCount ;

					//�Ƴ��˿�
					RemoveCard(tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i], cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= tmpCardResult.cbEachHandCardCount[i] ;

					//����˿�
					BYTE cbCurrentLargestLogicCard = GetCardLogicValue(tmpCardResult.cbCardData[i][0]) ;

					if (GetCardType(cbRemainCardData, cbRemainCardCount) == CT_ERROR && (cbCardType >= CT_THREE_LINE_TAKE_ONE && 
						cbCardType <= CT_THREE_LINE_TAKE_TWO && cbCurrentLargestLogicCard >= 11 && tmpCardResult.cbEachHandCardCount[i] <=5 || 
						cbCardType == CT_SINGLE_LINE && tmpCardResult.cbEachHandCardCount[i] <= 6 && cbCurrentLargestLogicCard >= 12 || 
						cbCardType >= CT_DOUBLE_LINE && cbCardType <= CT_THREE_LINE && cbCurrentLargestLogicCard >= 12 && tmpCardResult.cbEachHandCardCount[i] <= 8))
					{
						//��ʱ����
						if ( cbCardType >= CT_SINGLE_LINE && cbCardType <= CT_THREE_LINE &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][tmpCardResult.cbEachHandCardCount[i] - 1]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;

						if ( cbCardType >= CT_THREE_LINE_TAKE_ONE && cbCardType <= CT_THREE_LINE_TAKE_TWO &&
							GetCardLogicValue(tmpCardResult.cbCardData[i][0]) > GetCardLogicValue(cbHandCardData[cbHandCardCount-3]) )
							continue ;
					}
				}

				BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, tmpCardResult.cbCardData[i], tmpCardResult.cbEachHandCardCount[i]) ;

				BYTE cbMaxValue=0 ; 
				BYTE Index = 0 ;

				//����cbMinSingleCardCount[4]�����ֵ
				for(BYTE j=0; j<4; ++j)
				{
					if(cbMinSingleCardCount[j]>=cbTmpCount)
					{
						cbMinSingleCardCount[j] = cbTmpCount ;
						cbIndex[j] = i ;
						cbOutcardType[j] = cbCardType ;
						break ;
					}
				}

				//������Сֵ
				if(cbMinSingleCountInFour>=cbTmpCount)
				{
					//��С����
					cbMinCardType = cbCardType ;
					//��С�����е���С����
					cbMinSingleCountInFour=cbTmpCount ;						
					//��С�����е���С��
					cbMinIndex=i ;
				}
			}
		}

		if(cbMinSingleCountInFour!=MAX_COUNT)
		{
			BYTE Index = cbMinIndex ;

			//ѡ����С��
			for(BYTE i=0; i<4; ++i)
			{
				if(cbOutcardType[i]==cbMinCardType && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[cbIndex[i]][0])<GetCardLogicValue(CardTypeResult[cbMinCardType].cbCardData[Index][0]))
					Index = cbIndex[i] ;
			}

			//������һֻ
			if(cbHandCardCount==7 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}
			//����
			else if(cbHandCardCount==6 && GetCardColor(cbHandCardData[5])==0x40)
			{
                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = 0x4f ;
                OutCardResult.cbResultCard[1] = 0x4f ;
                OutCardResult.cbResultCard[2] = 0x4f ;
                OutCardResult.cbResultCard[3] = 0x4e ;
                OutCardResult.cbResultCard[4] = 0x4e ;
                OutCardResult.cbResultCard[5] = 0x4e ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                return ;
			}
			else
			{
				//���ñ���
				OutCardResult.cbCardCount=CardTypeResult[cbMinCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,CardTypeResult[cbMinCardType].cbCardData[Index],CardTypeResult[cbMinCardType].cbEachHandCardCount[Index]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			ASSERT(OutCardResult.cbCardCount>0) ;

            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}
		//���ֻʣը��
		else if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0) 
		{	
			//BYTE Index=0 ;
			//BYTE cbLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			////��Сը��
			//for(BYTE i=0; i<CardTypeResult[CT_BOMB_CARD].cbCardTypeCount; ++i)
			//	if(cbLogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]))
			//	{
			//		cbLogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].cbCardData[i][0]) ;
			//		Index = i ;
			//	}

            //���ñ���
            OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
            CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));

            return ;		
        }

		BYTE cbAllSingleCardData[MAX_COUNT], cbAllSingleCardCount ;
		cbAllSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, NULL, 0, cbAllSingleCardData) ;

		if ( cbAllSingleCardCount > 0 )
		{
			//�����û���������ͳ���С��һ��
			OutCardResult.cbCardCount = 1 ;
			OutCardResult.cbResultCard[0] = cbAllSingleCardData[cbAllSingleCardCount-1] ;

            g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

		//�����û���������ͳ���С��һ��
		OutCardResult.cbCardCount = 1 ;
		OutCardResult.cbResultCard[0] = cbHandCardData[cbHandCardCount-1] ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard1 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
}
//�����¼ң�����ƣ�
void CGameLogic::UndersideOfBankerOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wOutCardUser, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(tagOutCardResult)) ;

	//���±�û��
	tagOutCardTypeResult CardTypeResult[(12+1)] ;
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;
    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 ----start----cbHandCardCount[%d],cbHandCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbHandCardCount,cbHandCardData[0],cbHandCardData[1],cbHandCardData[2],cbHandCardData[3],cbHandCardData[4],cbHandCardData[5],cbHandCardData[6],cbHandCardData[7],cbHandCardData[8],cbHandCardData[9],cbHandCardData[10],cbHandCardData[11],cbHandCardData[12],cbHandCardData[13],cbHandCardData[14],cbHandCardData[15],cbHandCardData[16],cbHandCardData[17],cbHandCardData[18],cbHandCardData[19]);
    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 -------------cbTurnCardCount[%d],cbTurnCardData[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",cbTurnCardCount,cbTurnCardData[0],cbTurnCardData[1],cbTurnCardData[2],cbTurnCardData[3],cbTurnCardData[4],cbTurnCardData[5],cbTurnCardData[6],cbTurnCardData[7],cbTurnCardData[8],cbTurnCardData[9],cbTurnCardData[10],cbTurnCardData[11],cbTurnCardData[12],cbTurnCardData[13],cbTurnCardData[14],cbTurnCardData[15],cbTurnCardData[16],cbTurnCardData[17],cbTurnCardData[18],cbTurnCardData[19]);

	//��������
	BYTE cbOutCardType = GetCardType(cbTurnCardData,cbTurnCardCount) ;

	//�����ɳ���
	tagOutCardTypeResult BankerOutCardTypeResult[(12+1)] ;
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult)) ;

	AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], BankerOutCardTypeResult) ;
	AnalyseOutCardType(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount, CardTypeResult) ;

	//ֻʣը��
    if(cbHandCardCount==CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] && (cbOutCardType<CT_BOMB_CARD || 
        CompareCard(cbTurnCardData,CardTypeResult[CT_BOMB_CARD].cbCardData[0],cbTurnCardCount,CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0])))
	{
		OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0] ;
		CopyMemory(OutCardResult.cbResultCard,  CardTypeResult[CT_BOMB_CARD].cbCardData, OutCardResult.cbCardCount) ;

		return ;
	}
	//����ը����һ��
	else if(cbHandCardCount>6 && cbHandCardData[5]==0x4e && CT_ERROR!=GetCardType(cbHandCardData+6, cbHandCardCount-6) &&
		CT_FOUR_LINE_TAKE_ONE != GetCardType(cbHandCardData+6, cbHandCardCount-6) && CT_FOUR_LINE_TAKE_TWO != GetCardType(cbHandCardData+6, cbHandCardCount-6))
	{
		OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;
		return ;
	}

	//ը����һ��
	BYTE cbRemainCard[MAX_COUNT], cbRemainCount=0;
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
    tagBomCardResult AllBombBomCardResult = {0};
	GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbAllBombCard, cbAllBombCount, AllBombBomCardResult, ST_MISSILE_FIRST) ;

	//��ը�ж�
	if(cbAllBombCount>0 && wOutCardUser==m_wBankerUser)
	{
		//ʣ���˿�
		CopyMemory(cbRemainCard, cbHandCardData, cbHandCardCount) ;
		cbRemainCount = cbHandCardCount ;
		RemoveCard(cbAllBombCard, cbAllBombCount, cbRemainCard, cbRemainCount) ;
		cbRemainCount -= cbAllBombCount ;
		
		if(CT_ERROR != GetCardType(cbRemainCard, cbRemainCount) || 
			(2==cbRemainCount && GetCardLogicValue(cbRemainCard[0])>GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
		{
            BYTE cbTempAllBombCard[MAX_COUNT], cbTempAllBombCount=0 ;
            tagBomCardResult TempBomCardResult = {0};
            GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbTempAllBombCard, cbTempAllBombCount, TempBomCardResult) ;

            if((cbOutCardType<CT_BOMB_CARD || CompareCard(cbTurnCardData, cbTempAllBombCard, cbTurnCardCount, TempBomCardResult.cbCardCount[0])/*GetCardLogicValue(cbAllBombCard[0])>GetCardLogicValue(cbTurnCardData[0])*/) &&
				( CardTypeResult[cbOutCardType].cbCardTypeCount <= 0 || CT_ERROR != GetCardType(cbRemainCard, cbRemainCount)) )
			{
				//˫��ը��
				if(GetCardColor(cbAllBombCard[5])==0x40)
				{
                    OutCardResult.cbCardCount = 6 ;
                    OutCardResult.cbResultCard[0] = 0x4f ;
                    OutCardResult.cbResultCard[1] = 0x4f ;
                    OutCardResult.cbResultCard[2] = 0x4f ;
                    OutCardResult.cbResultCard[3] = 0x4e ;
                    OutCardResult.cbResultCard[4] = 0x4e ;
                    OutCardResult.cbResultCard[5] = 0x4e ;
					return ;
				}
				else
				{
					//����������
					BYTE cbBankerAllBombCard[MAX_COUNT], cbBankerAllBombCardCount ;
                    tagBomCardResult BankerAllBomCardResult = {0};
					GetAllBomCardOrder( m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbBankerAllBombCard, cbBankerAllBombCardCount, BankerAllBomCardResult) ;

                    BYTE cbBombCard[MAX_COUNT], cbBombCardCount ;
                    tagBomCardResult BomCardResult = {0};
                    GetAllBomCardOrder(cbHandCardData, cbHandCardCount, cbBombCard, cbBombCardCount, BomCardResult) ;
					if ( !CompareCard( cbTurnCardData, cbRemainCard, cbTurnCardCount, cbRemainCount) ||  cbBankerAllBombCardCount <= 0 
//						 || GetCardLogicValue( cbAllBombCard[0] ) > GetCardLogicValue( cbBankerAllBombCard[0] ) )
//                        CompareCard(cbBankerAllBombCard, cbBombCard, BankerAllBomCardResult.cbCardCount[0], BomCardResult.cbCardCount[0])
                        )
					{
						/*OutCardResult.cbCardCount = BomCardResult.cbCardCount[0] ;
						CopyMemory(OutCardResult.cbResultCard, cbBombCard, BomCardResult.cbCardCount[0]) ;
                        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;*/
                        //���ñ���
                        OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
                        CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
                        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                        return ;
					}

				}
			}
		}
	}

	//ֻʣһ�ֳ�ը
	if ( CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0 && GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) != CT_ERROR &&
		GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) <= CT_DOUBLE )
	{
		//ֻʣ����
		if ( GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == CT_SINGLE )
		{
			BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
			BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
			CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
			OutCardResult.cbCardCount = cbCardCount ;
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

			return ;
		}
		else if ( GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == CT_DOUBLE )
		{
			BYTE cbAllDoubleCard[MAX_COUNT], cbAllDoubleCount ;
			GetAllDoubleCard(cbHandCardData, cbHandCardCount, cbAllDoubleCard, cbAllDoubleCount) ;

			if ( cbAllDoubleCount > 0 && GetCardLogicValue(cbAllDoubleCard[cbAllDoubleCount - 1]) <= 10)
			{
				BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;
				BYTE cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], cbCardCount) ;
				OutCardResult.cbCardCount = cbCardCount ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

				return ;
			}			
		}

	}

	//ֻ��һ����
	if ( GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_ONE && GetCardType(cbHandCardData, cbHandCardCount) != CT_FOUR_LINE_TAKE_TWO &&
		CompareCard(cbTurnCardData, cbHandCardData, cbTurnCardCount, cbHandCardCount) )
	{
		OutCardResult.cbCardCount = cbHandCardCount ;
		CopyMemory(OutCardResult.cbResultCard, cbHandCardData, cbHandCardCount) ;
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

		return ;
	}

	//���ƽ����ж�
	if (1 == m_cbUserCardCount[m_wBankerUser] && cbHandCardCount >= 2 && cbOutCardType == CT_DOUBLE)
	{
		tagAnalyseResult AnalyseResult ;
		ZeroMemory(&AnalyseResult, sizeof(AnalyseResult)) ;

		//�����˿�
		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

		//�����ж�
		if (cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSixCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8) || 
			cbHandCardCount == (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSixCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8 + 1))
		{
			//�����ж�
			for (int nIndex = AnalyseResult.cbDoubleCount-1; nIndex>=0 ; --nIndex)
			{
				if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[nIndex*2]) > GetCardLogicValue(cbTurnCardData[0]))
				{
					OutCardResult.cbCardCount = 2 ;
					OutCardResult.cbResultCard[0] = AnalyseResult.cbDoubleCardData[nIndex*2] ;
					OutCardResult.cbResultCard[1] = AnalyseResult.cbDoubleCardData[nIndex*2+1] ;
                    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			//��ը�ж� ��Сը��
            if (AnalyseResult.cbFourCount > 0) 
            {
                BYTE cbLestBombIndex = AnalyseResult.cbFourCount-1 ;

                OutCardResult.cbCardCount = 4 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbFourCardData[cbLestBombIndex*4] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbFourCardData[cbLestBombIndex*4+3] ;

                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
                return ;
            }
            else if (AnalyseResult.cbFiveCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbFiveCount-1 ;

                OutCardResult.cbCardCount = 5 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbFiveCardData[cbLestBombIndex*5+4] ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

                return ;
            }
            else if (AnalyseResult.cbSixCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbSixCount-1 ;

                OutCardResult.cbCardCount = 6 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbSixCardData[cbLestBombIndex*6] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbSixCardData[cbLestBombIndex*6+5] ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

                return ;
            }
            else if (AnalyseResult.cbSenvenCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbSenvenCount-1 ;

                OutCardResult.cbCardCount = 7 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+5] ;
                OutCardResult.cbResultCard[6] = AnalyseResult.cbSenvenCardData[cbLestBombIndex*7+6] ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

                return ;
            }
            else if (AnalyseResult.cbEightCount > 0)
            {
                //��Сը��
                BYTE cbLestBombIndex = AnalyseResult.cbEightCount-1 ;

                OutCardResult.cbCardCount = 8 ;
                OutCardResult.cbResultCard[0] = AnalyseResult.cbEightCardData[cbLestBombIndex*8] ;
                OutCardResult.cbResultCard[1] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+1] ;
                OutCardResult.cbResultCard[2] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+2] ;
                OutCardResult.cbResultCard[3] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+3] ;
                OutCardResult.cbResultCard[4] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+4] ;
                OutCardResult.cbResultCard[5] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+5] ;
                OutCardResult.cbResultCard[6] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+6] ;
                OutCardResult.cbResultCard[7] = AnalyseResult.cbEightCardData[cbLestBombIndex*8+7] ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

                return ;
            }

		}

		//�����Լ�
		if ( wOutCardUser != m_wBankerUser )
		{
			tagAnalyseResult FriendAnalyseResult ;
			ZeroMemory( &FriendAnalyseResult, sizeof( FriendAnalyseResult ) ) ;

			//�����˿�
			AnalysebCardData( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendAnalyseResult ) ;

			//�����ж�
			if ( m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
				 m_cbUserCardCount[wOutCardUser] == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
			{
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
				return ;
			}

			//���±�û��
			tagOutCardTypeResult FriendCardTypeResult[(12+1)] ;
			ZeroMemory( FriendCardTypeResult, sizeof( FriendCardTypeResult ) ) ;

			AnalyseOutCardType( m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser], FriendCardTypeResult ) ;

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_SINGLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//ʣ���˿�
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//�����˿�
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//�����ж�
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
				{
                    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			for ( BYTE cbLineCardIdx = 0; cbLineCardIdx < FriendCardTypeResult[CT_DOUBLE_LINE].cbCardTypeCount; ++cbLineCardIdx )
			{
				//ʣ���˿�
				BYTE cbRemainCardData[MAX_COUNT], cbRemainCardCount ;

				cbRemainCardCount = m_cbUserCardCount[wOutCardUser] ;
				CopyMemory( cbRemainCardData, m_cbAllCardData[wOutCardUser], cbRemainCardCount ) ;
				RemoveCard( FriendCardTypeResult[CT_SINGLE_LINE].cbCardData[cbLineCardIdx],  FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx], 
					cbRemainCardData, cbRemainCardCount ) ;

				cbRemainCardCount -= FriendCardTypeResult[CT_SINGLE_LINE].cbEachHandCardCount[cbLineCardIdx] ;

				//�����˿�
				AnalysebCardData( cbRemainCardData, cbRemainCardCount, FriendAnalyseResult ) ;

				//�����ж�
				if (cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8) || 
					cbRemainCardCount == (FriendAnalyseResult.cbDoubleCount * 2 + FriendAnalyseResult.cbThreeCount * 3 + FriendAnalyseResult.cbFourCount * 4 + FriendAnalyseResult.cbFiveCount * 5 + FriendAnalyseResult.cbSixCount * 6 + FriendAnalyseResult.cbSenvenCount * 7 + FriendAnalyseResult.cbEightCount * 8 + 1))
				{
                    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}


		}
	}


	//�Լҿɷ����
	if( m_wBankerUser != wOutCardUser && ! CompareCard( cbTurnCardData, m_cbAllCardData[ m_wBankerUser ], cbTurnCardCount, m_cbUserCardCount[ m_wBankerUser ] ) )
	{
		//ׯ���˿�
		bool bBankerCanOut = false ;
		tagOutCardTypeResult BankerOutCardResult[(12+1)] ;
		ZeroMemory(BankerOutCardResult, sizeof(BankerOutCardResult)) ;

        //�����˿�
		AnalyseOutCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser], cbTurnCardData, cbTurnCardCount, BankerOutCardResult) ;
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType) if(BankerOutCardResult[cbCardType].cbCardTypeCount>0) bBankerCanOut = true ; 

		if(!bBankerCanOut)
		{
		//�Լ�ID
		WORD wFriendChairID = (m_wBankerUser+2)%GAME_PLAYER ;

		//�����˿�
		tagOutCardTypeResult FriendCardTypeResult[(12+1)] ;
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
		AnalyseOutCardType(m_cbAllCardData[wFriendChairID], m_cbUserCardCount[wFriendChairID], FriendCardTypeResult) ;

		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(FriendCardTypeResult[cbCardType].cbCardTypeCount>0)
			{
				for(LONG lIndex=0; lIndex<FriendCardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{

					if(TestOutAllCard(wFriendChairID, FriendCardTypeResult[cbCardType].cbCardData[lIndex], FriendCardTypeResult[cbCardType].cbEachHandCardCount[lIndex], true))
					{
						//��ѹ�Լ�
                        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}
			}
		}
	}

	//���߶Լ�
/*
	if (GetCardType(m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]) == GetCardType(cbTurnCardData, cbTurnCardCount) &&
		CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]))
    {
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
		return ;
    }

	if (CompareCard(cbTurnCardData, m_cbAllCardData[(m_wBankerUser + 2) % GAME_PLAYER], cbTurnCardCount, m_cbUserCardCount[(m_wBankerUser + 2) % GAME_PLAYER]))
    {
        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
        return ;
    }*/

	//�жϿɷ����
	BYTE cbSingleCardCount = MAX_COUNT+CT_MISSILE_CARD ;
	bool bFindBestCard = false ;
	for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
		if(CardTypeResult[cbCardType].cbCardTypeCount>0)
		{
			for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
			{
				WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;

				if(TestOutAllCard(wMeChairID, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], false))
				{
					//���㵥��
					BYTE cbTmpSingleCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;

					//����ж�
					if (cbTmpSingleCount >= MAX_COUNT) continue ;

					//ը�����ȼ��ź�
					BYTE cbBombCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
                    if (cbBombCardType == CT_BOMB_CARD|| cbBombCardType == CT_BOMB_CARD_5 || cbBombCardType == CT_BOMB_CARD_6 || cbBombCardType == CT_BOMB_CARD_7 || cbBombCardType == CT_BOMB_CARD_8 ||
                        cbBombCardType == CT_MISSILE_CARD_3 || cbBombCardType == CT_MISSILE_CARD_4 || cbBombCardType == CT_MISSILE_CARD_5) cbTmpSingleCount += 4 ;
					else if (cbBombCardType == CT_MISSILE_CARD_6) cbTmpSingleCount += 5 ;

					////�ı�Ȩֵ
					//if (cbBombCardType != CT_ERROR) cbTmpSingleCount += cbBombCardType ;

					//����ը��
					//BYTE cbWantOutCardType = GetCardType(CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]) ;
					//if (CardTypeResult[cbOutCardType].cbCardTypeCount > 0 && cbOutCardType < CT_BOMB_CARD && cbWantOutCardType >= CT_BOMB_CARD) continue ;

					if(cbTmpSingleCount <= cbSingleCardCount)
					{
						//���ñ���
						OutCardResult.cbCardCount=CardTypeResult[cbCardType].cbEachHandCardCount[lIndex];
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbCardType].cbCardData[lIndex], 
						CardTypeResult[cbCardType].cbEachHandCardCount[lIndex]*sizeof(BYTE));
						cbSingleCardCount = cbTmpSingleCount ;
						bFindBestCard = true ;
					}
				}
			}
		}
		//ֱ�ӷ���
		if (bFindBestCard) 
        {
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

	//ȡ���ĸ���С����
	BYTE cbMinSingleCardCount[4] ;
	cbMinSingleCardCount[0]=MAX_COUNT ;
	cbMinSingleCardCount[1]=MAX_COUNT ;
	cbMinSingleCardCount[2]=MAX_COUNT ;
	cbMinSingleCardCount[3]=MAX_COUNT ;
	BYTE cbIndex[4]={0} ;	
	BYTE cbMinSingleCountInFour=MAX_COUNT ;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
    tagOutCardTypeResult CanOutCard = {0};
    if (cbOutCardType < CT_BOMB_CARD)
    {
        CopyMemory(&CanOutCard, &CardTypeResult[cbOutCardType], sizeof(tagOutCardTypeResult));
    }

	for(BYTE i=0; i<CanOutCard.cbCardTypeCount; ++i)
	{
		//��С����
		BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,CanOutCard.cbCardData[i], CanOutCard.cbEachHandCardCount[i]) ; 
		BYTE cbMaxValue=0 ; 
		BYTE Index = 0 ;

		//����cbMinSingleCardCount[4]�����ֵ
		for(BYTE j=0; j<4; ++j)
		{
			if(cbMinSingleCardCount[j]>=cbTmpCount)
			{
				cbMinSingleCardCount[j] = cbTmpCount ;
				cbIndex[j] = i ;
				break ;
			}
		}

	}

	for(BYTE i=0; i<4; ++i)
		if(cbMinSingleCountInFour>cbMinSingleCardCount[i]) cbMinSingleCountInFour = cbMinSingleCardCount[i] ;


	//ԭʼ������
	BYTE cbOriginSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount,NULL,0) ;

	//���ѳ���
	bool bFriendOut = m_wBankerUser!=wOutCardUser ;
	if(bFriendOut)
	{
		//�������������һ����
		if (GetCardType(m_cbAllCardData[(2 + m_wBankerUser) % GAME_PLAYER], m_cbUserCardCount[(2 + m_wBankerUser) % GAME_PLAYER]) != CT_ERROR) 
        {
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }

		//�������TestOutAllCard���ѶԿɳ�ը�����������
		if(CanOutCard.cbCardTypeCount > 0 && CanOutCard.cbCardType < CT_BOMB_CARD &&  cbMinSingleCountInFour < MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;
				
				//�����������Ʋ��ӶԼ���
				if ( CanOutCard.cbCardType >= CT_THREE &&  CanOutCard.cbCardType <= CT_MISSILE_CARD && GetCardLogicValue(CanOutCard.cbCardData[Index][0]) >= 7 && 
					CanOutCard.cbEachHandCardCount[Index] <=5 )
					continue ;
				
				//С��J���ƣ�����С��K������ɢ��
				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+3 &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_TWO ) /*&& 
					(GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=11 || (cbMinSingleCardCount[i]<cbOriginSingleCardCount)&&GetCardLogicValue(CanOutCard.cbCardData[Index][0])<=13)*/) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]) && cbHandCardCount>5)
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true ;
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount) bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
				else if(cbHandCardCount<5 && cbMinSingleCardCount[i]<cbOriginSingleCardCount+4 && cbMinSingleCardCount[i]<=cbMinSingleCountInFour &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//�ܳ������Լҵ�2
					if ( GetCardLogicValue( CanOutCard.cbCardData[Index][0] ) >= 16 && GetCardLogicValue( cbTurnCardData[0] ) >= 15 )
						continue ;

					bFindCard = true ;
					cbCanOutIndex = Index ; 
					cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
				}
			}

			if(bFindCard)
			{

				//���ñ���
				OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
				CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

				return ;
			}
			//��������������
			else if(cbHandCardCount<=5)
			{
				BYTE cbMinLogicCard = GetCardLogicValue(0x4f)+1 ;
				BYTE cbCanOutIndex = 0 ;
				for(BYTE i=0; i<4; ++i)
					if(cbMinSingleCardCount[i]<MAX_COUNT && cbMinSingleCardCount[i]<=cbMinSingleCountInFour && cbMinLogicCard>GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) && 
						GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0])<=14)
					{
						cbMinLogicCard = GetCardLogicValue(CanOutCard.cbCardData[cbIndex[i]][0]) ;
						cbCanOutIndex = cbIndex[i] ;
					}

					if(cbMinLogicCard != (GetCardLogicValue(0x4f)+1))
					{
						//���ñ���
						OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
						CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));
                        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

						return ;
					}
			}
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

			return ;
		}
		else
		{
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
			return ;
		}

	}
	//��������
	else
	{
		if(CanOutCard.cbCardTypeCount>0 && cbMinSingleCountInFour < MAX_COUNT)
		{
			BYTE cbMinLogicCardValue = GetCardLogicValue(0x4F)+1 ;
			bool bFindCard = false ;
			BYTE cbCanOutIndex=0 ;
			for(BYTE i=0; i<4; ++i)
			{
				BYTE Index = cbIndex[i] ;

				if((cbMinSingleCardCount[i]<cbOriginSingleCardCount+4)  &&  (cbMinSingleCardCount[i]<=cbMinSingleCountInFour || cbMinSingleCardCount[i]<=cbMinSingleCountInFour+1 && 
				CanOutCard.cbCardType >= CT_THREE_LINE_TAKE_ONE && CanOutCard.cbCardType <= CT_THREE_LINE_TAKE_TWO ) &&
					cbMinLogicCardValue>GetCardLogicValue(CanOutCard.cbCardData[Index][0]))
				{
					//��Դ���
					bool bNoLargeCard = true ;

					//������������������4�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if(m_cbUserCardCount[m_wBankerUser]>=4 && cbHandCardCount>=5 && CanOutCard.cbEachHandCardCount[Index]>=2 && 
						GetCardLogicValue(CanOutCard.cbCardData[Index][0])>=15 &&
						GetCardLogicValue(cbTurnCardData[0])<13 &&
						GetCardLogicValue(cbTurnCardData[0])<GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) && 
						CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
						//bNoLargeCard=false ;

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for(BYTE k=3; k<CanOutCard.cbEachHandCardCount[Index]; ++k)
					{
						if(GetCardLogicValue(CanOutCard.cbCardData[Index][k])>=15 && 
							CanOutCard.cbEachHandCardCount[Index]!=cbHandCardCount)
							bNoLargeCard = false ;
					}
					if(bNoLargeCard)
					{
						bFindCard = true ;
						cbCanOutIndex = Index ; 
						cbMinLogicCardValue = GetCardLogicValue(CanOutCard.cbCardData[Index][0]) ;
					}
				}
			}

			if(bFindCard)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;
				bool bCanOut=true ;

				//��ֻѹ2
				if(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard)
				{
					if(GetCardColor(CanOutCard.cbCardData[cbCanOutIndex][0])==0x40 && GetCardLogicValue(cbTurnCardData[0])<=14 && cbHandCardCount>5) 								
					{
						bCanOut = false ;
					}
				}

                //�����ж�
				if(GetCardLogicValue(CanOutCard.cbCardData[cbCanOutIndex][0])>=16 && cbHandCardCount>=6 && cbHandCardData[5]==0x4E)
				{
					bool bOutMissileCard = false ;
					//һ���ƺ�һ��ը��
					BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
					CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
					RemoveCard(cbRemainCardData, 6, cbRemainCardData, cbRemainCardCount) ;
					cbRemainCardCount -= 6 ;
					if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutMissileCard = true;

					//ֻʣ������
					BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
					if(cbRemainCardCount<5 && cbRemainCardCount>0 && GetCardLogicValue(cbRemainCardData[0])>=14) bOutMissileCard = true;

					//ը������
					BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CanOutCard.cbCardData[cbCanOutIndex],
						CanOutCard.cbEachHandCardCount[cbCanOutIndex]) ;
					if(cbSingleCardCount<=1 && GetCardLogicValue(cbRemainCardData[0])>=11) bOutMissileCard = true;

					//����С��
					if (GetCardLogicValue(cbRemainCardData[0]) <= 10 && CT_ERROR == GetCardType(cbRemainCardData, cbRemainCardCount) && 
						GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) > 10)
						bOutMissileCard = false ;

					//����˿�
					if(bOutMissileCard)
					{
						//��������ը��
						BYTE cbIndex = CardTypeResult[CT_BOMB_CARD].cbCardTypeCount - 1 ;

						OutCardResult.cbCardCount = CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[cbIndex] ;
						CopyMemory(OutCardResult.cbResultCard, CardTypeResult[CT_BOMB_CARD].cbCardData[cbIndex], OutCardResult.cbCardCount) ;
                        g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
						return ;
					}
				}
				if(bCanOut)
				{
					//���ñ���
					OutCardResult.cbCardCount=CanOutCard.cbEachHandCardCount[cbCanOutIndex];
					CopyMemory(OutCardResult.cbResultCard,CanOutCard.cbCardData[cbCanOutIndex],CanOutCard.cbEachHandCardCount[cbCanOutIndex]*sizeof(BYTE));

                    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
					return ;
				}
			}

			if(cbOutCardType==CT_SINGLE)
			{
				//�����������
				BYTE cbLargestLogicCard = GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0]) ;

				if(GetCardLogicValue(cbTurnCardData[0])==14 || 
					GetCardLogicValue(cbTurnCardData[0])>=cbLargestLogicCard || 
					(GetCardLogicValue(cbTurnCardData[0])<cbLargestLogicCard-1) ||
					m_cbUserCardCount[m_wBankerUser]<=5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE cbIndex=MAX_COUNT ;
					for(BYTE i=0; i<cbHandCardCount; ++i)
						if(GetCardLogicValue(cbHandCardData[i])>GetCardLogicValue(cbTurnCardData[0]) &&
							GetCardLogicValue(cbHandCardData[i])>=15)
						{
							cbIndex = i ;
						}
						if(cbIndex!=MAX_COUNT)
						{
							//���ñ���
							OutCardResult.cbCardCount=1;
							OutCardResult.cbResultCard[0] = cbHandCardData[cbIndex] ;
                            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

							return ;
						}
				}
			}

			//�����Ѳ������ص���ʱ
			WORD wMeChairID = (m_wBankerUser+1)%GAME_PLAYER ;
			WORD wFriendID = (wMeChairID+1)%GAME_PLAYER ;

			tagOutCardTypeResult FriendCardTypeResult[(12+1)] ;
			ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult)) ;
			AnalyseOutCardType(m_cbAllCardData[wFriendID], m_cbUserCardCount[wFriendID], cbTurnCardData, cbTurnCardCount, FriendCardTypeResult) ;

			//�����Ѳ������ص���ʱ
			if(m_cbUserCardCount[m_wBankerUser]<=4 && FriendCardTypeResult[cbOutCardType].cbCardTypeCount==0 && CardTypeResult[cbOutCardType].cbCardTypeCount>0 && cbOutCardType < CT_BOMB_CARD)
			{
				BYTE cbMinSingleCount=MAX_COUNT ;
				BYTE Index=0 ;
				for(BYTE i=0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; ++i)
				{
					BYTE cbTmpCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i], CardTypeResult[cbOutCardType].cbEachHandCardCount[i]) ;
					if(cbMinSingleCount>=cbTmpCount)
					{
						cbMinSingleCount = cbTmpCount ;
						Index = i ;
					}
				}
				//���ñ���
				OutCardResult.cbCardCount=CardTypeResult[cbOutCardType].cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[Index], OutCardResult.cbCardCount) ;
                g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

				return ;
			}
		}

		//��Ҫ����ը��
		if(CardTypeResult[CT_BOMB_CARD].cbCardTypeCount>0/* && 
			(NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+2)%GAME_PLAYER])*/)
		{
			tagOutCardTypeResult const &BomCard = CardTypeResult[CT_BOMB_CARD] ;
            BYTE Index = 0 ;
            /*Ѱ����Сը��
			BYTE cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[0][0]) ;
			for(BYTE i=0; i<BomCard.cbCardTypeCount; ++i)
			{
				if(cbMinLogicValue>GetCardLogicValue(BomCard.cbCardData[i][0]))
				{
					cbMinLogicValue = GetCardLogicValue(BomCard.cbCardData[i][0]) ;
					Index = i ;
				}
			}*/
            Index = BomCard.cbCardTypeCount-1;
			bool bOutBomb = false ;

			//�����ж�
			if (NORMAL_COUNT  == cbHandCardCount && NORMAL_COUNT == m_cbUserCardCount[(m_wBankerUser+2)%GAME_PLAYER] && CT_ERROR != GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]))
				bOutBomb = true ;

			//һ���ƺ�һ��ը��
			BYTE cbRemainCardData[20], cbRemainCardCount=cbHandCardCount ;
			CopyMemory(cbRemainCardData, cbHandCardData, cbHandCardCount) ;
			RemoveCard(BomCard.cbCardData[Index], BomCard.cbEachHandCardCount[Index], cbRemainCardData, cbRemainCardCount) ;
			cbRemainCardCount -= BomCard.cbEachHandCardCount[Index] ;
			if(CT_ERROR!=GetCardType(cbRemainCardData, cbRemainCardCount)) bOutBomb = true ;

			//ը������
			BYTE cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
			if(cbSingleCardCount==0 && GetCardLogicValue(cbRemainCardData[0]) > GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])) 
				bOutBomb = true ;

			//ֻʣһ��
			BYTE cbRemainCardType = GetCardType(m_cbAllCardData[wOutCardUser], m_cbUserCardCount[wOutCardUser]) ;
			if(cbRemainCardType>CT_ERROR && cbRemainCardType<CT_FOUR_LINE_TAKE_ONE && GetCardLogicValue(m_cbAllCardData[wOutCardUser][0])<15 && 
				cbSingleCardCount < 2 && (GetCardLogicValue(cbRemainCardData[0]) >= GetCardLogicValue(m_cbAllCardData[m_wBankerUser][0])))
				bOutBomb = true ;

			//ֻʣ������
			BYTE cbRemainLargestCard = GetCardLogicValue(cbRemainCardData[0]) ;
			if(cbRemainCardCount<5 && cbRemainCardCount>0 && (cbRemainLargestCard!=GetCardLogicValue(BomCard.cbCardData[Index][0])) && 
				cbRemainLargestCard>GetCardLogicValue(m_cbAllCardData[wOutCardUser][0]) && cbRemainLargestCard > 14) bOutBomb = true ;

            //ը����������
            cbSingleCardCount = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]) ;
            if(cbSingleCardCount <= 1)
            {
                bOutBomb = true ;
            }
			//�����˿�
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbRemainCardData, cbRemainCardCount, AnalyseResult) ;

			if (m_cbUserCardCount[m_wBankerUser] == 1 && (AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbThreeCount * 3 + AnalyseResult.cbFourCount * 4 + AnalyseResult.cbFiveCount * 5 + AnalyseResult.cbSignedCount * 6 + AnalyseResult.cbSenvenCount * 7 + AnalyseResult.cbEightCount * 8 + 1 >= cbRemainCardCount)) bOutBomb = true ;


			//���ñ���
			if(bOutBomb)
			{
				OutCardResult.cbCardCount=BomCard.cbEachHandCardCount[Index];
				CopyMemory(OutCardResult.cbResultCard,BomCard.cbCardData[Index],BomCard.cbEachHandCardCount[Index]*sizeof(BYTE));
			}
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);

			return ;
		}
//         g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
// 
// 		return ;
	}
    //�������ֻʣ��һ�Ż���һ�֣�һ��Ҫѹ
    if (OutCardResult.cbCardCount == 0 && (m_cbUserCardCount[m_wBankerUser] == 1 || (GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) != CT_ERROR)))
    {
        //ը��ѹ
        if (CardTypeResult[CT_BOMB_CARD].cbCardTypeCount > 0)
        {
            OutCardResult.cbCardCount=CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1];
            CopyMemory(OutCardResult.cbResultCard,CardTypeResult[CT_BOMB_CARD].cbCardData[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1],CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[CardTypeResult[CT_BOMB_CARD].cbCardTypeCount-1]*sizeof(BYTE));
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
        //��ͨѹ ѡʣ�µ������ٵķ���
        if (cbOutCardType < CT_BOMB_CARD && CardTypeResult[cbOutCardType].cbCardTypeCount > 0)
        {
            BYTE bMinCardCount = 0;
            BYTE bCurMinNum = MAX_COUNT;
            for (BYTE i = 0; i<CardTypeResult[cbOutCardType].cbCardTypeCount; i++)
            {
                BYTE bMinNum = 0;
                bMinNum = AnalyseSinleCardCount(cbHandCardData, cbHandCardCount, CardTypeResult[cbOutCardType].cbCardData[i],CardTypeResult[cbOutCardType].cbEachHandCardCount[i]);
                if (bCurMinNum > bMinNum)
                {
                    bCurMinNum = bMinNum;
                    bMinCardCount = i;
                }
            }
            OutCardResult.cbCardCount = CardTypeResult[cbOutCardType].cbEachHandCardCount[bMinCardCount];
            CopyMemory(OutCardResult.cbResultCard, CardTypeResult[cbOutCardType].cbCardData[bMinCardCount], OutCardResult.cbCardCount*sizeof(BYTE));
            g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------cbTurnCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
            return ;
        }
    }

    g_Logger.WriteLog("---------------UndersideOfBankerOutCard2 line[%d]-------------OutCardResult.cbCardCount[%d],OutCardResult.cbResultCard[0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x]",__LINE__, OutCardResult.cbCardCount,OutCardResult.cbResultCard[0],OutCardResult.cbResultCard[1],OutCardResult.cbResultCard[2],OutCardResult.cbResultCard[3],OutCardResult.cbResultCard[4],OutCardResult.cbResultCard[5],OutCardResult.cbResultCard[6],OutCardResult.cbResultCard[7],OutCardResult.cbResultCard[8],OutCardResult.cbResultCard[9],OutCardResult.cbResultCard[10],OutCardResult.cbResultCard[11],OutCardResult.cbResultCard[12],OutCardResult.cbResultCard[13],OutCardResult.cbResultCard[14],OutCardResult.cbResultCard[15],OutCardResult.cbResultCard[16],OutCardResult.cbResultCard[17],OutCardResult.cbResultCard[18],OutCardResult.cbResultCard[19]);
	return ;
}

//�з��ж�
BYTE CGameLogic::LandScore(WORD wMeChairID, BYTE cbCurrentLandScore, BYTE cbCallScorePhase)
{
	//������Ŀ
	BYTE cbLargeCardCount = 0 ;
	BYTE Index=0 ;
	while(GetCardLogicValue(m_cbLandScoreCardData[Index++])>=15) ++cbLargeCardCount ;

    BYTE cbBomCardData[MAX_COUNT], cbBomCardCount=0;
    tagBomCardResult BomCardResult = {0};
    tagOutCardTypeResult CardTypeResult[12+1] = {0};
    GetAllBomCardOrder(m_cbLandScoreCardData, strlen((char *)&m_cbLandScoreCardData[0]),cbBomCardData,cbBomCardCount,BomCardResult, ST_MISSILE_FIRST);
    AnalyseOutCardType(m_cbLandScoreCardData, strlen((char *)&m_cbLandScoreCardData[0]), CardTypeResult);
    if (cbBomCardCount == 0)
        return 255;
	//���Ƹ���
    BYTE cbSingleCardCount = 0;
    if (m_bRandCardMode == enRandCard_DoubleGoodCard)
        cbSingleCardCount=AnalyseSinleCardCount(m_cbLandScoreCardData, strlen((char *)&m_cbLandScoreCardData[0]), NULL, 0) ;
    else
	    cbSingleCardCount=AnalyseSinleCardCount(m_cbLandScoreCardData, sizeof(m_cbLandScoreCardData), NULL, 0) ;

    //�ж��Ƿ������ը��
    tagOutCardTypeResult TempCardTypeResult[12+1] = {0};
    BYTE cChairID = (wMeChairID+1)>2?0:(wMeChairID+1);
    AnalyseOutCardType(m_cbAllCardData[cChairID], m_cbUserCardCount[cChairID], TempCardTypeResult);
    if (TempCardTypeResult[CT_BOMB_CARD].cbCardTypeCount != 0)
    {
        if (!CompareCard(TempCardTypeResult[CT_BOMB_CARD].cbCardData[0], CardTypeResult[CT_BOMB_CARD].cbCardData[0], TempCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0], CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0]))
            return 255;
    }
    ZeroMemory(TempCardTypeResult, sizeof(TempCardTypeResult)) ;
    cChairID = (cChairID+1)>2?0:(cChairID+1);
    AnalyseOutCardType(m_cbAllCardData[cChairID], m_cbUserCardCount[cChairID], TempCardTypeResult);
    if (TempCardTypeResult[CT_BOMB_CARD].cbCardTypeCount != 0)
    {
        if (!CompareCard(TempCardTypeResult[CT_BOMB_CARD].cbCardData[0], CardTypeResult[CT_BOMB_CARD].cbCardData[0], TempCardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0], CardTypeResult[CT_BOMB_CARD].cbEachHandCardCount[0]))
            return 255;
    }
	//������
    //if((GetCardColor(cbBomCardData[0]) == 0x40 && cbBomCardCount>=10) || (GetCardColor(cbBomCardData[0]) != 0x40 && cbBomCardCount>=12) && cbSingleCardCount<=2)
    if((CardTypeResult[CT_BOMB_CARD].cbCardTypeCount >= 3 && cbBomCardCount>=12) /*&& cbSingleCardCount<=2*/)
	{
		//�зֽ׶�
		if ( cbCallScorePhase == CSD_NORMAL ) 
		{
			return 3 ;
		}
		//������
		else if ( cbCallScorePhase == CSD_SNATCHLAND ) 
		{
			return 4;
		}
		//�����׶�
		else return 255;
	}

	//�����з�
	if(cbLargeCardCount<=3 || cbCurrentLandScore==1) return 255 ;

	//��������
	BYTE cbMinSingleCardCount=MAX_COUNT ;
	for(WORD wChairID=0 , i=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		BYTE cbTmpSingleCardCount = AnalyseSinleCardCount(m_cbAllCardData[wChairID], NORMAL_COUNT, NULL, 0) ;
		if(wChairID!=wMeChairID && cbTmpSingleCardCount<cbMinSingleCardCount) cbTmpSingleCardCount=cbMinSingleCardCount ;
	}

	//��һ��
	if(cbLargeCardCount>=4 && cbSingleCardCount<cbMinSingleCardCount-3) return 255 ;

	//�����з�
	return 255 ;
}

//���Ʋ���
bool CGameLogic::_TestOutAllCard(WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, BYTE	cbAllCardData[GAME_PLAYER][MAX_COUNT], BYTE cbUserCardCount[GAME_PLAYER], bool bFirstOutCard)
{
	//�˵ݹ��ݲ�����
	//return false ;

	//����ID
	WORD wFriendID = GAME_PLAYER ;
	if(wTestUser != m_wBankerUser) wFriendID = (wTestUser==((m_wBankerUser+1)%GAME_PLAYER)) ? (m_wBankerUser+2)%GAME_PLAYER : (m_wBankerUser+1)%GAME_PLAYER ;

	//��ʱֻ����������
//	for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser) if(wFriendID!=wUser && cbUserCardCount[wUser]>10) return false ;

//	m_lRecCount++ ;
	//��ֹ���ݹ�
//	if(m_lRecCount>50) return false ;
	
	BYTE cbWantOutCardType = GetCardType(cbWantOutCardData, cbWantOutCardCount) ;

	//�����ж�
	if ( cbUserCardCount[wTestUser] == cbWantOutCardCount ) return true ;

    //�����ж�
	try
	{
	for(WORD wUser=0; wUser<GAME_PLAYER; ++wUser)
	{
		if(wUser!=wTestUser && wFriendID!=wUser)
		{
			tagOutCardTypeResult CardTypeResult[(12+1)] ;
			ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

			//���Ʒ���
			try
			{
			AnalyseOutCardType(cbAllCardData[wUser], cbUserCardCount[wUser], cbWantOutCardData, cbWantOutCardCount, CardTypeResult) ;
			}
			catch(...)
			{
				return false ;
			}
			for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
				if(CardTypeResult[cbCardType].cbCardTypeCount>0) return false ;
		}
	}
	}
	catch(...)
	{
		return false ;
	}

	//��ը�ж�
	BYTE cbAllBombCard[MAX_COUNT], cbAllBombCount=0 ;
	GetAllBomCard(cbAllCardData[wTestUser], cbUserCardCount[wTestUser], cbAllBombCard, cbAllBombCount) ;
	if (cbAllBombCount > 0)
	{
		//ʣ���˿�
		BYTE cbRemainCard[MAX_COUNT], cbRemainCardCount ;

		CopyMemory(cbRemainCard, cbAllCardData[wTestUser], cbUserCardCount[wTestUser]) ;
		cbRemainCardCount = cbUserCardCount[wTestUser] ;

		RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		//�޴�����
		if (CT_FOUR_LINE_TAKE_ONE == cbWantOutCardType || CT_FOUR_LINE_TAKE_TWO==cbWantOutCardType) return false ;

		//ֻʣը��
		if (cbWantOutCardType >= CT_BOMB_CARD && cbWantOutCardCount == cbUserCardCount[wTestUser]) return true ; 

		//ֻʣը��
		if (cbAllBombCount == cbUserCardCount[wTestUser] && cbWantOutCardType >= CT_BOMB_CARD) return true ;

		//ը����һ��
		if ( cbWantOutCardType >= CT_BOMB_CARD && GetCardType(cbRemainCard, cbRemainCardCount) != CT_ERROR && 
			CT_FOUR_LINE_TAKE_TWO != GetCardType(cbRemainCard, cbRemainCardCount) && CT_FOUR_LINE_TAKE_ONE != GetCardType(cbRemainCard, cbRemainCardCount) )
			return true ;

		//�׳���ʱ����ը��
		if (cbWantOutCardType >= CT_BOMB_CARD && bFirstOutCard)
		{
			//����ֻʣһ����
			if (wTestUser != m_wBankerUser && GetCardType(m_cbAllCardData[m_wBankerUser], m_cbUserCardCount[m_wBankerUser]) == CT_ERROR)
				return false ;

				WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//�����¼�
				WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//�����ϼ�

			//�м�ֻʣһ����
			if (wTestUser == m_wBankerUser && GetCardType(m_cbAllCardData[wUndersideOfBanker], m_cbUserCardCount[wUndersideOfBanker]) == CT_ERROR &&
				GetCardType(m_cbAllCardData[wUpsideOfBanker], m_cbUserCardCount[wUpsideOfBanker]) == CT_ERROR)
				return false ;
		}

		//��ը�ж�
		if (cbWantOutCardType < CT_FOUR_LINE_TAKE_ONE && cbWantOutCardType != CT_SINGLE_LINE)
		{
			for (BYTE i=0; i < cbWantOutCardCount; ++i)
				for (BYTE j=0; j < cbAllBombCount; ++j)
				{
					if (GetCardValue(cbAllBombCard[j]) == GetCardValue(cbWantOutCardData[i])) return false ;
				}
		}
	}

	if(cbUserCardCount[wTestUser]==0) return true ;

	//�ݹ鴦��
	try
	{
	ASSERT(cbUserCardCount[wTestUser] >= cbWantOutCardCount) ;
	if (cbUserCardCount[wTestUser] < cbWantOutCardCount ) return false ;
	RemoveCard(cbWantOutCardData, cbWantOutCardCount, cbAllCardData[wTestUser], cbUserCardCount[wTestUser]) ;
	cbUserCardCount[wTestUser] -= cbWantOutCardCount ;

	//��������
	if (cbWantOutCardType >= CT_THREE_LINE_TAKE_ONE && cbWantOutCardType <= CT_FOUR_LINE_TAKE_TWO && !bFirstOutCard)
	{
		bool bHaveLargeCard = false ;
		for (BYTE i = 3; i < cbWantOutCardCount; ++i) 
			if (GetCardLogicValue(cbWantOutCardData[i]) >= 15)
			{
				bHaveLargeCard = true ;
				break ;
			}
		if (bHaveLargeCard)
		{
			for (BYTE i = 0; i < cbUserCardCount[wTestUser]; ++i)
				if (GetCardLogicValue(cbAllCardData[wTestUser][i]) < 15) return false ;
		}
	}
	}
	catch(...)
	{
		return false ;
	}
	if(cbUserCardCount[wTestUser]!=0)
	{
		tagOutCardTypeResult CardTypeResult[(12+1)] ;
		ZeroMemory(CardTypeResult, sizeof(CardTypeResult)) ;

		//���Ʒ���
		try
		{
		AnalyseOutCardType(cbAllCardData[wTestUser], cbUserCardCount[wTestUser], CardTypeResult) ;
		}
		catch(...)
		{
			return false ;
		}
		for(BYTE cbCardType=CT_SINGLE; cbCardType<=CT_MISSILE_CARD; ++cbCardType)
			if(CardTypeResult[cbCardType].cbCardTypeCount>0) 
			{
				for(LONG lIndex=0; lIndex<CardTypeResult[cbCardType].cbCardTypeCount; ++lIndex)
				{
					//��������
					BYTE cbTmpCardData[GAME_PLAYER][MAX_COUNT], cbTmpCardCount[GAME_PLAYER] ;
					CopyMemory(cbTmpCardData, cbAllCardData, sizeof(cbTmpCardData)) ;
					CopyMemory(cbTmpCardCount, cbUserCardCount, sizeof(cbTmpCardCount)) ;

					//�ݹ����
					try
					{
					if(_TestOutAllCard(wTestUser, CardTypeResult[cbCardType].cbCardData[lIndex], CardTypeResult[cbCardType].cbEachHandCardCount[lIndex], cbTmpCardData, cbTmpCardCount, bFirstOutCard))
						return true ;
					}
					catch(...)
					{
						return false ;
					}
				}
			}
	}
	else
		return true ;

	return false ;
}


//�Ĵ�����
bool CGameLogic::AnalyseFourCardType( BYTE const cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbEnemyCardData[MAX_COUNT], BYTE cbEnemyCardCount, tagOutCardResult &CardResult )
{
	//��ʼ����
	ZeroMemory(&CardResult, sizeof(tagOutCardResult)) ;

	//�����ж�
	if ( cbHandCardCount < 5 ) return false ;

	//�Է����ͷ���
	if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
	{
		//�����ж�
		if ( GetCardLogicValue(cbHandCardData[1] ) < 16 )
		{
			//�����˿�
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

			//��������
			if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
				AnalyseResult.cbFourCount <= 0 ) return false ;

			//�Է�����
			if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
			{
				//�����ж�
				if ( AnalyseResult.cbSignedCount < 2 ) return false ;

				//��С�ж�
				if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[0]) >= GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

				//���Ŵ�����
				if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 2 )
				{
					//ը������
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//���ؽ��
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[0] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[1] ;

					return true ;
				}
				//���Ŵ�����
				else if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 3 )
				{
					//ը������
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//���ؽ��
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[1] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[2] ;

					return true ;
				}
			}
			else if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_DOUBLE )
			{
			}

		}
		//�����ж�
		else if ( GetCardLogicValue(cbHandCardData[1] ) == 16 )
		{
			//�����˿�
			tagAnalyseResult AnalyseResult ;
			AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

			//��������
			if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
				AnalyseResult.cbFourCount <= 0 ) return false ;

			//�Է�����
			if ( GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_SINGLE )
			{
				//�����ж�
				if ( AnalyseResult.cbSignedCount < 4 ) return false ;

				//��С�ж�
				if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[2]) >= GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

				//���Ŵ�����
				if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 4 )
				{
					//ը������
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//���ؽ��
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[2] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[3] ;

					return true ;
				}
				//���Ŵ�����
				else if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 5 )
				{
					//ը������
					BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

					//���ؽ��
					CardResult.cbCardCount = 4 + 2 ;
					CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
					CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[3] ;
					CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[4] ;

					return true ;
				}
			}
		}
	}
	////һ�Ŵ��ƴ�һ��
	//else if (GetCardType(cbEnemyCardData, cbEnemyCardCount) == CT_ERROR && cbEnemyCardCount == 2 )
	//{
	//	//�����ж�
	//	if ( GetCardLogicValue(cbHandCardData[2] ) < 16 )
	//	{
	//		//�����˿�
	//		tagAnalyseResult AnalyseResult ;
	//		AnalysebCardData(cbHandCardData, cbHandCardCount, AnalyseResult) ;

	//		//��������
	//		if ( cbHandCardCount != ( AnalyseResult.cbFourCount * 4 + AnalyseResult.cbDoubleCount * 2 + AnalyseResult.cbSignedCount ) || 
	//			AnalyseResult.cbFourCount <= 0 ) return false ;

	//		//�����ж�
	//		if ( AnalyseResult.cbSignedCount < 2 ) return false ;

	//		//���Ŵ�����
	//		if ( AnalyseResult.cbFourCount >= 1 && AnalyseResult.cbSignedCount == 3 )
	//		{
	//			//��С�ж�
	//			if ( GetCardLogicValue(AnalyseResult.cbSignedCardData[0]) > GetCardLogicValue(cbEnemyCardData[0]) ) return false ;

	//			//ը������
	//			BYTE cbFourCardIndex = ( AnalyseResult.cbFourCount - 1 ) * 4 ;

	//			//���ؽ��
	//			CardResult.cbCardCount = 4 + 2 ;
	//			CopyMemory(CardResult.cbResultCard, AnalyseResult.cbFourCardData + cbFourCardIndex, 4 ) ;
	//			CardResult.cbResultCard[4] = AnalyseResult.cbSignedCardData[1] ;
	//			CardResult.cbResultCard[5] = AnalyseResult.cbSignedCardData[2] ;

	//			return true ;
	//		}
	//	}
	//}
	return false ;
}


//���Ʋ���
bool CGameLogic::TestOutAllCard( WORD wTestUser, BYTE cbWantOutCardData[], BYTE cbWantOutCardCount, bool bFirstOutCard ) {

	try {
		if ( ! VerifyOutCard( wTestUser, cbWantOutCardData, cbWantOutCardCount, m_cbAllCardData[ wTestUser ], m_cbUserCardCount[ wTestUser ], bFirstOutCard ) ) {
			return false;
		}

		//ģ��ݹ鴦��
		if ( cbWantOutCardCount != m_cbUserCardCount[ wTestUser ] ) {

			//��ʼջ
			m_StackHandCardInfo.InitStack();

			//��һ��Ԫ��
			tagHandCardInfo OriginHandCardInfo ;

			//�����˿�
			CopyMemory( OriginHandCardInfo.cbHandCardData, m_cbAllCardData[ wTestUser ], m_cbUserCardCount[ wTestUser ] );
			OriginHandCardInfo.cbHandCardCount = m_cbUserCardCount[ wTestUser ];

			//�Ƴ���һ����
			RemoveCard( cbWantOutCardData, cbWantOutCardCount, OriginHandCardInfo.cbHandCardData, OriginHandCardInfo.cbHandCardCount );
			OriginHandCardInfo.cbHandCardCount -= cbWantOutCardCount;

			//�����˿�
			try {
				AnalyseOutCardType( OriginHandCardInfo.cbHandCardData, OriginHandCardInfo.cbHandCardCount, OriginHandCardInfo.CardTypeResult ) ;
			}
			catch( ... ) {
				return false ;
			}

			//Ԫ��ѹջ
			m_StackHandCardInfo.Push( &OriginHandCardInfo );

			//��������
			LONG lJudgeCount = 0;

			while ( ! m_StackHandCardInfo.IsEmpty() ) {

				//��ֹ��ѭ��
				if ( ++lJudgeCount == 500 ) {

					return false;
				}

				//ջ��Ԫ��
				tagHandCardInfo *pTopHandCardInfo = NULL;
				m_StackHandCardInfo.GetTop( pTopHandCardInfo );

				//�Ϸ��ж�
				ASSERT( pTopHandCardInfo != NULL ) ;
				if ( pTopHandCardInfo == NULL ) {
					return false;
				}

				//��������
				tagOutCardTypeResult *pOutCardTypeResult = ( tagOutCardTypeResult * )pTopHandCardInfo->CardTypeResult;

				//��ֹ������
				pOutCardTypeResult[ CT_FOUR_LINE_TAKE_ONE ].cbCardTypeCount = 0;
				pOutCardTypeResult[ CT_FOUR_LINE_TAKE_TWO ].cbCardTypeCount = 0;

				//��������
				bool bBreakJudge = false;
				bool bFindLargestCard = false;
				for ( BYTE cbOutCardTypeIndx = CT_SINGLE; cbOutCardTypeIndx <= CT_MISSILE_CARD && ! bBreakJudge; ++cbOutCardTypeIndx ) {
					for ( BYTE cbHandCardIndx = 0; cbHandCardIndx < pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardTypeCount && ! bBreakJudge ; ++cbHandCardIndx ) {

						//�Ƿ��жϹ�
						if ( pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == 0 ) {
							continue;
						}

						//����ж�
						if ( IsLargestCard( wTestUser, pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] ) ) {

							//���һ��
							bool bIsLastHandCard = pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == pTopHandCardInfo->cbHandCardCount ;
							if ( bIsLastHandCard ) {
								return true ;
							}

							//�Ƿ��ܳ�
							if ( ! VerifyOutCard( wTestUser, pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], 
								pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ], pTopHandCardInfo->cbHandCardData, pTopHandCardInfo->cbHandCardCount, bFirstOutCard ) ) {

									//���������Ѿ��жϹ�
									pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] = 0;
									continue;
								}

								//�½�ջԪ��
								tagHandCardInfo NewHandCardInfo ;

								//�����˿�
								CopyMemory( NewHandCardInfo.cbHandCardData, pTopHandCardInfo->cbHandCardData, pTopHandCardInfo->cbHandCardCount );
								NewHandCardInfo.cbHandCardCount = pTopHandCardInfo->cbHandCardCount;

								//�Ƴ���ǰһ��
								RemoveCard( pOutCardTypeResult[ cbOutCardTypeIndx ].cbCardData[ cbHandCardIndx ], pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ],
									NewHandCardInfo.cbHandCardData, NewHandCardInfo.cbHandCardCount );
								NewHandCardInfo.cbHandCardCount -= pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ];

								//���������Ѿ��жϹ�
								pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] = 0;

								//�����˿�
								try {
									AnalyseOutCardType( NewHandCardInfo.cbHandCardData, NewHandCardInfo.cbHandCardCount, NewHandCardInfo.CardTypeResult ) ;
								}
								catch( ... ) {
									return false ;
								}

								//Ԫ��ѹջ
								m_StackHandCardInfo.Push( & NewHandCardInfo );

								//�ж�ѭ��
								bBreakJudge = true;

								//���ñ�ʶ
								bFindLargestCard = true;
						}
						//��ǰһ�ֵ���Ŀ���������˿���Ŀ
						else if (  pOutCardTypeResult[ cbOutCardTypeIndx ].cbEachHandCardCount[ cbHandCardIndx ] == pTopHandCardInfo->cbHandCardCount ) {
							return true ;
						}
					}
				}

				//��ջ�ж�
				if ( ! bFindLargestCard ) {
					m_StackHandCardInfo.Pop();
				}
			}
		}
		else {
			return true ;
		}
	}
	catch ( ... ) {
		ASSERT( false );
		return false;
	}

	return false ;
}

//����ж�
bool CGameLogic::IsLargestCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount ) {

	//����ID
	WORD wFriendID = GAME_PLAYER ;
	if ( wTestUser != m_wBankerUser ) {
		wFriendID = ( wTestUser == ( ( m_wBankerUser + 1 ) % GAME_PLAYER ) ) ? ( m_wBankerUser + 2 ) % GAME_PLAYER : 
			( m_wBankerUser + 1 ) % GAME_PLAYER ;
	}

	//�����ж�
	try{
		for ( WORD wUser = 0; wUser < GAME_PLAYER; ++wUser ) {
			if ( wUser != wTestUser && wFriendID != wUser ) {
				tagOutCardTypeResult CardTypeResult[ (12+1) ] ;
				ZeroMemory( CardTypeResult, sizeof( CardTypeResult ) ) ;

				//���Ʒ���
				try {
					AnalyseOutCardType( m_cbAllCardData[ wUser ], m_cbUserCardCount[ wUser ], cbWantOutCardData, cbWantOutCardCount, CardTypeResult ) ;
				}
				catch(...) {
					return false ;
				}

				for ( BYTE cbCardType = CT_SINGLE; cbCardType <= CT_MISSILE_CARD; ++cbCardType ) {
					if ( 0 < CardTypeResult[ cbCardType ].cbCardTypeCount ) {
						return false ;
					}
				}
			}
		}
	}
	catch( ... ) {
		return false ;
	}

	return true;
}

//�Ƿ��ܳ�
bool CGameLogic::VerifyOutCard( WORD wTestUser, BYTE const cbWantOutCardData[], BYTE const cbWantOutCardCount, BYTE const cbCurHandCardData[ MAX_COUNT ], BYTE const cbCurHandCardCount, bool bFirstOutCard ) {

	//����ID
	WORD wFriendID = GAME_PLAYER ;
	if ( wTestUser != m_wBankerUser ) {
		wFriendID = ( wTestUser == ( ( m_wBankerUser + 1 ) % GAME_PLAYER ) ) ? ( m_wBankerUser + 2 ) % GAME_PLAYER : 
			( m_wBankerUser + 1 ) % GAME_PLAYER ;
	}

	BYTE cbWantOutCardType = GetCardType( cbWantOutCardData, cbWantOutCardCount ) ;

	//�׳��ж�
	if ( bFirstOutCard && cbWantOutCardCount == cbCurHandCardCount ) {
		return true;
	}

	//�Ƿ����
	if ( ! IsLargestCard( wTestUser, cbWantOutCardData, cbWantOutCardCount ) ) {
		return false;
	}

	//��ը�ж�
	BYTE cbAllBombCard[ MAX_COUNT ], cbAllBombCount = 0 ;
	GetAllBomCard( cbCurHandCardData, cbCurHandCardCount, cbAllBombCard, cbAllBombCount ) ;

	if ( 0 < cbAllBombCount ) {
		//ʣ���˿�
		BYTE cbRemainCard [MAX_COUNT ], cbRemainCardCount ;

		CopyMemory( cbRemainCard, cbCurHandCardData, cbCurHandCardCount ) ;
		cbRemainCardCount = cbCurHandCardCount ;

		RemoveCard( cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount ) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		//�޴�����
		if ( CT_FOUR_LINE_TAKE_ONE == cbWantOutCardType || CT_FOUR_LINE_TAKE_TWO==cbWantOutCardType ) return false ;

		//ֻʣը��
		if ( CT_BOMB_CARD <= cbWantOutCardType && cbWantOutCardCount == cbCurHandCardCount ) return true ; 

		//ֻʣը��
		if ( cbAllBombCount == m_cbUserCardCount[ wTestUser ] && CT_BOMB_CARD <= cbWantOutCardType ) return true ;

		//ը����һ��
		if ( CT_BOMB_CARD <= cbWantOutCardType && GetCardType(cbRemainCard, cbRemainCardCount) != CT_ERROR && 
			CT_FOUR_LINE_TAKE_TWO != GetCardType( cbRemainCard, cbRemainCardCount ) && CT_FOUR_LINE_TAKE_ONE != GetCardType( cbRemainCard, cbRemainCardCount ) ) {
			return true ;
		}

		//�׳���ʱ����ը��
		if (CT_BOMB_CARD <= cbWantOutCardType && bFirstOutCard ) {
			//����ֻʣһ����
			if ( wTestUser != m_wBankerUser && GetCardType( m_cbAllCardData[ m_wBankerUser ], m_cbUserCardCount[ m_wBankerUser ] ) == CT_ERROR ) {
				return false ;
			}

			WORD wUndersideOfBanker = (m_wBankerUser+1)%GAME_PLAYER ;	//�����¼�
			WORD wUpsideOfBanker = (wUndersideOfBanker+1)%GAME_PLAYER ;	//�����ϼ�

			//�м�ֻʣһ����
			if ( wTestUser == m_wBankerUser && GetCardType( m_cbAllCardData[ wUndersideOfBanker ], m_cbUserCardCount[ wUndersideOfBanker ] ) == CT_ERROR &&
				GetCardType( m_cbAllCardData[ wUpsideOfBanker ], m_cbUserCardCount[ wUpsideOfBanker ] ) == CT_ERROR ) {
				return false ;
			}
		}

		//��ը�ж�
		if ( cbWantOutCardType < CT_FOUR_LINE_TAKE_ONE && cbWantOutCardType != CT_SINGLE_LINE ) {
			for ( BYTE i = 0; i < cbWantOutCardCount; ++i )
				for ( BYTE j = 0; j < cbAllBombCount; ++j ) {
					if ( GetCardValue( cbAllBombCard[ j ] ) == GetCardValue( cbWantOutCardData[ i ] ) ) return false ;
				}
		}
	}

	//�����ж�
	if ( cbCurHandCardCount == cbWantOutCardCount ) {
		return true ;
	}


	if ( cbCurHandCardCount == 0 ) {
		return true ;
	}

	try {
		ASSERT( cbWantOutCardCount <= cbCurHandCardCount ) ;
		if ( cbCurHandCardCount < cbWantOutCardCount ) {
			return false ;
		}

		//ʣ���˿�
		BYTE cbRemainCard [MAX_COUNT ], cbRemainCardCount ;

		CopyMemory( cbRemainCard, cbCurHandCardData, cbCurHandCardCount ) ;
		cbRemainCardCount = cbCurHandCardCount ;

		RemoveCard( cbWantOutCardData, cbWantOutCardCount, cbRemainCard, cbRemainCardCount ) ;
		cbRemainCardCount -= cbWantOutCardCount ;

		if ( cbRemainCardCount == 0 ) {
			return false;
		}

		//��������
		if ( CT_THREE_LINE_TAKE_ONE <= cbWantOutCardType && cbWantOutCardType <= CT_FOUR_LINE_TAKE_TWO && ! bFirstOutCard ) {
			bool bHaveLargeCard = false ;
			for ( BYTE i = 3; i < cbWantOutCardCount; ++i ) 
				if ( 15 <= GetCardLogicValue( cbWantOutCardData[ i ]) ) {
					bHaveLargeCard = true ;
					break ;
				}
				if ( bHaveLargeCard ) {
					for ( BYTE i = 0; i < cbRemainCardCount; ++i ) {
						if ( GetCardLogicValue( cbRemainCard[ i ]) < 15 ) {
							return false ;
						}
					}
				}
		}
	}
	catch(...) {
		return false ;
	}

	return true;
}

//��������
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{

    g_Logger.WriteLog("------------SearchOutCard1 -------------m_wBankerUser[%d]", m_wBankerUser);
    g_Logger.WriteLog("------------SearchOutCard1 -------------m_cbUserCardCount[0][%d],m_cbAllCardData[0][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]",m_cbUserCardCount[0],m_cbAllCardData[0][0],m_cbAllCardData[0][1],m_cbAllCardData[0][2],m_cbAllCardData[0][3],m_cbAllCardData[0][4],m_cbAllCardData[0][5],m_cbAllCardData[0][6],m_cbAllCardData[0][7],m_cbAllCardData[0][8],m_cbAllCardData[0][9],m_cbAllCardData[0][10],m_cbAllCardData[0][11],m_cbAllCardData[0][12],m_cbAllCardData[0][13],m_cbAllCardData[0][14],m_cbAllCardData[0][15],m_cbAllCardData[0][16],m_cbAllCardData[0][17],m_cbAllCardData[0][18],m_cbAllCardData[0][19]);
    g_Logger.WriteLog("------------SearchOutCard1 -------------m_cbUserCardCount[1][%d],m_cbAllCardData[1][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]",m_cbUserCardCount[1],m_cbAllCardData[1][0],m_cbAllCardData[1][1],m_cbAllCardData[1][2],m_cbAllCardData[1][3],m_cbAllCardData[1][4],m_cbAllCardData[1][5],m_cbAllCardData[1][6],m_cbAllCardData[1][7],m_cbAllCardData[1][8],m_cbAllCardData[1][9],m_cbAllCardData[1][10],m_cbAllCardData[1][11],m_cbAllCardData[1][12],m_cbAllCardData[1][13],m_cbAllCardData[1][14],m_cbAllCardData[1][15],m_cbAllCardData[1][16],m_cbAllCardData[1][17],m_cbAllCardData[1][18],m_cbAllCardData[1][19]);
    g_Logger.WriteLog("------------SearchOutCard1 -------------m_cbUserCardCount[2][%d],m_cbAllCardData[2][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x][%02x]",m_cbUserCardCount[2],m_cbAllCardData[2][0],m_cbAllCardData[2][1],m_cbAllCardData[2][2],m_cbAllCardData[2][3],m_cbAllCardData[2][4],m_cbAllCardData[2][5],m_cbAllCardData[2][6],m_cbAllCardData[2][7],m_cbAllCardData[2][8],m_cbAllCardData[2][9],m_cbAllCardData[2][10],m_cbAllCardData[2][11],m_cbAllCardData[2][12],m_cbAllCardData[2][13],m_cbAllCardData[2][14],m_cbAllCardData[2][15],m_cbAllCardData[2][16],m_cbAllCardData[2][17],m_cbAllCardData[2][18],m_cbAllCardData[2][19]);

    //���ý��
    ZeroMemory(&OutCardResult,sizeof(tagOutCardResult));

    //�����˿�
    BYTE cbCardData[MAX_COUNT];
    BYTE cbCardCount=cbHandCardCount;
    CopyMemory(cbCardData,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

    //�����˿�
    SortCardList(cbCardData,cbCardCount,ST_ORDER);

    //��ȡ����
    BYTE cbTurnOutType=GetCardType(cbTurnCardData,cbTurnCardCount);

    //���Ʒ���
    switch (cbTurnOutType)
    {
    case CT_ERROR:					//��������
        {
            //��ȡ��ֵ
            BYTE cbLogicValue = INVALID_BYTE;
            if (cbCardCount >= 1)
            {
                cbLogicValue = GetCardLogicValue(cbCardData[cbCardCount-1]);
            }
            else
            {
                //���ƴ���
                OutCardResult.cbCardCount=1;
                OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-1];
                return true;
            }

            //�����ж�
            BYTE cbSameCount=1;
            for (BYTE i=1;i<cbCardCount;i++)
            {
                if (GetCardLogicValue(cbCardData[cbCardCount-i-1])==cbLogicValue) cbSameCount++;
                else break;
            }

            //��ɴ���
            if (cbSameCount>1)
            {
                OutCardResult.cbCardCount=cbSameCount;
                for (BYTE j=0;j<cbSameCount;j++) OutCardResult.cbResultCard[j]=cbCardData[cbCardCount-1-j];
                return true;
            }

            //���ƴ���
            OutCardResult.cbCardCount=1;
            OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-1];

            return true;
        }
    case CT_SINGLE:					//��������
    case CT_DOUBLE:					//��������
    case CT_THREE:					//��������
        {
            //��ȡ��ֵ
            BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

            //�����˿�
            tagAnalyseResult AnalyseResult;
            AnalysebCardData(cbCardData,cbCardCount,AnalyseResult);

            //Ѱ�ҵ���
            if (cbTurnCardCount<=1)
            {
                for (BYTE i=0;i<AnalyseResult.cbSignedCount;i++)
                {
                    BYTE cbIndex=AnalyseResult.cbSignedCount-i-1;
                    if (GetCardLogicValue(AnalyseResult.cbSignedCardData[cbIndex])>cbLogicValue)
                    {
                        //���ý��
                        OutCardResult.cbCardCount=cbTurnCardCount;
                        CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbSignedCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

                        return true;
                    }
                }
            }

            //Ѱ�Ҷ���
            if (cbTurnCardCount<=2)
            {
                for (BYTE i=0;i<AnalyseResult.cbDoubleCount;i++)
                {
                    BYTE cbIndex=(AnalyseResult.cbDoubleCount-i-1)*2;
                    if (GetCardLogicValue(AnalyseResult.cbDoubleCardData[cbIndex])>cbLogicValue)
                    {
                        //���ý��
                        OutCardResult.cbCardCount=cbTurnCardCount;
                        CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbDoubleCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

                        return true;
                    }
                }
            }

            //Ѱ������
            if (cbTurnCardCount<=3)
            {
                for (BYTE i=0;i<AnalyseResult.cbThreeCount;i++)
                {
                    BYTE cbIndex=(AnalyseResult.cbThreeCount-i-1)*3;
                    if (GetCardLogicValue(AnalyseResult.cbThreeCardData[cbIndex])>cbLogicValue)
                    {
                        //���ý��
                        OutCardResult.cbCardCount=cbTurnCardCount;
                        CopyMemory(OutCardResult.cbResultCard,&AnalyseResult.cbThreeCardData[cbIndex],sizeof(BYTE)*cbTurnCardCount);

                        return true;
                    }
                }
            }

            break;
        }
    case CT_SINGLE_LINE:		//��������
        {
            //�����ж�
            if (cbCardCount<cbTurnCardCount) break;

            //��ȡ��ֵ
            BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

            //��������
            for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
            {
                //��ȡ��ֵ
                BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

                //�����ж�
                if (cbHandLogicValue>=15) break;
                if (cbHandLogicValue<=cbLogicValue) continue;

                //��������
                BYTE cbLineCount=0;
                for (BYTE j=(cbCardCount-i-1);j<cbCardCount;j++)
                {
                    if ((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue) 
                    {
                        //��������
                        OutCardResult.cbResultCard[cbLineCount++]=cbCardData[j];

                        //����ж�
                        if (cbLineCount==cbTurnCardCount)
                        {
                            OutCardResult.cbCardCount=cbTurnCardCount;
                            return true;
                        }
                    }
                }
            }

            break;
        }
    case CT_DOUBLE_LINE:		//��������
        {
            //�����ж�
            if (cbCardCount<cbTurnCardCount) break;

            //��ȡ��ֵ
            BYTE cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

            //��������
            for (BYTE i=(cbTurnCardCount-1);i<cbCardCount;i++)
            {
                //��ȡ��ֵ
                BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

                //�����ж�
                if (cbHandLogicValue<=cbLogicValue) continue;
                if ((cbTurnCardCount>1)&&(cbHandLogicValue>=15)) break;

                //��������
                BYTE cbLineCount=0;
                for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-1);j++)
                {
                    if (((GetCardLogicValue(cbCardData[j])+cbLineCount)==cbHandLogicValue)
                        &&((GetCardLogicValue(cbCardData[j+1])+cbLineCount)==cbHandLogicValue))
                    {
                        //��������
                        OutCardResult.cbResultCard[cbLineCount*2]=cbCardData[j];
                        OutCardResult.cbResultCard[(cbLineCount++)*2+1]=cbCardData[j+1];

                        //����ж�
                        if (cbLineCount*2==cbTurnCardCount)
                        {
                            OutCardResult.cbCardCount=cbTurnCardCount;
                            return true;
                        }
                    }
                }
            }

            break;
        }
    case CT_THREE_LINE:				//��������
    case CT_THREE_LINE_TAKE_ONE:	//����һ��
    case CT_THREE_LINE_TAKE_TWO:	//����һ��
        {
            //�����ж�
            if (cbCardCount<cbTurnCardCount) break;

            //��ȡ��ֵ
            BYTE cbLogicValue=0;
            for (BYTE i=0;i<cbTurnCardCount-2;i++)
            {
                cbLogicValue=GetCardLogicValue(cbTurnCardData[i]);
                if (GetCardLogicValue(cbTurnCardData[i+1])!=cbLogicValue) continue;
                if (GetCardLogicValue(cbTurnCardData[i+2])!=cbLogicValue) continue;
                break;
            }

            //������ֵ
            BYTE cbTurnLineCount=0;
            if (cbTurnOutType==CT_THREE_LINE_TAKE_ONE) cbTurnLineCount=cbTurnCardCount/4;
            else if (cbTurnOutType==CT_THREE_LINE_TAKE_TWO) cbTurnLineCount=cbTurnCardCount/5;
            else cbTurnLineCount=cbTurnCardCount/3;

            //��������
            for (BYTE i=cbTurnLineCount*3-1;i<cbCardCount;i++)
            {
                //��ȡ��ֵ
                BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

                //�����ж�
                if (cbHandLogicValue<=cbLogicValue) continue;
                if ((cbTurnLineCount>1)&&(cbHandLogicValue>=15)) break;

                //��������
                BYTE cbLineCount=0;
                for (BYTE j=(cbCardCount-i-1);j<(cbCardCount-2);j++)
                {
                    //���ñ���
                    OutCardResult.cbCardCount=0;

                    //�����ж�
                    if ((GetCardLogicValue(cbCardData[j])+cbLineCount)!=cbHandLogicValue) continue;
                    if ((GetCardLogicValue(cbCardData[j+1])+cbLineCount)!=cbHandLogicValue) continue;
                    if ((GetCardLogicValue(cbCardData[j+2])+cbLineCount)!=cbHandLogicValue) continue;

                    //��������
                    OutCardResult.cbResultCard[cbLineCount*3]=cbCardData[j];
                    OutCardResult.cbResultCard[cbLineCount*3+1]=cbCardData[j+1];
                    OutCardResult.cbResultCard[(cbLineCount++)*3+2]=cbCardData[j+2];

                    //����ж�
                    if (cbLineCount==cbTurnLineCount)
                    {
                        //��������
                        OutCardResult.cbCardCount=cbLineCount*3;

                        //�����˿�
                        BYTE cbLeftCardData[MAX_COUNT];
                        BYTE cbLeftCount=cbCardCount-OutCardResult.cbCardCount;
                        CopyMemory(cbLeftCardData,cbCardData,sizeof(BYTE)*cbCardCount);
                        RemoveCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount,cbLeftCardData,cbCardCount);

                        //�����˿�
                        tagAnalyseResult AnalyseResultLeft;
                        AnalysebCardData(cbLeftCardData,cbLeftCount,AnalyseResultLeft);

                        //���ƴ���
                        if (cbTurnOutType==CT_THREE_LINE_TAKE_ONE)
                        {
                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbSignedCount;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=AnalyseResultLeft.cbSignedCount-k-1;
                                BYTE cbSignedCard=AnalyseResultLeft.cbSignedCardData[cbIndex];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
                            }

                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbDoubleCount*2;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbDoubleCount*2-k-1);
                                BYTE cbSignedCard=AnalyseResultLeft.cbDoubleCardData[cbIndex];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
                            }

                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbThreeCount*3;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbThreeCount*3-k-1);
                                BYTE cbSignedCard=AnalyseResultLeft.cbThreeCardData[cbIndex];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
                            }

                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbFourCount*4;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbFourCount*4-k-1);
                                BYTE cbSignedCard=AnalyseResultLeft.cbFourCardData[cbIndex];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbSignedCard;
                            }
                        }

                        //���ƴ���
                        if (cbTurnOutType==CT_THREE_LINE_TAKE_TWO)
                        {
                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbDoubleCount;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbDoubleCount-k-1)*2;
                                BYTE cbCardData1=AnalyseResultLeft.cbDoubleCardData[cbIndex];
                                BYTE cbCardData2=AnalyseResultLeft.cbDoubleCardData[cbIndex+1];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
                            }

                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbThreeCount;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbThreeCount-k-1)*3;
                                BYTE cbCardData1=AnalyseResultLeft.cbThreeCardData[cbIndex];
                                BYTE cbCardData2=AnalyseResultLeft.cbThreeCardData[cbIndex+1];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
                            }

                            //��ȡ����
                            for (BYTE k=0;k<AnalyseResultLeft.cbFourCount;k++)
                            {
                                //��ֹ�ж�
                                if (OutCardResult.cbCardCount==cbTurnCardCount) break;

                                //�����˿�
                                BYTE cbIndex=(AnalyseResultLeft.cbFourCount-k-1)*4;
                                BYTE cbCardData1=AnalyseResultLeft.cbFourCardData[cbIndex];
                                BYTE cbCardData2=AnalyseResultLeft.cbFourCardData[cbIndex+1];
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData1;
                                OutCardResult.cbResultCard[OutCardResult.cbCardCount++]=cbCardData2;
                            }
                        }

                        //����ж�
                        if (OutCardResult.cbCardCount==cbTurnCardCount) return true;
                    }
                }
            }

            break;
        }
    case CT_JET:
        {
            //pass
            return true;
        }
     }

    //����ը��
    if ((cbCardCount>=4)&&(cbTurnOutType!=CT_MISSILE_CARD))
    {
        //��������
        BYTE cbLogicValue=0;
        if (cbTurnOutType==CT_BOMB_CARD) cbLogicValue=GetCardLogicValue(cbTurnCardData[0]);

        //����ը��
        for (BYTE i=3;i<cbCardCount;i++)
        {
            //��ȡ��ֵ
            BYTE cbHandLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);

            //�����ж�
            if (cbHandLogicValue<=cbLogicValue) continue;

            //ը���ж�
            BYTE cbTempLogicValue=GetCardLogicValue(cbCardData[cbCardCount-i-1]);
            BYTE j;
            for (j=1;j<4;j++)
            {
                if (GetCardLogicValue(cbCardData[cbCardCount+j-i-1])!=cbTempLogicValue) break;
            }
            if (j!=4) continue;

            //���ý��
            OutCardResult.cbCardCount=4;
            OutCardResult.cbResultCard[0]=cbCardData[cbCardCount-i-1];
            OutCardResult.cbResultCard[1]=cbCardData[cbCardCount-i];
            OutCardResult.cbResultCard[2]=cbCardData[cbCardCount-i+1];
            OutCardResult.cbResultCard[3]=cbCardData[cbCardCount-i+2];

            return true;
        }
    }

    tagAnalyseResult AnalyseHandCard;
    AnalysebCardData(cbHandCardData,cbHandCardCount,AnalyseHandCard);
    if (cbTurnOutType == CT_BOMB_CARD_5  || cbTurnOutType == CT_MISSILE_CARD)
    {
        SearchFiveBumb(cbCardData, cbCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult,AnalyseHandCard);
        return true;
    }
    else if (cbTurnOutType == CT_BOMB_CARD_6 || cbTurnOutType == CT_MISSILE_CARD_3)
    {
        SearchSixBumb(cbCardData, cbCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult,AnalyseHandCard);
        return true;
    }
    else if (cbTurnOutType == CT_BOMB_CARD_7 || cbTurnOutType == CT_MISSILE_CARD_4)
    {
        SearchSevenBumb(cbCardData, cbCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult,AnalyseHandCard);
        return true;
    }
    else if (cbTurnOutType == CT_BOMB_CARD_8 || cbTurnOutType == CT_MISSILE_CARD_5)
    {
        SearchEightBumb(cbCardData, cbCardCount, cbTurnCardData, cbTurnCardCount, OutCardResult,AnalyseHandCard);
        return true;
    }
   
    //�������
    if ((cbCardCount>=6)&&(cbCardData[5]==0x4E))
    {
        //���ý��
        OutCardResult.cbCardCount = 6 ;
        OutCardResult.cbResultCard[0] = 0x4f ;
        OutCardResult.cbResultCard[1] = 0x4f ;
        OutCardResult.cbResultCard[2] = 0x4f ;
        OutCardResult.cbResultCard[3] = 0x4e ;
        OutCardResult.cbResultCard[4] = 0x4e ;
        OutCardResult.cbResultCard[5] = 0x4e ;

        return true;
    }

    return false;
}

/************************************************************************/
/* ���ݷ������ �ж��Ƿ�ɻ�����                                        */
/* ���������ƺ� �������ܶ�������͵ķɻ�                              */
/************************************************************************/
//bool CGameLogic::IsJetCardType(const BYTE cbCardData[], BYTE cbCardCount)
//{
//    if (NULL == cbCardData)
//    {
//        return false;
//    }
//
//    //�ɻ����� ���ߴ�˫���������
//    if ((8 != cbCardCount) && (10 != cbCardCount) || (12 != cbCardCount) 
//        && (15 != cbCardCount) && (16 != cbCardCount) && (20 != cbCardCount))
//    {
//        return false;
//    }
//    //������Ҫ�ֲ�������Ϸɻ�
//    BYTE  i = 0;
//    BYTE cbCardCountSum[18] = {0};   //total 15 card, count each card, empty count 0 1 2 0 1->14 2->15 black joker->16 red joker->17
//    BYTE bThreeCardCount = 0;
//    BYTE bTempCardCount = cbCardCount;
//    BYTE bFirstThreeCardData = 0;
//    BYTE bLastThreeCardData = 0;
//
//    for (i = 0; i < cbCardCount; i++)
//    {
//        cbCardCountSum[GetCardLogicValue(cbCardData[i])]++;
//    }
//
//    //���3�����Ƿ����� 
//    //����Ҫ����2->15 �Լ� ��С�� 16 17
//    //index 0 1 2 Ϊ��
//    for (i = 3; i < 15; i++)
//    {
//        //check if any card count >= 3
//        // break out to check next 3 cards
//        if (cbCardCountSum[i] >= 3)
//        {
//            bThreeCardCount++;
//            bFirstThreeCardData = i;
//            break;
//        }
//    }
//
//    if (i >= 14)  /*С2 СA jokers*/
//    {
//        //no 3 cards and first 3cards is A
//        return false;
//    }
//    
//    BYTE bCardIndex = 0;
//    bCardIndex = i;
//    bTempCardCount -= cbCardCountSum[bCardIndex];
//    while (bTempCardCount >= 3)
//    {
//        bCardIndex++;
//        if (cbCardCountSum[bCardIndex] < 3)
//        {
//            bLastThreeCardData = bCardIndex - 1;
//            break;
//        }
//        else
//        {
//            bThreeCardCount++;
//            bTempCardCount -= cbCardCountSum[bCardIndex];
//        }
//    }
//
//    //3������С��2 ���Ƿɻ�
//    if (bThreeCardCount < 2)
//    {
//        return false;
//    }
//
//    //check �ɻ����
//    BYTE bWingCardCount = 0;
//    BYTE bWingCardSingleCount = 0;
//    BYTE bWingCardCoupleCount = 0;
//    BYTE bWingCardData[MAX_COUNT] = {0};
//    BYTE bTemp = 0;
//
//    for (i = 3; i < 18; i++)
//    {
//        if (cbCardCountSum[i] > 0)
//        {
//            if (cbCardCountSum[i] == 1)
//            {
//                bWingCardSingleCount++;
//            }
//            else if (cbCardCountSum[i] == 2)
//            {
//                bWingCardCoupleCount++;
//            }
//
//            //total
//            bTemp = (cbCardCountSum[i] >= 3) ? (cbCardCountSum[i] - 3) : (cbCardCountSum[i]);
//            memset(bWingCardData + bWingCardCount, i, sizeof(BYTE)*bTemp);
//            bWingCardCount += bTemp;
//        }
//    }
//    
//    //if wing is couple check whether all card is couple
//
//    BYTE bWingCardCountSum[18] = {0};
//
//    if (bWingCardCount == bThreeCardCount) /*����*/
//    {
//        ASSERT((cbCardCount == 8) || (cbCardCount == 12) || (cbCardCount == 16) || (cbCardCount == 20));
//        
//        return true;
//    }
//    else if (bWingCardCount == (2*bThreeCardCount)) /*����*/
//    {
//        ASSERT((cbCardCount == 10) || (cbCardCount == 15) || (cbCardCount == 20));
//        ASSERT(bWingCardSingleCount == 0);
//        //�жϴ������ �����������ڷֲ�4�� ��6���ɶ��ӵ����
//        //���bWingCardData�ǲ���ȫ�Ƕ���
//        for (i = 0; i < bWingCardCount; i++)
//        {
//            bWingCardCountSum[bWingCardData[i]]++;
//        }
//
//        for (i = 0; i < 18; i++)
//        {
//            if ((bWingCardCountSum[i] % 2) == 0)
//            {
//                continue;
//            }
//            else
//            {
//                return false;
//            }
//        }
//
//        return true;
//    }
//    else
//    {
//        BYTE bCount = 0;
//        if (bThreeCardCount > 2)
//        {
//            //���Բ��һ��3�� �������һ��3����Ϊ���
//            if ((bWingCardCount + cbCardCountSum[bFirstThreeCardData]) == (bThreeCardCount - 1)
//                || ((bWingCardCount + cbCardCountSum[bLastThreeCardData]) == (bThreeCardCount - 1)))
//            {
//                //��� �ɴ����Ʒɻ�
//                return true;
//            }
//
//            if ((bWingCardCount + cbCardCountSum[bFirstThreeCardData]) == 2* (bThreeCardCount - 1))
//            {
//                memset(bWingCardData + bWingCardCount, bFirstThreeCardData, sizeof(BYTE)*cbCardCountSum[bFirstThreeCardData]);   
//            }
//            else if ((bWingCardCount + cbCardCountSum[bLastThreeCardData]) == 2*(bThreeCardCount - 1))
//            {
//                memset(bWingCardData + bWingCardCount, bLastThreeCardData, sizeof(BYTE)*cbCardCountSum[bLastThreeCardData]); 
//            }
//
//            //����Ƿ�ȫ�Ƕ���
//            for (i = 0; i < bWingCardCount; i++)
//            {
//                bWingCardCountSum[bWingCardData[i]]++;
//            }
//
//            for (i = 0; i < 18; i++)
//            {
//                if ((bWingCardCountSum[i] % 2) == 0)
//                {
//                    continue;
//                }
//                else
//                {
//                    return false;
//                }
//            }
//
//            if (i == 18)
//            {
//                return true;
//            }
//        }
//
//        return false;
//    }
//}

//��ȡ�ɻ�����������3������ֵ
//
BYTE CGameLogic::GetJetMaxLogicValue(const BYTE cbCardData[], BYTE cbCardCount)
{
    if (NULL == cbCardData)
    {
        return 0;
    }

    //�ɻ����� ���ߴ�˫���������
    //if ((8 != cbCardCount) && (10 != cbCardCount) || (12 != cbCardCount) 
    //    && (15 != cbCardCount) && (16 != cbCardCount) && (20 != cbCardCount))
    //{
    //    return 0;
    //}
    //������Ҫ�ֲ�������Ϸɻ�
    BYTE  i = 0;
    BYTE cbCardCountSum[18] = {0};   //total 15 card, count each card, empty count 0 1 2 0 1->14 2->15 black joker->16 red joker->17
    BYTE bThreeCardCount = 0;
    BYTE bTempCardCount = cbCardCount;
    BYTE bFirstThreeCardData = 0;
    BYTE bLastThreeCardData = 0;

    for (i = 0; i < cbCardCount; i++)
    {
        cbCardCountSum[GetCardLogicValue(cbCardData[i])]++;
    }

    //���3�����Ƿ����� 
    //����Ҫ����2->15 �Լ� ��С�� 16 17
    //index 0 1 2 Ϊ��
    for (i = 3; i < 15; i++)
    {
        //check if any card count >= 3
        // break out to check next 3 cards
        if (cbCardCountSum[i] >= 3)
        {
            bThreeCardCount++;
            bFirstThreeCardData = i;
            break;
        }
    }

    if (i > 14)  /*С2 СA jokers*/
    {
        //no 3 cards and first 3cards is A
        return 0;
    }

    BYTE bCardIndex = 0;
    bCardIndex = i;
    bTempCardCount -= cbCardCountSum[bCardIndex];
    while (bTempCardCount >= 3)
    {
        bCardIndex++;
        if (cbCardCountSum[bCardIndex] < 3)
        {
            bLastThreeCardData = bCardIndex - 1;
            break;
        }
        else
        {
            bThreeCardCount++;
            bTempCardCount -= cbCardCountSum[bCardIndex];
            bLastThreeCardData = bCardIndex;
        }
    }

    //3������С��2 ���Ƿɻ�
    if (bThreeCardCount < 2)
    {
        return 0;
    }

    return bLastThreeCardData;
}

bool CGameLogic::IsJetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
    if (NULL == cbCardData || cbCardCount < 8)
    {
        return false;
    }
    
    //�ɻ����� ���ߴ�˫���������
    if ((8 != cbCardCount) && (10 != cbCardCount) && (12 != cbCardCount)
    && (15 != cbCardCount) && (16 != cbCardCount) && (20 != cbCardCount))
    {
        return false;
    }

	BYTE bNeedThreeLineGroup = 0;
	switch(cbCardCount)
	{
	case 8:
		bNeedThreeLineGroup = 2;break;
	case 10:
		bNeedThreeLineGroup = 2;break;
	case 12:
		bNeedThreeLineGroup = 3;break;
	case 15:
		bNeedThreeLineGroup = 3;break;
	case 16:
		bNeedThreeLineGroup = 4;break;
	case 20:
		bNeedThreeLineGroup = 4;break;
	default:
		ASSERT(false);break;
	}
    BYTE cbTmpCardData[MAX_COUNT] = {0};
    CopyMemory(cbTmpCardData, cbCardData, cbCardCount) ;

    BYTE cbHandThreeCard[MAX_COUNT];
    BYTE cbHandThreeCount=0 ;


    GetAllThreeCard(cbTmpCardData, cbCardCount, cbHandThreeCard, cbHandThreeCount) ;
	if (cbHandThreeCount < 6)
		return false;


    BYTE cbLeftThreeCardCount=0 ;
	BYTE cbLastIndex=0 ;
	BYTE cbThreeLineCard[MAX_COUNT];
	for(BYTE i = 0; i < cbHandThreeCount; i+=3)
	{
		BYTE cbLastLogicCard=GetCardLogicValue(cbHandThreeCard[i]);
        if (cbLastLogicCard >= 15) //�ɻ����� 2��������
            continue;
		cbThreeLineCard[cbLeftThreeCardCount++]=cbHandThreeCard[i];
		cbThreeLineCard[cbLeftThreeCardCount++]=cbHandThreeCard[i+1];
		cbThreeLineCard[cbLeftThreeCardCount++]=cbHandThreeCard[i+2];

		if (cbLeftThreeCardCount/3 == bNeedThreeLineGroup) //�ɻ�������
		{

			BYTE cbRemainCard[MAX_COUNT];
			BYTE cbRemainCardCount=cbCardCount-cbLeftThreeCardCount ;

			//�Ƴ�����
			CopyMemory(cbRemainCard, cbTmpCardData, (cbCardCount)*sizeof(BYTE));
			RemoveCard(cbThreeLineCard, cbLeftThreeCardCount, cbRemainCard, cbCardCount) ;

			BYTE bWingCardCount = 0;
			BYTE cbWingCardData[MAX_COUNT] = {0};
			switch(cbCardCount)
			{
			case 8:
				if (2 == cbRemainCardCount) return true;
				else goto next;
			case 10:
				GetAllDoubleCardEx(cbRemainCard, cbRemainCardCount, cbWingCardData, bWingCardCount) ;
				if (bWingCardCount/2 == 2) return true;
				else goto next;
			case 12:
				if (3 == cbRemainCardCount) return true;
				else goto next;
			case 15:
				GetAllDoubleCardEx(cbRemainCard, cbRemainCardCount, cbWingCardData, bWingCardCount) ;
				if (bWingCardCount/2 == 3) return true;
				else goto next;
			case 16:
				if (4 == cbRemainCardCount) return true;
				else goto next;
			case 20:
				GetAllDoubleCardEx(cbRemainCard, cbRemainCardCount, cbWingCardData, bWingCardCount) ;
				if (bWingCardCount/2 == 4) return true;
				else goto next;
			default:
				ASSERT(false);break;
			}


		}

		if(1!=(cbLastLogicCard-(GetCardLogicValue(cbHandThreeCard[i+3])))) //�������Ʋ���ʧ��
            goto next;
        continue;

//ѭ�����¿�ʼ��Ѱ����һ������
next: 
		cbLeftThreeCardCount = 0;
        i = 3 * cbLastIndex;
		cbLastIndex++;
		continue;
	}

    return false;
}

//�ж��Ƿ�ȫ�Ƕ���
bool CGameLogic::IsAllCouple(const BYTE cbCardData[], BYTE cbCardCount)
{
    if (NULL == cbCardData)
    {
        return false;
    }

    BYTE bWingCardCountSum[18] = {0};
    BYTE i = 0;

    //����Ƿ�ȫ�Ƕ���
    for (i = 0; i < cbCardCount; i++)
    {
        bWingCardCountSum[cbCardData[i]]++;
    }

    for (i = 0; i < 18; i++)
    {
        if ((bWingCardCountSum[i] % 2) == 0)
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    if (i == 18)
    {
        return true;
    }

    return false;
}


bool CGameLogic::SearchFiveBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard)
{
    //��������
    BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);


    //���������ҪѰ�������Ƶ�ը����1 �ϼҳ�����ը  2 �ϼҳ���5���Ƶ�ը��
    if(cbTurnCardCount == 2)
    {
        int nJokers = CountJokers(cbHandCardData, cbHandCardCount);
        if( nJokers == cbTurnCardCount )
        {
            int nBigJokers  = CountBigJoker(cbTurnCardData, cbTurnCardCount);
            if(nBigJokers == 0)
            {
                int nMeBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
                switch (nMeBigJokers) {
                    case 1:
                        {
                            OutCardResult.cbCardCount = 2;
                            OutCardResult.cbResultCard[0] = 0x4F;
                            OutCardResult.cbResultCard[1] = 0x4F;
                            return true;
                        }
                    case 2:
                        {
                            OutCardResult.cbCardCount = 2;
                            OutCardResult.cbResultCard[0] = 0x4F;
                            OutCardResult.cbResultCard[1] = 0x4F;
                            return true;

                        }
                }
            }
            else if(nBigJokers == 1)
            {
                int nMeBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
                if(nMeBigJokers == 2)
                {
                    OutCardResult.cbCardCount = 2;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    return true;
                }

            }
        }
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount < 5)
    {
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount == 5)
    {
        if(GetCardType(cbTurnCardData, cbTurnCardCount) == CT_THREE_LINE_TAKE_TWO || GetCardType(cbTurnCardData, cbTurnCardCount) == CT_SINGLE_LINE)
        {
            cbLogicValue = 0;
        }

    }


    //����ը��
    if(AnalyseHandCard.cbFiveCount > 0)
    {

        for(int i = AnalyseHandCard.cbFiveCount - 1;i >= 0;i--)
        {
            BYTE cbTempCardValue = GetCardLogicValue(AnalyseHandCard.cbFiveCardData[i * 5]);
            if(cbTempCardValue > cbLogicValue)
            {
                //���ý��
                OutCardResult.cbCardCount=5;
                OutCardResult.cbResultCard[0] = AnalyseHandCard.cbFiveCardData[i * 5];
                OutCardResult.cbResultCard[1] = AnalyseHandCard.cbFiveCardData[i * 5 + 1];
                OutCardResult.cbResultCard[2] = AnalyseHandCard.cbFiveCardData[i * 5 + 2];
                OutCardResult.cbResultCard[3] = AnalyseHandCard.cbFiveCardData[i * 5 + 3];
                OutCardResult.cbResultCard[4] = AnalyseHandCard.cbFiveCardData[i * 5 + 4];
                return true;
            }

        }
    }

    //������ը��
    int nJokersNumber = CountJokers(cbHandCardData, cbHandCardCount);
    if( nJokersNumber == 3)
    {
        int nBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
        OutCardResult.cbCardCount=3;
        for( int i = 0 ; i < nBigJokers;i++)
        {
            OutCardResult.cbResultCard[i] = 0x4F;
        }

        for(int i = 0 ; i < (3 - nBigJokers);i++)
        {
            OutCardResult.cbResultCard[nBigJokers + i] = 0x4E;
        }


        return true;
    }

    return SearchSixBumb(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult,AnalyseHandCard);
}

bool CGameLogic::SearchSixBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard)
{
    //��������
    BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);


    //���������ҪѰ�������Ƶ�ը����1 �ϼҳ�����ը  2 �ϼҳ���5���Ƶ�ը��
    if(cbTurnCardCount == 3)
    {
        int nJokers = CountJokers(cbHandCardData, cbHandCardCount);
        if(nJokers == 3)
        {
            int nBigJokers = CountBigJoker(cbTurnCardData, cbTurnCardCount);
            if(nBigJokers  == 0)
            {
                int nMeBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
                if( nMeBigJokers == 1 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4E;
                    OutCardResult.cbResultCard[2] = 0x4E;
                    return true;
                }
                if( nMeBigJokers == 2 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    OutCardResult.cbResultCard[2] = 0x4E;
                    return true;
                }
                else if( nMeBigJokers == 3 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    OutCardResult.cbResultCard[2] = 0x4F;
                    return true;
                }
            }
            else if(nBigJokers  == 1)
            {
                int nMeBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
                if( nMeBigJokers == 2 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    OutCardResult.cbResultCard[2] = 0x4E;
                    return true;
                }
                else if( nMeBigJokers == 3 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    OutCardResult.cbResultCard[2] = 0x4F;
                    return true;
                }
            }
            else if(nBigJokers == 2)
            {
                int nMeBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
                if( nMeBigJokers == 3 )
                {
                    OutCardResult.cbCardCount = 3;
                    OutCardResult.cbResultCard[0] = 0x4F;
                    OutCardResult.cbResultCard[1] = 0x4F;
                    OutCardResult.cbResultCard[2] = 0x4F;
                    return true;
                }
            }
        }
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount < 6)
    {
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount == 6)
    {
        if(GetCardType(cbTurnCardData, cbTurnCardCount) == CT_THREE_LINE || GetCardType(cbTurnCardData, cbTurnCardCount) == CT_SINGLE_LINE || GetCardType(cbTurnCardData, cbTurnCardCount) == CT_DOUBLE_LINE)
        {
            cbLogicValue = 0;
        }
    }



    //����ը��
    if(AnalyseHandCard.cbSixCount > 0)
    {
        for(int i = AnalyseHandCard.cbSixCount - 1;i >= 0;i--)
        {
            BYTE cbTempCardValue = GetCardLogicValue(AnalyseHandCard.cbSixCardData[i * 6]);
            if(cbTempCardValue > cbLogicValue)
            {
                //���ý��
                OutCardResult.cbCardCount=6;
                OutCardResult.cbResultCard[0] = AnalyseHandCard.cbSixCardData[i * 6];
                OutCardResult.cbResultCard[1] = AnalyseHandCard.cbSixCardData[i * 6 + 1];
                OutCardResult.cbResultCard[2] = AnalyseHandCard.cbSixCardData[i * 6 + 2];
                OutCardResult.cbResultCard[3] = AnalyseHandCard.cbSixCardData[i * 6 + 3];
                OutCardResult.cbResultCard[4] = AnalyseHandCard.cbSixCardData[i * 6 + 4];
                OutCardResult.cbResultCard[5] = AnalyseHandCard.cbSixCardData[i * 6 + 5];
                return true;
            }

        }
    }

    //��4��ը��
    int nJokersNumber = CountJokers(cbHandCardData, cbHandCardCount);
    if( nJokersNumber == 4)
    {
        int nBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);

        OutCardResult.cbCardCount = 4;
        for( int i = 0 ; i < nBigJokers;i++)
        {
            OutCardResult.cbResultCard[i] = 0x4F;
        }

        for(int i = 0 ; i < (4 - nBigJokers);i++)
        {
            OutCardResult.cbResultCard[nBigJokers + i] = 0x4E;
        }
        return true;
    }

    return SearchSevenBumb(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult,AnalyseHandCard);
}

bool CGameLogic::SearchSevenBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard)
{
    BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);

    //���������ҪѰ�������Ƶ�ը����1 �ϼҳ�����ը  2 �ϼҳ���5���Ƶ�ը��
    if(cbTurnCardCount == 4)
    {
        //�ϼҳ���������ը ����+����
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount < 7)
    {
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount == 7)
    {
        if(GetCardType(cbTurnCardData, cbTurnCardCount) == CT_SINGLE_LINE )
        {
            cbLogicValue = 0;
        }
    }



    //����ը��
    if(AnalyseHandCard.cbSenvenCount > 0)
    {

        for(int i = AnalyseHandCard.cbSenvenCount - 1;i >= 0;i--)
        {
            BYTE cbTempCardValue = GetCardLogicValue(AnalyseHandCard.cbSenvenCardData[i * 7]);
            if(cbTempCardValue > cbLogicValue)
            {
                //���ý��
                OutCardResult.cbCardCount=7;
                OutCardResult.cbResultCard[0] = AnalyseHandCard.cbSenvenCardData[i * 7];
                OutCardResult.cbResultCard[1] = AnalyseHandCard.cbSenvenCardData[i * 7 + 1];
                OutCardResult.cbResultCard[2] = AnalyseHandCard.cbSenvenCardData[i * 7 + 2];
                OutCardResult.cbResultCard[3] = AnalyseHandCard.cbSenvenCardData[i * 7 + 3];
                OutCardResult.cbResultCard[4] = AnalyseHandCard.cbSenvenCardData[i * 7 + 4];
                OutCardResult.cbResultCard[5] = AnalyseHandCard.cbSenvenCardData[i * 7 + 5];
                OutCardResult.cbResultCard[6] = AnalyseHandCard.cbSenvenCardData[i * 7 + 6];
                return true;
            }

        }
    }

    //��6��ը��
    int nJokersNumber = CountJokers(cbHandCardData, cbHandCardCount);
    if( nJokersNumber == 5)
    {
        int nBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
        if( nBigJokers > 0)
        {
            OutCardResult.cbCardCount=5;
            for( int i = 0 ; i < nBigJokers;i++)
            {
                OutCardResult.cbResultCard[i] = 0x4F;
            }

            for(int i = 0 ; i < (5 - nBigJokers);i++)
            {
                OutCardResult.cbResultCard[nBigJokers + i] = 0x4E;
            }
        }
        return true;
    }

    return SearchEightBumb(cbHandCardData,cbHandCardCount,cbTurnCardData,cbTurnCardCount,OutCardResult,AnalyseHandCard);
}

bool CGameLogic::SearchEightBumb(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult,tagAnalyseResult AnalyseHandCard)
{
    BYTE cbLogicValue = GetCardLogicValue(cbTurnCardData[0]);


    //���������ҪѰ�������Ƶ�ը����1 �ϼҳ�����ը  2 �ϼҳ���5���Ƶ�ը��
    if(cbTurnCardCount == 5)
    {
        //�ϼҳ���������ը ����+����
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount < 8)
    {
        cbLogicValue = 0;
    }
    else if(cbTurnCardCount == 8)
    {
        if(GetCardType(cbTurnCardData, cbTurnCardCount) == CT_SINGLE_LINE || GetCardType(cbTurnCardData, cbTurnCardCount) == CT_DOUBLE_LINE)
        {
            cbLogicValue = 0;
        }
    }



    //����ը��
    if(AnalyseHandCard.cbEightCount > 0)
    {

        for(int i = AnalyseHandCard.cbEightCount - 1;i >= 0;i--)
        {
            BYTE cbTempCardValue = GetCardLogicValue(AnalyseHandCard.cbEightCardData[i * 7]);
            if(cbTempCardValue > cbLogicValue)
            {
                //���ý��
                OutCardResult.cbCardCount=8;
                OutCardResult.cbResultCard[0] = AnalyseHandCard.cbEightCardData[i * 8];
                OutCardResult.cbResultCard[1] = AnalyseHandCard.cbEightCardData[i * 8 + 1];
                OutCardResult.cbResultCard[2] = AnalyseHandCard.cbEightCardData[i * 8 + 2];
                OutCardResult.cbResultCard[3] = AnalyseHandCard.cbEightCardData[i * 8 + 3];
                OutCardResult.cbResultCard[4] = AnalyseHandCard.cbEightCardData[i * 8 + 4];
                OutCardResult.cbResultCard[5] = AnalyseHandCard.cbEightCardData[i * 8 + 5];
                OutCardResult.cbResultCard[6] = AnalyseHandCard.cbEightCardData[i * 8 + 6];
                OutCardResult.cbResultCard[7] = AnalyseHandCard.cbEightCardData[i * 8 + 7];
                return true;
            }

        }
    }

    //��6��ը��
    int nJokersNumber = CountJokers(cbHandCardData, cbHandCardCount);
    if( nJokersNumber == 6)
    {
        int nBigJokers = CountBigJoker(cbHandCardData, cbHandCardCount);
        if( nBigJokers > 0)
        {
            OutCardResult.cbCardCount=3;
            for( int i = 0 ; i < nBigJokers;i++)
            {
                OutCardResult.cbResultCard[i] = 0x4F;
            }

            for(int i = 0 ; i < (3 - nBigJokers);i++)
            {
                OutCardResult.cbResultCard[nBigJokers + i + 1] = 0x4E;
            }
        }
        return true;
    }

    return false;
}

int CGameLogic::CountJokers(const BYTE cbCardData[], BYTE cbCardCount)
{
    int nCountNumbers = 0;
    for(int i = 0; i < cbCardCount;i++)
    {
        if(cbCardData[i]==0x4E || cbCardData[i]==0x4F)
        {
            nCountNumbers++;
        }
    }

    return nCountNumbers;
}


int CGameLogic::CountBigJoker(const BYTE cbCardData[], BYTE cbCardCount)
{
    int nCountNumbers = 0;
    for(int i = 0; i < cbCardCount;i++)
    {
        if(cbCardData[i]==0x4F)
        {
            nCountNumbers++;
        }
    }

    return nCountNumbers;
}

BYTE CGameLogic::SequentialThreeDataCount(BYTE bCardData[], BYTE bCardCount)
{
    BYTE bCount = 1;
    BYTE i = 0;

    for (i = 0; i < bCardCount; i++)
    {
        if (bCardData[i] == 0)
        {
            break;
        }

        if ((bCardData[i] + 1) == bCardData[i+1])
        {
            bCount++;
        }
    }

    return bCount;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////