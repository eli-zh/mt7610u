/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#define RTMP_MODULE_OS
#define RTMP_MODULE_OS_UTIL

/*#include "rt_config.h"
*/
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rtmp_osabl.h"


 u32 RalinkRate[256] = {
	2,  4, 11, 22,
	12, 18, 24, 36, 48, 72, 96, 108, 109, 110, 111, 112, /* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26, 52, 78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59, 87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */
	13, 26, 39, 52, 78, 104, 117, 130, 156, /* AC: 20Mhz, 800ns GI, MCS: 0~8 */
	27, 54, 81, 108, 162, 216, 243, 270, 324, 360, /* AC: 40Mhz, 800ns GI, MCS: 0~9 */
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /* AC: 80Mhz, 800ns GI, MCS: 0~9 */
	14, 29, 43, 57, 87, 115, 130, 144, 173, /* AC: 20Mhz, 400ns GI, MCS: 0~8 */
	30, 60, 90, 120, 180, 240, 270, 300, 360, 400, /* AC: 40Mhz, 400ns GI, MCS: 0~9 */
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867, /* AC: 80Mhz, 400ns GI, MCS: 0~9 */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47 /* 3*3 */
};

void RtmpDrvMaxRateGet(
	IN	void 				*pReserved,
/*	IN	PHTTRANSMIT_SETTING		pHtPhyMode,
*/
	IN	u8					MODE,
	IN	u8					ShortGI,
	IN	u8					BW,
	IN	u8					MCS,
	OUT	u32					*pRate)
{
	int rate_index = 0;

#ifdef DOT11_VHT_AC
       if (MODE >= MODE_VHT) {
               if (BW == 0 /* 20Mhz */) {
                       rate_index = 112 + ((u8)ShortGI * 29) + ((u8)MCS);
               } else if (BW == 1 /* 40Mhz */) {
                       rate_index = 121 + ((u8)ShortGI * 29) + ((u8)MCS);
               } else if (BW == 2 /* 80Mhz */) {
                       rate_index = 131 + ((u8)ShortGI * 29) + ((u8)MCS);
               }
       } else
#endif /* DOT11_VHT_AC */

#ifdef DOT11_N_SUPPORT
	if ((MODE >= MODE_HTMIX) && (MODE < MODE_VHT)) {
		/* rate_index = 16 + ((u8)pHtPhyMode->field.BW *16) + ((u8)pHtPhyMode->field.ShortGI *32) + ((u8)pHtPhyMode->field.MCS); */
		rate_index = 16 + ((u8)BW *24) + ((u8)ShortGI *48) + ((u8)MCS);
	} else
#endif /* DOT11_N_SUPPORT */
		if (MODE == MODE_OFDM)
			rate_index = (u8)(MCS) + 4;
		else
			rate_index = (u8)(MCS);

	if (rate_index < 0)
		rate_index = 0;

	if (rate_index > 255)
		rate_index = 255;

	*pRate = RalinkRate[rate_index] * 500000;
}


char *rtstrchr(const char * s, int c)
{
    for(; *s != (char) c; ++s)
        if (*s == '\0')
            return NULL;
    return (char *) s;
}


void RtmpMeshDown(
	IN void *pDrvCtrlBK,
	IN BOOLEAN WaitFlag,
	IN BOOLEAN	 (*RtmpMeshLinkCheck)(IN void *pAd))
{
}




BOOLEAN RtmpOsCmdDisplayLenCheck(
	IN u32 LenSrc,
	IN u32 Offset)
{
	if (LenSrc > (IW_PRIV_SIZE_MASK - Offset))
		return FALSE;

	return TRUE;
}


#if defined(WPA_SUPPLICANT_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT)
void WpaSendMicFailureToWpaSupplicant(
	IN struct net_device *pNetDev,
	IN BOOLEAN bUnicast)
{
	char custom[IW_CUSTOM_MAX] = {0};

	snprintf(custom, sizeof(custom), "MLME-MICHAELMICFAILURE.indication");
	if(bUnicast)
		sprintf(custom, "%s unicast", custom);

	RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM, -1, NULL, (u8 *)custom, strlen(custom));

	return;
}
#endif /* defined(WPA_SUPPLICANT_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT) */


#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
int wext_notify_event_assoc(
	IN struct net_device *pNetDev,
	IN u8 *ReqVarIEs,
	IN u32 ReqVarIELen)
{
	char custom[IW_CUSTOM_MAX] = {0};

	if (ReqVarIELen <= IW_CUSTOM_MAX)
	{
		memmove(custom, ReqVarIEs, ReqVarIELen);
		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_ASSOC_REQ_IE, -1, NULL,
								(u8 *)custom, ReqVarIELen);
	}
	else
	    DBGPRINT(RT_DEBUG_TRACE, ("pAd->StaCfg.ReqVarIELen > MAX_CUSTOM_LEN\n"));

	return 0;

}
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */


#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
void SendAssocIEsToWpaSupplicant(
	IN struct net_device *pNetDev,
	IN u8 *ReqVarIEs,
	IN u32 ReqVarIELen)
{
	STRING custom[IW_CUSTOM_MAX] = {0};

	if ((ReqVarIELen + 17) <= IW_CUSTOM_MAX)
	{
		snprintf(custom, sizeof(custom), "ASSOCINFO_ReqIEs=");
		memmove(custom+17, ReqVarIEs, ReqVarIELen);
		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM, RT_REQIE_EVENT_FLAG, NULL, (u8 *)custom, ReqVarIELen + 17);

		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM, RT_ASSOCINFO_EVENT_FLAG, NULL, NULL, 0);
	}
	else
		DBGPRINT(RT_DEBUG_TRACE, ("pAd->StaCfg.ReqVarIELen + 17 > MAX_CUSTOM_LEN\n"));

	return;
}
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */



