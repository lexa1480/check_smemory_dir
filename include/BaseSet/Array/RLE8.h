#ifndef _BSET_RLE8_H
#define _BSET_RLE8_H

#ifndef b_DLL
#define b_DLL
#endif

#ifndef WIN32
	#include "../BaseTypes_x.h"
#else
	#include <winsock2.h>
#endif //! WIN32

#include <string>

namespace b_set
{
	const BYTE _CESC = 0xff;

	inline BOOL b_DLL RLE8_Compress(LPBYTE lpSrc, DWORD dwSrcSize, LPBYTE lpDst, DWORD& dwDstSize)
	{
		if( lpSrc==NULL || lpDst==NULL || dwSrcSize==0 || dwDstSize==0 )
			return 0;

		BOOL bRes = 1;

		BYTE *pSrcPtr = lpSrc;
		BYTE *pSrcEnd = lpSrc + dwSrcSize;
		BYTE *pDstPtr = lpDst;
		BYTE *pDstEnd = lpDst + dwDstSize;

		while(pSrcPtr < pSrcEnd)
		{
			BYTE byte = *pSrcPtr;
			BYTE *pSrcTmp = pSrcPtr+1;
			WORD wCount;
			while(*pSrcTmp == byte)
			{
				pSrcTmp++;
				if( pSrcTmp >= pSrcEnd )
				{
					if( (pSrcTmp - pSrcPtr) > 0x0100 )	// wCount==0 means that real count is 256
						pSrcTmp--;
					break; 
				}
				if( (pSrcTmp - pSrcPtr) > 0x0100 )		// wCount==0 means that real count is 256
					{ pSrcTmp--; break; }
			}
			wCount = (WORD)(pSrcTmp - pSrcPtr);

			if(byte == _CESC || wCount > 3)
			{
				if( (pDstPtr+3) > pDstEnd )
				{
					bRes = 0;
					break;
				}
				*pDstPtr++ = _CESC;
				*pDstPtr++ = (BYTE)(wCount & 0x00FF);	// wCount==0x0100 is written as 0x00
				*pDstPtr++ = byte;
				pSrcPtr = pSrcTmp;
			}
			else
			{
				if( pDstPtr+wCount > pDstEnd )
				{
					bRes = 0;
					break;
				}
				do *pDstPtr++ = byte; while(++pSrcPtr != pSrcTmp);
			}
		}

		dwDstSize = (pDstPtr - lpDst);
		
		if(bRes)
			bRes = (pSrcPtr == pSrcEnd);

		return bRes;
	}

	inline BOOL b_DLL RLE8_DeCompress(LPBYTE lpSrc, DWORD dwSrcSize, LPBYTE lpDst, DWORD& dwDstSize)
	{
		if( lpSrc==NULL || lpDst==NULL || dwSrcSize==0 || dwDstSize==0 )
			return 0;

		BOOL bRes = 1;

		BYTE *pSrcPtr =	lpSrc;
		BYTE *pSrcEnd = lpSrc + dwSrcSize;
		BYTE *pDstPtr =	lpDst;
		BYTE *pDstEnd = lpDst + dwDstSize;

		while(pDstPtr < pDstEnd && pSrcPtr < pSrcEnd)
		{
			BYTE byte = *pSrcPtr++;
			if(byte==_CESC)
			{
				WORD wCount = *pSrcPtr++;
				wCount = wCount ? wCount : 0x0100;	// wCount==0 means that real count is 256
				if(wCount > (pDstEnd - pDstPtr) )
				{
					wCount = pDstEnd - pDstPtr;
					bRes = 0;
				}

				byte = *pSrcPtr++;
				memset(pDstPtr, byte, wCount);
				pDstPtr += wCount;
				if( !bRes )
					break;
			}
			else
			{
				if(pDstPtr > pDstEnd)
				{
					bRes = 0;
					break;
				}
				*pDstPtr++ = byte;
			}
		}

		dwDstSize = (pDstPtr - lpDst);

		if(bRes)
			bRes = (pSrcPtr == pSrcEnd);

		return bRes;
	}


} //!namespace b_set

#endif //_BSET_RLE8_H
