#include "canalog_data_sign.h"

CAnalog_data_sign::CAnalog_data_sign()
{
	m_wObjectType = GENERATRIX ;
	m_rcDraw = QRect( 0 , 0 , 0 , 0 );
	m_IsPowerOn = POWER_OFF ;
	m_IsDirection = VER_DRAW;
	QstrData.clear();
	QstrData = "0.0";
}

BOOL CAnalog_data_sign::DrawElemnt( QPainter *pPainter )
{
	if( !pPainter )
	{
		Q_ASSERT( FALSE );
		return FALSE ;
	}
	pPainter->save();
	if( m_IsPowerOn == POWER_ON )
		pPainter->setPen( Qt::red );
	else
		pPainter->setPen( Qt::green );

	pPainter->drawText(m_rcDraw,Qt::AlignVCenter,QstrData);


	pPainter->drawRect( m_rcDraw );
	pPainter->restore();
	return TRUE ;
}

BOOL CAnalog_data_sign::InitData( )
{
	return FALSE ;
}

BOOL CAnalog_data_sign::Create( QRect rc )
{
	m_rcDraw = rc ;
	return TRUE ;
}

BOOL CAnalog_data_sign::CompareAttributeData( )
{
	int AttributeDataSize = Yc_ElementAttributeArray.size();
	if( AttributeDataSize != 1 )
		return FALSE;

	PPIC_ATTRIBUTE_DATA_ELEMENT tmpData = Yc_ElementAttributeArray.at(0);
	if( tmpData->byDataType == PIC_ATTRIBUTE_DATA_ELEMENT::YC_DTYPE )
	{
		float fVal = 0;

		AfxGetYc(tmpData->wBusNo,tmpData->wAddr,tmpData->wPntNo,fVal);
		if( tmpData->dbVal != fVal )
		{
			QstrData.clear();
			QstrData = QString("%1").arg(fVal, 0, 'f', 2 );
			tmpData->dbVal = fVal;
			return TRUE;
		}
	}

	return FALSE ;
}
