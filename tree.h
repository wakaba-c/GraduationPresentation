//=============================================================================
//
// 木処理 [tree.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _TREE_H_
#define _TREE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// 列挙体定義
//=============================================================================
typedef enum
{
	TREETYPE_NORMAL = 0,
	TREETYPE_MAX
} TREETYPE;

//=============================================================================
// クラス定義
//=============================================================================
class CTree : public CScene
{
public:
	CTree(PRIORITY obj);
	~CTree();																		// デストラクタ
	HRESULT Init(void);																// 初期化処理
	void Uninit(void);																// 終了処理
	void Update(void);																// 更新処理
	void Draw(void);																// 描画処理

	static CTree *Create(TREETYPE type);											// 生成処理
	static HRESULT Load(void);														// 素材の読み込み
	static void Unload(void);														// 素材の開放

	static void LoadModel(char *add);												// 木の配置情報読み込み処理
	void SetType(TREETYPE type);													// 種類の設定関数

	TREETYPE GetType(void) { return m_type; }										// 種類の取得関数
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }								// ワールドマトリックスの取得

	void OnCollisionEnter(CCollider *pCol) {};
	void OnTriggerEnter(CCollider *pCol) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);																// デバッグ処理
#endif
	void MakeVertex(void);															// 頂点情報の作成処理

	static LPDIRECT3DTEXTURE9		m_pTexture;										// テクスチャへのポインタ
	D3DXVECTOR3						m_pos;											// ポジション
	D3DXVECTOR3						m_rot;											// 回転量
	D3DXVECTOR3						m_posOld;										// 前のポジション
	D3DXVECTOR3						m_size;											// 大きさ
	D3DXCOLOR						m_col;											// 色
	D3DXMATRIX						m_mtxWorld;										// ワールドマトリックス
	TREETYPE						m_type;											// 種類
	LPDIRECT3DVERTEXBUFFER9			m_pVtxBuff;										// 頂点バッファへのポインタ
};
#endif