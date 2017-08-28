<html>
<!-Copyright(c) Go Ahead software Inc.,2012-2012.All Rights Reserved.->
<head>
<title>北京易艾斯德科技有限公司 </title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
</head>
<body>
<h1 align="center"><font color=black face="楷体">北京易艾斯德科技有限公司</font></h1>
<h1 align="center"><font  size="5" face="宋体">http://www.esdtek.com/</font></h1>

<table align="center" >
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD><font size="4" color=blue>串口服务器配置界面</font></TD>
			<TD ><a href="/status.asp"><font size="4" color=red>串口服务器状态界面</font></TD>
    

    </tr>


</table>


</table>

<p>
</p>

<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr><font size="4" color=blue>软件版本：</font></></TD>
			
</tr>
<% ShowSoftVer();%>


</table>

<p>
<font size="2" face="宋体">
以下选项设置串口的硬件属性以及串口与网口的映射关系：
</font>
</p>
<form action=/goform/formTest method=POST>
<table id="viewTab"align="center" border="1">
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>串口属性</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>接口类型</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>波特率</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>数据位</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>停止位</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>奇偶校验</TD>
    

    </tr>
<% MakePortAttributeList();%>

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

<% MakePortNetworkList();%>
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
<% MakeNetconfigList();%>


</table>

<p>
</p>

<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>错误状态：</TD>
			
</tr>
<% MakeConfigStatus();%>


</table>


<table align="center" >
	<COL><COL WIDTH=300><COL WIDTH=300>
<tr>
   <td ALIGN="CENTER">
    <input type=submit name=ok value="保存并重启" ></td>
    <td ALIGN="CENTER">
     <input type=button name=ok value="取消" onclick=history.go(-0)></td>
</tr>
</table>





</form>


<form action=/goform/resetDev method=POST>
	<table align="right" >

<tr>
   <td ALIGN="CENTER">
    <input type=submit name=ok value="恢复出厂设置并重启" ></td>
</tr>
</table>

</form>

<div style=" bottom: 0px; text-align:center;"> 
本网站版权所有，北京易艾斯德科技有限公司 
</div> 

</body>
</html>