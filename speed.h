//=============================================================================
//
// 時速処理 [speed.h]
// Author : Seiya Takahashi
//
//=============================================================================
#ifndef _SPEED_H_
#define _SPEED_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_DIGIT 3		// 最大桁数

//=============================================================================
// 前方宣言
//=============================================================================
class CNumber;
class CUi;

//=============================================================================
// クラス定義
//=============================================================================
class CSpeed : public CScene
{
public:
	CSpeed(PRIORITY obj);							// コンストラクタ
	~CSpeed();										// デストラクタ
	HRESULT Init(void);								// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CSpeed *Create(void);					// クリエイト処理
	static void SetDigit(float fDigit) { m_fDigit = fDigit; }// 時速設定
	static float GetDigit(void) { return m_fDigit; }// 時速取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	void SetTime(int nTime);	// 時間設定

	CNumber *m_apNumber[MAX_DIGIT];// 数字ポインタ
	DWORD m_dTimeFrame;			// フレーム数

	static float m_fDigit;			// 時速
};
#endif