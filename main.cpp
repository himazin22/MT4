#include <Novice.h>
#define _USE_MATH_DEFINES
#include <Novice.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#define _USE_MATH_DEFINES
#include "MyMathUtility.h"
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <cmath>
#include <imgui.h>
#include <numbers>

using namespace KamataEngine;

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

struct Spherical {
	float radius;
	float theta;
	float phi;
};

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	Quaternion result;

	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;

	// スカラー部 (w) の計算
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	return result;
}
// 単位クォータニオン（無回転を表す）
Quaternion IdentityQuaternion() { return {0.0f, 0.0f, 0.0f, 1.0f}; }

// 共役クォータニオン（回転軸を逆向きにする）
Quaternion Conjugate(const Quaternion& quaternion) { return {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w}; }

// クォータニオンのノルム（長さ）
float Norm(const Quaternion& quaternion) { return std::sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w); }

// クォータニオンの正規化（長さを1にする。回転計算に必須）
Quaternion Normalize(const Quaternion& quaternion) {
	float norm = Norm(quaternion);
	if (norm > 0.0f) {
		return {quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm};
	}
	return quaternion;
}
// 逆クォータニオン（逆回転を表す）
Quaternion Inverse(const Quaternion& quaternion) {
	float normSq = quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w;
	if (normSq > 0.0f) {
		Quaternion conj = Conjugate(quaternion);
		return {conj.x / normSq, conj.y / normSq, conj.z / normSq, conj.w / normSq};
	}
	return quaternion;
}

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion q;

	float halfAngle = angle * 0.5f;

	float sinHalf = std::sin(halfAngle);
	float cosHalf = std::cos(halfAngle);
	q.x = axis.x * sinHalf;
	q.y = axis.y * sinHalf;
	q.z = axis.z * sinHalf;

	q.w = cosHalf;

	return q;
}

Vector3 RotateVector(const Vector3& vector, Quaternion& quaternion) {
	float qx = quaternion.x;
	float qy = quaternion.y;
	float qz = quaternion.z;
	float qw = quaternion.w;

	Vector3 uv;

	uv.x = qy * vector.z - qz * vector.y;
	uv.y = qz * vector.x - qx * vector.z;
	uv.z = qx * vector.y - qy * vector.x;

	Vector3 uuv;

	uuv.x = qy * uv.z - qz * uv.y;
	uuv.y = qz * uv.x - qx * uv.z;
	uuv.z = qx * uv.y - qy * uv.x;

	Vector3 result;

	result.x = vector.x + (uv.x * qw + uuv.x) * 2.0f;
	result.y = vector.y + (uv.y * qw + uuv.y) * 2.0f;
	result.z = vector.z + (uv.z * qw + uuv.z) * 2.0f;

	return result;
}

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 mat;

	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;

	float xy = quaternion.x * quaternion.y;
	float xz = quaternion.x * quaternion.z;
	float yz = quaternion.y * quaternion.z;

	float wx = quaternion.w * quaternion.x;
	float wy = quaternion.w * quaternion.y;
	float wz = quaternion.w * quaternion.z;

	// 1行目
	mat.m[0][0] = 1.0f - 2.0f * (yy + zz);
	mat.m[0][1] = 2.0f * (xy + wz); // 修正: -wz から +wz
	mat.m[0][2] = 2.0f * (xz - wy); // 修正: +wy から -wy
	mat.m[0][3] = 0.0f;

	// 2行目
	mat.m[1][0] = 2.0f * (xy - wz); // 修正: +wz から -wz
	mat.m[1][1] = 1.0f - 2.0f * (xx + zz);
	mat.m[1][2] = 2.0f * (yz + wx); // 修正: -wx から +wx
	mat.m[1][3] = 0.0f;

	// 3行目
	mat.m[2][0] = 2.0f * (xz + wy); // 修正: -wy から +wy
	mat.m[2][1] = 2.0f * (yz - wx); // 修正: +wx から -wx
	mat.m[2][2] = 1.0f - 2.0f * (xx + yy);
	mat.m[2][3] = 0.0f;

	// 4行目
	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 1.0f;

	return mat;
}
Vector3 ToCartesian(const Spherical& s) {
	float rho = s.radius * cos(s.theta);
	return {rho * cos(s.phi), s.radius * sin(s.theta), rho * sin(s.phi)};
}

enum InterpolationMode { Linear, Exponential };

Spherical ToSpherical(const Vector3& p) {
	float r = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	if (r == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}
	float sinTheta = std::clamp(p.y / r, -1.0f, 1.0f);
	float phi = 0.0f;
	if (p.x != 0.0f || p.z != 0.0f) {
		phi = std::atan2(p.z, p.x);
	}
	return {r, std::asin(sinTheta), phi};
}

// 内積：2つの矢印がどれだけ同じ向きか（同じ向き=1、直角=0、真逆=-1）
float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

// fromの向きをtoの向きへ回す単位クォータニオンを返す
Quaternion DirectionToDirection(const Vector3& from, const Vector3& to) {
	const float kEpsilon = 1.0e-6f; // 「ほぼ同じ」と見なす小さい数

	float dot = Dot(from, to);

	// ① ほぼ同じ向き → 回さなくてOK
	if (dot >= 1.0f - kEpsilon) {
		return IdentityQuaternion();
	}

	// ② ほぼ真逆の向き → 軸が決まらないので、fromに垂直な軸を自分で作る
	if (dot <= -1.0f + kEpsilon) {
		Vector3 axis;
		if (std::fabs(from.x) > kEpsilon || std::fabs(from.y) > kEpsilon) {
			// fromに垂直な軸 (x, yを入れ替えて片方の符号を反転)
			axis = {from.y, -from.x, 0.0f};
		} else {
			// fromがほぼZ軸のときは別の垂直な軸を使う
			axis = {from.z, 0.0f, -from.x};
		}
		axis = MyMathUtility::Normalize(axis);
		// 180度(π)回す
		return MakeRotateAxisAngleQuaternion(axis, std::numbers::pi_v<float>);
	}

	// ③ ふつうの場合 → 外積が軸、acos(内積)が角度
	Vector3 axis = MyMathUtility::Normalize(MyMathUtility::Cross(from, to));
	float angle = std::acos(std::clamp(dot, -1.0f, 1.0f)); // acosは範囲外だとエラーになるのでclamp
	return MakeRotateAxisAngleQuaternion(axis, angle);
}

const int kRowHeight = 20;

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * 60, y + (row + 1) * kRowHeight, "%6.3f", matrix.m[row][column]);
		}
	}
}

const char kWindowTitle[] = "LE2C_18_ツノダ_タケマサ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// マウス位置保持用
	int currentMouseX = 0;
	int currentMouseY = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;
	bool isFirstClick = true;

	Quaternion q1 = {2.0f, 3.0f, 4.0f, 1.0f};
	Quaternion q2 = {1.0f, 3.0f, 5.0f, 2.0f};

	// --------------------------------------------------
	// デバッグカメラ用変数
	// --------------------------------------------------
	bool enableDebugCamera = false; // デバッグカメラのON/OFFフラグ
	Vector3 cameraTranslate{0.0f, 4.0f, -10.0f};
	Vector3 cameraRotate{0.45f, 0.0f, 0.0f};
	float cameraSpeed = 0.08f;
	float mouseSensitivity = 0.005f;

	Spherical s{6.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f};

	// --------------------------------------------------
	// 円の追従補間用変数
	// --------------------------------------------------
	// 1. 円 A (赤色・マウス座標ターゲット / 半径 12)
	Vector2 circle_target = {0.0f, 0.0f};
	float radiusA = 12.0f;

	// 2. 円 B (緑色・追従側 / 初期座標 640, 360 / 半径 20)
	Vector2 circle_pos = {640.0f, 360.0f};
	float radiusB = 20.0f;

	// 4. 追従速度パラメータ (speed)
	float speed = 10.0f;

	//  60fps 前提の固定 deltaTime
	const float deltaTime = 1.0f / 60.0f;

	// 補間モード (デフォルトは線形補間)
	int currentMode = InterpolationMode::Linear;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// マウス位置の更新
		prevMouseX = currentMouseX;
		prevMouseY = currentMouseY;
		Novice::GetMousePosition(&currentMouseX, &currentMouseY);

		// 円 A (ターゲット) の位置をマウス位置に設定
		circle_target.x = static_cast<float>(currentMouseX);
		circle_target.y = static_cast<float>(currentMouseY);

		if (currentMode == InterpolationMode::Linear) {
			// 線形補間: pos += (speed * deltaTime) * (target - pos)
			circle_pos.x += (speed * deltaTime) * (circle_target.x - circle_pos.x);
			circle_pos.y += (speed * deltaTime) * (circle_target.y - circle_pos.y);
		} else if (currentMode == InterpolationMode::Exponential) {
			// 指数補間: factor = 1 - exp(-speed * deltaTime)
			float factor = 1.0f - std::exp(-speed * deltaTime);
			circle_pos.x += factor * (circle_target.x - circle_pos.x);
			circle_pos.y += factor * (circle_target.y - circle_pos.y);
		}
		// [C]キーでデバッグカメラのON/OFF切り替え
		if (preKeys[DIK_C] == 0 && keys[DIK_C] != 0) {
			enableDebugCamera = !enableDebugCamera;
		}

		// ==================================================
		// FPSスタイル・デバッグカメラ操作 (ONの時のみ動作)
		// ==================================================
		if (enableDebugCamera) {
			if (Novice::IsPressMouse(1)) {
				if (isFirstClick) {
					isFirstClick = false;
				} else {
					float deltaX = float(currentMouseX - prevMouseX);
					float deltaY = float(currentMouseY - prevMouseY);

					cameraRotate.y += deltaX * mouseSensitivity;
					cameraRotate.x += deltaY * mouseSensitivity;

					cameraRotate.x = std::clamp(cameraRotate.x, -float(M_PI) / 2.1f, float(M_PI) / 2.1f);
				}
			} else {
				isFirstClick = true;
			}

			Matrix4x4 rotationMatrix = MyMathUtility::Multiply(MyMathUtility::MakeRotateXMatrix(cameraRotate.x), MyMathUtility::MakeRotateYMatrix(cameraRotate.y));

			Vector3 moveDir = {0.0f, 0.0f, 0.0f};
			if (keys[DIK_W])
				moveDir.z += 1.0f;
			if (keys[DIK_S])
				moveDir.z -= 1.0f;
			if (keys[DIK_D])
				moveDir.x += 1.0f;
			if (keys[DIK_A])
				moveDir.x -= 1.0f;

			if (moveDir.x != 0.0f || moveDir.z != 0.0f) {
				Vector3 transformedDir = MyMathUtility::Transform(moveDir, rotationMatrix);
				cameraTranslate.x += transformedDir.x * cameraSpeed;
				cameraTranslate.y += transformedDir.y * cameraSpeed;
				cameraTranslate.z += transformedDir.z * cameraSpeed;
			}

			if (keys[DIK_SPACE])
				cameraTranslate.y += cameraSpeed;
			if (keys[DIK_LSHIFT])
				cameraTranslate.y -= cameraSpeed;

			if (keys[DIK_R]) {
				cameraTranslate = {0.0f, 4.0f, -10.0f};
				cameraRotate = {0.45f, 0.0f, 0.0f};
			}
		}

		// カメラ座標の制限・計算
		const float limit = std::numbers::pi_v<float> / 2.0f - 0.01f;
		s.radius = (std::max)(s.radius, 0.1f);
		s.theta = std::clamp(s.theta, -limit, limit);

		Vector3 target = {0.0f, 0.0f, 0.0f};
		Vector3 offset = ToCartesian(s);
		Vector3 eye = MyMathUtility::Add(target, offset);

		Vector3 worldUp = {0.0f, 1.0f, 0.0f};
		Vector3 forward = MyMathUtility::Normalize(MyMathUtility::Subtract(target, eye));
		Vector3 right = MyMathUtility::Normalize(MyMathUtility::Cross(worldUp, forward));
		Vector3 up = MyMathUtility::Cross(forward, right);

		Matrix4x4 cameraMatrix = {right.x, right.y, right.z, 0.0f, up.x, up.y, up.z, 0.0f, forward.x, forward.y, forward.z, 0.0f, eye.x, eye.y, eye.z, 1.0f};

		// --------------------------------------------------
		// 新しいクォータニオンと行列の計算
		// --------------------------------------------------

		Vector3 from0 = MyMathUtility::Normalize(Vector3{1.0f, 0.7f, 0.5f});
		Vector3 to0 = {-from0.x, -from0.y, -from0.z}; // -from0 と同じ
		Vector3 from1 = MyMathUtility::Normalize(Vector3{-0.6f, 0.9f, 0.2f});
		Vector3 to1 = MyMathUtility::Normalize(Vector3{0.4f, 0.7f, -0.5f});

		// DirectionToDirectionはQuaternionを返すので、MakeRotateMatrixで行列にする
		Matrix4x4 rotateMatrix0 = MakeRotateMatrix(DirectionToDirection(MyMathUtility::Normalize(Vector3{1.0f, 0.0f, 0.0f}), MyMathUtility::Normalize(Vector3{-1.0f, 0.0f, 0.0f})));
		Matrix4x4 rotateMatrix1 = MakeRotateMatrix(DirectionToDirection(from0, to0));
		Matrix4x4 rotateMatrix2 = MakeRotateMatrix(DirectionToDirection(from1, to1));

		// ===================================
		// ImGui の処理
		// ===================================

		ImGui::Begin("Interpolation Controller");

		ImGui::Checkbox("Enable Debug Camera (Hotkey: C)", &enableDebugCamera);
		ImGui::Separator();

		ImGui::Text("Target: Mouse Position (Red Circle)");
		ImGui::Separator();

		// モードテキストの表示切り替え
		if (currentMode == InterpolationMode::Linear) {
			ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "[Linear Interpolation Mode]");
		} else {
			ImGui::TextColored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), "[Exponential Decay Mode]");
		}

		// ★ 補間方式切り替え用ラジオボタン
		ImGui::RadioButton("Linear (線形)", &currentMode, InterpolationMode::Linear);
		ImGui::SameLine();
		ImGui::RadioButton("Exponential (指数)", &currentMode, InterpolationMode::Exponential);

		ImGui::SliderFloat("Speed", &speed, 0.0f, 100.0f, "%.1f");

		float diffX = circle_target.x - circle_pos.x;
		float diffY = circle_target.y - circle_pos.y;
		float distance = std::sqrt(diffX * diffX + diffY * diffY);

		ImGui::Text("Mouse Pos: (%.1f, %.1f)", circle_target.x, circle_target.y);
		ImGui::Text("Circle Pos: (%.1f, %.1f)", circle_pos.x, circle_pos.y);
		ImGui::Text("Distance to target: %.1f px", distance);

		ImGui::Separator();

		if (ImGui::Button("Snap to Target")) {
			circle_pos = circle_target;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset to Center")) {
			circle_pos = {640.0f, 360.0f};
		}

		ImGui::End();

		// 球面座標・カメラ状態用のウィンドウ (デバッグカメラが有効な時に表示)
		if (enableDebugCamera) {
			ImGui::Begin("Spherical Coordinates");

			ImGui::Text("Target: (0, 0, 0) / +Y up / Camera +Z forward");

			ImGui::DragFloat("Radius", &s.radius, 0.01f);
			ImGui::DragFloat("Theta: elevation (rad)", &s.theta, 0.01f);
			ImGui::DragFloat("Phi (rad)", &s.phi, 0.01f);

			ImGui::Text("Spherical: r = %.3f, theta = %.3f rad, phi = %.3f rad", s.radius, s.theta, s.phi);
			ImGui::Text("Cartesian: x = %.3f, y = %.3f, z = %.3f", eye.x, eye.y, eye.z);

			ImGui::Text("Camera matrix");
			ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[0][0], cameraMatrix.m[0][1], cameraMatrix.m[0][2], cameraMatrix.m[0][3]);
			ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[1][0], cameraMatrix.m[1][1], cameraMatrix.m[1][2], cameraMatrix.m[1][3]);
			ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[2][0], cameraMatrix.m[2][1], cameraMatrix.m[2][2], cameraMatrix.m[2][3]);
			ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[3][0], cameraMatrix.m[3][1], cameraMatrix.m[3][2], cameraMatrix.m[3][3]);

			ImGui::End();
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// ビュー・プロジェクション計算 (将来的な3D描画用)
		Matrix4x4 viewMatrix = MyMathUtility::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MyMathUtility::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = MyMathUtility::Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MyMathUtility::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		// 3. 円 A と 円 B の中心を結ぶ線を描画して追従の遅れを可視化
		Novice::DrawLine(static_cast<int>(circle_target.x), static_cast<int>(circle_target.y), static_cast<int>(circle_pos.x), static_cast<int>(circle_pos.y), WHITE);

		// 2. 円 B (緑色 / 半径20)
		Novice::DrawEllipse(static_cast<int>(circle_pos.x), static_cast<int>(circle_pos.y), static_cast<int>(radiusB), static_cast<int>(radiusB), 0.0f, GREEN, kFillModeSolid);

		// 1. 円 A (赤色 / 半径12)
		Novice::DrawEllipse(static_cast<int>(circle_target.x), static_cast<int>(circle_target.y), static_cast<int>(radiusA), static_cast<int>(radiusA), 0.0f, RED, kFillModeSolid);

		MatrixScreenPrintf(0, 0, rotateMatrix0, "rotateMatrix0");
		MatrixScreenPrintf(0, kRowHeight * 5, rotateMatrix1, "rotateMatrix1");
		MatrixScreenPrintf(0, kRowHeight * 10, rotateMatrix2, "rotateMatrix2");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}