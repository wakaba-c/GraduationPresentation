//=============================================================================
//
// キーボード入力処理 [inputKeyboard.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "inputKeyboard.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// directinputの初期化生成
	CInput::Init(hInstance, hWnd);

	// 入力デバイス（キーボード）の作成
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	m_pDevice->Acquire();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// 入力デバイス(キーボード)の開放
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();			//キーボードへのアクセス権を開放
		m_pDevice->Release();
		m_pDevice = NULL;
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
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];				//キーボードの入力情報
	int nCntKey;

	// デバイスからデータを取得
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), aKeyState)))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			// トリガー・リリース情報の作成
			m_akeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] & aKeyState[nCntKey]) ^ aKeyState[nCntKey];
			m_akeyStateUp[nCntKey] = m_aKeyState[nCntKey] ^ (m_aKeyState[nCntKey] & aKeyState[nCntKey]);
			m_aKeyState[nCntKey] = aKeyState[nCntKey];	//キープレス情報保存
		}
	}
	else
	{
		m_pDevice->Acquire();				//キーボードへのアクセス権を取得
	}
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool CInputKeyboard::GetTriggerKeyboard(int nKey)
{
	return(m_akeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリリース状態を取得
//=============================================================================
bool CInputKeyboard::GetReleaseKeyboard(int nKey)
{
	return(m_akeyStateUp[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool CInputKeyboard::GetPressKeyboard(int nKey)
{
	return(m_aKeyState[nKey] & 0x80) ? true : false;
}