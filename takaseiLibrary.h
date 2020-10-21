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

	// 処理：マトリックス計算
	// [pMtx] マトリックス情報
	// [pos]  位置
	// [rot]  回転
	static void CalcMatrix(D3DXMATRIX *pMtx, const D3DXVECTOR3 & pos, const D3DXVECTOR3 & rot);

	// 処理：回転の補正
	// [rot] 回転
	static void RotRevision(D3DXVECTOR3 *rot);

	// 処理：ビルボード化
	// [mtxWorld] マトリックス情報
	// [mtxView]  マトリックスビュー
	static void Billboard(D3DXMATRIX *mtxWorld, const D3DXMATRIX mtxView);

	// 処理：平方根計算
	// [difpos] 位置
	static float OutputSqrt(D3DXVECTOR3 difpos);

	// 処理：二つの点からベクトル算出
	// [start] 始点
	// [ned]   終点
	static D3DXVECTOR3 OutputVector(const D3DXVECTOR3 & start, const D3DXVECTOR3 & end);

	// 処理：二点間の距離計算
	// [onePoint] 一つ目の点
	// [twoPoint] 二つ目の点
	static float OutputDistance(const D3DXVECTOR3 & onePoint, const D3DXVECTOR3 & twoPoint);

	static HRESULT Up(CInputKeyboard	*Key, CInputController *pGamepad);	// 上入力
	static HRESULT Down(CInputKeyboard	*Key, CInputController *pGamepad);	// 下入力
	static HRESULT Left(CInputKeyboard	*Key, CInputController *pGamepad);	// 左入力
	static HRESULT Right(CInputKeyboard *Key, CInputController *pGamepad);	// 右入力
	static HRESULT Decide(CInputKeyboard *Key, CInputController *pGamepad);	// 決定入力
	static HRESULT Return(CInputKeyboard *Key, CInputController *pGamepad);	// 戻り入力
	static HRESULT Pause(CInputKeyboard *Key, CInputController *pGamepad);	// ポーズ入力

private:

};
#endif