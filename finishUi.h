//=============================================================================
//
// ゴール用UI処理 [finishUi.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _FINISHUI_H_
#define _FINISHUI_H_

//=============================================================================
// インクルード
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// クラス定義
//=============================================================================
class CFinishUi : public CScene2D
{
public:
	CFinishUi(PRIORITY obj);						// プレイヤーのコンストラクタ(オブジェクトタイプ)
	~CFinishUi();									// プレイヤーのデストラクタ
	HRESULT Init(void);							// 初期化処理
	void Uninit(void);							// 開放処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static CFinishUi *Create(void);				// プレイヤー生成
	static HRESULT Load(void);					// 素材データの取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);							// デバッグ処理関数
#endif

	/*================= プレイヤー関連 =================*/
	D3DXVECTOR3	m_move;							// 移動量
	bool		m_bGravity;						// 重力の有無
};
#endif