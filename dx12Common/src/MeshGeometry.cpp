#include "stdafx.h"
#include "MeshGeometry.h"
#include "DXUtil.h"


namespace dx12
{

	void MeshGeometry::Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList, MeshData* pMeshData)
	{
		// 主存缓冲区 - 顶点数据
		this->VBSizeInBytes = pMeshData->vbSizeInBytes();
		this->VBStrideInBytes = pMeshData->vbStrideInBytes();
		ThrowIfFailed(D3DCreateBlob(this->VBSizeInBytes, &this->VertexBufferCPU));
		CopyMemory(this->VertexBufferCPU->GetBufferPointer(), pMeshData->pvbInitData(), this->VBSizeInBytes);

		// 主存缓冲区 - 索引数据
		this->IBSizeInBytes = pMeshData->ibSizeInBytes();
		this->IBVFormat = MeshData::IBVFormat;
		ThrowIfFailed(D3DCreateBlob(this->IBSizeInBytes, &this->IndexBufferCPU));
		CopyMemory(this->IndexBufferCPU->GetBufferPointer(), pMeshData->pibInitData(), this->IBSizeInBytes);

		// 上传缓冲区 - 顶点数据
		this->VertexBufferGPU = DXUtils::CreateDefaultBuffer(pDevice, pCmdList,
			pMeshData->pvbInitData(), VBSizeInBytes, this->VertexBufferUploader);

		// 上传缓冲区 - 索引数据
		this->IndexBufferGPU = DXUtils::CreateDefaultBuffer(pDevice, pCmdList,
			pMeshData->pibInitData(), this->IBSizeInBytes, this->IndexBufferUploader);

	}


}
