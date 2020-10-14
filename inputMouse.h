//=============================================================================
//
// マウス入力処理 [inputMouse.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _INPUTMOUSE_H_
#define _INPUTMOUSE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "input.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	NUM_BUTTON_MAX			8		// ボタン(マウス)の最大数
#define MOUSE_LEFT				0		//左ボタン
#define MOUSE_RIGHT				1		//右ボタン
#define MOUSE_CENTER			2		//中ボタン

//=============================================================================
// クラス定義
//=============================================================================
class CInputMouse : public CInput
{
public:
	CInputMouse();									// コンストラクタ
	~CInputMouse();									// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);	// 初期化処理
	void Uninit(void);								// 開放処理
	void Update(void);								// 更新処理

	bool GetPressMouse(int nButton);				// プレス状態の取得
	bool GetTriggerMouse(int nButton);				// トリガー状態の取得
	bool GetReleaseMouse(int nButton);				// リリース状態の取得
	LONG GetMouseX(void);							// マウスのX座標の取得
	LONG GetMouseY(void);							// マウスのY座標の取得

private:
	DIMOUSESTATE2 m_mouseState;						// マウスのステータス
	float m_posX;									//X座標
	float m_posY;									//Y座標

	LPDIRECTINPUTDEVICE8	m_pDevMouse = NULL;						// 入力デバイス(マウス)へのポインタ
	BYTE					m_aButtonState[NUM_BUTTON_MAX];			// マウスの入力情報ワーク
	BYTE					m_aButtonTrigger[NUM_BUTTON_MAX];		// マウスのトリガー
	BYTE					m_aButtonRelease[NUM_BUTTON_MAX];		// マウスのリリース
	HWND					m_hWnd;									// ウィンドウハンドル
};
#endif