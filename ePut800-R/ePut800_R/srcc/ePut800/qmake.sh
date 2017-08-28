# !/bin/sh
make distclean
echo "./eput800.sh [type] type = cd/dd/cr(cross debug/desk debug/cross releas), other default is cr"

if [ $# -ne 1 ];
then
	echo "param is default"
	exit
# else
	# echo $1
fi

if [ "$1"x = "cd"x ];
then
	echo "cross debug project"
	QMAKE=/opt/qt-everywhere-opensource-src-4.7.3/bin/qmake
	CONFIG="CONFIG+=debug warn_on"
elif [ "$1"x = "dd"x ];
then
	echo "desk debug project"
	QMAKE=/home/mengqp/app/qtsdk-2010.05/qt/bin/qmake
	CONFIG="CONFIG+=debug warn_on"
elif [ "$1"x = "cr"x ];
then
	echo "cross release project"
	QMAKE=/opt/qt-everywhere-opensource-src-4.7.3/bin/qmake
	CONFIG="CONFIG+=release warn_on"
else
	echo "cross release project"
	QMAKE=/opt/qt-everywhere-opensource-src-4.7.3/bin/qmake
	CONFIG="CONFIG+=release warn_on"
fi

$QMAKE -project -Wall  -o  ./eput800.pro -after "TARGET=ePut800" $CONFIG "OBJECTS_DIR = ./obj/" "LIBS+=-ldl" "MOC_DIR=./moc/" "LIBS+=Dbase/libsqlite3.so.0 -ldl" "QT += sql"
$QMAKE -makefile ./eput800.pro   -o ./Makefile

make

