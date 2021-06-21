#ifndef _VECTORZ_H
#define _VECTORZ_H

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define rad(a) a * 0.01745329251
#define deg(a) a * 57.295779513082

#include <Windows.h>
#include <math.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <deque>
inline void SinCosX(const float rad, float &sin, float &cos)
{
	const __m128 _ps_fopi = _mm_set_ss(1.27323954473516f);

	const __m128 _ps_0p5 = _mm_set_ss(0.5f);
	const __m128 _ps_1 = _mm_set_ss(1.0f);

	const __m128 _ps_dp1 = _mm_set_ss(-0.7851562f);
	const __m128 _ps_dp2 = _mm_set_ss(-2.4187564849853515625e-4f);
	const __m128 _ps_dp3 = _mm_set_ss(-3.77489497744594108e-8f);

	const __m128 _ps_sincof_p0 = _mm_set_ss(2.443315711809948e-5f);
	const __m128 _ps_sincof_p1 = _mm_set_ss(8.3321608736e-3f);
	const __m128 _ps_sincof_p2 = _mm_set_ss(-1.6666654611e-1f);
	const __m128 _ps_coscof_p0 = _mm_set_ss(2.443315711809948e-5f);
	const __m128 _ps_coscof_p1 = _mm_set_ss(-1.388731625493765e-3f);
	const __m128 _ps_coscof_p2 = _mm_set_ss(4.166664568298827e-2f);

	const __m128i _pi32_1 = _mm_set1_epi32(1);
	const __m128i _pi32_i1 = _mm_set1_epi32(~1);
	const __m128i _pi32_2 = _mm_set1_epi32(2);
	const __m128i _pi32_4 = _mm_set1_epi32(4);

	const __m128 _mask_sign_raw = *(__m128*)&_mm_set1_epi32(0x80000000);
	const __m128 _mask_sign_inv = *(__m128*)&_mm_set1_epi32(~0x80000000);


	__m128  xmm3 = _mm_setzero_ps();
	__m128i emm0, emm2, emm4;

	__m128 sign_bit_cos, sign_bit_sin;

	__m128 x, y, z;
	__m128 y1, y2;

	__m128 a = _mm_set_ss(rad);

	x = _mm_and_ps(a, _mask_sign_inv);
	y = _mm_mul_ps(x, _ps_fopi);

	emm2 = _mm_cvtps_epi32(y);
	emm2 = _mm_add_epi32(emm2, _pi32_1);
	emm2 = _mm_and_si128(emm2, _pi32_i1);
	y = _mm_cvtepi32_ps(emm2);

	emm4 = emm2;

	emm0 = _mm_and_si128(emm2, _pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	__m128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

	emm2 = _mm_and_si128(emm2, _pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
	__m128 poly_mask = _mm_castsi128_ps(emm2);

	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp1));
	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp2));
	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp3));

	emm4 = _mm_sub_epi32(emm4, _pi32_2);
	emm4 = _mm_andnot_si128(emm4, _pi32_4);
	emm4 = _mm_slli_epi32(emm4, 29);

	sign_bit_cos = _mm_castsi128_ps(emm4);
	sign_bit_sin = _mm_xor_ps(_mm_and_ps(a, _mask_sign_raw), swap_sign_bit_sin);

	z = _mm_mul_ps(x, x);

	y1 = _mm_mul_ps(_ps_coscof_p0, z);
	y1 = _mm_add_ps(y1, _ps_coscof_p1);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_add_ps(y1, _ps_coscof_p2);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_sub_ps(y1, _mm_mul_ps(z, _ps_0p5));
	y1 = _mm_add_ps(y1, _ps_1);

	y2 = _mm_mul_ps(_ps_sincof_p0, z);
	y2 = _mm_add_ps(y2, _ps_sincof_p1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, _ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	xmm3 = poly_mask;

	__m128 ysin2 = _mm_and_ps(xmm3, y2);
	__m128 ysin1 = _mm_andnot_ps(xmm3, y1);

	sin = _mm_cvtss_f32(_mm_xor_ps(_mm_add_ps(ysin1, ysin2), sign_bit_sin));
	cos = _mm_cvtss_f32(_mm_xor_ps(_mm_add_ps(_mm_sub_ps(y1, ysin1), _mm_sub_ps(y2, ysin2)), sign_bit_cos));
}

// from the leaked sdk

#define  FORCEINLINE			__forceinline

typedef float vec_t;

inline vec_t BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t*>(&i);
}

inline float sqrt2(float sqr)
{
	float root = 0;

	__asm
	{
		sqrtss xmm0, sqr
		movss root, xmm0
	}

	return root;
}

#define FLOAT32_NAN_BITS     (unsigned long)0x7FC00000
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN

class vector
{
public:
	vec_t x, y, z;


	auto NormalizeX()
	{
		auto x_rev = this->x / 360.f;
		if (this->x > 180.f || this->x < -180.f)
		{
			x_rev = abs(x_rev);
			x_rev = round(x_rev);

			if (this->x < 0.f)
				this->x = (this->x + 360.f * x_rev);

			else
				this->x = (this->x - 360.f * x_rev);
		}

		auto y_rev = this->y / 360.f;
		if (this->y > 180.f || this->y < -180.f)
		{
			y_rev = abs(y_rev);
			y_rev = round(y_rev);

			if (this->y < 0.f)
				this->y = (this->y + 360.f * y_rev);

			else
				this->y = (this->y - 360.f * y_rev);
		}

		auto z_rev = this->z / 360.f;
		if (this->z > 180.f || this->z < -180.f)
		{
			z_rev = abs(z_rev);
			z_rev = round(z_rev);

			if (this->z < 0.f)
				this->z = (this->z + 360.f * z_rev);

			else
				this->z = (this->z - 360.f * z_rev);
		}
	}


	vector(void);
	vector(vec_t X, vec_t Y, vec_t Z);
	explicit vector(vec_t XYZ);

	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f);

	bool IsValid() const;
	void Invalidate();

	vec_t   operator[](int i) const;
	vec_t&  operator[](int i);

	vec_t* Base();
	vec_t const* Base() const;

	inline void Zero();

	bool operator==(const vector& v) const;
	bool operator!=(const vector& v) const;
	bool operator!() { return !x && !y && !z; }

	FORCEINLINE vector& operator+=(const vector& v);
	FORCEINLINE vector& operator-=(const vector& v);
	FORCEINLINE vector& operator*=(const vector& v);
	FORCEINLINE vector& operator*=(float s);
	FORCEINLINE vector& operator/=(const vector& v);
	FORCEINLINE vector& operator/=(float s);
	FORCEINLINE vector& operator+=(float fl);
	FORCEINLINE vector& operator-=(float fl);
	inline float Long() { return sqrt2(x*x + y*y + z*z); }

	void Negate();

	inline vec_t Length() const;

	inline vector Angle(vector* up = 0)
	{
		if (!x && !y)
			return vector(0, 0, 0);

		double roll = 0;

		if (up)
		{
			vector left = (*up).Cross(*this);

			roll = deg(atan2f(left.z, (left.y * x) - (left.x * y)));
		}

		return vector(deg(atan2f(-z, sqrtf(x*x + y*y))), deg(atan2f(y, x)), roll);
	}

	inline float vector::Dist(const vector &vOther)
	{
		vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.Length();
	}
	inline bool empty()
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
	inline vector Forward()
	{
		float cp, cy, sp, sy;

		SinCosX(rad(x), sp, cp);
		SinCosX(rad(y), sy, cy);

		return vector(cp*cy, cp*sy, -sp);
	}

	FORCEINLINE vec_t LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}
	inline void clearVekt()
	{
		this->x = this->y = this->z = 0.0f;
	}

	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}

	float vector::Size()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}


	vec_t NormalizeInPlace();
	vector Normalized() const;

	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float val) const;

	FORCEINLINE bool WithinAABox(vector const &boxmin, vector const &boxmax);

	vec_t DistTo(const vector& vOther) const;

	FORCEINLINE vec_t DistToSqr(const vector& vOther) const
	{
		vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	void CopyToArray(float* rgfl) const;
	void MulAdd(const vector& a, const vector& b, float scalar);

	vec_t Dot(const vector& vOther) const;

	vector& operator=(const vector& vOther);

	vec_t length2d(void) const;
	vec_t Length2DSqr(void) const;

	vector operator-(void) const;
	vector operator+(const vector& v) const;
	vector operator-(const vector& v) const;
	vector operator*(const vector& v) const;
	vector operator/(const vector& v) const;
	vector operator*(float fl) const;
	vector operator/(float fl) const;

	vector Cross(const vector& vOther) const;

	vector Min(const vector& vOther) const;
	vector Max(const vector& vOther) const;
	vector Normalize();
};

using QAngle = vector;

FORCEINLINE vector ReplicateToVector(float x)
{
	return vector(x, x, x);
}

inline vector::vector(void)
{

}

inline vector::vector(vec_t X, vec_t Y, vec_t Z)
{
	x = X; y = Y; z = Z;
}

inline vector::vector(vec_t XYZ)
{
	x = y = z = XYZ;
}

inline void vector::Init(vec_t ix, vec_t iy, vec_t iz)
{
	x = ix; y = iy; z = iz;
}

inline void vector::Zero()
{
	x = y = z = 0.0f;
}

inline void VectorClear(vector& a)
{
	a.x = a.y = a.z = 0.0f;
}

inline vector& vector::operator=(const vector& vOther)
{
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}

inline vec_t& vector::operator[](int i)
{
	return ((vec_t*)this)[i];
}

inline vec_t vector::operator[](int i) const
{
	return ((vec_t*)this)[i];
}



inline vec_t* vector::Base()
{
	return (vec_t*)this;
}

inline vec_t const* vector::Base() const
{
	return (vec_t const*)this;
}

inline bool vector::IsValid() const
{
	return (x == x && y == y && z == z);
}

inline void vector::Invalidate()
{
	x = y = z = VEC_T_NAN;
}

inline bool vector::operator==(const vector& src) const
{
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool vector::operator!=(const vector& src) const
{
	return (src.x != x) || (src.y != y) || (src.z != z);
}

FORCEINLINE void VectorCopy(const vector& src, vector& dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline void vector::CopyToArray(float* rgfl) const
{
	rgfl[0] = x; rgfl[1] = y; rgfl[2] = z;
}

inline void vector::Negate()
{
	x = -x; y = -y; z = -z;
}

FORCEINLINE vector& vector::operator+=(const vector& v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}

FORCEINLINE vector& vector::operator-=(const vector& v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

FORCEINLINE vector& vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	return *this;
}

FORCEINLINE vector& vector::operator*=(const vector& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

FORCEINLINE vector& vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	return *this;
}

FORCEINLINE vector& vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	return *this;
}

FORCEINLINE vector& vector::operator/=(float fl)
{
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	return *this;
}

FORCEINLINE vector& vector::operator/=(const vector& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

FORCEINLINE void VectorAdd(const vector& a, const vector& b, vector& c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

FORCEINLINE void VectorSubtract(const vector& a, const vector& b, vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

FORCEINLINE void VectorMultiply(const vector& a, vec_t b, vector& c)
{
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

FORCEINLINE void VectorMultiply(const vector& a, const vector& b, vector& c)
{
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}

inline void VectorScale(const vector& in, vec_t scale, vector& result)
{
	VectorMultiply(in, scale, result);
}

FORCEINLINE void VectorDivide(const vector& a, vec_t b, vector& c)
{
	vec_t oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
}

FORCEINLINE void VectorDivide(const vector& a, const vector& b, vector& c)
{
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
}

inline void vector::MulAdd(const vector& a, const vector& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
	z = a.z + b.z * scalar;
}

inline void VectorLerp(const vector& src1, const vector& src2, vec_t t, vector& dest)
{
	dest.x = src1.x + (src2.x - src1.x) * t;
	dest.y = src1.y + (src2.y - src1.y) * t;
	dest.z = src1.z + (src2.z - src1.z) * t;
}

FORCEINLINE vec_t DotProduct(const vector& a, const vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline vec_t vector::Dot(const vector& vOther) const
{
	return DotProduct(*this, vOther);
}

inline void CrossProduct(const vector& a, const vector& b, vector& result)
{
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

//inline vec_t DotProductAbs(const Vector& v0, const Vector& v1)
//{
//	return abs(v0.x * v1.x) + abs(v0.y * v1.y) + abs(v0.z * v1.z);
//}

inline vec_t VectorLength(const vector& v)
{
	return (vec_t)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline vec_t vector::Length(void) const
{
	return VectorLength(*this);
}

inline vec_t VectorNormalize(vector& v)
{
	vec_t l = v.Length();

	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; v.z = 1.0f;
	}

	return l;
}

FORCEINLINE float VectorNormalizer(float * v)
{
	return VectorNormalize(*(reinterpret_cast<vector *>(v)));
}
inline vec_t vector::NormalizeInPlace()
{
	return VectorNormalize(*this);
}

bool vector::WithinAABox(vector const &boxmin, vector const &boxmax)
{
	return (
		(x >= boxmin.x) && (x <= boxmax.x) &&
		(y >= boxmin.y) && (y <= boxmax.y) &&
		(z >= boxmin.z) && (z <= boxmax.z)
		);
}

inline vec_t vector::DistTo(const vector& vOther) const
{
	vector delta;
	VectorSubtract(*this, vOther, delta);
	return delta.Length();
}

inline vector vector::Min(const vector& vOther) const
{
	return vector(x < vOther.x ? x : vOther.x,
		y < vOther.y ? y : vOther.y,
		z < vOther.z ? z : vOther.z);
}

inline vector vector::Max(const vector& vOther) const
{
	return vector(x > vOther.x ? x : vOther.x,
		y > vOther.y ? y : vOther.y,
		z > vOther.z ? z : vOther.z);
}

inline vector vector::operator-(void) const
{
	return vector(-x, -y, -z);
}

inline vector vector::operator+(const vector& v) const
{
	vector res;
	VectorAdd(*this, v, res);
	return res;
}

inline vector vector::operator-(const vector& v) const
{
	vector res;
	VectorSubtract(*this, v, res);
	return res;
}

inline vector vector::operator*(float fl) const
{
	vector res;
	VectorMultiply(*this, fl, res);
	return res;
}

inline vector vector::operator*(const vector& v) const
{
	vector res;
	VectorMultiply(*this, v, res);
	return res;
}

inline vector vector::operator/(float fl) const
{
	vector res;
	VectorDivide(*this, fl, res);
	return res;
}

inline vector vector::operator/(const vector& v) const
{
	vector res;
	VectorDivide(*this, v, res);
	return res;
}

inline vector operator*(float fl, const vector& v)
{
	return v * fl;
}

inline vector vector::Cross(const vector& vOther) const
{
	vector res;
	CrossProduct(*this, vOther, res);
	return res;
}

inline vec_t vector::length2d(void) const
{
	return (vec_t)::sqrtf(x * x + y * y);
}

inline vec_t vector::Length2DSqr(void) const
{
	return (x * x + y * y);
}

inline vector CrossProduct(const vector& a, const vector& b)
{
	return vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline void VectorMin(const vector& a, const vector& b, vector& result)
{
	result.x = min(a.x, b.x);
	result.y = min(a.y, b.y);
	result.z = min(a.z, b.z);
}

inline void VectorMax(const vector& a, const vector& b, vector& result)
{
	result.x = max(a.x, b.x);
	result.y = max(a.y, b.y);
	result.z = max(a.z, b.z);
}


class VectorAligned : public vector
{
public:
	VectorAligned()
	{
		x = y = z = 0.0f;
	}

	VectorAligned(const vector& v)
	{
		x = v.x; y = v.y; z = v.z;
	}

	float w;
};


#endif // VECTOR_H