/***********************Project Version1.0*************************
@项目名:北斗传输4.0(C++)
@File:BD_Protocol_4_0.cpp
@File_Version:1.0a
@Author:lys
@QQ:591986780
@UpdateTime:2018年5月16日02:39:46

@说明:打印LOG

本程序基于C++的北斗短报文传输程序
在VS2017里运行通过

Viual Studio 2017 Version:V141
Windows SDK Version:10.0.16299.0
******************************************************************/

#include "BD_Protocol_4_0.h"

void print_dwxx()
{

}

void print_txxx()
{
	myprint("通讯信息:信息类别:%0x,发信方地址:%0x,%0x,%0x,发送时间:%d:%d,电文长度:%d位,CRC标志%d\n",
		bdxx.txxx.xxlb, bdxx.txxx.fxfdz[0], bdxx.txxx.fxfdz[1], bdxx.txxx.fxfdz[2],
		bdxx.txxx.fxsj_h, bdxx.txxx.fxsj_m, bdxx.txxx.dwcd, bdxx.txxx.crc);
	myprint("电文内容:");
	for (UINT i = 0; i < bdxx.txxx.dwcd / 8; ++i)
	{
		myprint("%0x ", bdxx.txxx.dwnr[i]);
	}
	myprint("\n");
}

void print_icxx()
{
	printf("ic信息:用户地址:%d,帧号:%d,通播ID:%d,用户特征:%d,服务频度:%d,通信等级:%d,加密标志:%d,下属用户数:%d\n",
		((UINT)bdxx.icxx.yhdz[0])*(1 << 16) + ((UINT)bdxx.icxx.yhdz[1])*(1 << 8) + (UINT)bdxx.icxx.yhdz[2], bdxx.icxx.zh,
		bdxx.icxx.tbid, bdxx.icxx.yhtz, bdxx.icxx.fwpd, bdxx.icxx.txdj, bdxx.icxx.jmbz, bdxx.icxx.xsyhzs);
}

void print_zjxx()
{
	myprint("系统自检:IC卡%d,硬件状态%d,电池电量%d,入站状态%s,%s\n", bdxx.zjxx.iczt, bdxx.zjxx.yjzt, bdxx.zjxx.dcdl, bdxx.zjxx.rzzt & 0x01 ? "抑制" : "非抑制", bdxx.zjxx.rzzt & 0x02 ? "非静默" : "静默");
	myprint("波束1:%d,波束2:%d,波束3:%d，波束4:%d，波束5:%d，波束6:%d\n", bdxx.zjxx.bsgl[0], bdxx.zjxx.bsgl[1], bdxx.zjxx.bsgl[2], bdxx.zjxx.bsgl[3], bdxx.zjxx.bsgl[4], bdxx.zjxx.bsgl[5]);
}

void print_sjxx()
{
	myprint("时间信息:%d年%d月%d日%d时%d分%d秒\n", bdxx.sjxx.year, bdxx.sjxx.month, bdxx.sjxx.day, bdxx.sjxx.hour, bdxx.sjxx.minute, bdxx.sjxx.second);
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


void print_gntx()
{
	myprint("GNTX:时区:%d,%d年%d月%d日%d时%d分%d秒\n", bdxx.gntx.sqlx, bdxx.gntx.year + 2000, bdxx.gntx.month, bdxx.gntx.day, bdxx.gntx.hour, bdxx.gntx.minute, bdxx.gntx.second);
}

void print_gnpx()
{
	myprint("GNPX:经度范围:%c,经度%d,经分%d,经秒%d,经小秒%d,纬度范围%c,纬度%d,纬分%d,纬秒%d,纬小秒%d,高度:%d\
,速度%d,方向%d,卫星数%d,状态%d,精度系数%d,估计误差:%d\n", bdxx.gnpx.jdfw, bdxx.gnpx.jd, bdxx.gnpx.jf, bdxx.gnpx.jm, bdxx.gnpx.jxm,
bdxx.gnpx.wdfw, bdxx.gnpx.wd, bdxx.gnpx.wf, bdxx.gnpx.wm, bdxx.gnpx.wxm, bdxx.gnpx.gd, bdxx.gnpx.sd, bdxx.gnpx.fx, bdxx.gnpx.wxs,
bdxx.gnpx.zt, bdxx.gnpx.jdxs, bdxx.gnpx.gjwc);
}
void print_gnvx()
{
	myprint("GNVX:卫星类别:%s,卫星个数:%d\n", bdxx.gnvx.wxlb == 1 ? "GPS卫星" : "BDS卫星", bdxx.gnvx.wxgs);
}

