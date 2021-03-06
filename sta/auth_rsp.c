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


#include "rt_config.h"

/*
    ==========================================================================
    Description:
        authentication state machine init procedure
    Parameters:
        Sm - the state machine

	IRQL = PASSIVE_LEVEL

    ==========================================================================
 */
void AuthRspStateMachineInit(
	IN struct rtmp_adapter *pAd,
	IN PSTATE_MACHINE Sm,
	IN STATE_MACHINE_FUNC Trans[])
{
	StateMachineInit(Sm, Trans, MAX_AUTH_RSP_STATE, MAX_AUTH_RSP_MSG,
			 (STATE_MACHINE_FUNC) Drop, AUTH_RSP_IDLE,
			 AUTH_RSP_MACHINE_BASE);

	/* column 1 */
	StateMachineSetAction(Sm, AUTH_RSP_IDLE, MT2_PEER_DEAUTH,
			      (STATE_MACHINE_FUNC) PeerDeauthAction);

	/* column 2 */
	StateMachineSetAction(Sm, AUTH_RSP_WAIT_CHAL, MT2_PEER_DEAUTH,
			      (STATE_MACHINE_FUNC) PeerDeauthAction);

}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
*/
void PeerAuthSimpleRspGenAndSend(
	IN struct rtmp_adapter *pAd,
	IN PHEADER_802_11 pHdr80211,
	IN USHORT Alg,
	IN USHORT Seq,
	IN USHORT Reason,
	IN USHORT Status)
{
	HEADER_802_11 AuthHdr;
	ULONG FrameLen = 0;
	u8 *pOutBuffer = NULL;

	if (Reason != MLME_SUCCESS) {
		DBGPRINT(RT_DEBUG_TRACE, ("Peer AUTH fail...\n"));
		return;
	}

	/*Get an unused nonpaged memory */
	pOutBuffer = kmalloc(MGMT_DMA_BUFFER_SIZE, GFP_ATOMIC);
	if (pOutBuffer == NULL)
		return;

	DBGPRINT(RT_DEBUG_TRACE, ("Send AUTH response (seq#2)...\n"));
	MgtMacHeaderInit(pAd, &AuthHdr, SUBTYPE_AUTH, 0, pHdr80211->Addr2,
						pAd->MlmeAux.Bssid);
	MakeOutgoingFrame(pOutBuffer, &FrameLen, sizeof (HEADER_802_11),
			  &AuthHdr, 2, &Alg, 2, &Seq, 2, &Reason, END_OF_ARGS);
	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
	kfree(pOutBuffer);
}

/*
    ==========================================================================
    Description:

	IRQL = DISPATCH_LEVEL

    ==========================================================================
*/
void PeerDeauthAction(
	IN struct rtmp_adapter *pAd,
	IN PMLME_QUEUE_ELEM Elem)
{
	u8 Addr1[ETH_ALEN];
	u8 Addr2[ETH_ALEN];
	u8 Addr3[ETH_ALEN];
	USHORT Reason;
	BOOLEAN bDoIterate = FALSE;

	if (PeerDeauthSanity
	    (pAd, Elem->Msg, Elem->MsgLen, Addr1, Addr2, Addr3, &Reason)) {
		if (INFRA_ON(pAd)
		    && (MAC_ADDR_EQUAL(Addr1, pAd->CurrentAddress)
			|| MAC_ADDR_EQUAL(Addr1, BROADCAST_ADDR))
		    && MAC_ADDR_EQUAL(Addr2, pAd->CommonCfg.Bssid)
		    && MAC_ADDR_EQUAL(Addr3, pAd->CommonCfg.Bssid)
		    ) {
			DBGPRINT(RT_DEBUG_TRACE,
				 ("AUTH_RSP - receive DE-AUTH from our AP (Reason=%d)\n",
				  Reason));

#ifdef WPA_SUPPLICANT_SUPPORT
			if ((pAd->StaCfg.WpaSupplicantUP !=
			     WPA_SUPPLICANT_DISABLE)
			    && (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2)
			    && (pAd->StaCfg.PortSecured ==
				WPA_802_1X_PORT_SECURED))
				pAd->StaCfg.bLostAp = TRUE;
#endif /* WPA_SUPPLICANT_SUPPORT */

			/*
			   Some customer would set AP1 & AP2 same SSID, AuthMode & EncrypType but different WPAPSK,
			   therefore we need to do iterate here.
			 */
			if ((pAd->StaCfg.PortSecured ==
			     WPA_802_1X_PORT_NOT_SECURED)
			    &&
			    ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)
			     || (pAd->StaCfg.AuthMode ==
				 Ndis802_11AuthModeWPA2PSK))
			    )
				bDoIterate = TRUE;

			LinkDown(pAd, TRUE);

			if (bDoIterate) {
				pAd->MlmeAux.BssIdx++;
				IterateOnBssTab(pAd);
			}

		}
	} else {
		DBGPRINT(RT_DEBUG_TRACE,
			 ("AUTH_RSP - PeerDeauthAction() sanity check fail\n"));
	}
}
