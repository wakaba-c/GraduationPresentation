//=============================================================================
//
// ライブラリ処理 [ takaseiLibrary.cpp ]
// Author : Seiya Takahashi
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
	if (rot->x > D3DX_PI)
	{
		// 一周戻す
		rot->x -= D3DX_PI * 2;
	}
	else if (rot->x < -D3DX_PI)
	{// 差が-D3DX_PIより小さいとき
	 // 一周増やす
		rot->x += D3DX_PI * 2;
	}

	// 絶対値が0.0fより小さいとき
	if (fabsf(rot->x) < 0.0f)
	{
		// 差をなくす
		rot->x = 0.0f;
	}

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

	// 差がD3DX_PIより大きいとき
	if (rot->z > D3DX_PI)
	{
		// 一周戻す
		rot->z -= D3DX_PI * 2;
	}
	else if (rot->z < -D3DX_PI)
	{// 差が-D3DX_PIより小さいとき
	 // 一周増やす
		rot->z += D3DX_PI * 2;
	}

	// 絶対値が0.0fより小さいとき
	if (fabsf(rot->z) < 0.0f)
	{
		// 差をなくす
		rot->z = 0.0f;
	}
}

//=============================================================================
// 平方根計算
//=============================================================================
float CTakaseiLibrary::OutputSqrt(D3DXVECTOR3 difpos)
{
	float fSqrt = sqrtf(difpos.x * difpos.x + difpos.y * difpos.y + difpos.z * difpos.z);
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
// 二点間の距離計算
//=============================================================================
float CTakaseiLibrary::OutputDistance(const D3DXVECTOR3 & onePoint, const D3DXVECTOR3 & twoPoint)
{
	// 差計算
	D3DXVECTOR3 point = onePoint - twoPoint;

	// 点と点の距離
	float fDistance = sqrtf(point.x * point.x + point.y * point.y + point.z * point.z);

	// 距離返す
	return fDistance;
}

//=============================================================================
// 加速度計算
//=============================================================================
float CTakaseiLibrary::OutputAcceleration(const float & fVelocity, const float & fRadius)
{
	return (fVelocity * fVelocity) / fRadius;
}

//=============================================================================
// ベクトルの内積計算
//=============================================================================
float CTakaseiLibrary::OutputInnerProduct(const D3DXVECTOR3 & vecA, const D3DXVECTOR3 & vecB)
{
	// 角度
	float fAngle = 0.0f;

	fAngle = (vecA.x * vecB.x + vecA.y * vecB.y + vecA.z * vecB.z) / (OutputSqrt(vecA) * OutputSqrt(vecB));

	return fAngle;
}

//=============================================================================
// キャラクター姿勢行列算出関数
//=============================================================================
D3DXMATRIX * CTakaseiLibrary::CalcLookAtMatrix(D3DXMATRIX* pout, D3DXVECTOR3* pPos, D3DXVECTOR3* pLook, D3DXVECTOR3* pUp)
{
	D3DXVECTOR3 X, Y, Z;

	// 二点間の距離からベクトル算出
	Z = CTakaseiLibrary::OutputVector(*pPos, *pLook);

	// 正規化
	D3DXVec3Normalize(&Z, &Z);
	// 直交ベクトル計算
	D3DXVec3Cross(&X, D3DXVec3Normalize(&Y, pUp), &Z);
	// 正規化
	D3DXVec3Normalize(&X, &X);
	// 正規化
	D3DXVec3Normalize(&Y, D3DXVec3Cross(&Y, &Z, &X));

	// 回転行列
	pout->_11 = X.x; pout->_12 = X.y; pout->_13 = X.z; pout->_14 = 0;
	pout->_21 = Y.x; pout->_22 = Y.y; pout->_23 = Y.z; pout->_24 = 0;
	pout->_31 = Z.x; pout->_32 = Z.y; pout->_33 = Z.z; pout->_34 = 0;
	pout->_41 = 0.0f; pout->_42 = 0.0f; pout->_43 = 0.0f; pout->_44 = 1.0f;

	return pout;
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
