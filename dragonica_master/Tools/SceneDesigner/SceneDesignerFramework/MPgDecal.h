#pragma once

#include <windows.h>
#include <list>

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
namespace Framework
{
	const long maxDecalVertices = 1024;
	const float decalEpsilon = 0.25F;


	struct ColorRGBA
	{
		float		red;
		float		green;
		float		blue;
		float		alpha;

		ColorRGBA() {}

		ColorRGBA(float r, float g, float b, float a)
		{
			red = r;
			green = g;
			blue = b;
			alpha = a;
		}
	};

	struct Triangle
	{
		unsigned short	index[3];
	};

	struct vector4
	{
		float x,y,z,w;
		vector4(){}
		vector4(float fx, float fy, float fz, float fw)
		{
			x = fx;
			y = fy;
			z = fz;
			w = fw;
		}
	};

	inline float DotProduct(const vector4& p, const NiPoint3& q)
	{
		return (p.x * q.x + p.y * q.y + p.z * q.z + p.w);
	}

	inline float DotProduct(const NiPoint3& p, const NiPoint3& q)
	{
		return (p.x * q.x + p.y * q.y + p.z * q.z);
	}

	inline NiPoint3 CrossProduct(const NiPoint3& p, const NiPoint3& q)
	{
		return NiPoint3(p.y*q.z-p.z*q.y, p.z*q.x-p.x*q.z, p.x*q.y-p.y*q.x);
	}

	class MPgDecal
	{
	public:
		MPgDecal(void);
		MPgDecal(const NiPoint3& center, const NiPoint3& normal, const NiPoint3& tangent, float width, float height, float depth);

		virtual ~MPgDecal(void);

		NiTriShape* GetShapeGeo();

	private:

		NiPoint3		decalCenter;
		NiPoint3		decalNormal;

		vector4		leftPlane;
		vector4		rightPlane;
		vector4		bottomPlane;
		vector4		topPlane;
		vector4		frontPlane;
		vector4		backPlane;

		long		decalVertexCount;
		long		decalTriangleCount;

		NiPoint3		vertexArray[maxDecalVertices];
		NiPoint2		texcoordArray[maxDecalVertices];
		ColorRGBA	colorArray[maxDecalVertices];
		Triangle	triangleArray[maxDecalVertices];

		NiTriShape*	m_spShapeGeo;

		bool AddPolygon(long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal);
		void ClipMesh(long triangleCount, const Triangle *triangle, const NiPoint3 *vertex, const NiPoint3 *normal);
		void ClipMesh(NiGeometry* pkGeometry);
		long ClipPolygon(long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal) const;
		static long ClipPolygonAgainstPlane(const vector4& plane, long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal);
	};

	public __gc class MPgDecalManager
	{
	public:
		MPgDecalManager(void);
		~MPgDecalManager(void);

		NiNode* GetDecalRoot();

		void LoadDecal();
		void MakeDecal(NiPoint3 pkCenter, NiPoint3 pkTangent, float fWidth, float fHeight, float fDepth);
		void MakeDecal();
		void SaveDecal();
		//void WriteDecal();
		void SetWidth(float fWidth);
		void SetHeight(float fHeight);
		void SetDepth(float fDepth);

	private:
		__property static ISelectionService* get_SelectionService();
		static ISelectionService* ms_pmSelectionService;

		ArrayList* m_pmNameList;

	private:
		NiNode* m_spDecalRoot;

		float m_fWidth;
		float m_fHeight;
		float m_fDepth;

	};
}}}}
