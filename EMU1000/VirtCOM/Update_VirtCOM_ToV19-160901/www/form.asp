<html>
<!-Copyright(c) Go Ahead software Inc.,2012-2012.All Rights Reserved.->
<head>
<title>�����װ�˹�¿Ƽ����޹�˾ </title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
</head>
<body>
<h1 align="center"><font color=black face="����">�����װ�˹�¿Ƽ����޹�˾</font></h1>
<h1 align="center"><font  size="5" face="����">http://www.esdtek.com/</font></h1>

<table align="center" >
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD><font size="4" color=blue>���ڷ��������ý���</font></TD>
			<TD ><a href="/status.asp"><font size="4" color=red>���ڷ�����״̬����</font></TD>
    

    </tr>


</table>


</table>

<p>
</p>

<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr><font size="4" color=blue>����汾��</font></></TD>
			
</tr>
<% ShowSoftVer();%>


</table>

<p>
<font size="2" face="����">
����ѡ�����ô��ڵ�Ӳ�������Լ����������ڵ�ӳ���ϵ��
</font>
</p>
<form action=/goform/formTest method=POST>
<table id="viewTab"align="center" border="1">
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>��������</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>�ӿ�����</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>������</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>����λ</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>ֹͣλ</TD>
			<TD width="17%" bgColor=#ABCCFF height=25>��żУ��</TD>
    

    </tr>
<% MakePortAttributeList();%>

</table>



<p>
</p>
<table id="viewTab" align="center"border="1">
		<COL><COL WIDTH=300><COL WIDTH=300>
	<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>���ں����ڵĹ�ϵ</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>�˿ں�</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>��������</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>�豸������ʽ</TD>
			<TD width="15%" bgColor=#ABCCFF height=25>������IP��ַ</TD>
    

    </tr>

<% MakePortNetworkList();%>
</table>
<p>
</p>
<font size="2" face="����">
��ע��
1�����豸������"������"ģʽʱ��"������IP��ַ"�ֶ�������Ч��
</font>
<p>
<font size="2" face="����">
��������IP��ַ��Ϣ��
</font>
</p>
<table  border="1">
<COL><COL WIDTH=300><COL WIDTH=300>
	<tr>
			<TD width="30%" bgColor=#ABCCFF>����</TD>
			<TD width="70%" bgColor=#ABCCFF>IP��ַ</TD>
   
    </tr>
<% MakeNetconfigList();%>


</table>

<p>
</p>

<tr>
			<TD width="15%" bgColor=#ABCCFF height=25 name=portattr>����״̬��</TD>
			
</tr>
<% MakeConfigStatus();%>


</table>


<table align="center" >
	<COL><COL WIDTH=300><COL WIDTH=300>
<tr>
   <td ALIGN="CENTER">
    <input type=submit name=ok value="���沢����" ></td>
    <td ALIGN="CENTER">
     <input type=button name=ok value="ȡ��" onclick=history.go(-0)></td>
</tr>
</table>





</form>


<form action=/goform/resetDev method=POST>
	<table align="right" >

<tr>
   <td ALIGN="CENTER">
    <input type=submit name=ok value="�ָ��������ò�����" ></td>
</tr>
</table>

</form>

<div style=" bottom: 0px; text-align:center;"> 
����վ��Ȩ���У������װ�˹�¿Ƽ����޹�˾ 
</div> 

</body>
</html>