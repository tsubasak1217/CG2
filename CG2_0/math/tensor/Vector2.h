#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x = 0;
	float y = 0;

	// ADD-----------------------------------
	Vector2 operator+(const Vector2& obj) const {
		Vector2 result;
		result.x = x + obj.x;
		result.y = y + obj.y;

		return result;
	}
	void operator+=(const Vector2& obj) {
		x += obj.x;
		y += obj.y;
	}
	Vector2 operator+(float obj) const {
		Vector2 result;
		result.x = x + obj;
		result.y = y + obj;

		return result;
	}
	void operator+=(float obj) {
		x += obj;
		y += obj;
	}

	// SUB---------------------------------
	Vector2 operator- (const Vector2& obj) const {
		Vector2 result;
		result.x = x - obj.x;
		result.y = y - obj.y;

		return result;
	}
	void operator-=(const Vector2& obj) {
		x -= obj.x;
		y -= obj.y;
	}
	Vector2 operator-(float obj) const {
		Vector2 result;
		result.x = x - obj;
		result.y = y - obj;

		return result;
	}
	void operator-=(float obj) {
		x -= obj;
		y -= obj;
	}

	// MULTIPLY-----------------------
	Vector2 operator*(const Vector2& obj) const {
		Vector2 result;
		result.x = x * obj.x;
		result.y = y * obj.y;

		return result;
	}
	void operator*=(const Vector2& obj) {
		x *= obj.x;
		y *= obj.y;
	}
	Vector2 operator*(float obj) const {
		Vector2 result;
		result.x = x * obj;
		result.y = y * obj;

		return result;
	}
	void operator*=(float obj) {
		x *= obj;
		y *= obj;
	}

	// DEVIDE---------------------------------
	Vector2 operator/(float obj) const {
		return  {
		y / obj,
		x / obj
		};
	}
	Vector2 operator/(const Vector2& obj) const {
		return  {
		x / obj.x,
		y / obj.y
		};
	}
	void operator/=(float obj) {
		x = x / obj;
		y = y / obj;
	}
	void operator/=(const Vector2& obj) {
		x = x / obj.x;
		y = y / obj.y;
	}
};