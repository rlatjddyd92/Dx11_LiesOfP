#pragma once

#pragma warning (disable : 4251)
#pragma warning(disable : 5208) //구조체 경고 끄기
#pragma warning(disable : 26495) //구조체 경고 끄기
#pragma warning (disable : 6031) // ReadFile 반환값 무시 경고 끄기 

#include <d3d11.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"

#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"

#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <DirectXTK/SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

/* d3dx11에 포함되어있었지만 너무 무겁다. */
/* 기존 d3dx11포함되어있었던 다양한 기능을 제공하는 헤더 + 라이브러리 파일을 선택적으로 추가한다. */
//#include <d3dx11.h>

#include <map>
#include <unordered_set>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");
	const _uint			g_iMaxMeshBones = 660;
}

using namespace Engine;

#include "PxPhysics.h"
#include "PxPhysicsAPI.h"
#include "gpu/PxGpu.h"
#include "gpu/PxPhysicsGpu.h"

using namespace physx;

#include "NvCloth/Cloth.h"
#include "NvCloth/Factory.h"
#include "NvCloth/Solver.h"
#include "NvCloth/DxContextManagerCallback.h"

using namespace nv::cloth;

#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#define TEXTURE_TYPE_MAX 18

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

namespace Engine {
	class CModel;  // CModel에 대한 포워드 선언
}

namespace physX
{
	enum GEOMETRY_TYPE { PX_CAPSULE, PX_SPHERE, PX_BOX, PX_MODEL, PX_END };

	struct ENGINE_DLL Geometry abstract
	{
	protected:
		Geometry() = delete;
		Geometry(GEOMETRY_TYPE eType)
			:m_eGeometryType(eType) {};
		GEOMETRY_TYPE m_eGeometryType = { PX_END };
	public:
		GEOMETRY_TYPE GetType() { return m_eGeometryType; };
	};

	struct ENGINE_DLL GeometryCapsule final : public Geometry
	{
		GeometryCapsule()
			: Geometry(PX_CAPSULE) {}
		_float fRadius = 0.f;
		_float fHeight = 0.f;
	};

	struct ENGINE_DLL GeometrySphere final : public Geometry
	{
		GeometrySphere() : Geometry(PX_SPHERE) {}
		_float fRadius = 0.f;
	};

	struct ENGINE_DLL GeometryBox final : public Geometry
	{
		GeometryBox()
			: Geometry(PX_BOX) {}
		_Vec3 vSize = { 0.f,0.f,0.f };
	};

	struct ENGINE_DLL GeometryTriangleMesh final : public Geometry
	{
		GeometryTriangleMesh()
			: Geometry(PX_MODEL), pModel(nullptr) {}
		class CModel* pModel = { nullptr };
	};
}


inline PxVec2 ConvertToPxVec2(const _Vec2& _vector)
{
	return PxVec2(_vector.x, _vector.y);
}

inline PxVec3 ConvertToPxVec3(const _Vec3& _vector)
{
	return PxVec3(_vector.x, _vector.y, _vector.z);
}

inline PxVec4 ConvertToPxVec4(const _Vec4& _vector)
{
	return PxVec4(_vector.x, _vector.y, _vector.z, _vector.w);
}

inline PxQuat ConvertToPxQuat(const _Quaternion& _Quaternion)
{
	return PxQuat(_Quaternion.x, _Quaternion.y, _Quaternion.z, _Quaternion.w);
}

inline PxMat33 ConvertToPxMat33(const _Matrix& _Matrix)
{
	return PxMat33(ConvertToPxVec3(_Matrix.Right()), ConvertToPxVec3(_Matrix.Up()), ConvertToPxVec3(_Matrix.Forward()));
}

inline PxMat44 ConvertToPxMat44(const _Matrix& _Matrix)
{
	return PxMat44(ConvertToPxVec3(_Matrix.Right()), ConvertToPxVec3(_Matrix.Up()), ConvertToPxVec3(_Matrix.Forward()), ConvertToPxVec3(_Matrix.Translation()));
}

inline PxTransform ConvertToPxTransform(const _Vec3& _vector, const _Quaternion& _Quaternion)
{
	return PxTransform(ConvertToPxVec3(_vector), ConvertToPxQuat(_Quaternion));
}