/*
 * (C) 2010-2013 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdafx.h"
#include "mplayerc.h"
#include "MainFrm.h"
#include "PlayerNavigationBar.h"
#include <afxwin.h>


// CPlayerCaptureBar

IMPLEMENT_DYNAMIC(CPlayerNavigationBar, CPlayerBar)
CPlayerNavigationBar::CPlayerNavigationBar(CMainFrame* pMainFrame)
    : m_pParent(nullptr)
    , m_navdlg(pMainFrame)
{
}

CPlayerNavigationBar::~CPlayerNavigationBar()
{
}

BOOL CPlayerNavigationBar::Create(CWnd* pParentWnd, UINT defDockBarID)
{
    if (!CPlayerBar::Create(ResStr(IDS_NAVIGATION_BAR), pParentWnd, ID_VIEW_NAVIGATION, defDockBarID, _T("Navigation Bar"))) {
        return FALSE;
    }

    m_pParent = pParentWnd;
    m_navdlg.Create(this);
    m_navdlg.ShowWindow(SW_SHOWNORMAL);

    CRect r;
    m_navdlg.GetWindowRect(r);
    m_szMinVert = m_szVert = r.Size();
    m_szMinHorz = m_szHorz = r.Size();
    m_szMinFloat = m_szFloat = r.Size();
    m_bFixedFloat = true;
    m_szFixedFloat = r.Size();

    return TRUE;
}

static WNDPROC g_parentFrameOrigWndProc = nullptr;
LRESULT CALLBACK ParentFrameSubclassWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ASSERT(g_parentFrameOrigWndProc);
    if (message == WM_SYSCOMMAND && wParam == SC_CLOSE) {
        AfxGetAppSettings().fHideNavigation = true;
    }
    return CallWindowProc(g_parentFrameOrigWndProc, hwnd, message, wParam, lParam);
}

BOOL CPlayerNavigationBar::PreTranslateMessage(MSG* pMsg)
{
    if (CWnd* pParent1 = GetParent()) {
        CWnd* pParent2 = pParent1->GetParent();
        if (pParent2 != m_pParent) {
            if (!g_parentFrameOrigWndProc) {
                g_parentFrameOrigWndProc = SubclassWindow(pParent2->m_hWnd, ParentFrameSubclassWndProc);
            }
        } else {
            g_parentFrameOrigWndProc = nullptr;
        }
    }

    if (IsWindow(pMsg->hwnd) && IsVisible() && pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
        if (IsDialogMessage(pMsg)) {
            return TRUE;
        }
    }

    return __super::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CPlayerNavigationBar, CPlayerBar)
    ON_WM_SIZE()
    ON_WM_NCLBUTTONUP()
END_MESSAGE_MAP()

// CPlayerShaderEditorBar message handlers

void CPlayerNavigationBar::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    if (::IsWindow(m_navdlg.m_hWnd)) {
        CRect r, rectButtonInfo, rectButtonScan;
        LONG totalsize, separation, sizeButtonInfo, sizeButtonScan;
        GetClientRect(r);
        m_navdlg.MoveWindow(r);
        r.DeflateRect(3, 3, 3, 58);
        m_navdlg.m_ChannelList.MoveWindow(r);

        m_navdlg.m_ButtonInfo.GetClientRect(rectButtonInfo);
        m_navdlg.m_ButtonScan.GetClientRect(rectButtonScan);
        sizeButtonInfo = rectButtonInfo.right - rectButtonInfo.left;
        sizeButtonScan = rectButtonScan.right - rectButtonScan.left;
        totalsize = r.right - r.left;
        separation = (totalsize - sizeButtonInfo - sizeButtonScan);
        if (separation < 0) {
            separation = 0;
        }
        m_navdlg.m_ButtonInfo.SetWindowPos(nullptr, r.left, r.bottom + 8, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        m_navdlg.m_ButtonScan.SetWindowPos(nullptr, r.left + sizeButtonInfo + separation, r.bottom + 8, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        m_navdlg.m_ButtonFilterStations.SetWindowPos(nullptr, r.left, r.bottom + 35, totalsize, 21, SWP_NOZORDER);
    }

}

void CPlayerNavigationBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    __super::OnNcLButtonUp(nHitTest, point);

    if (nHitTest == HTCLOSE) {
        AfxGetAppSettings().fHideNavigation = true;
    }
}
