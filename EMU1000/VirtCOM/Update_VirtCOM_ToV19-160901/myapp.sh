echo  Now Execute Script in myapp/myapp.sh
echo  Setting IP address
insmod /myapp/gpio_drv.ko
insmod /myapp/SerialExt.ko
insmod /myapp/dm9ka.ko
insmod /myapp/ID.ko

sleep 1

/myapp/SetMac

ifconfig eth0 192.168.1.211 netmask 255.255.255.0
ifconfig eth1 192.168.2.211 netmask 255.255.255.0

ifconfig eth2 192.168.3.211 netmask 255.255.255.0
ifconfig eth3 192.168.4.211 netmask 255.255.255.0

sleep 1
chmod +x /myapp/ttyrun
/myapp/user.sh
