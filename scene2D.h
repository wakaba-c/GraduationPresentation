//=============================================================================
//
// scene2D処理 [scene2D.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SCENE2D_H_
#define _SCENE2D_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// クラス定義
//=============================================================================
class CScene2D : public CScene
{
public:
	// 中心位置
	typedef enum
	{
		TEXTUREVTX_CENTER = 0,
		TEXTUREVTX_LEFT,
		TEXTUREVTX_MAX
	} TEXTUREVTX;

	CScene2D(PRIORITY obj);															// コンストラクタ
	~CScene2D();																	// デストラクタ
	HRESULT Init(void);																// 初期化処理
	void Uninit(void);																// 開放処理
	void Update(void);																// 更新処理
	void Draw(void);																// 描画処理

	static CScene2D *Create(CScene::PRIORITY obj);									// 作成
	void BindTexture(LPDIRECT3DTEXTURE9 tex);										// テクスチャの設定

	void SetTransform(void);														// 頂点情報を更新
	void SetTransformTurnOver(void);												// 画像の反転
	void SetSize(D3DXVECTOR3 size);													// 大きさを設定
	void SetDrawPos(D3DXVECTOR2 tex);												// テクスチャの描画位置を設定
	void SpriteAnimation(D3DXVECTOR2 patternNo, int nPatternAnim, int nHeight);		// スプライトアニメーション
	void SetColor(D3DXCOLOR col);													// 色の設定
	void SetCenter(TEXTUREVTX vtx);													// 中心の設定
	void SetRotation(D3DXVECTOR3 rot);												// 回転量の設定

	bool HitCollision2D(CScene2D *obj, bool bTrigger);								//当たり判定(ぶつかる)
	bool InCollision2D(CScene2D *obj);												//当たり判定(中にいる)

	D3DXVECTOR2 GetTex(void) { return m_DrawPos; }									// テクスチャ位置の取得
	D3DXVECTOR3 GetRotation(void) { return m_rot; }									// 回転量の取得
	TEXTUREVTX GetCenter(void) { return m_Vtx; }									// 中心の取得
	D3DXCOLOR GetColor(void) { return m_col; }										// 色の取得
	D3DXVECTOR3 GetSize(void) { return m_size; }									// 大きさを取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	LPDIRECT3DTEXTURE9			m_pTexture;											// テクスチャのポインタ
	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;											// 頂点バッファへのポインタ
	TEXTUREVTX					m_Vtx;												// 中心
	D3DXVECTOR3					m_size;												// 大きさ
	D3DXVECTOR2					m_DrawPos;											// テクスチャの描画位置
	D3DXVECTOR3					m_rot;												// 回転量
	D3DXCOLOR					m_col;												// 色
	char						*m_pTexAdd;											// テクスチャアドレス
	float m_fAngle;																	// 角度
	float m_fLength;																// 長さ
};
#endif