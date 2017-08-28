#include <QtGui/QApplication>
#include "widget.h"
#include <QTextCodec>
#include "cglobal.h"

#define MAX_LENTH 600

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget theApp;

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    QFont font;
    font.setFamily(("FZZhunYuan-M02"));
    a.setFont(font);



    int cxSize = Widget::HOR_SIZE ;
    int cySize = Widget::VER_SIZE ;
    theApp.resize( QSize( cxSize , cySize ));
    //theApp.showFullScreen();



    AfxSetMainWidet( &theApp );
    AfxSetCurPageIndex( Widget::PAGE_HOME );
//    theApp.show( );
    theApp.showFullScreen();

    //if Initialization is failed app exit
    if( !theApp.OnInitialize() )
         exit( 0 );
//start
		time_t timeflag = 0;
		timeflag = time(NULL);
		if((timeflag % 10) == 0)
			AfxGetWidget()->repaint();
//end

    return a.exec();
}
