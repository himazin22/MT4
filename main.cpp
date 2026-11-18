#include <Novice.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>

struct Vec2 {
	float x, y;
};

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// ---------- 台の形と導関数 ----------
float f(float x) { return 0.125f * x * x * x + 0.25f * x * x; }
float fPrime(float x) { return 0.375f * x * x + 0.5f * x; } // 手で微分した結果

// ---------- ワールド(m, y上) → スクリーン(px, y下) ----------
Vec2 ToScreen(float x, float y) { return {400.0f + 240.0f * x, 640.0f - 240.0f * y}; }

// ---------- 正規化 ----------
Vec2 Normalize(Vec2 v) {
	float len = std::sqrt(v.x * v.x + v.y * v.y);
	if (len == 0.0f)
		return {0.0f, 0.0f};
	return {v.x / len, v.y / len};
}

const char kWindowTitle[] = "Jump Ramp";

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	char keys[256] = {0};
	char preKeys[256] = {0};

	float x = 0.0f;            // バイクの位置(m)
	const float kSpeed = 1.0f; // 1 m/s
	const float kDeltaTime = 1.0f / 60.0f;
	const float kHalfLength = 0.15f;  // 車体の長さの半分(m)
	const float kWheelRadius = 10.0f; // 車輪の半径(px)
	const int kDivision = 200;        // 折れ線の分割数

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// ===== 更新 =====
		if (keys[DIK_D]) {
			x += kSpeed * kDeltaTime;
		}

		if (keys[DIK_A]) {
			x -= kSpeed * kDeltaTime;
		}

		x = std::clamp(x, 0.0f, 2.0f); // 0〜2に収める

		float y = f(x);
		float slope = fPrime(x);
		Vec2 forward = Normalize({1.0f, slope}); // 台に沿った向き

		ImGui::Begin("Jump Ramp");
		ImGui::Text("A / D : move");
		ImGui::Text("x: %.3f", x);
		ImGui::Text("f'(x): %.3f", slope);
		ImGui::Text("Forward: (%.3f, %.3f)", forward.x, forward.y);
		ImGui::End();

		// ===== 描画 =====
		// 地面 (y = 0)
		Vec2 g0 = ToScreen(-2.0f, 0.0f);
		Novice::DrawLine(0, (int)g0.y, kWindowWidth, (int)g0.y, WHITE);

		// 台（折れ線）
		for (int i = 0; i < kDivision; ++i) {
			float x0 = 2.0f * i / kDivision;
			float x1 = 2.0f * (i + 1) / kDivision;
			Vec2 p0 = ToScreen(x0, f(x0));
			Vec2 p1 = ToScreen(x1, f(x1));
			Novice::DrawLine((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y, WHITE);
		}
		// 台の右端の縦線 (x = 2)
		Vec2 top = ToScreen(2.0f, f(2.0f));
		Vec2 bottom = ToScreen(2.0f, 0.0f);
		Novice::DrawLine((int)top.x, (int)top.y, (int)bottom.x, (int)bottom.y, WHITE);

		// バイク：前輪と後輪（ワールド座標で計算してから変換）
		Vec2 front = ToScreen(x + forward.x * kHalfLength, y + forward.y * kHalfLength);
		Vec2 rear = ToScreen(x - forward.x * kHalfLength, y - forward.y * kHalfLength);

		Novice::DrawLine((int)rear.x, (int)rear.y, (int)front.x, (int)front.y, 0xFFA500FF);
		Novice::DrawEllipse((int)front.x, (int)front.y, (int)kWheelRadius, (int)kWheelRadius, 0.0f, 0xFFA500FF, kFillModeSolid);
		Novice::DrawEllipse((int)rear.x, (int)rear.y, (int)kWheelRadius, (int)kWheelRadius, 0.0f, 0xFFA500FF, kFillModeSolid);

		Novice::EndFrame();
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
			break;
	}

	Novice::Finalize();
	return 0;
}