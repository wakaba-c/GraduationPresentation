//=============================================================================
//
// scene3D処理 [scene3D.h]
// Author : masayasu wakita
//
//=============================================================================
#include "main.h"
#include "scene.h"

#ifndef _SCENE3D_H_
#define _SCENE3D_H_

//=============================================================================
// クラス定義
//=============================================================================
class CScene3D : public CScene
{
public:
	// 中心位置
	typedef enum
	{
		TEXTUREVTX_CENTER = 0,
		TEXTUREVTX_LEFT,
		TEXTUREVTX_MAX
	} TEXTUREVTX;

	CScene3D(PRIORITY obj);									// コンストラクタ
	~CScene3D();											// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CScene3D *Create(CScene::PRIORITY obj);			// クリエイト処理
	void BindTexture(LPDIRECT3DTEXTURE9 tex);				// テクスチャの設定

	void MakeVertex(void);									// 頂点情報の作成
	void SetCenter(TEXTUREVTX vtx);							// 中心位置の設定
	void SetSize(D3DXVECTOR3 size);							// サイズの設定
	void SetColor(D3DXCOLOR col);							// 色の設定

	TEXTUREVTX GetCenter(void) { return	 m_Vtx; }			// 中心の取得
	D3DXCOLOR GetColor(void) { return m_col; }				// 色の取得
	D3DXVECTOR3 GetSize(void) { return m_size; }			// サイズの取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	LPDIRECT3DTEXTURE9			m_pTexture;					//テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;					//頂点バッファへのポインタ
	D3DXVECTOR3 m_size;										//大きさ
	D3DXVECTOR3 m_rot;										//回転量
	D3DXCOLOR	m_col;										//色
	D3DXMATRIX	m_mtxWorld;									//ワールドマトリックス
	TEXTUREVTX	m_Vtx;										//中心
};
#endif