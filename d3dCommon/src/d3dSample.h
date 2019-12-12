/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
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
		// ��ʼ��
		virtual void Initialize() = 0;

		// ������С
		virtual void Resizing() = 0;

		// ������С���
		virtual void Resized() = 0;

		// ����
		virtual void Update() = 0;

		// ����
		virtual void Draw() = 0;

		// �ر�
		virtual void Shutdown() = 0;

		// Ӧ�ÿ���
		virtual void DoAppIdle();


	protected:
		//WARP��Windows Advanced Rasterization Platform��
		//�ṩ��һ���������ʽ�����Կ�Ӳ����ʽ�����Ⱦ����CPUģ��GPU���ܣ�
		bool m_useWarpDevice;

		// ��ʾ֡״̬
		bool m_bFrameStats;

		// 4X MSAA enabled
		bool m_4xMsaaEnable;
		UINT m_4xMsaaQuality;

		// ������Ӧ���������캯����������Щֵ���Զ�����ʼֵ��
		DXGI_FORMAT m_BackBufferFormat;
		DXGI_FORMAT m_DepthStencilFormat;

		// �豸����
		D3D_DRIVER_TYPE m_d3dDriverType;

		// ����
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
