//=============================================================================
//
//	ライブラリ処理 [ kananlibrary.cpp ]
// Author : KANAN NAGANAWA
//
//=============================================================================
#include "takaseiLibrary.h"
#include "manager.h"
#include "renderer.h"
#include "scene2D.h"
#include "scene3D.h"
#include "sceneX.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "light.h"
#include "game.h"

//=============================================================================
// マトリックス計算
//=============================================================================
void CTakaseiLibrary::CalcMatrix(D3DXMATRIX * pMtx, const D3DXVECTOR3 & pos, const D3DXVECTOR3 & rot)
{
	D3DXMATRIX	mtxRot, mtxTrans;			//計算用

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(pMtx);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(pMtx, pMtx, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(pMtx, pMtx, &mtxTrans);
}

//=============================================================================
// 回転の補正
//=============================================================================
void CTakaseiLibrary::RotRevision(D3DXVECTOR3 * rot)
{
	// 差がD3DX_PIより大きいとき
	if (rot->y > D3DX_PI)
	{
		// 一周戻す
		rot->y -= D3DX_PI * 2;
	}
	else if (rot->y < -D3DX_PI)
	{// 差が-D3DX_PIより小さいとき
		// 一周増やす
		rot->y += D3DX_PI * 2;
	}

	// 絶対値が0.0fより小さいとき
	if (fabsf(rot->y) < 0.0f)
	{
		// 差をなくす
		rot->y = 0.0f;
	}
}

//=============================================================================
// 平方根計算
//=============================================================================
float CTakaseiLibrary::OutputSqrt(D3DXVECTOR3 difpos)
{
	float fSqrt = sqrt(difpos.x * difpos.x + difpos.y * difpos.y + difpos.z * difpos.z);
	return fSqrt;
}

//=============================================================================
// ベクトル計算
//=============================================================================
D3DXVECTOR3 CTakaseiLibrary::OutputVector(const D3DXVECTOR3 & start, const D3DXVECTOR3 & end)
{
	// ベクトルを保存する変数
	D3DXVECTOR3 vector = end - start;
	// ベクトルを正規化
	D3DXVec3Normalize(&vector, &vector);
	// ベクトルを返す
	return vector;
}

//=============================================================================
// ビルボード化処理
//=============================================================================
void CTakaseiLibrary::Billboard(D3DXMATRIX *mtxWorld, const D3DXMATRIX mtxView)
{
	mtxWorld->_11 = mtxView._11;
	mtxWorld->_12 = mtxView._21;
	mtxWorld->_13 = mtxView._31;
	mtxWorld->_21 = mtxView._12;
	mtxWorld->_22 = mtxView._22;
	mtxWorld->_23 = mtxView._32;
	mtxWorld->_31 = mtxView._13;
	mtxWorld->_32 = mtxView._23;
	mtxWorld->_33 = mtxView._33;
}

//=============================================================================
// 上入力
//=============================================================================
HRESULT CTakaseiLibrary::Up(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_W))
		return S_OK;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// 左スティック取得
		float fValueX, fValueZ;
		pGamepad->GetJoypadStickLeft(0, &fValueX, &fValueZ);

		// スティック入力
		if (fValueX <= 1.0f && fValueZ > 0.0f)
			return S_OK;

		// キー入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_UP))
			return S_OK;
	}

	// 入力無し
	return E_FAIL;
}

//=============================================================================
// 下入力
//=============================================================================
HRESULT CTakaseiLibrary::Down(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_S))
		return S_OK;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// 左スティック取得
		float fValueX, fValueZ;
		pGamepad->GetJoypadStickLeft(0, &fValueX, &fValueZ);

		// スティック入力
		if (fValueX >= -1.0f && fValueZ < 0.0f)
			return S_OK;

		// キー入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_DOWN))
			return S_OK;
	}

	// 入力無し
	return E_FAIL;
}

//=============================================================================
// 左入力
//=============================================================================
HRESULT CTakaseiLibrary::Left(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_A))
		return TRUE;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// 左スティック取得
		float fValueX, fValueZ;
		pGamepad->GetJoypadStickLeft(0, &fValueX, &fValueZ);

		// スティック入力
		if (fValueX < 0 && fValueZ >= -1.0f)
			return TRUE;

		// キー入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_LEFT))
			return TRUE;
	}

	// 入力無し
	return FALSE;
}

//=============================================================================
// 右入力
//=============================================================================
HRESULT CTakaseiLibrary::Right(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_D))
		return TRUE;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// 左スティック取得
		float fValueX, fValueZ;
		pGamepad->GetJoypadStickLeft(0, &fValueX, &fValueZ);

		// スティック入力
		if (fValueX < 0 && fValueZ <= 1.0f)
			return TRUE;

		// キー入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_RIGHT))
			return TRUE;
	}

	// 入力無し
	return FALSE;
}

//=============================================================================
// 決定入力
//=============================================================================
HRESULT CTakaseiLibrary::Decide(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_RETURN))
		return S_OK;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// スティック入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_A))
			return S_OK;
	}

	// 入力無し
	return E_FAIL;
}

//=============================================================================
// 戻り入力
//=============================================================================
HRESULT CTakaseiLibrary::Return(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_BACKSPACE))
		return S_OK;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// スティック入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_B))
			return S_OK;
	}

	// 入力無し
	return E_FAIL;
}

//=============================================================================
// ポーズ入力
//=============================================================================
HRESULT CTakaseiLibrary::Pause(CInputKeyboard * Key, CInputController * pGamepad)
{
	// キーボード入力
	if (Key && Key->GetTriggerKeyboard(DIK_P))
		return TRUE;

	// ゲームパッド接続確認
	if (pGamepad->GetJoypadUse(0))
	{
		// スティック入力
		if (pGamepad->GetControllerTrigger(0, JOYPADKEY_START))
			return TRUE;
	}

	// 入力無し
	return FALSE;
}
