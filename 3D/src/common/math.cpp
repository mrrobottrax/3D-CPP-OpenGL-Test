#include <pch.h>
#include <common/math.h>

#include <glm.hpp>

namespace gMath
{
	float SqrMagnitude(const glm::vec3 vec)
	{
		float sqrMagnitude = 0;
		for (int i = 0; i < 3; ++i)
		{
			sqrMagnitude += vec[i] * vec[i];
		}

		return sqrMagnitude;
	};

	float SqrDist(const glm::vec3 vertexA, const glm::vec3 vertexB)
	{
		return SqrMagnitude(vertexA - vertexB);
	}

	float DistFromLine(const glm::vec3 linePointA, const glm::vec3 linePointB, const glm::vec3 point)
	{
		float dist = glm::length(glm::cross(point - linePointA, point - linePointB));
		dist /= glm::length(linePointB - linePointA);

		return dist;
	}

	float SignedDistFromPlane(const Plane plane, const glm::vec3 point)
	{
		return glm::dot(plane.normal, point) - plane.dist;
	}

	float SignedDistFromTriPlane(const glm::vec3 triPointA, const glm::vec3 triPointB, const glm::vec3 triPointC, const glm::vec3 point)
	{
		Plane plane = PlaneFromTri(triPointA, triPointB, triPointC);

		return SignedDistFromPlane(plane, point);
	}

	Plane PlaneFromTri(const glm::vec3 triPointA, const glm::vec3 triPointB, const glm::vec3 triPointC)
	{
		Plane plane;

		plane.normal = glm::normalize(glm::cross(triPointB - triPointA, triPointC - triPointB));
		plane.dist = 0;

		plane.dist += glm::dot(plane.normal, triPointA);
		plane.dist += glm::dot(plane.normal, triPointB);
		plane.dist += glm::dot(plane.normal, triPointC);

		plane.dist /= 3;

		return plane;
	}

	Plane PlaneFromTri(const float triPointA[3], const float triPointB[3], const float triPointC[3])
	{
		glm::vec3 a(triPointA[0], triPointA[1], triPointA[2]);
		glm::vec3 b(triPointB[0], triPointB[1], triPointB[2]);
		glm::vec3 c(triPointC[0], triPointC[1], triPointC[2]);

		return PlaneFromTri(a, b, c);
	}

	glm::vec3 Lerp(glm::vec3 x, glm::vec3 y, float t)
	{
		return x * (1.0f - t) + y * t;
	}

	Matrix::Matrix(const int& rows, const int& columns)
	{
		this->rows = rows;
		this->columns = columns;

		m = new float* [rows];

		for (int i = 0; i < rows; ++i)
		{
			m[i] = new float[columns];
		}
	}

	Matrix::~Matrix()
	{
		for (int i = 0; i < rows; ++i)
		{
			delete[] m[i];
		}

		delete[] m;
	}

	// Multiply to arbitrarily sized matrices
	// Returns null when matrices cannot be multiplied
	// Size is rows, columns
	Matrix* MatrixMultiply(const Matrix& left, const Matrix& right)
	{
		const int rows = left.GetRows();
		const int columns = right.GetColumns();

		if (left.GetColumns() != right.GetRows())
			return nullptr;

		Matrix& matrix = *new Matrix(rows, columns);

		// For each row
		for (int i = 0; i < rows; ++i)
		{
			// For each column
			for (int j = 0; j < columns; ++j)
			{
				float sum = 0;
				for (int k = 0; k < left.GetColumns(); ++k)
				{
					sum += left[i][k] * right[k][j];
				}

				matrix[i][j] = sum;
			}
		}

		return &matrix;
	}
}
