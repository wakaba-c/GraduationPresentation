//=============================================================================
//
// クリアタイム処理 [clearTime.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _CLEARTIME_H_
#define _CLEARTIME_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_CLEARTIME_NUMBER 4

//=============================================================================
// 前方宣言
//=============================================================================
class CNumber;

//=============================================================================
// クラス定義
//=============================================================================
class CClearTime : public CScene2D
{
public:
	CClearTime(PRIORITY obj);		// コンストラクタ
	~CClearTime();					// デストラクタ
	HRESULT Init(void);				// 初期化処理
	void Uninit(void);				// 開放処理
	void Update(void);				// 更新処理
	void Draw(void);				// 描画処理

	static CClearTime *Create(void);		// クリエイト処理
	static HRESULT Load(void);				// ロード処理

	void SetNumberPos(D3DXVECTOR3 pos, float fInterval);	// 数字の位置設定
	void SetNumderSize(D3DXVECTOR3 size);					// 数字のサイズ設定
	void SetNumber(int nValue);								// 数の更新
	void AddNumber(int nValue);								// 数の加算

	int GetNumber(void) { return m_nNumber; }				// 数字の取得

	void ShowInspector(void) {};

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;					// テクスチャ

	CNumber *m_apNumber[MAX_CLEARTIME_NUMBER];				// 数字のポインタ
	CScene2D *m_pScene2D;									// カテゴリ画像のポインタ
	int m_nNumber;											// 数
};
#endif