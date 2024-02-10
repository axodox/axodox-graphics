#pragma once
#define NOMINMAX

#ifndef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS
#endif

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wincodec.h>

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.Threading.h>

#include <Include/Axodox.Infrastructure.h>
#include <Include/Axodox.Collections.h>
#include <Include/Axodox.Storage.h>
#include <Include/Axodox.Threading.h>

#ifdef AXODOX_GRAPHICS_EXPORT
#define AXODOX_GRAPHICS_API __declspec(dllexport)
#else
#define AXODOX_GRAPHICS_API __declspec(dllimport)
#pragma comment (lib,"Axodox.Graphics.lib")
#endif

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")