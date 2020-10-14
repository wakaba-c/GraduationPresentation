//=============================================================================
//
// コントローラー処理 [inputController.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "inputController.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPDIRECTINPUTDEVICE8 CInputController::m_apDIDevJoypad[NUM_JOYPAD_MAX] = {};		// IDirectInputDevice8へのポインタ（ジョイパッド）
int CInputController::m_nCntPad = 0;												// ジョイパッドの個数

//=============================================================================
// コンストラクタ
//=============================================================================
CInputController::CInputController()
{
	// 値の初期化
	for (int nCount = 0; nCount < NUM_JOYPAD_MAX; nCount++)
	{
		m_bUse[nCount] = false;		// 使用不能にする
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
CInputController::~CInputController()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CInputController::Init(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	// directinputの初期化生成
	CInput::Init(hInstance, hWnd);

	hr = m_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyCallbackJoypad, NULL, DIEDFL_ATTACHEDONLY);

	for (int nCount = 0; nCount < NUM_KEY_MAX; nCount++)
	{
		if (FAILED(hr) || m_apDIDevJoypad[nCount] == NULL)
		{
			return hr;
		}

		// 値の初期化
		m_aJoypadState[nCount].lX = (LONG)0.0;
		m_aJoypadState[nCount].lY = (LONG)0.0;
		m_aJoypadState[nCount].lZ = (LONG)0.0;
		m_aJoypadState[nCount].lRz = (LONG)0.0;

		// データフォーマットを設定
		hr = m_apDIDevJoypad[nCount]->SetDataFormat(&c_dfDIJoystick2);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ジョイパッドのデータフォーマットを設定できませんでした。", "警告", MB_ICONWARNING);
			return hr;
		}

		// 協調モードを設定（フォアグラウンド＆非排他モード）
		hr = m_apDIDevJoypad[nCount]->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ジョイパッドの協調モードを設定できませんでした。", "警告", MB_ICONWARNING);
			return hr;
		}

		hr = m_apDIDevJoypad[nCount]->EnumObjects(EnumAxesCallbackJoypad, NULL, DIDFT_AXIS);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ジョイパッドが見つかりませんでした。", "警告", MB_ICONWARNING);
			return hr;
		}

		// デバイスの設定
		DIPROPRANGE diprg;

		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);

		diprg.diph.dwObj = DIJOFS_X;
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -(LONG)JOY_MAX_RANGE;
		diprg.lMax = (LONG)JOY_MAX_RANGE;

		// X方向の範囲を指定
		m_apDIDevJoypad[nCount]->SetProperty(DIPROP_RANGE, &diprg.diph);

		diprg.diph.dwObj = DIJOFS_Y;
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -(LONG)JOY_MAX_RANGE;
		diprg.lMax = (LONG)JOY_MAX_RANGE;

		// Y方向の範囲を指定
		m_apDIDevJoypad[nCount]->SetProperty(DIPROP_RANGE, &diprg.diph);

		diprg.diph.dwObj = DIJOFS_Z;
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -(LONG)JOY_MAX_RANGE;
		diprg.lMax = (LONG)JOY_MAX_RANGE;

		// Z方向の範囲を指定
		m_apDIDevJoypad[nCount]->SetProperty(DIPROP_RANGE, &diprg.diph);

		diprg.diph.dwObj = DIJOFS_RZ;
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -(LONG)JOY_MAX_RANGE;
		diprg.lMax = (LONG)JOY_MAX_RANGE;

		// RZ方向の範囲を指定
		m_apDIDevJoypad[nCount]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// コントローラーへのアクセス権を獲得(入力制御開始)
		m_apDIDevJoypad[nCount]->Acquire();
		m_bUse[nCount] = true;														// 使用可能に設定

		// ワーククリア
		memset(&m_aJoypadState[nCount], 0, sizeof m_aJoyKeyState[nCount]);
		memset(m_aKeyStateJoypad[nCount], 0, sizeof m_aKeyStateJoypad[nCount]);
		memset(m_aKeyStateTriggerJoypad[nCount], 0, sizeof m_aKeyStateTriggerJoypad[nCount]);
		memset(m_aKeyStateReleaseJoypad[nCount], 0, sizeof m_aKeyStateReleaseJoypad[nCount]);
	}

	return hr;
}

//=============================================================================
// 開放処理
//=============================================================================
void CInputController::Uninit(void)
{
	// ジョイパッドの終了処理
	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		// 入力デバイス(コントローラー)の開放
		if (m_apDIDevJoypad[m_nCntPad] != NULL)
		{
			m_apDIDevJoypad[m_nCntPad]->Unacquire();			//コントローラーへのアクセス権を開放
			m_apDIDevJoypad[m_nCntPad]->Release();
			m_apDIDevJoypad[m_nCntPad] = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CInputController::Update(void)
{
	HRESULT hr;
	bool aKeyStateOld[JOYPADKEY_MAX];

	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		if (m_apDIDevJoypad[nCntPad] != NULL)
		{
			// 前回のデータを保存
			for (int nCntKey = 0; nCntKey < JOYPADKEY_MAX; nCntKey++)
			{
				aKeyStateOld[nCntKey] = m_aKeyStateJoypad[nCntPad][nCntKey];
			}

			// デバイスからデータを取得
			hr = m_apDIDevJoypad[nCntPad]->GetDeviceState(sizeof(m_aJoypadState[nCntPad]), &m_aJoypadState[nCntPad]);
			if (SUCCEEDED(hr))
			{
				// キー情報設定
				SetKeyStateJoypad(nCntPad);

				// 軸位置を記録
				m_aKeyStateAxis[nCntPad].x = (float)m_aJoypadState[nCntPad].lX / JOY_MAX_RANGE;
				m_aKeyStateAxis[nCntPad].y = (float)m_aJoypadState[nCntPad].lY / JOY_MAX_RANGE;
				m_aKeyStateAxis[nCntPad].z = (float)m_aJoypadState[nCntPad].lZ / JOY_MAX_RANGE;

				for (int nCntKey = 0; nCntKey < JOYPADKEY_MAX; nCntKey++)
				{
					// トリガー・リリース情報の作成
					m_aKeyStateReleaseJoypad[nCntPad][nCntKey] = aKeyStateOld[nCntKey] ^ (aKeyStateOld[nCntKey] & m_aKeyStateJoypad[nCntPad][nCntKey]);
					m_aKeyStateTriggerJoypad[nCntPad][nCntKey] = (aKeyStateOld[nCntKey] & m_aKeyStateJoypad[nCntPad][nCntKey]) ^ m_aKeyStateJoypad[nCntPad][nCntKey];
				}
			}
			else
			{
				m_apDIDevJoypad[nCntPad]->Acquire();
			}
		}
	}
#ifdef _DEBUG
	CDebugProc::Log("ゲームパッドの総数 : %d\n", m_nCntPad);
#endif
}

//=============================================================================
// ジョイスティック問い合わせ用コールバック関数
//=============================================================================
BOOL CALLBACK CInputController::EnumJoyCallbackJoypad(const DIDEVICEINSTANCE* lpddi, VOID* pvRef)
{
	static GUID pad_discrimination[NUM_JOYPAD_MAX];	// 各デバイスの識別子を格納
	DIDEVCAPS	diDevCaps;				// デバイス情報
	HRESULT		hRes;

	// ジョイスティック用デバイスオブジェクトを作成
	hRes = m_pInput->CreateDevice(lpddi->guidInstance, &m_apDIDevJoypad[m_nCntPad], NULL);

	if (FAILED(hRes))
	{// デバイスオブジェクト の生成ができなかったとき
		MessageBox(NULL, "デバイスオブジェクトが生成できませんでした", "警告", MB_ICONWARNING);
		return DIENUM_CONTINUE;			// 列挙を続ける
	}

	// ジョイスティックの能力を調べる
	diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hRes = m_apDIDevJoypad[m_nCntPad]->GetCapabilities(&diDevCaps);
	if (FAILED(hRes))
	{
		if (m_apDIDevJoypad[m_nCntPad])
		{
			m_apDIDevJoypad[m_nCntPad]->Release();
			m_apDIDevJoypad[m_nCntPad] = NULL;
			return DIENUM_CONTINUE;			// 列挙を続ける
		}
	}

	// デバイスの識別子を保存
	pad_discrimination[m_nCntPad] = lpddi->guidInstance;

	// このデバイスを使うので列挙を終了する
	m_nCntPad++;																	// ゲームパッド の総数に1台加える

	return DIENUM_STOP;			// 列挙を続ける
}

//=============================================================================
// ジョイスティック問い合わせ用コールバック関数
//=============================================================================
BOOL CALLBACK CInputController::EnumAxesCallbackJoypad(const LPCDIDEVICEOBJECTINSTANCE lpddi, LPVOID pvRef)
{
	HRESULT hr;

	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		DIPROPRANGE diprg;

		// スティック軸の値の範囲を設定（-32768～32767）
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwObj = lpddi->dwType;
		diprg.diph.dwHow = DIPH_BYID;
		diprg.lMin = -32768;
		diprg.lMax = 32767;

		if (m_apDIDevJoypad[nCntPad] != NULL)
		{
			hr = m_apDIDevJoypad[nCntPad]->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
	}

	return DIENUM_CONTINUE;

}

//=============================================================================
// ジョイパッドのキー情報設定
//=============================================================================
void CInputController::SetKeyStateJoypad(int nIDPad)
{
	if (m_aJoypadState[nIDPad].rgdwPOV[0] >= 225 * 100 && m_aJoypadState[nIDPad].rgdwPOV[0] <= 315 * 100)
	{//十字キー[左]が押されている
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_LEFT] = true;
	}
	else
	{
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_LEFT] = false;
	}

	if (m_aJoypadState[nIDPad].rgdwPOV[0] >= 45 * 100 && m_aJoypadState[nIDPad].rgdwPOV[0] <= 135 * 100)
	{//十字キー[右]が押されている
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_RIGHT] = true;
	}
	else
	{
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_RIGHT] = false;
	}

	if ((m_aJoypadState[nIDPad].rgdwPOV[0] >= 315 * 100 && m_aJoypadState[nIDPad].rgdwPOV[0] <= 360 * 100) || (m_aJoypadState[nIDPad].rgdwPOV[0] >= 0 * 100 && m_aJoypadState[nIDPad].rgdwPOV[0] <= 45 * 100))
	{//十字キー[上]が押されている
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_UP] = true;
	}
	else
	{
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_UP] = false;
	}

	if (m_aJoypadState[nIDPad].rgdwPOV[0] >= 135 * 100 && m_aJoypadState[nIDPad].rgdwPOV[0] <= 225 * 100)
	{//十字キー[下]が押されている
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_DOWN] = true;
	}
	else
	{
		m_aKeyStateJoypad[nIDPad][JOYPADKEY_DOWN] = false;
	}

	for (int nKey = JOYPADKEY_X; nKey <= JOYPADKEY_START; nKey++)
	{
		if (m_aJoypadState[nIDPad].rgbButtons[nKey])
		{//ボタンが押されている
			m_aKeyStateJoypad[nIDPad][nKey] = true;
		}
		else
		{
			m_aKeyStateJoypad[nIDPad][nKey] = false;
		}
	}
}

//=============================================================================
// ジョイパッドの使用許諾確認
//=============================================================================
bool CInputController::GetJoypadUse(int nIDPad)
{
	return m_bUse[nIDPad];											// 選択されたコントローラーの状態を返す
}

//=============================================================================
// プレス状態の取得
//=============================================================================
bool CInputController::GetControllerPress(int nIDPad, JOYPADKEY nKey)
{
	return m_aKeyStateJoypad[nIDPad][nKey];
}

//=============================================================================
// トリガー状態の取得
//=============================================================================
bool CInputController::GetControllerTrigger(int nIDPad, JOYPADKEY nKey)
{
	return m_aKeyStateTriggerJoypad[nIDPad][nKey];
}

//=============================================================================
// リリース状態の取得
//=============================================================================
bool CInputController::GetControllerRelease(int nIDPad, JOYPADKEY nKey)
{
	return m_aKeyStateReleaseJoypad[nIDPad][nKey];
}

//=============================================================================
// ジョイパッドのトリガー（左）の値取得
//=============================================================================
int CInputController::GetJoypadTriggerLeft(int nIDPad)
{
	return m_aJoypadState[nIDPad].rgbButtons[JOYPADKEY_LEFT_TRIGGER];
}

//=============================================================================
// ジョイパッドのトリガー（右）の値取得
//=============================================================================
int CInputController::GetJoypadTriggerRight(int nIDPad)
{
	return m_aJoypadState[nIDPad].rgbButtons[JOYPADKEY_RIGHT_TRIGGER];
}

//=============================================================================
// ジョイパッドのアナログスティック（左）の値取得
//=============================================================================
void CInputController::GetJoypadStickLeft(int nIDPad, float *pValueH, float *pValueV)
{
	*pValueV = -m_aJoypadState[nIDPad].lY / JOY_MAX_RANGE;
	*pValueH = -m_aJoypadState[nIDPad].lX / JOY_MAX_RANGE;
}

//=============================================================================
// ジョイパッドのアナログスティック（右）の値取得
//=============================================================================
void CInputController::GetJoypadStickRight(int nIDPad, float *pValueH, float *pValueV)
{
	*pValueH = -m_aJoypadState[nIDPad].lZ / JOY_MAX_RANGE;
	*pValueV = -m_aJoypadState[nIDPad].lRz / JOY_MAX_RANGE;
}