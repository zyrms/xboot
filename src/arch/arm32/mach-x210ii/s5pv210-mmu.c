/*
 * s5pv210-mmu.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <s5pv210-cp15.h>

////
// MMU/Cache
//
#define DESC_SEC        (0x2|(0<<4))

#define STSH				(0<<12)|(0<<2)		//Strongly-ordered, Shareable in C-A8. <> cache_off,WR_BUF off in ARM9
#define DESH				(0<<12)|(1<<2)		//Device, Shareable in C-A8 <> cache_off,WR_BUF on in ARM9
#define CNB				(0<<12)|(2<<2)		//Outer and Inner Write-Through, no Write-Allocate in C-A8 <> cache_on, write_through in ARM9
#define CB				(0<<12)|(3<<2)		//Outer and Inner Write-Back, no Write-Allocate in C-A8 <> cache_on, write_back
#define NCNB				(0<<12)|(0<<2)		//Outer and Inner Non-Cacheable
#define NCB				(1<<12)|(1<<2)		//Reserved
#define IMDF				(1<<12)|(2<<2)		//Implementation defined
#define CBWA			(1<<12)|(3<<2)		//Outer and Inner Write-Back, Write-Allocate
#define NSHD				(2<<12)|(0<<2)		//Non-shareable Device
#define RES2				(2<<12)|(1<<2)		//Reserved
#define RES3				(2<<12)|(2<<2)		//Reserved
#define RES4				(2<<12)|(3<<2)		//Reserved

#define WA				(1<<12)				// write allocation

#define AP_RW			(3<<10)				//supervisor=RW, user=RW
#define AP_RO			(2<<10)				//supervisor=RW, user=RO
#define AP_NO			(1<<10)				//supervisor=RW, user=No access

#define DOMAIN_FAULT	(0x0)
#define DOMAIN_CHK		(0x1)
#define DOMAIN_NOTCHK	(0x3)
#define DOMAIN0			(0x0<<5)
#define DOMAIN1			(0x1<<5)

#define DOMAIN0_ATTR	(DOMAIN_CHK<<0)
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2)

#define RW_CB			(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB			(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_CBWA		(AP_RW|DOMAIN0|CBWA|DESC_SEC)
#define RW_NCB			(AP_RW|DOMAIN0|NCB|DESC_SEC)
#define RW_NCNB			(AP_RW|DOMAIN0|NCNB|DESC_SEC)
#define RW_FAULT		(AP_RW|DOMAIN1|NCNB|DESC_SEC)
#define RW_NSNCNB		((0<<19)|AP_RW|DOMAIN0|NCNB|DESC_SEC)

#define _DRAM0_BaseAddress	(0x30000000)
#define _DRAM_BaseAddress	(0x40000000)

//#define	_MMUTT_BaseAddress		(_DRAM_BaseAddress+0x07ff8000) //128MB:0x07ff8000, 96MB(OneDram):0x05ff8000
#define	_MMUTT_BaseAddress		(0x30000000 + 0x02000000) //128MB:0x07ff8000, 96MB(OneDram):0x05ff8000

void SetTransTable(u32_t uVaStart, u32_t uVaEnd, u32_t uPaStart, u32_t attr)
{
	u32_t *pTT;
	u32_t i, nNumOfSec;

//	Assert((uPaStart&0xfffff) == 0);
//	Assert((uVaStart&0xfffff) == 0);

	pTT = (u32_t *)_MMUTT_BaseAddress+(uVaStart>>20);

	nNumOfSec = (0x1000+(uVaEnd>>20)-(uVaStart>>20))%0x1000;

	for(i=0;i<=nNumOfSec;i++)*pTT++=attr |(((uPaStart>>20)+i)<<20);
}

void CoInvalidateL2Cache(void)
{
	int i,j;

	// L2 Dcache 512KB [31:29]Way, [15:6]Set, [3:1]Level
	for(i=0;i<8;i++)
		for(j=0;j<1024;j++)
			CoInvalidateDCacheIndex((i<<29)|(j<<6)|(1<<1)); // D-cache is cleared.
}

void CoStartL2Cache(void)
{
//	Assert(bDCacheEnabled==false);
//	Assert(bICacheEnabled==false);

	CoSetL2CacheAuxCrtlReg(0);
	CoEnableL2Cache();
}

void CoStartMmuAndCache(void)
{
	// L2 setting & Invalidate Entire D cache
	CoDisableL2Cache();
	CoInvalidateL2Cache();
	CoStartL2Cache();

	CoInvalidateICache();
	CoInvalidateDTlb();

	CoDisableICache();
	CoDisableDCache();
	CoDisableMmu();

	// min. table unit 0x100000 : 1MB)
	SetTransTable(0x00000000,0x08000000,0x00000000,RW_NCNB);	// iROM(0x0~0x7FFF) & iRAM(0x20000~0x37FFF)
	SetTransTable(0x08000000,0x10000000,0x08000000,RW_FAULT); // Reserved
	SetTransTable(0x10000000,0x20000000,0x10000000,RW_FAULT); // Reserved


	// Memory Port0 64MB
	SetTransTable(_DRAM0_BaseAddress + 0x00000000, _DRAM0_BaseAddress + 0x04000000, _DRAM0_BaseAddress+0x00000000, RW_NCNB);

	// 0x34000000 -0x3fffffff
	SetTransTable(_DRAM0_BaseAddress + 0x04000000,
			_DRAM0_BaseAddress + 0x04000000 + 0x0a000000,
			_DRAM0_BaseAddress + 0x04000000, RW_CBWA);
/*

	SetTransTable(0x26000000,0x30000000,0x26000000,RW_FAULT); // Reserved
	SetTransTable(0x30000000,0x40000000,0x30000000,RW_FAULT); // Reserved
*/

	// Memory Port1 256 MB
	SetTransTable(_DRAM_BaseAddress + 0x00000000,
	_DRAM_BaseAddress + 0x10000000, _DRAM_BaseAddress + 0x00000000,
	RW_CBWA);

/*
	SetTransTable(_DRAM_BaseAddress+0x01000000,_DRAM_BaseAddress+0x07f00000,_DRAM_BaseAddress+0x01000000,RW_NCNB);
	SetTransTable(_DRAM_BaseAddress+0x07f00000,_DRAM_BaseAddress+0x08000000,_DRAM_BaseAddress+0x07f00000,RW_CBWA);
	SetTransTable(0x48000000,0x50000000,0x48000000,RW_FAULT); // Reserved
	SetTransTable(0x50000000,0x60000000,0x50000000,RW_FAULT); // Reserved
*/


	/////////////////////////
	SetTransTable(0x60000000,0x80000000,0x60000000,RW_FAULT); // Reserved

	SetTransTable(0x80000000,0x80200000,0x80000000,RW_CB); // SROMC Bank0 for 1MB code region using SRAM
	SetTransTable(0x80200000,0x88000000,0x80100000,RW_NCNB); // SROMC Bank0 remained region
	SetTransTable(0x88000000,0x90000000,0x88000000,RW_NCNB); // SROMC Bank1
	SetTransTable(0x90000000,0x98000000,0x90000000,RW_NCNB); // SROMC Bank2
	SetTransTable(0x98000000,0xa0000000,0x98000000,RW_NCNB); // SROMC Bank3
	SetTransTable(0xa0000000,0xa8000000,0xa0000000,RW_NCNB); // SROMC Bank4
	SetTransTable(0xa8000000,0xb0000000,0xa8000000,RW_NCNB); // SROMC Bank5

	SetTransTable(0xb0000000,0xb8000000,0xb0000000,RW_NCNB); // OneNAND0
	SetTransTable(0xb8000000,0xc0000000,0xb8000000,RW_NCNB); // OneNAND1
	SetTransTable(0xc0000000,0xd0000000,0xc0000000,RW_NCNB); // MP3_SRAM

	SetTransTable(0xd0000000,0xd8000000,0xd0000000,RW_NCNB); // IROMC,IRAMC
	SetTransTable(0xd8000000,0xe0000000,0xd8000000,RW_NCNB); // DMZ ROM

	SetTransTable(0xe0000000,0x00000000,0xe0000000,RW_NCNB); // SFR

	CoSetTTBase(_MMUTT_BaseAddress);

	//DOMAIN1: no_access, DOMAIN0,2~15=client(AP is checked)
	CoSetDomain(0x55555550|DOMAIN1_ATTR|DOMAIN0_ATTR);

	//CoInvalidateICache();
	//CoCleanDCacheIndex(0);

	//CoSetProcessId(0x0);
	CoEnableAlignFault();	// not support unaligned data access. produce alignment fault
	//CoDisableAlignFault();	// support unaligned data access

	CoEnableMmu();
	CoEnableICache();
	CoEnableDCache(); // DCache should be turned on after MMU is turned on.
	CoEnableBranchPrediction();
}
