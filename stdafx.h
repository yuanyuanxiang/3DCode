
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

// 非MFC程序请添加此项到预处理器
#ifndef NO_STDAFX_H

#ifdef USING_VLD
#include "vld/include/vld.h" // should appear before '#include <afxwin.h>'
#pragma comment(lib, "vld/lib/Win32/vld.lib")
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }/**< 安全删除指针p */

#define SAFE_NEW(type, p) if ((p) == NULL) { (p) = new type; }		/**< 仅当p为空时候分配内存 */

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)				/**< 计算每行字节数 */

#undef ANDROID_CPP

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#else

// DEBUG_NEW
#define DEBUG_NEW new

// ASSERT
#include <assert.h>
#define ASSERT assert

#ifdef USING_VLD
#include "vld/include/vld.h"
#pragma comment(lib, "vld/lib/Win32/vld.lib")
#endif

// DataTypes
#include "DataTypes.h"

#endif // STDAFX_H