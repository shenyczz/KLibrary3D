#pragma once

// ≤®¿À
class Waves
{
public:
	Waves(int m, int n, float dx, float dt, float speed, float damping);
	~Waves();


public:
	int RowCount() const { return m_Rows; }
	int ColCount() const { return m_Cols; }
	int VertexCount() const { return m_VertexCount; }
	int TriangleCount() const { return m_TriangleCount; }
	float Width() const { return m_SpatialStep * m_Cols; }
	float Depth() const { return m_SpatialStep * m_Rows; }


	// Returns the solution at the ith grid point.
	const XMFLOAT3& Position(int i) const { return m_CurrSolution[i]; }

	// Returns the solution normal at the ith grid point.
	const XMFLOAT3& Normal(int i) const { return m_Normals[i]; }

	// Returns the unit tangent vector at the ith grid point in the local x-axis direction.
	const XMFLOAT3& TangentX(int i)const { return m_TangentX[i]; }


public:
	void Update(float dt);
	void Disturb(int i, int j, float magnitude);




private:
	int m_Rows;
	int m_Cols;

	int m_VertexCount;
	int m_TriangleCount;

	// Simulation constants we can precompute.
	float m_K1 = 0.0f;
	float m_K2 = 0.0f;
	float m_K3 = 0.0f;

	// Time & Spatial step
	float m_TimeStep = 0.0f;
	float m_SpatialStep = 0.0f;

	std::vector<DirectX::XMFLOAT3> m_PrevSolution;
	std::vector<DirectX::XMFLOAT3> m_CurrSolution;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT3> m_TangentX;


	//@EndOf(Waves)
};

