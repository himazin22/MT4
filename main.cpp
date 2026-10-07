#include <Novice.h>
#define _USE_MATH_DEFINES
#include <numbers>
#include <cmath>
#include <algorithm>
#include "MyMathUtility.h"
#include <assert.h>
#include <imgui.h>

using namespace KamataEngine;

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

struct Spherical {
	float radius;
	float theta;
	float phi;
};

Vector3 ToCartesian(const Spherical& s) { 
	float rho = s.radius * cos(s.theta);
	return {rho * cos(s.phi), s.radius * sin(s.theta), rho * sin(s.phi)};
}

Spherical ToSpherical(const Vector3& p) { float r = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
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

const char kWindowTitle[] = "LE2C_18_ツノダ_タケマサ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// マウス位置保持用
	int currentMouseX = 0;
	int currentMouseY = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;
	bool isFirstClick = true;

	// デバッグカメラ用の初期位置
	Vector3 cameraTranslate{0.0f, 4.0f, -10.0f};
	Vector3 cameraRotate{0.45f, 0.0f, 0.0f};
	float cameraSpeed = 0.08f;
	float mouseSensitivity = 0.005f;

	Spherical s{6.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f};

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

		// ==================================================
		// FPSスタイル・デバッグカメラ操作
		// ==================================================
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

		// ==================================================
		// カメラ操作での中心と真上・真下を避ける制限
		// ==================================================
		// 真上(theta = pi/2)や真下(-pi/2)では世界の上と前Fが平行になり右Rの軸を作れないため制限する
		// 中心(r = 0)では注視点とカメラが重なり前Fが定まらないため制限する[cite: 11]
		const float limit = std::numbers::pi_v<float> / 2.0f - 0.01f;
		s.radius = (std::max)(s.radius, 0.1f);
		s.theta = std::clamp(s.theta, -limit, limit);

		// ==================================================
		// 球面座標からカメラ位置(eye)の計算
		// ==================================================

		Vector3 target = {0.0f, 0.0f, 0.0f};

		Vector3 offset = ToCartesian(s);
		Vector3 eye = MyMathUtility::Add(target, offset);

		// ==================================================
		// 注視点を向くカメラのワールド行列の作成
		// ==================================================
		Vector3 worldUp = {0.0f, 1.0f, 0.0f};

		// 1. 注視点 - カメラ位置で前 F を求める
		Vector3 forward = MyMathUtility::Normalize(MyMathUtility::Subtract(target, eye));

		// 2. 世界の上と前 F の外積で右 R を求める[cite: 9]
		Vector3 right = MyMathUtility::Normalize(MyMathUtility::Cross(worldUp, forward));

		// 3. 前 F と右 R の外積でカメラ自身の上 U を再計算する[cite: 9]
		Vector3 up = MyMathUtility::Cross(forward, right);

		// 求めた右・上・前とカメラ位置を行列に格納する[cite: 6]
		Matrix4x4 cameraMatrix = {right.x, right.y, right.z, 0.0f, up.x, up.y, up.z, 0.0f, forward.x, forward.y, forward.z, 0.0f, eye.x, eye.y, eye.z, 1.0f}; //[cite: 10]
		
		// ===================================
		// ImGui の処理
		// ===================================
		ImGui::Begin("Spherical Coordinates");

		ImGui::Text("Target: (0, 0, 0) / +Y up / Camera +Z forward");

		// 球面座標(角度はrad表記)を編集可能にする
		ImGui::DragFloat("Radius", &s.radius, 0.01f);
		ImGui::DragFloat("Theta: elevation (rad)", &s.theta, 0.01f);
		ImGui::DragFloat("Phi (rad)", &s.phi, 0.01f);

		// 変換した直交座標と、作成したカメラ行列(4x4)を表示する
		ImGui::Text("Spherical: r = %.3f, theta = %.3f rad, phi = %.3f rad", s.radius, s.theta, s.phi);
		ImGui::Text("Cartesian: x = %.3f, y = %.3f, z = %.3f", eye.x, eye.y, eye.z);                  

		ImGui::Text("Camera matrix"); 
		ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[0][0], cameraMatrix.m[0][1], cameraMatrix.m[0][2], cameraMatrix.m[0][3]);
		ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[1][0], cameraMatrix.m[1][1], cameraMatrix.m[1][2], cameraMatrix.m[1][3]);
		ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[2][0], cameraMatrix.m[2][1], cameraMatrix.m[2][2], cameraMatrix.m[2][3]);
		ImGui::Text("    %.3f    %.3f    %.3f    %.3f", cameraMatrix.m[3][0], cameraMatrix.m[3][1], cameraMatrix.m[3][2], cameraMatrix.m[3][3]);

		ImGui::End();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		// ビュー・プロジェクション計算
		Matrix4x4 viewMatrix = MyMathUtility::Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MyMathUtility::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = MyMathUtility::Multiply(viewMatrix, projectionMatrix);

		Matrix4x4 viewportMatrix = MyMathUtility::MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);



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
