/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

namespace dxCommon
{

	class D3DCOMMON_EXPORTS_CLASS d3dSample : public KSample
	{
	public:
		d3dSample();
		~d3dSample();

	private:
		// 1.Initialize
		void OnInitialize() override;

		// 2.Resize
		void OnResize() override;

		// 3.Update
		void OnUpdate() override;

		// 4.Render
		void OnRender() override;

		// 5.Destory
		void OnDestroy() override;

		// 6.AppIdle() 
		void OnAppIdle() override;


	private:
		void OnMouseDown(WPARAM wParam, int x, int y) override {}
		void OnMouseUp(WPARAM wParam, int x, int y) override {}
		void OnMouseMove(WPARAM wParam, int x, int y) override {}
		void OnMouseWheel(WPARAM wParam, LPARAM lParam) override {}

	protected:
		// 初始化
		virtual void Initialize() = 0;

		// 调整大小
		virtual void Resizing() = 0;

		// 调整大小完成
		virtual void Resized() = 0;

		// 更新
		virtual void Update() = 0;

		// 绘制
		virtual void Draw() = 0;

		// 关闭
		virtual void Shutdown() = 0;

		// 应用空闲
		virtual void DoAppIdle();


	protected:
		//WARP（Windows Advanced Rasterization Platform）
		//提供了一种以软件方式代替显卡硬件方式完成渲染（用CPU模拟GPU功能）
		bool m_useWarpDevice;

		// 显示帧状态
		bool m_bFrameStats;

		// 4X MSAA enabled
		bool m_4xMsaaEnable;
		UINT m_4xMsaaQuality;

		// 派生类应在派生构造函数中设置这些值以自定义起始值。
		DXGI_FORMAT m_BackBufferFormat;
		DXGI_FORMAT m_DepthStencilFormat;

		// 设备类型
		D3D_DRIVER_TYPE m_d3dDriverType;

		// 矩阵
		XMFLOAT4X4 m_World;
		XMFLOAT4X4 m_View;
		XMFLOAT4X4 m_Proj;

	protected:
		void EnableFrameStats()
		{
			m_bFrameStats = true;
		}

	private:
		void CalculateFrameStats();


		//@EndOf(d3dSample)
	};

}
