<html>
<!-Copyright(c) Go Ahead software Inc.,2012-2012.All Rights Reserved.->
<head>
<title>�����װ�˹�¿Ƽ����޹�˾</title>
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
</head>
<body>
	
	
<h1 align="center"><font color=black face="����">�����װ�˹�¿Ƽ����޹�˾</font></h1>
<h1 align="center"><font  size="5" face="����">http://www.esdtek.com/</font></h1>


<table align="center" >
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD><a href="/form.asp"><font size="4" color=red>���ڷ��������ý���</font></TD>
			<TD ><font size="4" color=blue>���ڷ�����״̬����</font></TD>
				<TD >	<input type=button name=ok font size="4" value="ˢ��" onclick=history.go(-0)></TD>
    </tr>


</table>


<p>
</p>



<p>
<font size="2" face="����">
����ѡ����ʾ���ڵ�Ӳ�������Լ����������ڵ�ӳ���ϵ��
</font>
</p>

<table id="viewTab"align="center" border="1">
	<COL align="center" WIDTH=300><COL WIDTH=300><COL WIDTH=300>
   <tr>
			<TD width="9%" bgColor=#ABCCFF height=25 name=portattr>��������</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>�ӿ�״̬</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>�ӿ�����</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>������</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>����λ</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>ֹͣλ</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>��żУ��</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>���ڷ���</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>���ڽ���</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>���ڷ���</TD>
			<TD width="9%" bgColor=#ABCCFF height=25>���ڽ���</TD>
    </tr>
<% getPortStatus();%>

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

<% getPortNetworkStatus();%>
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
<% getNetconfigStatus();%>


</table>



<div style=" bottom: 0px; text-align:center;"> 
����վ��Ȩ���У������װ�˹�¿Ƽ����޹�˾ 
</div> 

</body>
</html>