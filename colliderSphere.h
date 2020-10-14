//=============================================================================
//
// スフィアコライダー処理 [colliderSphere.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _COLLIDERSPHERE_H_
#define _COLLIDERSPHERE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "collider.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshSphere;

//=============================================================================
// クラス定義
//=============================================================================
class CColliderSphere : public CCollider
{
public:
	CColliderSphere(COLLISIONTYPE type);					// コンストラクタ
	~CColliderSphere();										// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CColliderSphere *Create(bool bTrigger, float fRadius);			// クリエイト処理

	void SetRadius(float fValue);							// 半径の設定
	float GetRadius(void) { return m_fRadius; }				// 半径の取得

	void OnTriggerEnter(CCollider *col) {};					// 当たり判定(Trigger) のコールバック関数
	void OnCollisionEnter(CCollider *col) {};				// 当たり判定 のコールバック関数

private:
#ifdef _DEBUG
	CMeshSphere *m_pSphere;									// スフィアコライダーポインタ
#endif

	float m_fRadius;						// 半径
};
#endif