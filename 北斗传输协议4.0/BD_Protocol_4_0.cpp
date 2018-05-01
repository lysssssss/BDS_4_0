#include "BD_Protocol_4_0.h"
RE_BUFFER rebuff;
BDXX bdxx;
UCHR status = 0x80;
UCHR SEND_BLOCKTIME = 0;
UCHR error_flag = 0;
int(*myprint)(_In_z_ _Printf_format_string_ char const* const, ...);
void init()
{
	//结构体初始化0 TODO
	rebuff.rp = 0;
	rebuff.wp = 0;
	myprint = &printf;
}

void check_status()
{
	if (status & STATUS_BIT_ANSWER)//有回复
	{
		switch (status & ~STATUS_BIT_ANSWER)//检查确认位
		{
		case (STEP_NONE| STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);//初始化
			break;
		case (STEP_ICJC | STATUS_BIT_CONFIRM):
			status = (STEP_XJZJ | STATUS_BIT_ANSWER);
			break;
		case (STEP_ICJC | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_XJZJ| STATUS_BIT_CONFIRM):
			status = (STEP_SJSC | STATUS_BIT_ANSWER);
			break;
		case (STEP_XJZJ | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		case (STEP_SJSC | STATUS_BIT_CONFIRM):
			status = (STEP_READY | STATUS_BIT_ANSWER);//就绪
			break;
		case (STEP_SJSC | STATUS_BIT_NO_CONFIRM):
			status = (STEP_ICJC | STATUS_BIT_ANSWER);
			break;
		default:
			char aaa[255];
			for (int i = 0; i < 22; ++i)
			{
				aaa[i] = '*';
			}
			bd_send(aaa, (UINT)(22), bdxx.icxx.yhdz);
			break;
		}
	}
	if ((status & (STATUS_BIT_STEP| STATUS_BIT_ANSWER)) == (STEP_ICJC| STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		icjc_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_XJZJ | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		xyzj_send();
	}
	else if ((status & (STATUS_BIT_STEP | STATUS_BIT_ANSWER)) == (STEP_SJSC | STATUS_BIT_ANSWER))
	{
		status &= ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
		sjsc_send();
	}
	//else if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == (STEP_READY | STATUS_BIT_ANSWER))
	//{
	//	return;
	//}
}


void icjc_send()
{
	UCHR sendbuffer[13] = "$ICJC0?0000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 12;
	sendbuffer[7] = 0;
	sendbuffer[8] = 0;
	sendbuffer[9] = 0;
	sendbuffer[10] = 0;
	sendbuffer[11] = xor_checksum2(sendbuffer, 11);
	mySerialPort.WriteData(sendbuffer, 12);
}

void xyzj_send()
{
	UCHR sendbuffer[14] = "$XTZJ0?00000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = bdxx.xtzj.lenth;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = 0;
	sendbuffer[11] = 10;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}

void sjsc_send()
{
	UCHR sendbuffer[14] = "$SJSC0?00000?";
	sendbuffer[5] = 0;
	sendbuffer[6] = 13;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = 0;
	sendbuffer[11] = 1;
	sendbuffer[12] = xor_checksum2(sendbuffer, 12);
	mySerialPort.WriteData(sendbuffer, 13);
}


char* data_encapsulation(char *send_buffer, const char *data, const UINT length_data)
{
	UINT i, length = 2 + length_data, crc = 0;
	for (i = 0; i < length_data; ++i)
		crc += data[i];
	crc += (length >> 8);
	crc += (length & 0xff);
	send_buffer[0] = 0x11;
	send_buffer[1] = (UCHR)(length >> 8);
	send_buffer[2] = (UCHR)(length & 0xff);
	send_buffer[length_data + DATA_FIRM_LENTH - 3] = (crc >> 8) & 0x0ff;
	send_buffer[length_data + DATA_FIRM_LENTH - 2] = crc & 0x0ff;
	send_buffer[length_data + DATA_FIRM_LENTH - 1] = 0x16;
	for (i = 0; i < length_data; ++i)
		send_buffer[3 + i] = data[i];
	return send_buffer;
}

void bd_send(const char *buffer, UINT len, UCHR *dis)
{
	if (SEND_BLOCKTIME&&len > 0)
	{
		char *send_buffer;
		send_buffer = (char *)malloc(sizeof(char)*(len + DATA_FIRM_LENTH));
		txsq_send(data_encapsulation(send_buffer, buffer, len), len + DATA_FIRM_LENTH, dis);
		free(send_buffer);
	}
}

void txsq_send(const char *buffer, UINT len, UCHR *dis)
{
	UINT lenth = bdxx.txsq.lenth + len, i = 0;;
	UCHR sendbuffer[255] = "$TXSQ0?00000?";//最多只能发送210字节
	sendbuffer[5] = lenth >> 8;
	sendbuffer[6] = lenth & 0xff;
	sendbuffer[7] = bdxx.icxx.yhdz[0];
	sendbuffer[8] = bdxx.icxx.yhdz[1];
	sendbuffer[9] = bdxx.icxx.yhdz[2];
	sendbuffer[10] = 0B01000110;
	sendbuffer[11] = dis[0];
	sendbuffer[12] = dis[1];
	sendbuffer[13] = dis[2];
	sendbuffer[14] = (len * 8) >> 8;
	sendbuffer[15] = (len * 8) & 0xff;
	sendbuffer[16] = 0;
	for (; i < len; ++i)
		sendbuffer[17 + i] = *(buffer + i);
	sendbuffer[lenth - 1] = xor_checksum2(sendbuffer, lenth - 1);
	mySerialPort.WriteData(sendbuffer, lenth);
}

void dwsq_send()
{
	//TODO
	//UINT lenth = bdxx.txsq.lenth + len, i = 0;;
	//UCHR sendbuffer[255] = "$TXSQ0?00000?";//最多只能发送210字节
	//sendbuffer[5] = lenth >> 8;
	//sendbuffer[6] = lenth & 0xff;
	//sendbuffer[7] = bdxx.icxx.yhdz[0];
	//sendbuffer[8] = bdxx.icxx.yhdz[1];
	//sendbuffer[9] = bdxx.icxx.yhdz[2];
	//sendbuffer[10] = 0B01000110;
	//sendbuffer[11] = dis[0];
	//sendbuffer[12] = dis[1];
	//sendbuffer[13] = dis[2];
	//sendbuffer[14] = 0;
	//sendbuffer[15] = (len * 8) >> 8;
	//sendbuffer[16] = (len * 8) & 0xff;
	//for (; i < len; ++i)
	//	sendbuffer[17 + i] = *(buffer + i);
	//sendbuffer[lenth - 1] = xor_checksum2(sendbuffer, lenth - 1);
	//mySerialPort.WriteData(sendbuffer, lenth);
}
/*
接收用
异或校验和算法
*/
UCHR xor_checksum(UCHR *buf, UINT location, UINT len)
{
	UINT i, ii;
	UCHR checksum = 0;

	for (i = 0; i < len; ++i)
	{
		ii = (location + i) & RE_BUFFER_SIZE;
		checksum ^= *(buf + ii);
	}

	return checksum;
}

/*
发送用
异或校验和算法
*/
UCHR xor_checksum2(UCHR *buf, UINT len)
{
	UINT i;
	UCHR checksum = 0;

	for (i = 0; i < len; ++i)
	{
		checksum ^= *(buf + i);
	}

	return checksum;
}

void Extract_DWXX(UCHR *buf, UINT i)
{
	bdxx.dwxx.xxlb = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[0] = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[1] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[2] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.dwxx.T = (*(buf + ((i + 14)&RE_BUFFER_SIZE)) << 24) & 0xff000000;
	bdxx.dwxx.T += (*(buf + ((i + 15)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.T += (*(buf + ((i + 16)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.T += *(buf + ((i + 17)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.L = (*(buf + ((i + 18)&RE_BUFFER_SIZE)) << 24) & 0xff000000;
	bdxx.dwxx.L += (*(buf + ((i + 19)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.L += (*(buf + ((i + 20)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.L += *(buf + ((i + 21)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.B = (*(buf + ((i + 22)&RE_BUFFER_SIZE)) << 24) & 0xff000000;
	bdxx.dwxx.B += (*(buf + ((i + 23)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.B += (*(buf + ((i + 24)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.B += *(buf + ((i + 25)&RE_BUFFER_SIZE)) & 0x000000ff;
	bdxx.dwxx.H = UCHRtoUINT(*(buf + ((i + 26)&RE_BUFFER_SIZE)), *(buf + ((i + 27)&RE_BUFFER_SIZE)));
	bdxx.dwxx.S = UCHRtoUINT(*(buf + ((i + 28)&RE_BUFFER_SIZE)), *(buf + ((i + 29)&RE_BUFFER_SIZE)));
	print_dwxx();
}
void print_dwxx()
{

}
void Extract_TXXX(UCHR *buf, UINT i)
{
	bdxx.txxx.xxlb = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[0] = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[1] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[2] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.txxx.fxsj_h = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.txxx.fxsj_m = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.txxx.dwcd = UCHRtoUINT(*(buf + ((i + 16)&RE_BUFFER_SIZE)), *(buf + ((i + 17)&RE_BUFFER_SIZE)));
	for (UINT ii = 0; ii * 8 < bdxx.txxx.dwcd; ++ii)
	{
		bdxx.txxx.dwnr[ii] = *(buf + ((i + 18 + ii)&RE_BUFFER_SIZE));
	}
	Analysis_data(bdxx.txxx.fxfdz, bdxx.txxx.fxsj_h, bdxx.txxx.fxsj_m, bdxx.txxx.dwnr, bdxx.txxx.dwcd);
	//注意有长度无内容的情况 TODO
	//注意长度不是字节整数
	if (bdxx.txxx.dwcd % 8 == 0)
		bdxx.txxx.crc = *(buf + ((i + 18 + bdxx.txxx.dwcd / 8)&RE_BUFFER_SIZE));
	else
		bdxx.txxx.crc = *(buf + ((i + 18 + bdxx.txxx.dwcd / 8 + 1)&RE_BUFFER_SIZE));
	print_txxx();


}
void print_txxx()
{
	myprint("信息类别:%0x,发信方地址:%0x,%0x,%0x,发送时间:%d:%d,电文长度:%d位,CRC标志%d\n",
		bdxx.txxx.xxlb, bdxx.txxx.fxfdz[0], bdxx.txxx.fxfdz[1], bdxx.txxx.fxfdz[2],
		bdxx.txxx.fxsj_h, bdxx.txxx.fxsj_m, bdxx.txxx.dwcd, bdxx.txxx.crc);
	myprint("电文内容:");
	for (UINT i = 0; i < bdxx.txxx.dwcd / 8; ++i)
	{
		myprint("%0x ", bdxx.txxx.dwnr[i]);
	}
	myprint("\n");
}
void Extract_ICXX(UCHR *buf, UINT i)
{
	bdxx.icxx.yhdz[0] = *(buf + ((i + 7)&RE_BUFFER_SIZE));
	bdxx.icxx.yhdz[1] = *(buf + ((i + 8)&RE_BUFFER_SIZE));
	bdxx.icxx.yhdz[2] = *(buf + ((i + 9)&RE_BUFFER_SIZE));
	bdxx.icxx.zh = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.icxx.tbid = UCHRtoUINT(*(buf + ((i + 11)&RE_BUFFER_SIZE)), UCHRtoUINT(*(buf + ((i + 12)&RE_BUFFER_SIZE)), *(buf + ((i + 13)&RE_BUFFER_SIZE))));
	bdxx.icxx.yhtz = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.icxx.fwpd = UCHRtoUINT(*(buf + ((i + 15)&RE_BUFFER_SIZE)), *(buf + ((i + 16)&RE_BUFFER_SIZE)));
	bdxx.icxx.txdj = *(buf + ((i + 17)&RE_BUFFER_SIZE));
	bdxx.icxx.jmbz = *(buf + ((i + 18)&RE_BUFFER_SIZE));
	bdxx.icxx.xsyhzs = UCHRtoUINT(*(buf + ((i + 19)&RE_BUFFER_SIZE)), *(buf + ((i + 20)&RE_BUFFER_SIZE)));
	if ((status & 0x3f) == 0x01)
		status |= 0xC0;
	print_icxx();
}
void print_icxx()
{
	printf("ic信息:用户地址:%d,帧号:%d,通播ID:%d,用户特征:%d,服务频度:%d,通信等级:%d,加密标志:%d,下属用户数:%d\n",
		((UINT)bdxx.icxx.yhdz[0])*(1 << 16) + ((UINT)bdxx.icxx.yhdz[1])*(1 << 8) + (UINT)bdxx.icxx.yhdz[2], bdxx.icxx.zh,
		bdxx.icxx.tbid, bdxx.icxx.yhtz, bdxx.icxx.fwpd, bdxx.icxx.txdj, bdxx.icxx.jmbz, bdxx.icxx.xsyhzs);
}
void Extract_ZJXX(UCHR *buf, UINT i)
{
	bool flag = false;
	bdxx.zjxx.iczt = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.zjxx.yjzt = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.zjxx.dcdl = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.zjxx.rzzt = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	for (int ii = 0; ii < 6; ++ii) {
		if (bdxx.zjxx.bsgl[ii] = *(buf + ((i + 14 + ii)&RE_BUFFER_SIZE)))
			flag = true;
	}
	print_zjxx();
	if ((status & 0x3f) == 0x02)
	{
		if (flag)
			status |= 0xC0;
		else
		{
			status &= 0B10111111;
			status |= 0x80;
		}
	}
}
void print_zjxx()
{
	myprint("系统自检:IC卡%d,硬件状态%d,电池电量%d,入站状态%s,%s\n", bdxx.zjxx.iczt, bdxx.zjxx.yjzt, bdxx.zjxx.dcdl, bdxx.zjxx.rzzt & 0x01 ? "抑制" : "非抑制", bdxx.zjxx.rzzt & 0x02 ? "非静默" : "静默");
	myprint("波束1:%d,波束2:%d,波束3:%d，波束4:%d，波束5:%d，波束6:%d\n", bdxx.zjxx.bsgl[0], bdxx.zjxx.bsgl[1], bdxx.zjxx.bsgl[2], bdxx.zjxx.bsgl[3], bdxx.zjxx.bsgl[4], bdxx.zjxx.bsgl[5]);
}
void Extract_SJXX(UCHR *buf, UINT _i)
{
	bdxx.sjxx.year = UCHRtoUINT(*(buf + ((_i + 10)&RE_BUFFER_SIZE)), *(buf + ((_i + 11)&RE_BUFFER_SIZE)));
	bdxx.sjxx.month = *(buf + ((_i + 12)&RE_BUFFER_SIZE));
	bdxx.sjxx.day = *(buf + ((_i + 13)&RE_BUFFER_SIZE));
	bdxx.sjxx.hour = *(buf + ((_i + 14)&RE_BUFFER_SIZE));
	bdxx.sjxx.minute = *(buf + ((_i + 15)&RE_BUFFER_SIZE));
	bdxx.sjxx.second = *(buf + ((_i + 16)&RE_BUFFER_SIZE));
	print_sjxx();
	if ((status & 0x3f) == 0x03)
		status |= 0xC0;
}
void print_sjxx()
{
	myprint("时间信息:%d年%d月%d日%d时%d分%d秒\n", bdxx.sjxx.year, bdxx.sjxx.month, bdxx.sjxx.day, bdxx.sjxx.hour, bdxx.sjxx.minute, bdxx.sjxx.second);
}
void Extract_FKXX(UCHR *buf, UINT i)
{

	bdxx.fkxx.flbz = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[0] = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[1] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[2] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[3] = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	print_fkxx();
	/*if ((status & 0x3f) == 0x01)  //TODO
		status |= 0xC0;*/
}
void Handle_FXXX()
{
	if (bdxx.fkxx.flbz == 4)//发射频度未到
	{
		SEND_BLOCKTIME = bdxx.fkxx.fjxx[3];
	}
}
void print_fkxx()
{
	myprint("反馈信息:");
	if (bdxx.fkxx.flbz == 0)
	{
		myprint("成功,指令:%c%c%c%c\n", bdxx.fkxx.fjxx[0], bdxx.fkxx.fjxx[1], bdxx.fkxx.fjxx[2], bdxx.fkxx.fjxx[3]);
		if (bdxx.fkxx.fjxx[0] == 'T'&&bdxx.fkxx.fjxx[1] == 'X'&&bdxx.fkxx.fjxx[2] == 'S'&&bdxx.fkxx.fjxx[3] == 'Q')
		{
			myprint("发送成功\n");
		}
	}
	else if (bdxx.fkxx.flbz == 1)
		myprint("失败,指令:%c%c%c%c\n", bdxx.fkxx.fjxx[0], bdxx.fkxx.fjxx[1], bdxx.fkxx.fjxx[2], bdxx.fkxx.fjxx[3]);
	else if (bdxx.fkxx.flbz == 2)
		myprint("信号未锁定\n");
	else if (bdxx.fkxx.flbz == 3)
		myprint("电量不足\n");
	else if (bdxx.fkxx.flbz == 4)
		myprint("发射频度未到,时间:%d秒\n", bdxx.fkxx.fjxx[3]);
	else if (bdxx.fkxx.flbz == 5)
		myprint("加解密错误\n");
	else if (bdxx.fkxx.flbz == 6)
		myprint("CRC错误,指令:%c%c%c%c\n", bdxx.fkxx.fjxx[0], bdxx.fkxx.fjxx[1], bdxx.fkxx.fjxx[2], bdxx.fkxx.fjxx[3]);
	else if (bdxx.fkxx.flbz == 7)
		myprint("用户级被抑制\n");
	else if (bdxx.fkxx.flbz == 8)
		myprint("抑制解除\n");
}

void Extract_GNTX(UCHR *buf, UINT i)
{

	bdxx.gntx.sqlx = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.gntx.year = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.gntx.month = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.gntx.day = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.gntx.hour = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.gntx.minute = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.gntx.second = *(buf + ((i + 16)&RE_BUFFER_SIZE));
	print_gntx();
	/*if ((status & 0x3f) == 0x01)  //TODO
	status |= 0xC0;*/
}

void print_gntx()
{
	myprint("GNTX:时区:%d,%d年%d月%d日%d时%d分%d秒\n", bdxx.gntx.sqlx, bdxx.gntx.year + 2000, bdxx.gntx.month, bdxx.gntx.day, bdxx.gntx.hour, bdxx.gntx.minute, bdxx.gntx.second);
}

UINT UCHRtoUINT(UCHR a, UCHR b)
{
	return ((UINT)a << 8) & 0xff00 + (UINT)b;
}

void Extract_GNPX(UCHR *buf, UINT i)
{

	bdxx.gnpx.jdfw = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.gnpx.jd = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.gnpx.jf = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.gnpx.jm = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.gnpx.jxm = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.gnpx.wdfw = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.gnpx.wd = *(buf + ((i + 16)&RE_BUFFER_SIZE));
	bdxx.gnpx.wf = *(buf + ((i + 17)&RE_BUFFER_SIZE));
	bdxx.gnpx.wm = *(buf + ((i + 18)&RE_BUFFER_SIZE));
	bdxx.gnpx.wxm = *(buf + ((i + 19)&RE_BUFFER_SIZE));
	bdxx.gnpx.gd = UCHRtoUINT(*(buf + ((i + 20)&RE_BUFFER_SIZE)), *(buf + ((i + 21)&RE_BUFFER_SIZE)));
	bdxx.gnpx.sd = UCHRtoUINT(*(buf + ((i + 22)&RE_BUFFER_SIZE)), *(buf + ((i + 23)&RE_BUFFER_SIZE)));
	bdxx.gnpx.fx = UCHRtoUINT(*(buf + ((i + 24)&RE_BUFFER_SIZE)), *(buf + ((i + 25)&RE_BUFFER_SIZE)));
	bdxx.gnpx.wxs = *(buf + ((i + 26)&RE_BUFFER_SIZE));
	bdxx.gnpx.zt = *(buf + ((i + 27)&RE_BUFFER_SIZE));
	bdxx.gnpx.jdxs = *(buf + ((i + 28)&RE_BUFFER_SIZE));
	bdxx.gnpx.gjwc = UCHRtoUINT(*(buf + ((i + 29)&RE_BUFFER_SIZE)), *(buf + ((i + 30)&RE_BUFFER_SIZE)));
	print_gnpx();
	/*if ((status & 0x3f) == 0x01)  //TODO
	status |= 0xC0;*/
}

void print_gnpx()
{
	myprint("GNPX:经度范围:%d,经度%d,经分%d,经秒%d,经小秒%d,纬度范围%d,纬度%d,纬分%d,纬秒%d,纬小秒%d,高度:%d\
			,速度%d,方向%d,卫星数%d,状态%d,精度系数%d,估计误差:%d", bdxx.gnpx.jdfw, bdxx.gnpx.jd, bdxx.gnpx.jf, bdxx.gnpx.jm, bdxx.gnpx.jxm,
		bdxx.gnpx.wdfw, bdxx.gnpx.wd, bdxx.gnpx.wf, bdxx.gnpx.wm, bdxx.gnpx.wxm, bdxx.gnpx.gd, bdxx.gnpx.sd, bdxx.gnpx.fx, bdxx.gnpx.wxs,
		bdxx.gnpx.zt, bdxx.gnpx.jdxs, bdxx.gnpx.gjwc);
}

void Extract_GNVX(UCHR *buf, UINT i)
{

	bdxx.gnvx.wxlb = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.gnvx.wxgs = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	for (UINT _i = 0; _i < bdxx.gnvx.wxgs; ++_i)
	{
		UINT j = i + 12 + _i * 5;
		bdxx.gnvx.wxxx[_i].wxbh = *(buf + (j &RE_BUFFER_SIZE));
		bdxx.gnvx.wxxx[_i].wxyj = *(buf + ((j + 1) &RE_BUFFER_SIZE));
		bdxx.gnvx.wxxx[_i].fwj = UCHRtoUINT(*(buf + ((j + 1) &RE_BUFFER_SIZE)), *(buf + ((j + 2) &RE_BUFFER_SIZE)));
		bdxx.gnvx.wxxx[_i].xzb = *(buf + ((j + 3) &RE_BUFFER_SIZE));
	}
	print_gnvx();
	/*if ((status & 0x3f) == 0x01)  //TODO
	status |= 0xC0;*/
}

void print_gnvx()
{
	myprint("GNVX:卫星类别:%s,卫星个数:%d\n", bdxx.gnvx.wxlb == 1 ? "GPS卫星" : "BDS卫星", bdxx.gnvx.wxgs);
}

void Receive_Protocol()
{
	UINT i = rebuff.rp;
	static UINT error_count = 0;
	if (rebuff.rp != rebuff.wp)
	{
		if (error_count >= 10)
		{
			//读取错误 TODO
		}
		if (check_overflow(&rebuff, 7))//检查是否符合读取条件，7为读取到长度的字节数。&XXXXMM
		{
			if (rebuff.buffer[i] == '$')
			{

				if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'T'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNTX
				{
					UINT _lenth = 0;
#ifdef GNTX_LENTH
					_lenth = GNTX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNTX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
			}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'P'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNPX
				{
					UINT _lenth = 0;
#ifdef GNPX_LENTH
					_lenth = GNPX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNPX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
		}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'N'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'V'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//GNVX
				{
					UINT _lenth = 0;
#ifdef GNVX_LENTH
					_lenth = GNVX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{

						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_GNVX(rebuff.buffer, i);
						}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'D'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'W'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//定位信息
				{
					UINT _lenth = 0;
#ifdef DWXX_LENTH
					_lenth = DWXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_DWXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}

	}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'T'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//通信信息
				{
					UINT _lenth = 0;
#ifdef TXXX_LENTH
					_lenth = TXXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_TXXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
					}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'I'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'C'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//IC信息
				{
					UINT _lenth = 0;
#ifdef ICXX_LENTH
					_lenth = ICXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_ICXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'Z'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'J'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//自检信息
				{
					UINT _lenth = 0;
#ifdef ZJXX_LENTH
					_lenth = ZJXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_ZJXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'S'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'J'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//时间信息
				{
					UINT _lenth = 0;
#ifdef SJXX_LENTH
					_lenth = SJXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_SJXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'B'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'B'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//版本信息
				{

				}
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'F'
					&&rebuff.buffer[(i + 2) & RE_BUFFER_SIZE] == 'K'
					&&rebuff.buffer[(i + 3) & RE_BUFFER_SIZE] == 'X'
					&&rebuff.buffer[(i + 4) & RE_BUFFER_SIZE] == 'X')//反馈信息
				{
					UINT _lenth = 0;
#ifdef FKXX_LENTH
					_lenth = FKXX_LENTH;
#else
					_lenth = (UINT)rebuff.buffer[(i + 5) & RE_BUFFER_SIZE] * 256 + (UINT)rebuff.buffer[(i + 6) & RE_BUFFER_SIZE];
#endif
					if (check_overflow(&rebuff, _lenth))
					{
						if (rebuff.buffer[(rebuff.rp + _lenth - 1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth - 1))
						{
							Extract_FKXX(rebuff.buffer, i);
						}

						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
				}
					else
					{
						++error_count;
						return;
					}
				}
				else
				{
					//不可解析
					rebuff.rp = (rebuff.rp + 1) & RE_BUFFER_SIZE;
				}
	}
			else
			{
				rebuff.rp = (rebuff.rp + 1) & RE_BUFFER_SIZE;
			}
}
		else
		{
			++error_count;
		}
	}
}

bool check_overflow(RE_BUFFER *buff, UINT value)
{
	if (buff->rp <= buff->wp)//说明写指针未回溯
	{
		if (buff->rp + value <= buff->wp)
			return true;
		else
			return false;
	}
	else
	{
		if (buff->rp + value <= (buff->wp + RE_BUFFER_SIZE))
			return true;
		else
			return false;
	}
}

void Analysis_data(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *buffer, const UINT package_length)
{
	UINT crc = 0, crc1 = 0;
	UINT length, length1;

	if (buffer[package_length - 1] != 0x16)
	{
		return;
	}
	else if (buffer[0] == 0x11)
	{
		length1 = length = ((UINT)buffer[1] << 8) + (UINT)buffer[2];
		crc = ((((UINT)buffer[length + 1]) << 8) & 0xff00) + (buffer[length + 2] & 0x00ff);
		if (length1 < package_length)
		{
			for (; length1 > 0; length1--)
			{
				crc1 += buffer[length1];
			}
			if (crc == crc1)
			{

				UCHR *exdata = (UCHR *)malloc(sizeof(UCHR)*(length - 2));
				memcpy(exdata, buffer + 3, length - 2);
				DATA_Handler(fxfdz, h, m, exdata, length - 2);
				free(exdata);
			}

		}
	}
}

void DATA_Handler(const UCHR *fxfdz, const UCHR h, const UCHR m, const UCHR *data, const UINT lenth)
{
	//TODO

}

