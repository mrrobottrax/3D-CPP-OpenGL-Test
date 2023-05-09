#pragma once

namespace gMath
{
	struct Plane
	{
		glm::vec3 normal;
		float dist;

		void Invert()
		{
			normal = -normal;
			dist = -dist;
		}

		Plane(glm::vec3 normal, float dist) : normal(normal), dist(dist) {};
		Plane() : normal(0, 0, 0), dist(0) {};

		bool operator ==(Plane& other)
		{
			return normal == other.normal && dist == other.dist;
		}
	};

	struct Line
	{
		glm::vec3 pointA;
		glm::vec3 pointB;
	};

	float SqrMagnitude(glm::vec3 vector);
	float SqrDist(glm::vec3 vertexA, glm::vec3 vertexB);
	float DistFromLine(glm::vec3 linePointA, glm::vec3 linePointB, glm::vec3 point);
	float SignedDistFromPlane(Plane plane, glm::vec3 point);
	float SignedDistFromTriPlane(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC, glm::vec3 point);
	Plane PlaneFromTri(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC);
	Plane PlaneFromTri(const float triPointA[3], const float triPointB[3], const float triPointC[3]);

	glm::vec3 Lerp(glm::vec3 x, glm::vec3 y, float t);

	struct Matrix
	{
	private:
		float** m;
		int rows, columns;

	public:
		Matrix(const int& rows, const int& columns);
		~Matrix();

		inline const int GetRows() const
		{
			return rows;
		};

		inline const int GetColumns() const
		{
			return columns;
		};

		inline constexpr float* operator[] (int i) const
		{
			return m[i];
		};

		//friend constexpr Matrix operator* (Matrix lhs, const Matrix& rhs);
	};

	Matrix* MatrixMultiply(const Matrix& left, const Matrix& right);
}