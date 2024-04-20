#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x = 0;
	float y = 0;
	float z = 0;

	// ADD----------------------------------
	Vector3 operator+(const Vector3& obj) const {
		Vector3 result;
		result.x = x + obj.x;
		result.y = y + obj.y;
		result.z = z + obj.z;

		return result;
	}
	void operator+=(const Vector3& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
	}
	Vector3 operator+(float obj) const {
		Vector3 result;
		result.x = x + obj;
		result.y = y + obj;
		result.z = z + obj;

		return result;
	}
	void operator+=(float obj) {
		x += obj;
		y += obj;
		z += obj;
	}

	// SUB--------------------------
	Vector3 operator-(const Vector3& obj) const {
		Vector3 result;
		result.x = x - obj.x;
		result.y = y - obj.y;
		result.z = z - obj.z;

		return result;
	}
	void operator-=(const Vector3& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
	}
	Vector3 operator-(float obj) const {
		Vector3 result;
		result.x = x - obj;
		result.y = y - obj;
		result.z = z - obj;

		return result;
	}
	void operator-=(float obj) {
		x -= obj;
		y -= obj;
		z -= obj;
	}

	// MULTIPLY-----------------------------
	Vector3 operator*(const Vector3& obj) const {
		Vector3 result;
		result.x = x * obj.x;
		result.y = y * obj.y;
		result.z = z * obj.z;

		return result;
	}
	void operator*=(const Vector3& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
	}
	Vector3 operator*(float obj) const {
		Vector3 result;
		result.x = x * obj;
		result.y = y * obj;
		result.z = z * obj;

		return result;
	}
	void operator*=(float obj) {
		x *= obj;
		y *= obj;
		z *= obj;
	}
	// DEVIDE---------------------------------
	Vector3 operator/(float obj) const { return {x / obj, y / obj, z / obj}; }
	Vector3 operator/(const Vector3& obj) const { return {x / obj.x, y / obj.y, z / obj.z}; }
	void operator/=(float obj) {
		x = x / obj;
		y = y / obj;
		z = z / obj;
	}
	void operator/=(const Vector3& obj) {
		x = x / obj.x;
		y = y / obj.y;
		z = z / obj.z;
	}
};