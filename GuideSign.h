//=============================================================================
//
// 案内?処理 [GuideSign.h]
// Author : Seiya Takahashi
//
//=============================================================================
#ifndef _GUIDESIGN_H_
#define _GUIDESIGN_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "sceneX.h"

//=============================================================================
// クラス定義
//=============================================================================
class CGuideSign : public CSceneX
{
public:
	CGuideSign(PRIORITY obj);						// コンストラクタ
	~CGuideSign();									// デストラクタ
	HRESULT Init(void);								// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CGuideSign *Create(void);				// クリエイト処理
	static HRESULT Load(void);						// ロード処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);								// デバッグ処理
#endif
};
#endif