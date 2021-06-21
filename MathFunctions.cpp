#include "MathFunctions.h"
#include "CommonIncludes.h"

void AngleVectors(const vector &angles, vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}
void vect_transform(const vector& in1, const matrix3x4& in2, vector& out)
{
	out[0] = DotProduct(in1, vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}
void VectorTransform(const vector in1, float in2[3][4], vector &out)
{
	out[0] = DotProduct(in1, vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = DotProduct(in1, vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = DotProduct(in1, vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

void SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}


float Math_trash::distance_to_ray(const vector& pos, const vector& rayStart, const vector& rayEnd, float* along,
	vector* pointOnRay) {
	vector to = pos - rayStart;
	vector dir = rayEnd - rayStart;
	float Length = dir.NormalizeInPlace();

	float rangeAlong = dir.Dot(to);
	if (along)
		*along = rangeAlong;

	float range;

	if (rangeAlong < 0.0f) {
		range = -(pos - rayStart).Length();

		if (pointOnRay)
			*pointOnRay = rayStart;
	}
	else if (rangeAlong > Length) {
		range = -(pos - rayEnd).Length();

		if (pointOnRay)
			*pointOnRay = rayEnd;
	}
	else {
		vector onRay = rayStart + rangeAlong * dir;
		range = (pos - onRay).Length();

		if (pointOnRay)
			*pointOnRay = onRay;
	}

	return range;
}

void VectorAngles(vector forward, vector &angles)
{
	float tmp, yaw, pitch;

	if (forward[2] == 0 && forward[0] == 0)
	{
		yaw = 0;

		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / 3.141);

		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / PI);

		if (pitch < 0)
			pitch += 360;
	}

	if (pitch > 180)
		pitch -= 360;
	else if (pitch < -180)
		pitch += 360;

	if (yaw > 180)
		yaw -= 360;
	else if (yaw < -180)
		yaw += 360;

	if (pitch > 89)
		pitch = 89;
	else if (pitch < -89)
		pitch = -89;

	if (yaw > 180)
		yaw = 180;
	else if (yaw < -180)
		yaw = -180;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}



void AngleVectors(const vector &angles, vector *forward, vector *right, vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

void Normalize(vector &vIn, vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0){
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}


void CalcAngle(vector src, vector dst, vector &angles)
{
	vector delta = src - dst;
	double hyp = delta.length2d(); 
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}

QAngle CalcAngleTrash(vector src, vector dst)
{
	QAngle angles;
	vector delta = src - dst;

	VectorAngles(delta, angles);

	Normalize;

	return angles;
}

void kms::CalcAngle1(vector src, vector &angles)
{
	vector delta = src;
	double hyp = delta.length2d();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}


bool IsVisible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID)
{
	return 0;
}

void NormalizeVector(vector& vec) {
	for (int i = 0; i < 3; ++i) {
		while (vec[i] > 180.f)
			vec[i] -= 360.f;

		while (vec[i] < -180.f)
			vec[i] += 360.f;
	}
	vec[2] = 0.f;
}

void CalcAngleYawOnly(vector src, vector dst, vector &angles)
{
	vector delta = src - dst;
	double hyp = delta.length2d();
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}

QAngle Math_trash::CalcAngleTrash(vector src, vector dst)
{
	QAngle angles;
	vector delta = src - dst;

	VectorAngles(delta, angles);

	Normalize;

	return angles;
}

