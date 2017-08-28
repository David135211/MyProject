<html>
<!-Copyright(c) Go Ahead software Inc.,2012-2012.All Rights Reserved.->
<head>
<title>北京易艾斯德科技有限公司</title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
</head>
<body>
	
	
<h1 align="center"><font color=black face="楷体">北京易艾斯德科技有限公司</font></h1>
<h1 align="center"><font  size="5" face="宋体">http://www.esdtek.com/</font></h1>


<table align="center" >
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD><a href="/form.asp"><font size="4" color=red>串口服务器配置界面</font></TD>
			<TD ><font size="4" color=blue>串口服务器状态界面</font></TD>
				<TD >	<input type=button name=ok font size="4" value="刷新" onclick=history.go(-0)></TD>
    </tr>


</table>


<p>
</p>



<p>
<font size="2" face="宋体">
以下选项显示串口的硬件属性以及串口与网口的映射关系：
</font>
</p>

<table id="viewTab"align="center" border="1">
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD width="9%" bgColor=#ABCCFF height=25 name=portattr>串口属性</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>接口状态</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>接口类型</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>波特率</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>数据位</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>停止位</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>奇偶校验</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>网口发送</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>网口接收</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>串口发送</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>串口接收</TD>
    </tr>
<% getPortStatus();%>

</table>



<p>
</p>
<table id="viewTab" align="center"border="1">
		<COL><COL WIDTH=300><COL WIDTH=300>
	<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>串口和网口的关系</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>端口号</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>连接类型</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>设备工作方式</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>服务器IP地址</TD>
    

    </tr>

<% getPortNetworkStatus();%>
</table>

<p>
</p>
<font size="2" face="宋体">
备注：
1）当设备工作于"服务器"模式时，"服务器IP地址"字段设置无效。
</font>
<p>
<font size="2" face="宋体">
配置网口IP地址信息：
</font>
</p>
<table  border="1">
<COL><COL WIDTH=300><COL WIDTH=300>
	<tr>
			<TD width="30%" bgColor=#ABCCFF>网口</TD>
			<TD width="70%" bgColor=#ABCCFF>IP地址</TD>
   
    </tr>
<% getNetconfigStatus();%>


</table>



<div style=" bottom: 0px; text-align:center;"> 
本网站版权所有，北京易艾斯德科技有限公司 
</div> 

</body>
</html>