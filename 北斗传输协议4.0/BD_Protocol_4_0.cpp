#include "BD_Protocol_4_0.h"
RE_BUFFER rebuff;
BDXX bdxx;
void init()
{
	rebuff.rp = 0;
	rebuff.wp = 0;
}

/*
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
void Extract_DWXX(UCHR *buf,UINT i)
{
	bdxx.dwxx.xxlb = *(buf + ((i+11)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[0] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[1] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.dwxx.cxdz[2] = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.dwxx.T = (*(buf + ((i + 15)&RE_BUFFER_SIZE))<<24)&0xff000000;
	bdxx.dwxx.T += (*(buf + ((i + 16)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.T += (*(buf + ((i + 17)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.T += *(buf + ((i + 18)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.L = (*(buf + ((i + 19)&RE_BUFFER_SIZE)) << 24) & 0xff000000;
	bdxx.dwxx.L += (*(buf + ((i + 20)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.L+= (*(buf + ((i + 21)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.L += *(buf + ((i + 22)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.B = (*(buf + ((i + 23)&RE_BUFFER_SIZE)) << 24) & 0xff000000;
	bdxx.dwxx.B += (*(buf + ((i + 24)&RE_BUFFER_SIZE)) << 16) & 0xff0000;
	bdxx.dwxx.B += (*(buf + ((i + 25)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.B += *(buf + ((i + 26)&RE_BUFFER_SIZE)) & 0x000000ff;
	bdxx.dwxx.H = (*(buf + ((i + 27)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.H += *(buf + ((i + 28)&RE_BUFFER_SIZE)) & 0xff;
	bdxx.dwxx.S = (*(buf + ((i + 29)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.dwxx.S += *(buf + ((i + 30)&RE_BUFFER_SIZE)) & 0xff;
}

void Extract_TXXX(UCHR *buf, UINT i)
{
	bdxx.txxx.xxlb = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[0] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[1] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.txxx.fxfdz[2] = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.txxx.fxsj_h = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.txxx.fxsj_m = *(buf + ((i + 16)&RE_BUFFER_SIZE));
	bdxx.txxx.dwcd = (*(buf + ((i + 17)&RE_BUFFER_SIZE)) << 8) & 0xff00;
	bdxx.txxx.dwcd += *(buf + ((i + 18)&RE_BUFFER_SIZE)) & 0xff;
	for (UINT ii = 0; ii < bdxx.txxx.dwcd; ++ii)
	{
		bdxx.txxx.dwnr[ii] = *(buf + ((i + 19 + ii)&RE_BUFFER_SIZE));
	}
	//注意有长度无内容的情况 TODO
	bdxx.txxx.crc= *(buf + ((i + 19+ bdxx.txxx.dwcd)&RE_BUFFER_SIZE));


}

void Extract_ICXX(UCHR *buf, UINT i)
{
	bdxx.icxx.zh = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.icxx.tbid = (UINT)(*(buf + ((i + 12)&RE_BUFFER_SIZE)))*(1<<16);
	bdxx.icxx.tbid += (UINT)(*(buf + ((i + 13)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.tbid += (UINT)(*(buf + ((i + 14)&RE_BUFFER_SIZE)));
	bdxx.icxx.yhtz = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.icxx.fwpd = (UINT)(*(buf + ((i + 16)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.fwpd += (UINT)(*(buf + ((i + 17)&RE_BUFFER_SIZE)));
	bdxx.icxx.txdj = *(buf + ((i + 18)&RE_BUFFER_SIZE));
	bdxx.icxx.jmbz = *(buf + ((i + 19)&RE_BUFFER_SIZE));
	bdxx.icxx.xsyhzs = (UINT)(*(buf + ((i + 20)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.icxx.xsyhzs += (UINT)(*(buf + ((i + 21)&RE_BUFFER_SIZE)));
}

void Extract_ZJXX(UCHR *buf, UINT i)
{
	bdxx.zjxx.iczt = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.zjxx.yjzt = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.zjxx.dcdl = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.zjxx.rzzt = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	for (int ii = 0; ii < 6; ++ii) {
		bdxx.zjxx.bsgl[ii] = *(buf + ((i + 15 + ii)&RE_BUFFER_SIZE));
	}
}

void Extract_SJXX(UCHR *buf, UINT i)
{
	bdxx.sjxx.year = (UINT)(*(buf + ((i + 11)&RE_BUFFER_SIZE)))*(1<<8);
	bdxx.sjxx.year += (UINT)*(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.sjxx.month = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.sjxx.day = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	bdxx.sjxx.hour = *(buf + ((i + 15)&RE_BUFFER_SIZE));
	bdxx.sjxx.minute = *(buf + ((i + 16)&RE_BUFFER_SIZE));
	bdxx.sjxx.second = *(buf + ((i + 17)&RE_BUFFER_SIZE));
}

void Extract_FKXX(UCHR *buf, UINT i)
{

	bdxx.fkxx.flbz = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx = (UINT)(*(buf + ((i + 12)&RE_BUFFER_SIZE)))*(1 << 16);
	bdxx.fkxx.fjxx += (UINT)(*(buf + ((i + 13)&RE_BUFFER_SIZE)))*(1 << 8);
	bdxx.fkxx.fjxx += (UINT)(*(buf + ((i + 14)&RE_BUFFER_SIZE)));
}

void Receive_Protocol()
{
	UINT i = rebuff.rp;
	static UINT error_count=0;
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
				if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'D'
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
						if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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
				else if (rebuff.buffer[(i + 1) & RE_BUFFER_SIZE] == 'G'
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

						//if (rebuff.buffer[(rebuff.rp + _lenth) & RE_BUFFER_SIZE] == xor_checksum(rebuff.buffer, i, _lenth))
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

bool check_overflow( RE_BUFFER *buff, UINT value)
{
	if (buff->rp <=buff->wp)//说明写指针未回溯
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