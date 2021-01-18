//=============================================================================
//
// オブジェクト処理 [object.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _OBJECT_H_
#define _OBJECT_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "sceneX.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define DEPTH	20											// 奥行き
#define WIDE 20												// 幅

//=============================================================================
// 前方宣言
//=============================================================================
class CColliderBox;
class CColliderSphere;

//=============================================================================
// クラス定義
//=============================================================================
class CObject : public CSceneX
{
public:
	// 列挙体定義
	typedef enum
	{
		LOADTYPE_ADD = 0,
		LOADTYPE_POS_X,
		LOADTYPE_POS_Y,
		LOADTYPE_POS_Z,
		LOADTYPE_ROT_X,
		LOADTYPE_ROT_Y,
		LOADTYPE_ROT_Z,
		LOADTYPE_MAX
	} LOADTYPE;

	CObject(PRIORITY obj);							// コンストラクタ
	~CObject();										// デストラクタ
	HRESULT Init(void);								// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CObject *Create(void);					// クリエイト処理
	static HRESULT Load(void);						// ロード処理
	static void Unload(void);						// テクスチャの開放処理
	static void LoadScript(void);					// オブジェクトアセットのロード処理

	static void LoadModel(char *add);				// モデルのロード処理
	static void LoadModelTest(char *add);			// モデルのロード処理

	void BindModel(std::string add);

	std::string GetAdd(void) { return m_Add; }
	LPD3DXMESH GetMesh(void) { return m_pMesh; }	// メッシュ情報の取得
	static std::vector<CObject*> GetPointObj(void) { return m_vPointObj; }
	static void SetPointNum(unsigned int pointNum) { m_pointNum = pointNum; }
	static unsigned int GetPointNum(void) { return m_pointNum; }

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void);

	static bool Collide(D3DXVECTOR3 vStart, D3DXVECTOR3 vDir, FLOAT* pfDistance, D3DXVECTOR3* pvNormal, D3DXMATRIX mtx);			// 当たり判定
	static HRESULT FindVerticesOnPoly(LPD3DXMESH pMesh, DWORD dwPolyIndex, D3DXVECTOR3* pvVertices);
	static void ReleaseCheckPoint(void);

private:
#ifdef _DEBUG
	void Debug(void);								// デバッグ処理
#endif
	void SetAdd(std::string &Add) { m_Add = Add; }

	LPD3DXMESH		m_pMesh;							// メッシュ情報へのポインタ
	DWORD			m_nNumMat;							// マテリアル情報の数
	LPD3DXBUFFER		m_pBuffMat;						// マテリアル情報へのポインタ
	CColliderBox *m_pBox;								// 当たり判定ボックス

	static CColliderSphere *m_pSphere;					// 当たり判定スフィア
	static unsigned int m_pointNum;						// ポイントの数
	static std::vector<CObject*> m_vPointObj;

	std::string m_Add;														// モデルのアドレス
};
#endif