#include "ceelment.h"
#include "cgeneratrix.h"
#include "clinker.h"
#include "cpower.h"
#include "ctransformer.h"
#include "cct_sign.h"
#include "cpt_sign.h"
#include "cbreaker.h"
#include "canalog_data_sign.h"
#include "ctext_sign.h"
#include "ccircle_indicator_sign.h"
#include "crect_indicator_sign.h"
#include "clightning_protector.h"
#include "ccontactor.h"
#include "cswitch_2pos.h"
#include "cswitch_3pos.h"
#include "cload_breaker_2pos.h"
#include "cload_breaker_3pos.h"
#include "cswitch_connect_earth.h"
//start
#include "cimage_yc.h"
#include "calarm_indicator_yx.h"
//end

CElement::CElement()
{
    m_byIsChoice = FALSE;
    m_byTransparency = 255;
    YkActionType = YK_NULL;
	w_blink_flag = 2;
    m_wObjectNodeNextState = POWER_OFF;
}

//CElement * CElement::CreateElement( WORD wType )
CElement * CElement::CreateElement( PELE_FST pele_fst)
{

    if( pele_fst->wElementType < CElement::GENERATRIX || pele_fst->wElementType >= CElement::END_PIC )
        return NULL ;

    CElement * pElement = NULL ;
    switch( pele_fst->wElementType )
    {
    case GENERATRIX:
        pElement = new CGeneratrix ;
        break;
    case LINKER:
        pElement = new CLinker ;
        break;
    case POWER:
        pElement = new CPower ;
        break;
    case TRANSFORMER:
        pElement = new CTransformer ;
        break;
    case CT_SIGN:
        pElement = new CCt_sign ;
        break;
    case PT_SIGN:
        pElement = new CPt_sign ;
        break;
    case BREAKER:
        pElement = new CBreaker ;
        break;
    case ANALOG_DATA_SIGN:
        pElement = new CAnalog_data_sign ;
        break;
    case TEXT_SIGN:
        pElement = new CText_sign ;
        break;
    case CIRCLE_INDICATOR_SIGN:
        pElement = new CCircle_indicator_sign ;
        break;
    case RECT_INDICATOR_SIGN:
        pElement = new CRect_indicator_sign ;
        break;
    case LIGHTNING_PROTECTOR:
        pElement = new CLightning_protector ;
        break;
    case CONTACTOR:
        pElement = new CContactor ;
        break;
    case SWITCH_2POS:
        pElement = new CSwitch_2pos ;
        break;
    case SWITCH_3POS:
        pElement = new CSwitch_3pos ;
        break;
    case LOAD_BREAKER_2POS:
        pElement = new CLoad_breaker_2pos ;
        break;
    case LOAD_BREAKER_3POS:
        pElement = new CLoad_breaker_3pos ;
        break;
    case SWITCH_CONNECT_EARTH:
        pElement = new CSwitch_connect_earth ;
        break;
		//start
	case IMAGE_FOR_AI:										//遥测定制图片
		pElement = new CImage_Yc(pele_fst->picname);
		break;
	case ALARM_FOR_AI:										//告警图元!
		pElement = new CAlarm_indicator_yx(pele_fst->picname);
		break;
		//end
    default:
        pElement = NULL ;
        break;
    }

    return pElement ;
}

WORD CElement::GetObjectType()
{
    return m_wObjectType;
}

void CElement::SetObjectType(WORD ElementType)
{
    m_wObjectType = ElementType;
}

QRect CElement::GetQRect()
{
    return m_rcDraw;
}

void CElement::TableShow(QAction *action)
{
    if( action == NULL )
        return;
    BYTE byVal = 0;
    float fVal = 0.0;

    if( action->text() == UTF8("遥信数据") )
    {
        int Yx_AttributeDataSize = Yx_ElementAttributeArray.size();

        int Yx_AttributeDataNo = 0;
        for( Yx_AttributeDataNo = 0; Yx_AttributeDataNo < Yx_AttributeDataSize; Yx_AttributeDataNo++ )
        {
            PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yx_ElementAttributeArray.at(Yx_AttributeDataNo);
            switch( tmpData->byDataType )
            {
            case PIC_ATTRIBUTE_DATA_ELEMENT::YX_DTYPE:
                byVal = 0;
                AfxGetYx(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,byVal);
                //if( tmpData->dbVal != byVal )
                {
                    tmpData->dbVal = byVal;
                    QTableWidgetItem * tmpItem = m_YxTab->item(Yx_AttributeDataNo,2);
                    QString qstrVal = QString("%1").arg(byVal);
                    tmpItem->setText(qstrVal);
                    //m_YxTab->setItem(Yx_AttributeDataNo,2,tmpItem);
                }
                break;
            default:
                break;
            }
        }

        m_YxTab->show();
    }
    else if( action->text() == UTF8("遥测数据") )
    {
        int Yc_AttributeDataSize = Yc_ElementAttributeArray.size();

        int Yc_AttributeDataNo = 0;
        for( Yc_AttributeDataNo = 0; Yc_AttributeDataNo < Yc_AttributeDataSize; Yc_AttributeDataNo++ )
        {
            PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yc_ElementAttributeArray.at(Yc_AttributeDataNo);
            switch( tmpData->byDataType )
            {
            case PIC_ATTRIBUTE_DATA_ELEMENT::YC_DTYPE:
                fVal = 0.0;
                AfxGetYc(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,fVal);
                //if( tmpData->dbVal != fVal )
                {
                    tmpData->dbVal = fVal;
                    QTableWidgetItem * tmpItem = m_YcTab->item(Yc_AttributeDataNo,2);
                    QString qstrVal = QString("%1").arg(fVal,0,'f',3);//arg(fVal);//(fVal,0,'f',3));
                    tmpItem->setText(qstrVal);
                    //m_YcTab->setItem(Yc_AttributeDataNo,2,tmpItem);
                }
                break;
            default:
                break;
            }
        }

        m_YcTab->show();
    }
    else if( action->text() == UTF8("遥脉数据") )
        m_YmTab->show();
    else if( action->text() == UTF8("遥控数据") )
        m_YkTab->show();

}


BOOL CElement::InitYkData()
{
    int AttributeDataNo = 0;
    int AttributeDataSize = Yk_ElementAttributeArray.size();
    for( AttributeDataNo = 0; AttributeDataNo < AttributeDataSize; AttributeDataNo++ )
    {
        PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yk_ElementAttributeArray.at(AttributeDataNo);
        switch( tmpData->byDataType )
        {
        case PIC_ATTRIBUTE_DATA_ELEMENT::YK_DTYPE:
            if( tmpData->byAttributeDataByte == 1 )
            {
                YkData[YKACTIONOPEN].wBusNoDest = tmpData->wBusNo;
                YkData[YKACTIONOPEN].wAddrDest = tmpData->wAddr;
                YkData[YKACTIONOPEN].wPntNoDest = tmpData->wPntNo;
                YkData[YKACTIONOPEN].wBusNoSrc = 14;
                YkData[YKACTIONOPEN].wAddrSrc = 1;
                YkData[YKACTIONOPEN].wPntNoSrc = 0;
                YkData[YKACTIONOPEN].byAction = YK_OPEN;
                YkData[YKACTIONOPEN].wVal = YK_CLOSE;
            }
            else if( tmpData->byAttributeDataByte == 2 )
            {
                YkData[YKACTIONCLOSE].wBusNoDest = tmpData->wBusNo;
                YkData[YKACTIONCLOSE].wAddrDest = tmpData->wAddr;
                YkData[YKACTIONCLOSE].wPntNoDest = tmpData->wPntNo;
                YkData[YKACTIONCLOSE].wBusNoSrc = 14;
                YkData[YKACTIONCLOSE].wAddrSrc = 1;
                YkData[YKACTIONCLOSE].wPntNoSrc = 0;
                YkData[YKACTIONCLOSE].byAction = YK_CLOSE;
                YkData[YKACTIONCLOSE].wVal = YK_CLOSE;
            }
            else if( tmpData->byAttributeDataByte == 3 )
            {
                YkData[YKACTIONOPEN].wBusNoDest = tmpData->wBusNo;
                YkData[YKACTIONOPEN].wAddrDest = tmpData->wAddr;
                YkData[YKACTIONOPEN].wPntNoDest = tmpData->wPntNo;
                YkData[YKACTIONOPEN].wBusNoSrc = 14;
                YkData[YKACTIONOPEN].wAddrSrc = 1;
                YkData[YKACTIONOPEN].wPntNoSrc = 0;
                YkData[YKACTIONOPEN].byAction = YK_OPEN;
                YkData[YKACTIONOPEN].wVal = YK_OPEN;
                YkData[YKACTIONCLOSE].wBusNoDest = tmpData->wBusNo;
                YkData[YKACTIONCLOSE].wAddrDest = tmpData->wAddr;
                YkData[YKACTIONCLOSE].wPntNoDest = tmpData->wPntNo;
                YkData[YKACTIONCLOSE].wBusNoSrc = 14;
                YkData[YKACTIONCLOSE].wAddrSrc = 1;
                YkData[YKACTIONCLOSE].wPntNoSrc = 0;
                YkData[YKACTIONCLOSE].byAction = YK_CLOSE;
                YkData[YKACTIONCLOSE].wVal = YK_CLOSE;
            }
            else if( tmpData->byAttributeDataByte == 4 )
            {
                YkData[YKEARTHACTIONOPEN].wBusNoDest = tmpData->wBusNo;
                YkData[YKEARTHACTIONOPEN].wAddrDest = tmpData->wAddr;
                YkData[YKEARTHACTIONOPEN].wPntNoDest = tmpData->wPntNo;
                YkData[YKEARTHACTIONOPEN].wBusNoSrc = 14;
                YkData[YKEARTHACTIONOPEN].wAddrSrc = 1;
                YkData[YKEARTHACTIONOPEN].wPntNoSrc = 0;
                YkData[YKEARTHACTIONOPEN].byAction = YK_OPEN;
                YkData[YKEARTHACTIONOPEN].wVal = YK_CLOSE;
            }
            else if( tmpData->byAttributeDataByte == 5 )
            {
                YkData[YKEARTHACTIONCLOSE].wBusNoDest = tmpData->wBusNo;
                YkData[YKEARTHACTIONCLOSE].wAddrDest = tmpData->wAddr;
                YkData[YKEARTHACTIONCLOSE].wPntNoDest = tmpData->wPntNo;
                YkData[YKEARTHACTIONCLOSE].wBusNoSrc = 14;
                YkData[YKEARTHACTIONCLOSE].wAddrSrc = 1;
                YkData[YKEARTHACTIONCLOSE].wPntNoSrc = 0;
                YkData[YKEARTHACTIONCLOSE].byAction = YK_CLOSE;
                YkData[YKEARTHACTIONCLOSE].wVal = YK_CLOSE;
            }
            else if( tmpData->byAttributeDataByte == 6 )
            {
                YkData[YKEARTHACTIONOPEN].wBusNoDest = tmpData->wBusNo;
                YkData[YKEARTHACTIONOPEN].wAddrDest = tmpData->wAddr;
                YkData[YKEARTHACTIONOPEN].wPntNoDest = tmpData->wPntNo;
                YkData[YKEARTHACTIONOPEN].wBusNoSrc = 14;
                YkData[YKEARTHACTIONOPEN].wAddrSrc = 1;
                YkData[YKEARTHACTIONOPEN].wPntNoSrc = 0;
                YkData[YKEARTHACTIONOPEN].byAction = YK_OPEN;
                YkData[YKEARTHACTIONOPEN].wVal = YK_OPEN;
                YkData[YKEARTHACTIONCLOSE].wBusNoDest = tmpData->wBusNo;
                YkData[YKEARTHACTIONCLOSE].wAddrDest = tmpData->wAddr;
                YkData[YKEARTHACTIONCLOSE].wPntNoDest = tmpData->wPntNo;
                YkData[YKEARTHACTIONCLOSE].wBusNoSrc = 14;
                YkData[YKEARTHACTIONCLOSE].wAddrSrc = 1;
                YkData[YKEARTHACTIONCLOSE].wPntNoSrc = 0;
                YkData[YKEARTHACTIONCLOSE].byAction = YK_CLOSE;
                YkData[YKEARTHACTIONCLOSE].wVal = YK_CLOSE;
            }
            else
                continue;
        default:
            continue;
        }
    }
    return TRUE;
}



