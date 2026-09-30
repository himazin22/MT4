#pragma once
#include "KamataEngine.h"

// 線分を表す構造体（クラスの外、グローバル空間に定義します）
struct Segment {
	KamataEngine::Vector3 origin;
	KamataEngine::Vector3 diff;
};

// 名前が被らないように MyMathUtility に変更
class MyMathUtility {
public:
	static KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
	static KamataEngine::Matrix4x4 MakeRotationMatrix(const KamataEngine::Vector3& rotation);
	static KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
	static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

	static KamataEngine::Matrix4x4 Add(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	static KamataEngine::Matrix4x4 Subtract(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
	static KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4& m);
	static KamataEngine::Matrix4x4 Transpose(const KamataEngine::Matrix4x4& m);
	static KamataEngine::Matrix4x4 MakeIdentity4x4();
	static KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);

	static KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
	static KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
	static KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

	static KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

	static KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
	static KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	static KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	// 内積
	static float Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	// ベクトルの減算
	static KamataEngine::Vector3 Subtract(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	// ベクトルの加算
	static KamataEngine::Vector3 Add(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	// ベクトルの長さの二乗
	static float LengthSquared(const KamataEngine::Vector3& v);

	// 正射影ベクトル
	static KamataEngine::Vector3 Project(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
	// 最近接点
	static KamataEngine::Vector3 ClosestPoint(const KamataEngine::Vector3& point, const Segment& segment);

	// ベクトルの長さを求める
	static float Length(const KamataEngine::Vector3& v);

	// 既存の Multiply(Matrix4x4, Matrix4x4) などとは別に、以下の2つを追加します。
	static KamataEngine::Vector3 Multiply(float scalar, const KamataEngine::Vector3& v);

	// ベクトルの外積（クロス積）
	static KamataEngine::Vector3 Cross(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

	// ※正規化（Normalize）がまだ無い場合はこれも追加
	static KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);
};