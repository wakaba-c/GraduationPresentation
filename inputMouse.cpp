//=============================================================================
//
// マウス入力処理 [inputMouse.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "inputMouse.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CInputMouse::CInputMouse()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CInputMouse::~CInputMouse()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// directinputの初期化生成
	CInput::Init(hInstance, hWnd);

	// DirectInputオブジェクトの作成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, NULL)))
	{
		return E_FAIL;
	}

	m_pInput->CreateDevice(GUID_SysMouse, &m_pDevMouse, NULL);
	m_pDevMouse->SetDataFormat(&c_dfDIMouse2); // マウス用のデータ・フォーマットを設定

	m_pDevMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL; // 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）
	m_pDevMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	// 入力制御開始
	m_pDevMouse->Acquire();

	// 代入
	m_hWnd = hWnd;

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CInputMouse::Uninit(void)
{
	// 入力デバイス(マウス)の開放
	if (m_pDevMouse != NULL)
	{
		m_pDevMouse->Unacquire();	// マウスへのアクセス権を開放
		m_pDevMouse->Release();
		m_pDevMouse = NULL;
	}
	// DirectInputオブジェクトの開放
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CInputMouse::Update(void)
{
	// 変数宣言
	int nCntButton;
	POINT point;

	// 値の初期化
	m_mouseState.rgbButtons[0] = 0;

	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);
	m_posX = (float)point.x;
	m_posY = (float)point.y;

	// 値の更新
	if (SUCCEEDED(m_pDevMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_mouseState)))
	{
		for (nCntButton = 0; nCntButton < NUM_BUTTON_MAX; nCntButton++)
		{
			// 排他的論理和かどうか判断
			m_aButtonRelease[nCntButton] = m_aButtonState[nCntButton] ^ (m_mouseState.rgbButtons[nCntButton] & m_mouseState.rgbButtons[nCntButton]);
			m_aButtonTrigger[nCntButton] = (m_aButtonState[nCntButton] & m_mouseState.rgbButtons[nCntButton]) ^ m_mouseState.rgbButtons[nCntButton];
			m_aButtonState[nCntButton] = m_mouseState.rgbButtons[nCntButton];	// キープレス情報保存
		}
	}
	else
	{
		m_pDevMouse->Acquire(); // １発目や２発目にエラーが出るが無視してよい。
	}
}

//=============================================================================
// プレス状態を取得
//=============================================================================
bool CInputMouse::GetPressMouse(int nButton)
{
	return (m_mouseState.rgbButtons[nButton] & 0x80) ? true : false;
}

//=============================================================================
// トリガー状態の取得
//=============================================================================
bool CInputMouse::GetTriggerMouse(int nButton)
{
	return (m_aButtonTrigger[nButton] & 0x80) ? true : false;
}

//=============================================================================
// リリース状態の取得
//=============================================================================
bool CInputMouse::GetReleaseMouse(int nButton)
{
	return (m_aButtonRelease[nButton] & 0x80) ? true : false;
}

//=============================================================================
// マウスのX座標の取得
//=============================================================================
LONG CInputMouse::GetMouseX(void)
{
	return (LONG)m_posX;
}

//=============================================================================
// マウスのY座標の取得
//=============================================================================
LONG CInputMouse::GetMouseY(void)
{
	return ((LONG)m_posY);
}