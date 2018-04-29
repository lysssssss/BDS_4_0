#include "BD_Protocol_4_0.h"
RE_BUFFER rebuff;
BDXX bdxx;
UCHR status = 0x80;
int(*myprint)(_In_z_ _Printf_format_string_ char const* const, ...);
void init()
{
	//结构体初始化0
	rebuff.rp = 0;
	rebuff.wp = 0;
	myprint = &printf;
}

void check_status()//第八位为回复位，第七位为确认位
{
	if (status & 0x80)//有回复
	{
		switch (status & 0x7f)//检查确认位
		{
		case 0x00:
			status = 0x81;//初始化
			break;
		case 0x41:
			status = 0x82;
			break;
		case 0x01:
			status = 0x81;
			break;
		case 0x42:
			status = 0x83;
			break;
		case 0x02:
			status = 0x81;
			break;
		case 0x43:
			status = 0x84;//就绪
			//txsq_send("hello",5,bdxx.icxx.yhdz);
			break;
		case 0x03:
			status = 0x81;
			break;
		default:
			char aaa[2048];
			for (int i = 0; i < 22; ++i)
			{
				aaa[i] = '*';
			}
			txsq_send(aaa, (UINT)(22), bdxx.icxx.yhdz);
			break;
		}
	}
	if ((status & 0xdf) == 0x81)
	{
		status &= 0x3f;
		icjc_send();
	}
	else if ((status & 0xdf) == 0x82)
	{
		status &= 0x3f;
		xyzj_send();
	}
	else if ((status & 0xdf) == 0x83)
	{
		status &= 0x3f;
		sjsc_send();
	}
	else if ((status & 0xdf) == 0x84)
	{
		return;
	}
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
	bdxx.dwxx.H = (*(buf + ((i + 26)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.H += *(buf + ((i + 27)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.S = (*(buf + ((i + 28)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.S += *(buf + ((i + 29)&RE_BUFFER_SIZE)) & 0xff;
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
	bdxx.txxx.dwcd = (*(buf + ((i + 16)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.txxx.dwcd += *(buf + ((i + 17)&RE_BUFFER_SIZE)) & 0xff;
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
	for (int i = 0; i < bdxx.txxx.dwcd / 8; ++i)
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
	bdxx.icxx.tbid = (UINT)(*(buf + ((i + 11)&RE_BUFFER_SIZE)))*(1 << 16);
	bdxx.icxx.tbid += (UINT)(*(buf + ((i + 12)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.tbid += (UINT)(*(buf + ((i + 13)&RE_BUFFER_SIZE)));
	bdxx.icxx.yhtz = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.icxx.fwpd = (UINT)(*(buf + ((i + 15)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.fwpd += (UINT)(*(buf + ((i + 16)&RE_BUFFER_SIZE)));
	bdxx.icxx.txdj = *(buf + ((i + 17)&RE_BUFFER_SIZE));
	bdxx.icxx.jmbz = *(buf + ((i + 18)&RE_BUFFER_SIZE));
	bdxx.icxx.xsyhzs = (UINT)(*(buf + ((i + 19)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.xsyhzs += (UINT)(*(buf + ((i + 20)&RE_BUFFER_SIZE)));
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
	bdxx.sjxx.year = (UINT)(*(buf + ((_i + 10)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.sjxx.year += (UINT)*(buf + ((_i + 11)&RE_BUFFER_SIZE));
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
		myprint("发射频度未到,时间:%d\n", bdxx.fkxx.fjxx[3]);
	else if (bdxx.fkxx.flbz == 5)
		myprint("加解密错误\n");
	else if (bdxx.fkxx.flbz == 6)
		myprint("CRC错误,指令:%c%c%c%c\n", bdxx.fkxx.fjxx[0], bdxx.fkxx.fjxx[1], bdxx.fkxx.fjxx[2], bdxx.fkxx.fjxx[3]);
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

						//if (rebuff.buffer[(rebuff.rp + _lenth-1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth-1))
						//{
						//	Extract_FKXX(rebuff.buffer, i);
						//}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
					}
					else
					{
						++error_count;
						return;
					}
					//myprint("未知包1");
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

						//if (rebuff.buffer[(rebuff.rp + _lenth-1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth-1))
						//{
						//	Extract_FKXX(rebuff.buffer, i);
						//}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
					}
					else
					{
						++error_count;
						return;
					}
					//myprint("未知包2");
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

						//if (rebuff.buffer[(rebuff.rp + _lenth-1) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth-1))
						//{
						//	Extract_FKXX(rebuff.buffer, i);
						//}
						rebuff.rp = (rebuff.rp + _lenth)& RE_BUFFER_SIZE;
					}
					else
					{
						++error_count;
						return;
					}
					//myprint("未知包3");
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
