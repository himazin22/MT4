#include "MyMathUtility.h" // 変更
#include <cmath>
#include <algorithm>
using namespace KamataEngine;

// ※中身の計算式は変更していません。MathUtility:: を MyMathUtility:: にしただけです。
Matrix4x4 MyMathUtility::MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMathUtility::MakeRotationMatrix(const Vector3& rotation) {
	float sinX = std::sin(rotation.x), cosX = std::cos(rotation.x);
	float sinY = std::sin(rotation.y), cosY = std::cos(rotation.y);
	float sinZ = std::sin(rotation.z), cosZ = std::cos(rotation.z);

	Matrix4x4 result{};
	result.m[0][0] = cosY * cosZ;
	result.m[0][1] = cosY * sinZ;
	result.m[0][2] = -sinY;
	result.m[0][3] = 0.0f;

	result.m[1][0] = sinX * sinY * cosZ - cosX * sinZ;
	result.m[1][1] = sinX * sinY * sinZ + cosX * cosZ;
	result.m[1][2] = sinX * cosY;
	result.m[1][3] = 0.0f;

	result.m[2][0] = cosX * sinY * cosZ + sinX * sinZ;
	result.m[2][1] = cosX * sinY * sinZ - sinX * cosZ;
	result.m[2][2] = cosX * cosY;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMathUtility::MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MyMathUtility::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
}

// ========== ここから追加分 ==========

// 加算
Matrix4x4 MyMathUtility::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

// 減算
Matrix4x4 MyMathUtility::Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

// 逆行列
Matrix4x4 MyMathUtility::Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float det = m.m[0][0] * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[1][3] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[1][3] -
	                         m.m[2][1] * m.m[1][2] * m.m[3][3] - m.m[1][1] * m.m[3][2] * m.m[2][3]) -
	            m.m[0][1] * (m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[1][3] + m.m[3][0] * m.m[1][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[1][3] -
	                         m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[1][0] * m.m[3][2] * m.m[2][3]) +
	            m.m[0][2] * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[1][3] + m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[1][3] -
	                         m.m[2][0] * m.m[1][1] * m.m[3][3] - m.m[1][0] * m.m[3][1] * m.m[2][3]) -
	            m.m[0][3] * (m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[1][2] + m.m[3][0] * m.m[1][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[1][2] -
	                         m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[1][0] * m.m[3][1] * m.m[2][2]);

	if (det != 0.0f) {
		float invDet = 1.0f / det;
		result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[1][3] + m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[1][3] -
		                  m.m[2][1] * m.m[1][2] * m.m[3][3] - m.m[1][1] * m.m[3][2] * m.m[2][3]) *
		                 invDet;
		result.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[2][1] * m.m[3][2] * m.m[0][3] + m.m[3][1] * m.m[0][2] * m.m[2][3] - m.m[3][1] * m.m[2][2] * m.m[0][3] -
		                   m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[0][1] * m.m[3][2] * m.m[2][3]) *
		                 invDet;
		result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[1][1] * m.m[3][2] * m.m[0][3] + m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[1][2] * m.m[0][3] -
		                  m.m[1][1] * m.m[0][2] * m.m[3][3] - m.m[0][1] * m.m[3][2] * m.m[1][3]) *
		                 invDet;
		result.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[1][1] * m.m[2][2] * m.m[0][3] + m.m[2][1] * m.m[0][2] * m.m[1][3] - m.m[2][1] * m.m[1][2] * m.m[0][3] -
		                   m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[0][1] * m.m[2][2] * m.m[1][3]) *
		                 invDet;

		result.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[1][3] + m.m[3][0] * m.m[1][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[1][3] -
		                   m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[1][0] * m.m[3][2] * m.m[2][3]) *
		                 invDet;
		result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[2][0] * m.m[3][2] * m.m[0][3] + m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[2][2] * m.m[0][3] -
		                  m.m[2][0] * m.m[0][2] * m.m[3][3] - m.m[0][0] * m.m[3][2] * m.m[2][3]) *
		                 invDet;
		result.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[1][0] * m.m[3][2] * m.m[0][3] + m.m[3][0] * m.m[0][2] * m.m[1][3] - m.m[3][0] * m.m[1][2] * m.m[0][3] -
		                   m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[0][0] * m.m[3][2] * m.m[1][3]) *
		                 invDet;
		result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[1][0] * m.m[2][2] * m.m[0][3] + m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[1][2] * m.m[0][3] -
		                  m.m[1][0] * m.m[0][2] * m.m[2][3] - m.m[0][0] * m.m[2][2] * m.m[1][3]) *
		                 invDet;

		result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[1][3] + m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[1][3] -
		                  m.m[2][0] * m.m[1][1] * m.m[3][3] - m.m[1][0] * m.m[3][1] * m.m[2][3]) *
		                 invDet;
		result.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[2][0] * m.m[3][1] * m.m[0][3] + m.m[3][0] * m.m[0][1] * m.m[2][3] - m.m[3][0] * m.m[2][1] * m.m[0][3] -
		                   m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[0][0] * m.m[3][1] * m.m[2][3]) *
		                 invDet;
		result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[1][0] * m.m[3][1] * m.m[0][3] + m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[1][1] * m.m[0][3] -
		                  m.m[1][0] * m.m[0][1] * m.m[3][3] - m.m[0][0] * m.m[3][1] * m.m[1][3]) *
		                 invDet;
		result.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[1][0] * m.m[2][1] * m.m[0][3] + m.m[2][0] * m.m[0][1] * m.m[1][3] - m.m[2][0] * m.m[1][1] * m.m[0][3] -
		                   m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[0][0] * m.m[2][1] * m.m[1][3]) *
		                 invDet;

		result.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[1][2] + m.m[3][0] * m.m[1][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[1][2] -
		                   m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[1][0] * m.m[3][1] * m.m[2][2]) *
		                 invDet;
		result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[2][0] * m.m[3][1] * m.m[0][2] + m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[2][1] * m.m[0][2] -
		                  m.m[2][0] * m.m[0][1] * m.m[3][2] - m.m[0][0] * m.m[3][1] * m.m[2][2]) *
		                 invDet;
		result.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[1][0] * m.m[3][1] * m.m[0][2] + m.m[3][0] * m.m[0][1] * m.m[1][2] - m.m[3][0] * m.m[1][1] * m.m[0][2] -
		                   m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[0][0] * m.m[3][1] * m.m[1][2]) *
		                 invDet;
		result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[1][0] * m.m[2][1] * m.m[0][2] + m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[1][1] * m.m[0][2] -
		                  m.m[1][0] * m.m[0][1] * m.m[2][2] - m.m[0][0] * m.m[2][1] * m.m[1][2]) *
		                 invDet;
	}
	return result;
}

// 転置行列
Matrix4x4 MyMathUtility::Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 MyMathUtility::MakeIdentity4x4() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}
	return result;
}

// 座標変換
Vector3 MyMathUtility::Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f); // wが0になるとゼロ除算が発生するためストップさせる

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

// X軸の回転行列
Matrix4x4 MyMathUtility::MakeRotateXMatrix(float radian) {
	Matrix4x4 result{};
	float cos = std::cos(radian);
	float sin = std::sin(radian);

	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = cos;
	result.m[1][2] = sin;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = -sin;
	result.m[2][2] = cos;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// Y軸の回転行列
Matrix4x4 MyMathUtility::MakeRotateYMatrix(float radian) {
	Matrix4x4 result{};
	float cos = std::cos(radian);
	float sin = std::sin(radian);

	result.m[0][0] = cos;
	result.m[0][1] = 0.0f;
	result.m[0][2] = -sin;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = sin;
	result.m[2][1] = 0.0f;
	result.m[2][2] = cos;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// Z軸の回転行列
Matrix4x4 MyMathUtility::MakeRotateZMatrix(float radian) {
	Matrix4x4 result{};
	float cos = std::cos(radian);
	float sin = std::sin(radian);

	result.m[0][0] = cos;
	result.m[0][1] = sin;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = -sin;
	result.m[1][1] = cos;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MyMathUtility::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// 1. 各要素の行列を作成
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	// 2. 回転行列を合成 (X * Y * Z)
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	// 3. 拡大縮小 * 回転 * 平行移動 の順で合成 (S * R * T)
	Matrix4x4 result = Multiply(scaleMatrix, Multiply(rotateXYZMatrix, translateMatrix));

	return result;
}

// 透視投影行列 (Perspective Projection Matrix)
Matrix4x4 MyMathUtility::MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};
	float cot = 1.0f / std::tan(fovY / 2.0f);
	result.m[0][0] = (1.0f / aspectRatio) * cot;
	result.m[1][1] = cot;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

// 正射影行列 (Orthographic Projection Matrix)
Matrix4x4 MyMathUtility::MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;
	return result;
}

// ビューポート変換行列 (Viewport Matrix)
Matrix4x4 MyMathUtility::MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result{};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + (width / 2.0f);
	result.m[3][1] = top + (height / 2.0f);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;
	return result;
}

// 内積の計算
float MyMathUtility::Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// ベクトルの減算
Vector3 MyMathUtility::Subtract(const Vector3& v1, const Vector3& v2) { return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z}; }

// ベクトルの加算
Vector3 MyMathUtility::Add(const Vector3& v1, const Vector3& v2) { return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; }

// ベクトルの長さの二乗（割る処理の高速化・最適化用）
float MyMathUtility::LengthSquared(const Vector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

// 正射影ベクトルを求める
Vector3 MyMathUtility::Project(const Vector3& v1, const Vector3& v2) {
	float lengthSq = LengthSquared(v2);
	if (lengthSq == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	float t = Dot(v1, v2) / lengthSq;
	return {v2.x * t, v2.y * t, v2.z * t};
}

// 点から線分への最近接点を求める
Vector3 MyMathUtility::ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 v1 = Subtract(point, segment.origin);
	float lengthSq = LengthSquared(segment.diff);
	float t = 0.0f;
	if (lengthSq != 0.0f) {
		t = Dot(v1, segment.diff) / lengthSq;
	}
	t = std::clamp(t, 0.0f, 1.0f); // 線分なので0～1の範囲に制限

	return {segment.origin.x + segment.diff.x * t, segment.origin.y + segment.diff.y * t, segment.origin.z + segment.diff.z * t};
}

float MyMathUtility::Length(const Vector3& v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

// 数値 × ベクトルの計算
Vector3 MyMathUtility::Multiply(float scalar, const Vector3& v) { return {scalar * v.x, scalar * v.y, scalar * v.z}; }

// 外積（クロス積）の計算
Vector3 MyMathUtility::Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

// 正規化（長さを1にする） 
Vector3 MyMathUtility::Normalize(const Vector3& v) {
	float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len != 0.0f) {
		return {v.x / len, v.y / len, v.z / len};
	}
	return v;
}