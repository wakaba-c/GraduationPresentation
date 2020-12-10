//=============================================================================
//
// スタートシグナル処理 [startSignal.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _STARTSIGNAL_H_
#define _STARTSIGNAL_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CTime;

//=============================================================================
// クラス定義
//=============================================================================
class CStartSignal : public CScene
{
public:
	CStartSignal(PRIORITY obj);					// コンストラクタ
	~CStartSignal();							// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 開放処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static CStartSignal *Create(void);			// クリエイト処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	CTime *m_pTime;
	CUi *m_pUi;

	bool m_bEnd;
};
#endif