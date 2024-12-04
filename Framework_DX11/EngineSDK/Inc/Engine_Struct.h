#pragma once

namespace Engine
{
	typedef struct
	{
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		bool			isWindowsed;
	}ENGINE_DESC;

	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_GODRAY, TYPE_END };

		TYPE		eType;
		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	typedef struct
	{
		class CTexture* pMaterialTextures[18]{ nullptr };
	}MESH_MATERIAL;

	typedef struct
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		_double			TrackPosition;
	}KEYFRAME;

	// 고준호 파티클
	typedef struct
	{
		_float3		vPosition;
		_float2		vPSize;
		_float4		vRight;
		_float4		vUp;
		_float4		vLook;
		_float4		vTranslation;
		_float2		vLifeTime;
		_float4		vColor;
		_float		fSpeed;
		_float4		vCurrenrRandomDir;
		_float4		vNextRandomDir;
	}PARTICLE;

	typedef struct
	{
		vector<string>			m_ModelFilePaths;
	}FilePathStruct;

	typedef struct
	{
		vector<FilePathStruct>		pStruct;
	}FilePathStructStack;

	typedef struct
	{
		vector<FilePathStructStack>		pLump;
	}StructStacklump;


	typedef struct
	{
		_uint	iNextAnimIndex;
		_uint	iStartFrame;
		_float	fChangeTime = 0.f;
		_float	fChangeDuration = 0.f;
	}CHANGEANIMATION_DESC;

#pragma region 후처리 관련
	typedef struct
	{
		_int isOnSSAO = false;
		_float fRadius = 0.94f;
		_float fBias = 0.335f;
		_float fAmount = 3.f;
	}SSAO_DESC;

	typedef struct
	{
		_bool isOnHDR = false;
		_float fMiddleGrey = 0.7f;
		_float fLumWhiteSqr = 0.3f;
	}HDR_DESC;

	typedef struct
	{
		_bool isOnBloom = false;
		_float fThreshold = 0.f;
	}BLOOM_DESC;

	typedef struct
	{
		_bool isOnDOF = false;
		_float fDOF = 10.f;
	}DOF_DESC;

	typedef struct
	{
		_bool isOnRadial = false;
		_Vec2	vRadialCenterPos = { 0.5f,0.5f };
		_float	fRadius = 0.1f;
		_float	fRadialPower = 0.f;
	}RADIAL_DESC;
#pragma endregion

	typedef struct
	{
		SimpleMath::Matrix  WorldMatrix;
		_float4				vHashColor;

	}INSTANCE_DATA;

	//오브젝트 저장 및 불러오기에 사용
	typedef struct
	{
		_tchar		szModelTag[MAX_PATH];
		_tchar		szTextureTag_Diffuse[MAX_PATH];
		_tchar		szTextureTag_Normal[MAX_PATH];
		_tchar		szTextureTag_ARM[MAX_PATH];
		_uint		iID;
		_int		iCurrentCellNum;
		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
		_bool		isInstance = { false };
		_bool		bShadow = { false };
		_bool		bDecal = { false };
		_bool		bNormal = { false };
		_bool		bARM = { false };
		_bool		bUseWorldColor = { false };

	}OBJECT_DEFAULT_DESC;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;	

		static const unsigned int	iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];	
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXCUBETEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;		
		XMFLOAT3		vTangent;		

		static const unsigned int	iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices; /* 이 정점은 어떤 뼈(최대 4개 뼈의 인덱스)의 상태를 따라가야하는가? */
		XMFLOAT4		vBlendWeights; /* 위에서 정의한 네개의 뼈가 이 정점에게 각각 몇 퍼센트(0.f ~ 1.f)나 영향을 주는지? */

		static const unsigned int	iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMESH;


	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;			
		XMFLOAT2		vLifeTime;

		static const unsigned int	iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
		
	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT2		vLifeTime;
		XMFLOAT4		vColor;

		static const unsigned int	iNumElements = 8;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];

	}VTXPOINTINSTANCE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3 vCurPos;
		XMFLOAT3 vPrePos;
		XMFLOAT3 vMoveDir;

		_float2 vLifeTime;

		static const unsigned int	iNumElements = 5;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXTRAIL_ONEPOINT_INSTANCE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3 vFirstTopPos;
		XMFLOAT3 vFirstBottomPos;

		XMFLOAT3 vSecondTopPos;
		XMFLOAT3 vSecondBottomPos;

		XMFLOAT3 vThirdTopPos;
		XMFLOAT3 vThirdBottomPos;

		XMFLOAT3 vForthTopPos;
		XMFLOAT3 vForthBottomPos;

		XMFLOAT2 vLifeTime;
		_float	fIndex;

		static const unsigned int	iNumElements = 11;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXTRAIL_TWOPOINT_INSTANCE;

	typedef struct ENGINE_DLL
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vTranslation;
		XMFLOAT4		vHashColor;

		static const unsigned int	iNumElements = 9;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODELINSTANCE;

	//우송 컷신
	typedef struct
	{
		_bool bUseDof = { false };
		_float fDof = { 0.f };
	}SHADER_DESC;

	typedef struct
	{
		_bool bFadeOut = { false };
		_bool bFadeIn = { false };
		_float fTime = { 0.f };
		_Vec3 fColor = { 0.f,0.f,0.f };
	}UI_DESC;

	typedef struct
	{
		_bool bTeleport = { false };
		_bool bTurn = { false };
		_bool bZoomIn = { false };
		_bool bZoomOut = { false };
		_bool bLerpMove = { false };

		_Matrix mCameraWorlMatrix;

		_Vec3 vPitchTawRoll = {};
		_float fTurn_Speed = {0.f};

		_float fZoomDuration = { 0.f };
		_float fFovy = { XMConvertToRadians(60.f)};

		_float fMoveSpeed = { 0.f };
		_Vec3 vTargetPos = {};
	}SCENE_CAMERA_DESC;

	typedef struct
	{
		_float			fTrackPosition;
		_bool			bActived = false;
		SHADER_DESC		ShaderDesc;
		UI_DESC			UI_DESC;
		SCENE_CAMERA_DESC	Camera_Desc;
	}CUTSCENE_DESC;

}

