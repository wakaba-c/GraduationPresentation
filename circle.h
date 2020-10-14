//=============================================================================
//
// 円板処理 [circle.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _CIRCLE_H_
#define _CIRCLE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define DIVISION_CIRCLE 30					//分割数
#define DEPTH 	20							//幅

//=============================================================================
// クラス定義
//=============================================================================
class CCircle
{
public:
	CCircle();						// コンストラクタ
	~CCircle();						// デストラクタ処理
	HRESULT Init(void);				// 初期化処理
	void Uninit(void);				// 開放処理
	void Update(void);				// 更新処理
	void Draw(void);				// 描画処理

	static CCircle *Create(void);	// クリエイト処理

	static HRESULT Load(void);
	static void Unload(void);

	void MakeVertex(void);									// 頂点情報の作成
	void SetPosition(D3DXVECTOR3 pos);						// 位置を設定する
	void SetRotation(D3DXVECTOR3 rot);						// 回転を設定する
	void SetColor(D3DXCOLOR col);							// 色を設定する
	void SetRadius(float fValue);							// 半径を設定する
	void SetSize(D3DXVECTOR3 size);							// 大きさを設定する

	D3DXVECTOR3 GetSize(void) { return m_size; }			// 大きさを返す
	D3DXVECTOR3 GetPosition(void) { return m_pos; }			// 位置を返す
	D3DXVECTOR3 GetRotation(void) { return m_rot; }			// 回転を返す
	D3DXCOLOR GetColor(void) { return m_col; }				// 色を返す
	float GetRadius(void) { return m_fRadius; }				// 半径を返す

private:
	void Debug(void);

	static LPDIRECT3DTEXTURE9	m_pTexture;					// テクスチャへのポインタ

	D3DXVECTOR3				m_pos;							// 位置
	D3DXVECTOR3				m_rot;							// 回転
	D3DXVECTOR3				m_size;							// サイズ
	float					m_fRadius;						// 半径
	D3DXCOLOR				m_col;							// 色
	D3DXMATRIX				m_mtxWorld;						// ワールドマトリックス

	int m_nNumVertex;										// 総頂点数
	int m_nNumIndex;										// インデックス数
	int m_nNumPolygon;										// 総ポリゴン数

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;					// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9		m_pIdxBuff;					// インデックスバッファへのポインタ
};
#endif