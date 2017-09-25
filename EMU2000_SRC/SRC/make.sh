#!/bin/bash
# 在当前文件夹下，输入./make.sh d为编入debug的程序，其它为release的程序.

if [ "$1"x = "d"x ];
then
	echo "make debug"
	sleep 2s
	make distclean;
	make;
else
	echo "make release"
	sleep 2s
	make distclean ver=release;
	make ver=release;
fi
