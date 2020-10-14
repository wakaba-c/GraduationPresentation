//=============================================================================
//
// ボックスコライダー処理 [colliderBox.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _COLLIDERBOX_H_
#define _COLLIDERBOX_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "collider.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCube;

//=============================================================================
// クラス定義
//=============================================================================
class CColliderBox : public CCollider
{
public:
	CColliderBox(COLLISIONTYPE type);						// コンストラクタ処理
	~CColliderBox();										// デストラクタ処理
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CColliderBox *Create(bool bTrigger, D3DXVECTOR3 size);			// クリエイト処理

	void SetSize(D3DXVECTOR3 size);							// サイズの設定
	D3DXVECTOR3 GetSize(void) { return m_size; }			// サイズの取得

	void OnTriggerEnter(CCollider *col) {};					// 当たり判定(Trigger) のコールバック関数
	void OnCollisionEnter(CCollider *col) {};				// 当たり判定 のコールバック関数

private:
#ifdef _DEBUG
	CMeshCube *m_pCube;										// ボックスコライダーポインタ
#endif

	D3DXVECTOR3				m_size;							// サイズ
};
#endif