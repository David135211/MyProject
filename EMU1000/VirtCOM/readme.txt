boa文件拷贝位置，参见下面命令：
mkdir /etc/boa
mkdir /www
mkdir /www/cgi-bin

cp /mnt/nfs/mime.types /etc/
cp /mnt/nfs/boa /etc/boa/
cp /mnt/nfs/boa.conf  /etc/boa/
cp /mnt/nfs/html/* /www/
cp /mnt/nfs/test.cgi /www/cgi-bin/

/etc/boa/boa


CGI端的C代码程序为cgi目录下的文件，最后产生test.cgi
Web端的对应C代码文件，合并到VirCOM中了。文件为：webserver.c，配置文件为：webconfig.h
