boa�ļ�����λ�ã��μ��������
mkdir /etc/boa
mkdir /www
mkdir /www/cgi-bin

cp /mnt/nfs/mime.types /etc/
cp /mnt/nfs/boa /etc/boa/
cp /mnt/nfs/boa.conf  /etc/boa/
cp /mnt/nfs/html/* /www/
cp /mnt/nfs/test.cgi /www/cgi-bin/

/etc/boa/boa


CGI�˵�C�������ΪcgiĿ¼�µ��ļ���������test.cgi
Web�˵Ķ�ӦC�����ļ����ϲ���VirCOM���ˡ��ļ�Ϊ��webserver.c�������ļ�Ϊ��webconfig.h
