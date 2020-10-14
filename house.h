//=============================================================================
//
// 家処理 [house.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _HOUSE_H_
#define _HOUSE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "object.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define DEPTH	20											// 奥行き
#define WIDE 20												// 幅

//=============================================================================
// 前方宣言
//=============================================================================
class CHouseUi;
class CPhase;
class CEnemyUi;

//=============================================================================
// クラス定義
//=============================================================================
class CHouse : public CObject
{
public:
	CHouse(PRIORITY obj);										// コンストラクタ
	~CHouse();													// デストラクタ
	HRESULT Init(void);											// 初期化処理
	void Uninit(void);											// 開放処理
	void Update(void);											// 更新処理
	void Draw(void);											// 描画処理

	static CHouse *Create();									// クリエイト処理
	static HRESULT Load(void);									// ロード処理

	CPhase *GetPhase(void) { return m_pPhase; }					// フェーズ数のポインタ取得
	CEnemyUi *GetEnemyUi(void) { return m_pEnemyUi; }			// 敵の残り数のポインタ取得

	void OnTriggerEnter(CCollider *col);
	void OnCollisionEnter(CCollider *col);

private:
#ifdef _DEBUG
	void Debug(void);											// デバッグ処理
#endif

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;							// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;							// インデックスバッファへのポインタ

	static LPD3DXMESH m_pMesh;									// メッシュ情報へのポインタ
	static DWORD m_nNumMat;										// マテリアル情報の数
	static LPD3DXBUFFER	m_pBuffMat;								// マテリアル情報へのポインタ

	CHouseUi *m_pHouseUi;										// おばあちゃん家の耐久値UI
	CPhase *m_pPhase;											// 現在のphase数のUI
	CEnemyUi *m_pEnemyUi;										// 敵の残り数のUI

	int						m_nLife;							// 家の耐久値

	CColliderBox *m_ColliderBox;								// ボックスコライダーのポインタ
	CColliderSphere *m_ColliderSphere;							// スフィアコライダーのポインタ
};
#endif