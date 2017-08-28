rm cscope*
find `pwd` -name '*.cpp' -o -name '*.c' -o -name '*.h' > cscope.files
cscope -Rbq
ctags -R
