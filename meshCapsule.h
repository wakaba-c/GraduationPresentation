//=============================================================================
//
// メッシュカプセル処理 [meshCapsule.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MESHCAPSULE_H_
#define _MESHCAPSULE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define RADIUS	50.0f										//半径
#define CAPSULE_DIVISION 100								//分割数

//=============================================================================
// クラス定義
//=============================================================================
class CMeshCapsule : public CScene
{
public:
	CMeshCapsule(PRIORITY obj);								// コンストラクタ
	~CMeshCapsule();										// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CMeshCapsule *Create(void);						// クリエイト処理
	static HRESULT Load(void);								// ロード処理

	void SetSize(D3DXVECTOR3 size);							// 大きさを設定する
	void SetColor(D3DXCOLOR col);							// 色を設定する

	D3DXVECTOR3 GetSize(void) { return m_size; }			// 大きさを取得
	D3DXCOLOR GetColor(void) { return m_col; }				// 色を取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);										// デバッグ処理
#endif
	void MakeVertex(void);									// 頂点情報の更新

	static LPDIRECT3DTEXTURE9	m_pTexture;								//テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuffSphere;						//頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9		m_pIdxBuffSphere;						//インデックスバッファへのポインタ

	D3DXVECTOR3 apNor[CAPSULE_DIVISION * CAPSULE_DIVISION * 2];			//1面ごとの法線ベクトルの配列

	D3DXVECTOR3				m_rot;							//ポリゴンの向き(回転)
	D3DXVECTOR3				m_size;							//ポリゴンのサイズ
	D3DXCOLOR				m_col;							//ポリゴンの色
	D3DXMATRIX				m_mtxWorld;						//ワールドマトリックス

	int m_nNumVertexSphere;									//総頂点数
	int m_nNumIndexSphere;									//インデックス数
	int m_nNumPolygonSphere;								//総ポリゴン数

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;					//頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9		m_pIdxBuff;					//インデックスバッファへのポインタ

	int m_nNumVertex;										//総頂点数
	int m_nNumIndex;										//インデックス数
	int m_nNumPolygon;										//総ポリゴン数
};
#endif