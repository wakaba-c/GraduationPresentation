//=============================================================================
//
// 目的表示処理 [messageWindow.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MESSAGEWINDOW_H_
#define _MESSAGEWINDOW_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "scene2D.h"

//=============================================================================
// 列挙型定義
//=============================================================================
typedef enum
{
	FADE_NONE = 0,		// 何もない状態
	FADE_IN,			// フェードイン処理
	FADE_TELOP,			// テロップ処理
	FADE_OUT,			// フェードアウト処理
	FADE_MAX
} FADE;

//=============================================================================
// クラス定義
//=============================================================================
class CMessageWindow : public CScene2D
{
public:
	// テロップの種類
	typedef enum
	{
		MESSAGETYPE_NONE = -1,		// メッセージなし
		MESSAGETYPE_START = 0,		// 家防衛開始
		MESSAGETYPE_CLEAR,			// 家防衛クリア
		MESSAGETYPE_MAX
	} MESSAGETYPE;

	CMessageWindow(PRIORITY obj);			// コンストラクタ
	~CMessageWindow();						// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 開放処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static CMessageWindow *Create(MESSAGETYPE type);		// クリエイト処理
	static HRESULT Load(void);								// ロード処理

	void ShowInspector(void) {};

private:
	CScene2D *m_pMessage;					// メッセージのポインタ
	D3DXCOLOR	m_col;						// 色
	D3DXVECTOR3 m_size;						// フレーム以外の大きさ
	D3DXVECTOR3 m_frameSize;				// フレームの大きさ
	MESSAGETYPE m_type;						// メッセージタイプ
	FADE		m_fade;						// フェードのタイプ
	float m_nDamage;						// ダメージ量
	float m_fHP;							// HP
	int		m_nPage;						// ページ数
	int		m_nCount;						// カウンター
	float m_fAngle;							// 半径
	float m_fLength;						// 長さ
};
#endif