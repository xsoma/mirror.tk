#pragma once

#include "Vector.h"
#include "SDK.h"
#include <stdint.h>

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define RADPI 57.295779513082f
#define rad(a) a * 0.01745329251

void AngleVectors(const vector &angles, vector *forward);
void VectorTransform(const vector in1, float in2[3][4], vector &out);
void vect_transform(const vector& in1, const matrix3x4& in2, vector& out);
void SinCos(float a, float* s, float*c);
void VectorAngles(vector forward, vector &angles);
void AngleVectors(const vector &angles, vector *forward, vector *right, vector *up);
void Normalize(vector &vIn, vector &vOut);
void CalcAngle(vector src, vector dst, vector &angles);
bool IsVisible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID);
void CalcAngleYawOnly(vector src, vector dst, vector &angles);
void NormalizeVector(vector& vec);

inline vector angle_vector(vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));

	return vector(cp*cy, cp*sy, -sp);
}

inline float distance_point_to_line(vector Point, vector LineOrigin, vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

class kms
{
public:
	void CalcAngle1(vector src, vector &angles);
};

namespace Math_trash
{
	float distance_to_ray(const vector & pos, const vector & rayStart, const vector & rayEnd, float * along, vector * pointOnRay);
	//	void CalcAngle1(Vector src, Vector &angles);
	QAngle CalcAngleTrash(vector src, vector dst);
};