#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <atlbase.h>
#include "sxusbcam.h"

ULONG cxVideoImage, cyVideoImage, dxVideoImage, expVideoImage, xVideoBin, yVideoBin;
ULONG cxVideoWindow, cyVideoWindow, VideoPalette, NextVideoPalette;
HANDLE hCam;
ULONG ExposureDuration[] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000};
USHORT     *VideoImage;
BYTE       *VideoDIBits;
BITMAPINFO *VideoDIBInfo;
struct t_sxccd_params ccd_params;
int exposure;

#define DEG2RAD 0.01745329

int SetPalette(int pal)
{
    int i, r, g, b;
    float f;

    switch (pal++)
    {
    	case 0:
            /*
             * Linear red palettes.
             */
            for (i = 0; i < 256; i++)
            {
                VideoDIBInfo->bmiColors[i].rgbBlue  = 0;
                VideoDIBInfo->bmiColors[i].rgbGreen = 0;
                VideoDIBInfo->bmiColors[i].rgbRed   = i;
            }
            break;
        case 1:
            /*
             * GammaLog red palettes.
             */
            for (i = 0; i < 256; i++)
            {
                f = log10(pow((i/255.0), 1.0)*9.0 + 1.0) * 255.0;
                VideoDIBInfo->bmiColors[i].rgbBlue  = 0;
                VideoDIBInfo->bmiColors[i].rgbGreen = 0;
                VideoDIBInfo->bmiColors[i].rgbRed   = f;
            }
            break;
        case 2:
            /*
             * Inversion red palette.
             */
            for (i = 0; i < 256; i++)
            {
                VideoDIBInfo->bmiColors[i].rgbBlue  = 0;
                VideoDIBInfo->bmiColors[i].rgbGreen = 0;
                VideoDIBInfo->bmiColors[i].rgbRed   = 255 - i;
            }
            break;
    	case 3:
            /*
             * Linear palettes.
             */
            for (i = 0; i < 256; i++)
            {
                VideoDIBInfo->bmiColors[i].rgbBlue  =
                VideoDIBInfo->bmiColors[i].rgbGreen =
                VideoDIBInfo->bmiColors[i].rgbRed   = i;
            }
            break;
        case 4:
            /*
             * GammaLog palettes.
             */
            for (i = 0; i < 256; i++)
            {
                f = log10(pow((i/255.0), 1.0)*9.0 + 1.0) * 255.0;
                VideoDIBInfo->bmiColors[i].rgbBlue  =
                VideoDIBInfo->bmiColors[i].rgbGreen =
                VideoDIBInfo->bmiColors[i].rgbRed   = f;
            }
            break;
        case 5:
            /*
             * Inversion palette.
             */
            for (i = 0; i < 256; i++)
            {
                VideoDIBInfo->bmiColors[i].rgbBlue  =
                VideoDIBInfo->bmiColors[i].rgbGreen =
                VideoDIBInfo->bmiColors[i].rgbRed   = 255 - i;
            }
            break;
        case 6:
            /*
             * False color palette #1.
             */
            for (i = 0; i < 256; i++)
            {
                r = (sin((i/255.0 * 360.0 + 0.0)   * DEG2RAD) * 0.5 + 0.5) * 255.0;
                g = (sin((i/255.0 * 360.0 + 120.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                b = (sin((i/255.0 * 360.0 + 240.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                VideoDIBInfo->bmiColors[i].rgbBlue  = b;
                VideoDIBInfo->bmiColors[i].rgbGreen = g;
                VideoDIBInfo->bmiColors[i].rgbRed   = r;
            }
            break;
        case 7:
            /*
             * False color palette #2.
             */
            for (i = 0; i < 256; i++)
            {
                r = (sin((i/255.0 * 360.0 + 120.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                g = (sin((i/255.0 * 360.0 + 240.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                b = (sin((i/255.0 * 360.0 + 0.0)   * DEG2RAD) * 0.5 + 0.5) * 255.0;
                VideoDIBInfo->bmiColors[i].rgbBlue  = b;
                VideoDIBInfo->bmiColors[i].rgbGreen = g;
                VideoDIBInfo->bmiColors[i].rgbRed   = r;
            }
            break;
        case 8:
            /*
             * False color palette #3.
             */
            for (i = 0; i < 256; i++)
            {
                r = (sin((i/255.0 * 360.0 + 240.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                g = (sin((i/255.0 * 360.0 + 0.0)   * DEG2RAD) * 0.5 + 0.5) * 255.0;
                b = (sin((i/255.0 * 360.0 + 120.0) * DEG2RAD) * 0.5 + 0.5) * 255.0;
                VideoDIBInfo->bmiColors[i].rgbBlue  = b;
                VideoDIBInfo->bmiColors[i].rgbGreen = g;
                VideoDIBInfo->bmiColors[i].rgbRed   = r;
            }
            pal = 0;
            break;
    }
    return (pal);
}

#undef DEG2RAD

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	enum {camIndex = 0};
	USHORT xoffset = 0;
	USHORT yoffset = 0;
	USES_CONVERSION;
    HDC         hDC;
    PAINTSTRUCT ps;
    RECT        rect;
    USHORT     *srcPix, minPix, maxPix;
    BYTE       *dstPix;
    int         x, y;
    float       scale;

    switch (message)
    {
        case WM_TIMER:
			{
			char pChar[99];
            /*
             * Read pixels from camera.
             */
			/* begin next exposure */
			sxExposePixels(hCam, SXCCD_EXP_FLAGS_FIELD_BOTH, camIndex, xoffset, yoffset, ccd_params.width, ccd_params.height, xVideoBin, yVideoBin, exposure);
            sxReadPixels(hCam, VideoImage, cxVideoImage * cyVideoImage);
            /*
             * Scan for min and max pixel values.
             */
            minPix = 0xFFFF;
            maxPix = 0x0000;
            srcPix = VideoImage;
            for (y = 0; y < cyVideoImage; y++)
            {
                for (x = 0; x < cxVideoImage; x++, srcPix++)
                {
                	if (*srcPix > maxPix) maxPix = *srcPix;
                	if (*srcPix < minPix) minPix = *srcPix;
                }
            }
            if (maxPix == minPix)
                scale = 0.0;
            else
                scale = 255.0 / (maxPix - minPix);
            /*
             * Copy scaled pixels into DIB.
             */
            srcPix = VideoImage;
            for (y = 0; y < cyVideoImage; y++)
            {
                dstPix = VideoDIBits + y * dxVideoImage;
#if 0
                for (x = 0; x < cxVideoImage; x++)
                {
                	dstPix[x] = (*srcPix++ - minPix) * scale + (*srcPix++ - minPix) * scale;
                }
                for (x = 0; x < cxVideoImage; x++)
                {
                	dstPix[x] += (*srcPix++ - minPix) * scale + (*srcPix++ - minPix) * scale;
                }
				dstPix += cxVideoImage;
#else
                for (x = 0; x < cxVideoImage; x++)
                {
                	*dstPix++ = (*srcPix++ - minPix) * scale;
                }				
#endif
            }
			sprintf(pChar, "Palette %d, Exposure time %dmsec, MinPix %d, MaxPix %d",
					VideoPalette, exposure, minPix, maxPix);
			SetWindowText(hWnd, _T(pChar));
            InvalidateRect(hWnd, NULL, TRUE);
            return (0);
			}
        case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            StretchDIBits(hDC,
                          0,
                          0,
                          cxVideoWindow,
                          cyVideoWindow,
                          0,
                          0,
                          cxVideoImage,
                          cyVideoImage,
                          VideoDIBits,
                          VideoDIBInfo,
                          0,
                          SRCCOPY);
            EndPaint(hWnd, &ps);
            return (0);
        case WM_SIZE:
            cxVideoWindow = LOWORD(lParam);
            cyVideoWindow = HIWORD(lParam);
            return (0);
        case WM_KEYUP:
            if (LOWORD(wParam) >= VK_F1 && LOWORD(wParam) <= VK_F12)
            {
				assert(LOWORD(wParam) - VK_F1 < sizeof(ExposureDuration)/sizeof(*ExposureDuration));
				exposure = ExposureDuration[LOWORD(wParam) - VK_F1];
            }
            if (LOWORD(wParam) == VK_SPACE)
            {
				 VideoPalette = NextVideoPalette;
            	 NextVideoPalette = SetPalette(VideoPalette);
            }
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, exposure+1, NULL);
            return (0);
        case WM_CREATE:
            SetTimer(hWnd, 1, 10, NULL);
            return (0);
         case WM_DESTROY:
            KillTimer(hWnd, 1);
            PostQuitMessage(0);
            return (0);
    }
    return (DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI SxVideoMain(HINSTANCE hInst, HINSTANCE hPrevInst, PSTR szCmdLine, int cmdShow)
{
    HWND     hWnd;
    MSG      msg;
    WNDCLASS wndClass;
    HANDLE   hCams[SXCCD_MAX_CAMS];
    int      nCams;

    /*
     * Open the first camera in list.
     */
    if ((nCams = sxOpen(hCams)) == 0)
        return (0);
    while (nCams-- > 1)
        sxClose(hCams[nCams]);
    hCam = hCams[0];
    sxGetCameraParams(hCam, 0, &ccd_params);
#if 1
    /*
     * Bin image down into the 640x480 range.
     */
    for (xVideoBin = 1; ccd_params.width  / xVideoBin >= 320; xVideoBin++);
    for (yVideoBin = 1; ccd_params.height / yVideoBin >= 240; yVideoBin++);
	if (xVideoBin > 4) xVideoBin = 4;
	if (yVideoBin > 4) yVideoBin = 4;
#if 1
	xVideoBin = 2;
	yVideoBin = 2;
#endif
    cxVideoWindow  = cxVideoImage = ccd_params.width  / xVideoBin;
    cyVideoWindow  = cyVideoImage = ccd_params.height / yVideoBin;
#else
	xVideoBin = 1;
	yVideoBin = 1;
	cxVideoWindow  = cxVideoImage = ccd_params.width  / (xVideoBin*2);
    cyVideoWindow  = cyVideoImage = ccd_params.height / (yVideoBin*2);
#endif
    dxVideoImage   = (cxVideoImage + 3) & ~3;
    VideoImage     = (USHORT *)malloc(2 * cxVideoImage * cyVideoImage);
    VideoDIBits    = (BYTE *)malloc(dxVideoImage * cyVideoImage);
    VideoDIBInfo   = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 255);
    VideoDIBInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    VideoDIBInfo->bmiHeader.biHeight        = -(LONG)cyVideoImage;
    VideoDIBInfo->bmiHeader.biWidth         = cxVideoImage;
    VideoDIBInfo->bmiHeader.biPlanes        = 1;
    VideoDIBInfo->bmiHeader.biBitCount      = 8;
    VideoDIBInfo->bmiHeader.biCompression   = 0;
    VideoDIBInfo->bmiHeader.biSizeImage     = dxVideoImage * cyVideoImage;
    VideoDIBInfo->bmiHeader.biXPelsPerMeter = 72;
    VideoDIBInfo->bmiHeader.biYPelsPerMeter = 72;
    VideoDIBInfo->bmiHeader.biClrUsed       = 256;
    VideoDIBInfo->bmiHeader.biClrImportant  = 256;
	exposure = ExposureDuration[0];
    VideoPalette = 0;
    SetPalette(VideoPalette);
    /*
     * Create main window.
     */
    wndClass.style         = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc   = WndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = hInst;
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)NULL;
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = TEXT("SX Video");
    if (!RegisterClass(&wndClass))
        return (0);
    hWnd = CreateWindow(wndClass.lpszClassName,
                        TEXT("Starlight Xpress Video"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        cxVideoWindow,
                        cyVideoWindow,
                        NULL,
                        NULL,
                        hInst,
                        NULL);
    ShowWindow(hWnd, cmdShow);
    UpdateWindow(hWnd);
    /*
     * Message loop.
     */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    free(VideoDIBInfo);
    free(VideoDIBits);
    free(VideoImage);
    sxClose(hCam);
    return (msg.wParam);
}

