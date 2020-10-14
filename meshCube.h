//=============================================================================
//
// メッシュキューブ処理 [meshCube.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MESHCUBE_H_
#define _MESHCUBE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define DEPTH	20											// 奥行き
#define WIDE 20												// 幅

//=============================================================================
// クラス定義
//=============================================================================
class CMeshCube : public CScene
{
public:
	CMeshCube(PRIORITY obj);								// コンストラクタ
	~CMeshCube();											// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CMeshCube *Create(void);							// クリエイト処理
	static HRESULT Load(void);								// ロード処理

	void SetSize(D3DXVECTOR3 size);							// ポリゴンの大きさを設定する
	void SetColor(D3DXCOLOR col);							// ポリゴンの色を設定する
	void MakeVertex(void);									// 頂点情報の更新

	D3DXVECTOR3 GetSize(void) { return m_size; }			// ポリゴンの大きさを返す
	D3DXCOLOR GetColor(void) { return m_col; }				// ポリゴンの色を返す

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);										// デバッグ処理
#endif

	static LPDIRECT3DTEXTURE9	m_pTexture;					// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;						// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	m_pIdxBuff;						// インデックスバッファへのポインタ

	D3DXVECTOR3 apNor[DEPTH * WIDE * 2];					// 1面ごとの法線ベクトルの配列

	D3DXVECTOR3				m_rot;							// ポリゴンの向き(回転)
	D3DXVECTOR3				m_size;							// ポリゴンのサイズ
	D3DXCOLOR				m_col;							// ポリゴンの色
	D3DXMATRIX				m_mtxWorld;						// ワールドマトリックス

	D3DXMATRIX				*m_mtxWorldParent;				// 親のワールドマトリックス

	int m_nNumVertex;										// 総頂点数
	int m_nNumIndex;										// インデックス数
	int m_nNumPolygon;										// 総ポリゴン数
};
#endif