/*
 * Window definitions
 *
 * Copyright 1993 Alexandre Julliard
 */

#ifndef __WINE_WIN_H
#define __WINE_WIN_H

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winproc.h"
#include "winuser.h"
#include "wine/windef16.h"

#define WND_MAGIC     0x444e4957  /* 'WIND' */

struct tagCLASS;
struct tagDCE;
struct tagMESSAGEQUEUE;

typedef struct tagWND
{
    struct tagWND *next;          /* Next sibling */
    struct tagWND *child;         /* First child */
    struct tagWND *parent;        /* Window parent (from CreateWindow) */
    HWND           owner;         /* Window owner */
    struct tagCLASS *class;       /* Window class */
    HWINDOWPROC    winproc;       /* Window procedure */
    DWORD          dwMagic;       /* Magic number (must be WND_MAGIC) */
    DWORD          tid;           /* Owner thread id */
    HWND         hwndSelf;      /* Handle of this window */
    HINSTANCE    hInstance;     /* Window hInstance (from CreateWindow) */
    RECT         rectClient;    /* Client area rel. to parent client area */
    RECT         rectWindow;    /* Whole window rel. to parent client area */
    LPWSTR        text;           /* Window text */
    void          *pVScroll;      /* Vertical scroll-bar info */
    void          *pHScroll;      /* Horizontal scroll-bar info */
    void          *pProp;         /* Pointer to properties list */
    struct tagDCE *dce;           /* Window DCE (if CS_OWNDC or CS_CLASSDC) */
    HGLOBAL16      hmemTaskQ;     /* Task queue global memory handle */
    HRGN           hrgnUpdate;    /* Update region */
    HRGN           hrgnWnd;       /* window's region */
    HWND           hwndLastActive;/* Last active popup hwnd */
    DWORD          dwStyle;       /* Window style (from CreateWindow) */
    DWORD          dwExStyle;     /* Extended style (from CreateWindowEx) */
    DWORD          clsStyle;      /* Class style at window creation */
    UINT           wIDmenu;       /* ID or hmenu (from CreateWindow) */
    DWORD          helpContext;   /* Help context ID */
    UINT           flags;         /* Misc. flags (see below) */
    HMENU16        hSysMenu;      /* window's copy of System Menu */
    int            cbWndExtra;    /* class cbWndExtra at window creation */
    int            irefCount;     /* window's reference count*/
    DWORD          userdata;      /* User private data */
    void          *pDriverData;   /* Window driver data */
    DWORD          wExtra[1];     /* Window extra bytes */
} WND;

typedef struct
{
    RECT16	   rectNormal;
    POINT16	   ptIconPos;
    POINT16	   ptMaxPos;
    HWND           hwndIconTitle;
} INTERNALPOS, *LPINTERNALPOS;

  /* WND flags values */
#define WIN_NEEDS_BEGINPAINT   0x0001 /* WM_PAINT sent to window */
#define WIN_NEEDS_ERASEBKGND   0x0002 /* WM_ERASEBKGND must be sent to window*/
#define WIN_NEEDS_NCPAINT      0x0004 /* WM_NCPAINT must be sent to window */
#define WIN_RESTORE_MAX        0x0008 /* Maximize when restoring */
#define WIN_INTERNAL_PAINT     0x0010 /* Internal WM_PAINT message pending */
#define WIN_NEED_SIZE          0x0040 /* Internal WM_SIZE is needed */
#define WIN_NCACTIVATED        0x0080 /* last WM_NCACTIVATE was positive */
#define WIN_ISDIALOG           0x0200 /* Window is a dialog */
#define WIN_ISWIN32            0x0400 /* Understands Win32 messages */
#define WIN_NEEDS_SHOW_OWNEDPOPUP 0x0800 /* WM_SHOWWINDOW:SC_SHOW must be sent in the next ShowOwnedPopup call */
#define WIN_NEEDS_INTERNALSOP  0x1000 /* Window was hidden by WIN_InternalShowOwnedPopups */

  /* Window functions */
extern WND *WIN_GetWndPtr( HWND hwnd );
extern int    WIN_SuspendWndsLock( void );
extern void   WIN_RestoreWndsLock(int ipreviousLock);
extern WND*   WIN_FindWndPtr( HWND hwnd );
extern WND*   WIN_LockWndPtr(WND *wndPtr);
extern void   WIN_ReleaseWndPtr(WND *wndPtr);
extern void   WIN_UpdateWndPtr(WND **oldPtr,WND *newPtr);
extern HWND WIN_Handle32( HWND16 hwnd16 );
extern BOOL WIN_IsCurrentProcess( HWND hwnd );
extern BOOL WIN_IsCurrentThread( HWND hwnd );
extern void WIN_LinkWindow( HWND hwnd, HWND parent, HWND hwndInsertAfter );
extern void WIN_UnlinkWindow( HWND hwnd );
extern HWND WIN_FindWinToRepaint( HWND hwnd );
extern void WIN_DestroyThreadWindows( HWND hwnd );
extern BOOL WIN_CreateDesktopWindow(void);
extern BOOL WIN_IsWindowDrawable(WND*, BOOL );
extern HWND *WIN_ListParents( HWND hwnd );
extern HWND *WIN_ListChildren( HWND hwnd );
extern BOOL WIN_InternalShowOwnedPopups( HWND owner, BOOL fShow, BOOL unmanagedOnly );

inline static HWND WIN_GetFullHandle( HWND hwnd )
{
    if (!HIWORD(hwnd) && hwnd) hwnd = WIN_Handle32( LOWORD(hwnd) );
    return hwnd;
}

inline static HWND16 WIN_Handle16( HWND hwnd )
{
    return LOWORD(hwnd);
}

inline static WND *WIN_FindWndPtr16( HWND16 hwnd )
{
    /* don't bother with full conversion */
    return WIN_FindWndPtr( (HWND)(ULONG_PTR)hwnd );
}

#define BAD_WND_PTR ((WND *)1)  /* returned by WIN_GetWndPtr on bad window handles */

extern HWND CARET_GetHwnd(void);
extern void CARET_GetRect(LPRECT lprc);  /* windows/caret.c */

extern BOOL16 DRAG_QueryUpdate( HWND, SEGPTR, BOOL );
extern HBRUSH DEFWND_ControlColor( HDC hDC, UINT ctlType );  /* windows/defwnd.c */

extern void PROPERTY_RemoveWindowProps( WND *pWnd );  		      /* windows/property.c */

/* Classes functions */
struct tagCLASS;  /* opaque structure */
struct builtin_class_descr;
extern ATOM CLASS_RegisterBuiltinClass( const struct builtin_class_descr *descr );
extern struct tagCLASS *CLASS_AddWindow( ATOM atom, HINSTANCE inst, WINDOWPROCTYPE type,
                                         INT *winExtra, WNDPROC *winproc,
                                         DWORD *style, struct tagDCE **dce );
extern void CLASS_RemoveWindow( struct tagCLASS *cls );
extern void CLASS_FreeModuleClasses( HMODULE16 hModule );

/* windows/focus.c */
extern void FOCUS_SwitchFocus( struct tagMESSAGEQUEUE *pMsgQ, HWND , HWND );

#endif  /* __WINE_WIN_H */
