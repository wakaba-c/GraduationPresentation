//=============================================================================
//
// コントローラー入力処理 [inputController.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _INPUTCONTROLLER_H_
#define _INPUTCONTROLLER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "input.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	NUM_KEY_MAX			(256)						// キーの最大数
#define COUNT_WAIT_REPEAT	(20)						//リピート開始までの待ち時間
#define INTERVAL_REPEAT		(1)							//リピート間隔
#define	NUM_JOYPAD_MAX		(4)							//パッド最大数
#define JOY_MAX_RANGE		(1024.0f)					//パッドの有効範囲

//=============================================================================
// 構造体定義
//=============================================================================
// キーの種類
typedef enum
{
	JOYPADKEY_X = 0,				//[0]　Xボタン
	JOYPADKEY_Y,					//[1]　Yボタン
	JOYPADKEY_A,					//[2]　Aボタン
	JOYPADKEY_B,					//[3]　Bボタン
	JOYPADKEY_LEFT_SHOULDER,		//[4]　左手前ボタン
	JOYPADKEY_RIGHT_SHOULDER,		//[5]　右手前ボタン
	JOYPADKEY_LEFT_TRIGGER,			//[6]　左ボタン
	JOYPADKEY_RIGHT_TRIGGER,		//[7]　右ボタン
	JOYPADKEY_LEFT_THUMB,			//[8]　右スティックボタン
	JOYPADKEY_RIGHT_THUMB,			//[9]　左スティックボタン
	JOYPADKEY_BACK,					//[10]　バックボタン
	JOYPADKEY_START,				//[11]　スタートボタン
	JOYPADKEY_LEFT,					//[12]　方向キー[左]
	JOYPADKEY_RIGHT,				//[13]　方向キー[右]
	JOYPADKEY_UP,					//[14]　方向キー[上]
	JOYPADKEY_DOWN,					//[15]　方向キー[下]
	JOYPADKEY_MAX
}JOYPADKEY;

//=============================================================================
// クラス定義
//=============================================================================
class CInputController : public CInput
{
public:
	CInputController();											// コンストラクタ
	~CInputController();										// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);				// 初期化処理
	void Uninit(void);											// 開放処理
	void Update(void);											// 更新処理

	bool GetControllerPress(int nIDPad, JOYPADKEY nKey);		// プレス状態の取得
	bool GetControllerTrigger(int nIDPad, JOYPADKEY nKey);		// トリガー状態の取得
	bool GetControllerRelease(int nIDPad, JOYPADKEY nKey);		// リリース状態の取得

	int GetJoypadTriggerLeft(int nIDPad);						// 左トリガー状態の取得
	int GetJoypadTriggerRight(int nIDPad);						// 右トリガー状態の取得

	void GetJoypadStickLeft(int nIDPad, float *pValueH, float *pValueV);	// 左スティックの状態取得
	void GetJoypadStickRight(int nIDPad, float *pValueH, float *pValueV);	// 右スティックの状態取得

	void SetKeyStateJoypad(int nIDPad);							// ジョイパッドのキー情報設定
	bool GetJoypadUse(int nIDPad);								// ジョイパッドの使用許諾確認

private:
	static BOOL CALLBACK EnumJoyCallbackJoypad(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);				// ジョイスティック問い合わせ用コールバック関数
	static BOOL CALLBACK EnumAxesCallbackJoypad(const LPCDIDEVICEOBJECTINSTANCE lpddi, LPVOID pvRef);	// ジョイスティック問い合わせ用コールバック関数

	BYTE m_aJoyKeyState[NUM_KEY_MAX];										// キー状態
	static LPDIRECTINPUTDEVICE8 m_apDIDevJoypad[NUM_JOYPAD_MAX];			// IDirectInputDevice8へのポインタ（ジョイパッド）
	DIJOYSTATE2 m_aJoypadState[NUM_JOYPAD_MAX];								// ゲームパッド 状態を受け取るワーク
	bool m_aKeyStateJoypad[NUM_JOYPAD_MAX][NUM_KEY_MAX];					// ゲームパッド の押下状態を保持するワーク
	bool m_aKeyStateTriggerJoypad[NUM_JOYPAD_MAX][NUM_KEY_MAX];				// ゲームパッド のトリガー状態を保持するワーク
	bool m_aKeyStateReleaseJoypad[NUM_JOYPAD_MAX][NUM_KEY_MAX];				// ゲームパッド のリリース状態を保持するワーク
	D3DXVECTOR3 m_aKeyStateAxis[NUM_JOYPAD_MAX];							// 軸位置
	static int m_nCntPad;													// ゲームパッド の総数
	bool m_bUse[NUM_JOYPAD_MAX];											// 使用可能な状態かどうか
};
#endif