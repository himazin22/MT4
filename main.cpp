#include <Novice.h>
#include <cmath>
#include <cstring>
#include <imgui.h>

// ---------- 2次元ベクトル ----------
struct Vector2 {
	float x;
	float y;
};

// ---------- 演算子（使う関数より上に書く） ----------
// ベクトル + ベクトル
Vector2 operator+(const Vector2& a, const Vector2& b) { return {a.x + b.x, a.y + b.y}; }

// ベクトル - ベクトル
Vector2 operator-(const Vector2& a, const Vector2& b) { return {a.x - b.x, a.y - b.y}; }

// 数 * ベクトル
Vector2 operator*(float s, const Vector2& v) { return {s * v.x, s * v.y}; }

// ---------- 正規化（長さを1にする） ----------
Vector2 Normalize(const Vector2& v) {
	float length = std::sqrt(v.x * v.x + v.y * v.y);
	if (length == 0.0f) {
		return {0.0f, 0.0f}; // 0で割らないようにする
	}
	return {v.x / length, v.y / length};
}

// ---------- 2次ベジェ曲線：位置 B(t) ----------
Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t) {
	float u = 1.0f - t;
	return (u * u) * p0 + (2.0f * u * t) * p1 + (t * t) * p2;
}

// ---------- 2次ベジェ曲線：接線ベクトル B'(t) ----------
Vector2 BezierDerivative(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t) {
	float u = 1.0f - t;
	return (2.0f * u) * (p1 - p0) + (2.0f * t) * (p2 - p1);
}

const char kWindowTitle[] = "Bezier Road";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	char keys[256] = {0};
	char preKeys[256] = {0};

	// 道の制御点（画面座標）
	Vector2 p0 = {200.0f, 570.0f};
	Vector2 p1 = {800.0f, 70.0f};
	Vector2 p2 = {1080.0f, 570.0f};

	float t = 0.0f; // 曲線上の位置（0～1）

	const int kRoadSegments = 64;          // 道を分ける本数
	const float kDirectionLength = 120.0f; // 向きを表す線の長さ(px)
	const int kPointRadius = 10;           // 曲線上の点の半径(px)
	const int kControlRadius = 5;          // 制御点の半径(px)
	const unsigned int kColorRoad = 0xFFFFFFFF;
	const unsigned int kColorPoint = 0xFFA500FF;
	const unsigned int kColorDirection = 0x00FF00FF;
	const unsigned int kColorControl = 0x888888FF;

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		std::memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// ===== 更新 =====
		// 先にスライダーで t と p1 を変える
		ImGui::Begin("Bezier");
		ImGui::SliderFloat("t", &t, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("p1.x", &p1.x, 400.0f, 880.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("p1.y", &p1.y, 20.0f, 570.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		// 変えた後の値で計算する
		Vector2 position = Bezier(p0, p1, p2, t);
		Vector2 derivative = BezierDerivative(p0, p1, p2, t);

		ImGui::Text("B(t) : (%.2f, %.2f)", position.x, position.y);
		ImGui::Text("B'(t): (%.2f, %.2f)", derivative.x, derivative.y);
		ImGui::End();

		// 向きの線の終点 = 点の位置 + 120 * 長さ1の向き
		Vector2 directionEnd = position + kDirectionLength * Normalize(derivative);

		// ===== 描画 =====
		// 道
		for (int i = 0; i < kRoadSegments; ++i) {
			float t0 = static_cast<float>(i) / static_cast<float>(kRoadSegments);
			float t1 = static_cast<float>(i + 1) / static_cast<float>(kRoadSegments);
			Vector2 c0 = Bezier(p0, p1, p2, t0);
			Vector2 c1 = Bezier(p0, p1, p2, t1);
			Novice::DrawLine(static_cast<int>(c0.x), static_cast<int>(c0.y), static_cast<int>(c1.x), static_cast<int>(c1.y), kColorRoad);
		}

		// 制御点（確認用）
		const Vector2 controlPoints[3] = {p0, p1, p2};
		for (const Vector2& c : controlPoints) {
			Novice::DrawEllipse(static_cast<int>(c.x), static_cast<int>(c.y), kControlRadius, kControlRadius, 0.0f, kColorControl, kFillModeSolid);
		}

		// 向きの線
		Novice::DrawLine(static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(directionEnd.x), static_cast<int>(directionEnd.y), kColorDirection);

		// 曲線上の点
		Novice::DrawEllipse(static_cast<int>(position.x), static_cast<int>(position.y), kPointRadius, kPointRadius, 0.0f, kColorPoint, kFillModeSolid);

		Novice::EndFrame();
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}