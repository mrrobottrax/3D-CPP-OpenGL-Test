#pragma once

namespace gmath
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

	float SqrMagnitude(const glm::vec3& vector);
	float SqrDist(const glm::vec3& vertexA, const glm::vec3& vertexB);
	float DistFromLine(glm::vec3 linePointA, glm::vec3 linePointB, glm::vec3 point);
	float SignedDistFromPlane(Plane plane, glm::vec3 point);
	float SignedDistFromTriPlane(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC, glm::vec3 point);
	Plane PlaneFromTri(glm::vec3 triPointA, glm::vec3 triPointB, glm::vec3 triPointC);
	Plane PlaneFromTri(const float triPointA[3], const float triPointB[3], const float triPointC[3]);
	glm::vec3 LineAndPlane(const Line&, const Plane&);
	void ScalePlane(Plane&, const glm::vec3&);

	// TODO: Make these const references
	glm::vec3 Lerp(glm::vec3 x, glm::vec3 y, float t);
	glm::vec3 Lerp(Line, float t);

	glm::fquat EulerToQuaternion(const glm::vec3&);

	struct Matrix
	{
	private:
		float* m = nullptr;
		int rows, columns = 0;

	public:
		Matrix(const int& rows, const int& columns);
		Matrix(const Matrix& matrix);
		inline Matrix() : m(), rows(), columns() { };
		inline ~Matrix()
		{
			if (m)
				delete[] m;
		};

	public:
		void Initialize(int rows, int columns);

		inline const int GetRows() const
		{
			return rows;
		};

		inline const int GetColumns() const
		{
			return columns;
		};

		void Transform();

		inline constexpr float* operator[] (int i) const
		{
			return &(m[i * columns]);
		};

		Matrix& operator= (const Matrix& matrix);
	};

	void MatrixMultiply(const Matrix& left, const Matrix& right, Matrix& result);
}