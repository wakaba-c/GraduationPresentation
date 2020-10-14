//=============================================================================
//
// 時間処理 [time.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _TIME_H_
#define _TIME_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_POINTER 2								//時計の針の数

//=============================================================================
// クラス定義
//=============================================================================
class CTime : public CScene
{
public:
	CTime(PRIORITY obj);							// コンストラクタ
	~CTime();										// デストラクタ
	HRESULT Init(void);								// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CTime *Create(void);						// クリエイト処理

	int GetMinutes(void) { return m_nMinutes; }		// 分数の取得
	int GetSeconds(void) { return m_nSeconds; }		// 秒数の取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	void SetTime(int nTime);	// 時間設定
	void SetSeconds(int nTime);	// 時間設定

	int m_nMinutes;				// 分
	int m_nSeconds;				// 秒
	int m_nMaxSeconds;			// 秒数の最大値
	DWORD m_dTimeFrame;			// フレーム数
	bool m_bStart;				// タイマーの動作
};
#endif