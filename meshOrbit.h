//=============================================================================
//
// 軌跡処理 [meshOrbit.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MESHORBIT_H_
#define _MESHORBIT_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define ORBIT_DEPTH	1
#define ORBIT_WIDE 20

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	D3DXVECTOR3 pos;										//位置
	D3DXVECTOR3 rot;										//回転
	D3DXMATRIX	mtxWorld;									//ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffOrbit;					//頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	pIdxBuffMeshOrbit;				//インデックスバッファへのポインタ
	int nNumVertexMeshOrbit;								//総頂点数
	int nNumPolygonMeshOrbit;								//総ポリゴン数
} ORBIT;

typedef struct
{
	D3DXVECTOR3 pos;										//位置
	D3DXVECTOR3 rot;										//回転
	D3DXMATRIX	mtxWorld;									//ワールドマトリックス
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffOrbit;					//頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	pIdxBuffMeshOrbit;				//インデックスバッファへのポインタ
	int nNumVertexMeshOrbit;								//総頂点数
	int nNumPolygonMeshOrbit;								//総ポリゴン数
} ORBITMTX;

//=============================================================================
// クラス定義
//=============================================================================
class CMeshOrbit : public CScene
{
public:
	CMeshOrbit(PRIORITY obj);								// コンストラクタ
	~CMeshOrbit();											// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CMeshOrbit *Create(void);						// クリエイト処理
	static HRESULT Load(void);								// ロード処理

	void SetOrbit(bool bValue);								// 軌跡の作成
	bool GetOrbit(void) { return m_bUse; }					// 軌跡の取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);
#endif

	void MakeVertex(void);									// 頂点情報の作成

	static LPDIRECT3DTEXTURE9	m_pTexture;					// テクスチャへのポインタ
	LPDIRECT3DTEXTURE9			m_pTextureOrbit = NULL;		// テクスチャへのポインタ
	ORBIT						m_aOrbit;					// 軌跡
	ORBITMTX					m_aMtxMeshOrbit;			// 軌跡
	int							m_nNumIndexOrbit;			// インデックス数
	bool						m_bUse;						// 使うかどうか

	//持ち手
	D3DXMATRIX					m_mtxWorldMeshOrbit;
	D3DXMATRIX					m_mtxWorldMeshOrbitOld[ORBIT_WIDE];
	//剣先
	D3DXMATRIX					m_mtxWorldWeapon;
	D3DXMATRIX					m_mtxWorldWeaponOld[ORBIT_WIDE];
};
#endif