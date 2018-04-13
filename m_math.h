/***********************************************************
* A single-header inlined math library, extended on demand *
* no warranty implied | use at your own risk               *
* author: Andreas Mantler (ands) | last change: 13.04.2018 *
*                                                          *
* License:                                                 *
* This software is in the public domain.                   *
* Where that dedication is not recognized,                 *
* you are granted a perpetual, irrevocable license to copy *
* and modify this file however you want.                   *
***********************************************************/

#if defined(_MSC_VER) && !defined(__cplusplus) // TODO: specific versions only?
#define inline __inline
#endif

typedef int m_bool;
#define M_FALSE 0
#define M_TRUE  1

#if defined(_MSC_VER) && (_MSC_VER <= 1700)
static inline m_bool m_finite(float a) { return _finite(a); }
#else
static inline m_bool m_finite(float a) { return isfinite(a); }
#endif

static inline int      m_mini     (int    a, int    b) { return a < b ? a : b; }
static inline int      m_maxi     (int    a, int    b) { return a > b ? a : b; }
static inline int      m_absi     (int    a          ) { return a < 0 ? -a : a; }
static inline float    m_minf     (float  a, float  b) { return a < b ? a : b; }
static inline float    m_maxf     (float  a, float  b) { return a > b ? a : b; }
static inline float    m_absf     (float  a          ) { return a < 0.0f ? -a : a; }
static inline float    m_pmodf    (float  a, float  b) { return (a < 0.0f ? 1.0f : 0.0f) + (float)fmod(a, b); } // positive mod

typedef struct m_ivec2 { int x, y; } m_ivec2;
static inline m_ivec2 m_i2        (int    x, int    y) { m_ivec2 v = { x, y }; return v; }

typedef struct m_vec2 { float x, y; } m_vec2;
static inline m_vec2  m_v2i       (int    x, int    y) { m_vec2 v = { (float)x, (float)y }; return v; }
static inline m_vec2  m_v2        (float  x, float  y) { m_vec2 v = { x, y }; return v; }
static inline m_vec2  m_negate2   (m_vec2 a          ) { return m_v2(-a.x, -a.y); }
static inline m_vec2  m_add2      (m_vec2 a, m_vec2 b) { return m_v2(a.x + b.x, a.y + b.y); }
static inline m_vec2  m_sub2      (m_vec2 a, m_vec2 b) { return m_v2(a.x - b.x, a.y - b.y); }
static inline m_vec2  m_mul2      (m_vec2 a, m_vec2 b) { return m_v2(a.x * b.x, a.y * b.y); }
static inline m_vec2  m_scale2    (m_vec2 a, float  b) { return m_v2(a.x * b, a.y * b); }
static inline m_vec2  m_div2      (m_vec2 a, float  b) { return m_scale2(a, 1.0f / b); }
static inline m_vec2  m_pmod2     (m_vec2 a, float  b) { return m_v2(m_pmodf(a.x, b), m_pmodf(a.y, b)); }
static inline m_vec2  m_min2      (m_vec2 a, m_vec2 b) { return m_v2(m_minf(a.x, b.x), m_minf(a.y, b.y)); }
static inline m_vec2  m_max2      (m_vec2 a, m_vec2 b) { return m_v2(m_maxf(a.x, b.x), m_maxf(a.y, b.y)); }
static inline m_vec2  m_abs2      (m_vec2 a          ) { return m_v2(m_absf(a.x), m_absf(a.y)); }
static inline m_vec2  m_floor2    (m_vec2 a          ) { return m_v2(floorf(a.x), floorf(a.y)); }
static inline m_vec2  m_ceil2     (m_vec2 a          ) { return m_v2(ceilf (a.x), ceilf (a.y)); }
static inline float   m_dot2      (m_vec2 a, m_vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float   m_cross2    (m_vec2 a, m_vec2 b) { return a.x * b.y - a.y * b.x; } // pseudo cross product
static inline float   m_length2sq (m_vec2 a          ) { return a.x * a.x + a.y * a.y; }
static inline float   m_length2   (m_vec2 a          ) { return sqrtf(m_length2sq(a)); }
static inline m_vec2  m_normalize2(m_vec2 a          ) { return m_div2(a, m_length2(a)); }
static inline m_bool  m_finite2   (m_vec2 a          ) { return m_finite(a.x) && m_finite(a.y); }

typedef struct m_vec3 { float x, y, z; } m_vec3;
static inline m_vec3  m_v3        (float  x, float  y, float  z) { m_vec3 v = { x, y, z }; return v; }
static inline m_vec3  m_negate3   (m_vec3 a          ) { return m_v3(-a.x, -a.y, -a.z); }
static inline m_vec3  m_add3      (m_vec3 a, m_vec3 b) { return m_v3(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline m_vec3  m_sub3      (m_vec3 a, m_vec3 b) { return m_v3(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline m_vec3  m_mul3      (m_vec3 a, m_vec3 b) { return m_v3(a.x * b.x, a.y * b.y, a.z * b.z); }
static inline m_vec3  m_scale3    (m_vec3 a, float  b) { return m_v3(a.x * b, a.y * b, a.z * b); }
static inline m_vec3  m_div3      (m_vec3 a, float  b) { return m_scale3(a, 1.0f / b); }
static inline m_vec3  m_pmod3     (m_vec3 a, float  b) { return m_v3(m_pmodf(a.x, b), m_pmodf(a.y, b), m_pmodf(a.z, b)); }
static inline m_vec3  m_min3      (m_vec3 a, m_vec3 b) { return m_v3(m_minf(a.x, b.x), m_minf(a.y, b.y), m_minf(a.z, b.z)); }
static inline m_vec3  m_max3      (m_vec3 a, m_vec3 b) { return m_v3(m_maxf(a.x, b.x), m_maxf(a.y, b.y), m_maxf(a.z, b.z)); }
static inline m_vec3  m_abs3      (m_vec3 a          ) { return m_v3(m_absf(a.x), m_absf(a.y), m_absf(a.z)); }
static inline m_vec3  m_floor3    (m_vec3 a          ) { return m_v3(floorf(a.x), floorf(a.y), floorf(a.z)); }
static inline m_vec3  m_ceil3     (m_vec3 a          ) { return m_v3(ceilf (a.x), ceilf (a.y), ceilf (a.z)); }
static inline float   m_dot3      (m_vec3 a, m_vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline m_vec3  m_cross3    (m_vec3 a, m_vec3 b) { return m_v3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y); }
static inline float   m_length3sq (m_vec3 a          ) { return a.x * a.x + a.y * a.y + a.z * a.z; }
static inline float   m_length3   (m_vec3 a          ) { return sqrtf(m_length3sq(a)); }
static inline m_vec3  m_normalize3(m_vec3 a          ) { return m_div3(a, m_length3(a)); }
static inline m_bool  m_finite3   (m_vec3 a          ) { return m_finite(a.x) && m_finite(a.y) && m_finite(a.z); }

#define M_M_PI 3.14159265358979323846f

static void m_mul44(float *out, float *a, float *b)
{
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
			out[y * 4 + x] = a[x] * b[y * 4] + a[4 + x] * b[y * 4 + 1] + a[8 + x] * b[y * 4 + 2] + a[12 + x] * b[y * 4 + 3];
}
static void m_translation44(float *out, float x, float y, float z)
{
	out[ 0] = 1.0f; out[ 1] = 0.0f; out[ 2] = 0.0f; out[ 3] = 0.0f;
	out[ 4] = 0.0f; out[ 5] = 1.0f; out[ 6] = 0.0f; out[ 7] = 0.0f;
	out[ 8] = 0.0f; out[ 9] = 0.0f; out[10] = 1.0f; out[11] = 0.0f;
	out[12] = x;    out[13] = y;    out[14] = z;    out[15] = 1.0f;
}
static void m_rotation44(float *out, float angle, float x, float y, float z)
{
	angle *= M_M_PI / 180.0f;
	float c = cosf(angle), s = sinf(angle), c2 = 1.0f - c;
	out[ 0] = x*x*c2 + c;   out[ 1] = y*x*c2 + z*s; out[ 2] = x*z*c2 - y*s; out[ 3] = 0.0f;
	out[ 4] = x*y*c2 - z*s; out[ 5] = y*y*c2 + c;   out[ 6] = y*z*c2 + x*s; out[ 7] = 0.0f;
	out[ 8] = x*z*c2 + y*s; out[ 9] = y*z*c2 - x*s; out[10] = z*z*c2 + c;   out[11] = 0.0f;
	out[12] = 0.0f;         out[13] = 0.0f;         out[14] = 0.0f;         out[15] = 1.0f;
}
static void m_transform44(float *out, float *m, float *p)
{
	float d = 1.0f / (m[3] * p[0] + m[7] * p[1] + m[11] * p[2] + m[15]);
	out[2] =     d * (m[2] * p[0] + m[6] * p[1] + m[10] * p[2] + m[14]);
	out[1] =     d * (m[1] * p[0] + m[5] * p[1] + m[ 9] * p[2] + m[13]);
	out[0] =     d * (m[0] * p[0] + m[4] * p[1] + m[ 8] * p[2] + m[12]);
}
static void m_transpose44(float *out, float *m)
{
	out[ 0] = m[0]; out[ 1] = m[4]; out[ 2] = m[ 8]; out[ 3] = m[12];
	out[ 4] = m[1]; out[ 5] = m[5]; out[ 6] = m[ 9]; out[ 7] = m[13];
	out[ 8] = m[2]; out[ 9] = m[6]; out[10] = m[10]; out[11] = m[14];
	out[12] = m[3]; out[13] = m[7]; out[14] = m[11]; out[15] = m[15];
}
static void m_perspective44(float *out, float fovy, float aspect, float zNear, float zFar)
{
	float f = 1.0f / tanf(fovy * M_M_PI / 360.0f);
	float izFN = 1.0f / (zNear - zFar);
	out[ 0] = f / aspect; out[ 1] = 0.0f; out[ 2] = 0.0f;                       out[ 3] = 0.0f;
	out[ 4] = 0.0f;       out[ 5] = f;    out[ 6] = 0.0f;                       out[ 7] = 0.0f;
	out[ 8] = 0.0f;       out[ 9] = 0.0f; out[10] = (zFar + zNear) * izFN;      out[11] = -1.0f;
	out[12] = 0.0f;       out[13] = 0.0f; out[14] = 2.0f * zFar * zNear * izFN; out[15] = 0.0f;
}