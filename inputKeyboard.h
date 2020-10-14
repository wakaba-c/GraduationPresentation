//=============================================================================
//
// 入力処理 [inputKeyboard.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _INPUTKEYBOARD_H_
#define _INPUTKEYBOARD_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "input.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	NUM_KEY_MAX			(256)	// キーの最大数

//=============================================================================
// クラス定義
//=============================================================================
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();								// コンストラクタ
	~CInputKeyboard();								// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);	// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理

	bool GetTriggerKeyboard(int nKey);				// トリガー状態を取得
	bool GetReleaseKeyboard(int nKey);				// リピート状態を取得
	bool GetPressKeyboard(int nKey);				// プレス状態を取得

private:
	BYTE m_aKeyState[NUM_KEY_MAX] = {};				// プレス状態
	BYTE m_akeyStateTrigger[NUM_KEY_MAX] = {};		// トリガー状態
	BYTE m_akeyStateUp[NUM_KEY_MAX] = {};			// リリース状態
};
#endif