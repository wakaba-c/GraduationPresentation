//=============================================================================
//
// 空処理 [sky.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SKY_H_
#define _SKY_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshSphere;

//=============================================================================
// クラス定義
//=============================================================================
class CSky : public CScene
{
public:
	CSky(PRIORITY obj);									// コンストラクタ
	~CSky();											// デストラクタ
	HRESULT Init(void);									// 初期化処理
	void Uninit(void);									// 開放処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	static CSky *Create(void);							// クリエイト処理
	static HRESULT Load(void);							// 素材データの取得
	static void Unload(void);							// 素材データの開放

	void MakeVertex(void);								// 頂点情報の作成

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);
#endif
	D3DXVECTOR3 m_size;									// 大きさ
	D3DXVECTOR3 m_rot;									// 回転量
	D3DXCOLOR	m_color;								// 色
	D3DXMATRIX	m_mtxWorld;								// ワールドマトリックス
	bool m_bDraw;										// 描画するかどうか

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9		m_pIdxBuff;				// インデックスバッファへのポインタ
	LPD3DXMESH		m_pMesh;							// メッシュ情報へのポインタ
	DWORD			m_nNumMat;							// マテリアル情報の数
	LPD3DXBUFFER	m_pBuffMat;							// マテリアル情報へのポインタ

	CMeshSphere		*m_pMeshSphere;						// スフィアのポインタ

	float m_fAngle;										// 角度
	float m_fLength;									// 長さ

	float					m_fRadius;					// ポリゴンの半径
	D3DXCOLOR				m_col;						// ポリゴンの色

	int m_nNumVertexSphere;								// 総頂点数
	int m_nNumIndexSphere;								// インデックス数
	int m_nNumPolygonSphere;							// 総ポリゴン数

	int m_nNumVertex;									// 総頂点数
	int m_nNumIndex;									// インデックス数
	int m_nNumPolygon;									// 総ポリゴン数
};
#endif