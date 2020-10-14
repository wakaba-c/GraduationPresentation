//=============================================================================
//
// スコア処理 [score.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SCORE_H_
#define _SCORE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_SCORE_NUMBER 8

//=============================================================================
// 前方宣言
//=============================================================================
class CNumber;

//=============================================================================
// クラス定義
//=============================================================================
class CScore : public CScene2D
{
public:
	CScore(PRIORITY obj);						// コンストラクタ
	~CScore();									// デストラクタ
	HRESULT Init(void);							// 初期化処理
	void Uninit(void);							// 開放処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static CScore *Create(void);				// クリエイト処理
	static HRESULT Load(void);					// ロード処理

	void SetNumberPos(D3DXVECTOR3 pos, float fInterval);		// 数字の位置設定
	void SetNumderSize(D3DXVECTOR3 size);						// 数字の大きさ設定
	void SetNumber(int nValue);					// 数の設定
	void AddNumber(int nValue);					// 数の加算

	int GetNumber(void) { return m_nNumber; }	// 数の取得

	void ShowInspector(void) {};

private:
	CNumber *m_apNumber[MAX_SCORE_NUMBER];		// 数字のポインタ
	int m_nNumber;								// 数
};
#endif