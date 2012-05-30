/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSIMD_P_H
#define QSIMD_P_H

#include <qglobal.h>

QT_BEGIN_HEADER

/*
 * qt_module_config.prf defines the QT_COMPILER_SUPPORTS_XXX macros.
 * They mean the compiler supports the necessary flags and the headers
 * for the x86 and ARM intrinsics:
 *  - GCC: the -mXXX or march=YYY flag is necessary before #include
 *  - Intel CC: #include can happen unconditionally
 *  - MSVC: #include can happen unconditionally
 *  - RVCT: ???
 *
 * We will try to include all headers possible under this configuration.
 *
 * Supported XXX are:
 *   Flag  | Arch |  GCC  | Intel CC |  MSVC  |
 *  NEON   | ARM  | I & C | None     |   ?    |
 *  IWMMXT | ARM  | I & C | None     | I & C  |
 *  SSE2   | x86  | I & C | I & C    | I & C  |
 *  SSE3   | x86  | I & C | I & C    | I only |
 *  SSSE3  | x86  | I & C | I & C    | I only |
 *  SSE4_1 | x86  | I & C | I & C    | I only |
 *  SSE4_2 | x86  | I & C | I & C    | I only |
 *  AVX    | x86  | I & C | I & C    | I & C  |
 *  AVX2   | x86  | I & C | I & C    | I only |
 * I = intrinsics; C = code generation
 */

#ifdef __MINGW64_VERSION_MAJOR
#include <intrin.h>
#endif

// SSE intrinsics
#if defined(__SSE2__) || (defined(QT_COMPILER_SUPPORTS_SSE2) && defined(Q_CC_MSVC))
#if defined(QT_LINUXBASE)
/// this is an evil hack - the posix_memalign declaration in LSB
/// is wrong - see http://bugs.linuxbase.org/show_bug.cgi?id=2431
#  define posix_memalign _lsb_hack_posix_memalign
#  include <emmintrin.h>
#  undef posix_memalign
#else
#  include <emmintrin.h>
#endif
#endif

// SSE3 intrinsics
#if defined(__SSE3__) || (defined(QT_COMPILER_SUPPORTS_SSE3) && defined(Q_CC_MSVC))
#include <pmmintrin.h>
#endif

// SSSE3 intrinsics
#if defined(__SSSE3__) || (defined(QT_COMPILER_SUPPORTS_SSSE3) && defined(Q_CC_MSVC))
#include <tmmintrin.h>
#endif

// SSE4.1 intrinsics
#if defined(__SSE4_1__) || (defined(QT_COMPILER_SUPPORTS_SSE4_1) && defined(Q_CC_MSVC))
#include <smmintrin.h>
#endif

// SSE4.2 intrinsics
#if defined(__SSE4_2__) || (defined(QT_COMPILER_SUPPORTS_SSE4_2) && defined(Q_CC_MSVC))
#include <nmmintrin.h>
#endif

// AVX intrinsics
#if defined(__AVX__) || (defined(QT_COMPILER_SUPPORTS_AVX) && defined(Q_CC_MSVC))
// immintrin.h is the ultimate header, we don't need anything else after this
#include <immintrin.h>
#endif

// NEON intrinsics
#if defined __ARM_NEON__
#define QT_ALWAYS_HAVE_NEON
#include <arm_neon.h>
#endif


// IWMMXT intrinsics
#if defined(QT_COMPILER_SUPPORTS_IWMMXT)
#include <mmintrin.h>
#if defined(Q_OS_WINCE)
#  include "qplatformdefs.h"
#endif
#endif

#if defined(QT_COMPILER_SUPPORTS_IWMMXT)
#if !defined(__IWMMXT__) && !defined(Q_OS_WINCE)
#  include <xmmintrin.h>
#elif defined(Q_OS_WINCE_STD) && defined(_X86_)
#  pragma warning(disable: 4391)
#  include <xmmintrin.h>
#endif
#endif

QT_BEGIN_NAMESPACE


enum CPUFeatures {
    None        = 0,
    IWMMXT      = 0x1,
    NEON        = 0x2,
    SSE2        = 0x4,
    SSE3        = 0x8,
    SSSE3       = 0x10,
    SSE4_1      = 0x20,
    SSE4_2      = 0x40,
    AVX         = 0x80,
    AVX2        = 0x100,
    HLE         = 0x200,
    RTM         = 0x400
};

Q_CORE_EXPORT uint qDetectCPUFeatures();


#define ALIGNMENT_PROLOGUE_16BYTES(ptr, i, length) \
    for (; i < static_cast<int>(qMin(static_cast<quintptr>(length), ((4 - ((reinterpret_cast<quintptr>(ptr) >> 2) & 0x3)) & 0x3))); ++i)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIMD_P_H
