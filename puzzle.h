//=============================================================================
//
// パズル処理 [puzzle.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _PUZZLE_H_
#define _PUZZLE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "box.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CBox;

//=============================================================================
// クラス定義
//=============================================================================
class CPuzzle
{
public:
	CPuzzle();					// コンストラクタ
	~CPuzzle();					// デストラクタ
	HRESULT Init(void);			// 初期化処理
	void Uninit(void);			// 開放処理
	void Update(void);			// 更新処理
	void Draw(void);			// 描画処理

	static void	LoadAsset(void);												// アセットの読み込み処理
	static float GetSpeed(int nPieceNum) { return m_fSpeed[nPieceNum]; }		// スピードの取得
	static float GetRate(int nPieceNum) { return m_fRate[nPieceNum]; }			// スピード上昇率の取得
	static float GetTurning(int nPieceNum) { return m_fTurning[nPieceNum]; }	// 旋回速度取得
	static float GetDecay(int nPieceNum) { return m_fDecay[nPieceNum]; }		// 減衰率取得
	static int GetPower(int nPieceNum) { return m_nPower[nPieceNum]; }			// パワー取得
	static int GetPieceNum(void) { return m_nPieceNum; }						// ピース数
	static bool GetRoute(int nPieceNum) { return m_bRoute[nPieceNum]; }			// ルート

private:
	CBox *m_pBox;								// ボックスのポインタ

	static float m_fSpeed[Piece_Num];			// スピード
	static float m_fRate[Piece_Num];			// スピード上昇率
	static float m_fTurning[Piece_Num];			// 旋回速度
	static float m_fDecay[Piece_Num];			// 減衰率
	static int m_nPower[Piece_Num];				// パワー
	static bool m_bRoute[Piece_Num];			// ルートを表示するか
	static int m_nPieceNum;						// ピース数

};
#endif