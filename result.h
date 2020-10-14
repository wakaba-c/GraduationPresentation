//=============================================================================
//
// リザルト処理 [result.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _RESULT_H_
#define _RESULT_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// クラス定義
//=============================================================================
class CResult
{
public:
	CResult();								// コンストラクタ
	~CResult();								// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 開放処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static void LoadAsset(void);			// アセットの読み込み

	static void SetIdxKill(int nValue);		// 倒した数を設定
	static void SetSeconds(int nValue);		// クリアした秒数を設定
	static void SetMinutes(int nValue);		// クリアした分数を設定

private:
	static int m_nKill;						// 倒した数
	static int m_nSeconds;					// 秒数
	static int m_nMinutes;					// 分数

};
#endif