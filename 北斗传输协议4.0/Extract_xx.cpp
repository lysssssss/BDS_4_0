#include "BD_Protocol_4_0.h"

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
	Analysis_data(bdxx.txxx.fxfdz, bdxx.txxx.fxsj_h, bdxx.txxx.fxsj_m, bdxx.txxx.dwnr, bdxx.txxx.dwcd/8);
	//注意有长度无内容的情况 TODO
	//注意长度不是字节整数
	if (bdxx.txxx.dwcd % 8 == 0)
		bdxx.txxx.crc = *(buf + ((i + 18 + bdxx.txxx.dwcd / 8)&RE_BUFFER_SIZE));
	else
		bdxx.txxx.crc = *(buf + ((i + 18 + bdxx.txxx.dwcd / 8 + 1)&RE_BUFFER_SIZE));
	print_txxx();


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
	if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_ICJC)
		status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
	print_icxx();
}

void Extract_ZJXX(UCHR *buf, UINT i)
{
	bdxx.zjxx.iczt = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.zjxx.yjzt = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.zjxx.dcdl = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.zjxx.rzzt = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	for (int ii = 0; ii < 6; ++ii) {
		bdxx.zjxx.bsgl[ii] = *(buf + ((i + 14 + ii)&RE_BUFFER_SIZE));
	}
	Handle_ZJXX();
	print_zjxx();
	if ((status &  ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_XJZJ)
	{
			status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);

	}
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
	if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_SJSC)
		status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
}

void Extract_FKXX(UCHR *buf, UINT i)
{

	bdxx.fkxx.flbz = *(buf + ((i + 10)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[0] = *(buf + ((i + 11)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[1] = *(buf + ((i + 12)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[2] = *(buf + ((i + 13)&RE_BUFFER_SIZE));
	bdxx.fkxx.fjxx[3] = *(buf + ((i + 14)&RE_BUFFER_SIZE));
	Handle_FXXX();
	print_fkxx();
	/*if ((status & 0x3f) == 0x01)  //TODO
	status |= 0xC0;*/
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
	if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_GNTS)
		status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
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
	bdxx.gnpx.gd = UCHRtoINT(*(buf + ((i + 20)&RE_BUFFER_SIZE)), *(buf + ((i + 21)&RE_BUFFER_SIZE)));
	bdxx.gnpx.sd = UCHRtoUINT(*(buf + ((i + 22)&RE_BUFFER_SIZE)), *(buf + ((i + 23)&RE_BUFFER_SIZE)));
	bdxx.gnpx.fx = UCHRtoUINT(*(buf + ((i + 24)&RE_BUFFER_SIZE)), *(buf + ((i + 25)&RE_BUFFER_SIZE)));
	bdxx.gnpx.wxs = *(buf + ((i + 26)&RE_BUFFER_SIZE));
	bdxx.gnpx.zt = *(buf + ((i + 27)&RE_BUFFER_SIZE));
	bdxx.gnpx.jdxs = *(buf + ((i + 28)&RE_BUFFER_SIZE));
	bdxx.gnpx.gjwc = UCHRtoUINT(*(buf + ((i + 29)&RE_BUFFER_SIZE)), *(buf + ((i + 30)&RE_BUFFER_SIZE)));
	print_gnpx();
	if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_GNPS)
		status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
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
	if ((status & ~(STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM)) == STEP_GNVS)
		status |= (STATUS_BIT_ANSWER | STATUS_BIT_CONFIRM);
}