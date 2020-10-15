//=============================================================================
//
// ライブラリ処理 [ takaseiLbrary.h ]
// Author : Seiya Takahashi
//
//=============================================================================
#ifndef _TAKASEILIBRARY_H_
#define _TAKASEILIBRARY_H_

#define _CRT_SECURE_NO_WARNINGS // 警告除去

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
// ベクトル
#define ZeroVector3		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
#define ZeroVector4		(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f))

// カラー
#define ZeroColor		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f))		// 無色
#define AlphaColor		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f))		// 透明
#define NormalColor		(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))		// 通常
#define BlackColor		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f))		// 黒

// 時間
#define ONE_SECOND_FPS		(60)								// 一秒毎のフレーム数

//=============================================================================
// 前方宣言
//=============================================================================
class CInputKeyboard;
class CInputController;

//=============================================================================
// 構造体定義
//=============================================================================
typedef enum
{	// 移動用のキー
	MOVE_ACCEL = DIK_W,		// アクセル入力
	MOVE_BRAKE = DIK_S,		// ブレーキ入力
	MOVE_LEFT  = DIK_A,		// 左入力
	MOVE_RIGHT = DIK_D,		// 右入力
	MOVE_JUMP  = DIK_SPACE,	// ジャンプ入力
} PLAYER_MOVE_KEY;

//=============================================================================
// クラス定義
//=============================================================================
class CTakaseiLibrary
{
public:
	CTakaseiLibrary() {};
	~CTakaseiLibrary() {};

	static void CalcMatrix(D3DXMATRIX *pMtx, const D3DXVECTOR3 & pos, const D3DXVECTOR3 & rot);			// マトリックス計算

	static void Billboard(D3DXMATRIX *mtxWorld, const D3DXMATRIX mtxView);								// ビルボード化処理

	static HRESULT Up(CInputKeyboard	*Key, CInputController *pGamepad);	// 上入力
	static HRESULT Down(CInputKeyboard	*Key, CInputController *pGamepad);	// 下入力
	static HRESULT Left(CInputKeyboard	*Key, CInputController *pGamepad);	// 左入力
	static HRESULT Right(CInputKeyboard *Key, CInputController *pGamepad);	// 右入力
	static HRESULT Decide(CInputKeyboard *Key, CInputController *pGamepad);	// 決定入力
	static HRESULT Return(CInputKeyboard *Key, CInputController *pGamepad);	// 戻り入力
	static HRESULT Pause(CInputKeyboard *Key, CInputController *pGamepad);	// ポーズ入力

	static float OutputSqrt(D3DXVECTOR3 difpos);				// 平方根計算
	static float OutputDistance(D3DXVECTOR3 difpos);			// 距離計算
	static D3DXVECTOR3 OutputVector(const D3DXVECTOR3 & start, const D3DXVECTOR3 & end);	// ベクトル計算

private:

};
#endif