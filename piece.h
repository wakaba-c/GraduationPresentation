//==================================================================================================================
//
// ピース[piece.h]
// Author:Ryouma Inoue
//
//==================================================================================================================
#ifndef _PIECE_H_
#define _PIECE_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define Piece_Depth 8																// 縦のポリゴン数
#define Piece_Width 15																// 横のブロック数

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;
class CScene2D;

//==================================================================================================================
//
// クラスの定義
//
//==================================================================================================================
class CPiece : public CScene
{
public:
	enum PieceType
	{
		PieceType_None = 0,
		PieceType_Square,
		PieceType_Rectangle,
		PieceType_T_Type,
		PieceType_L_Type,
		PieceType_Square_1,
		PieceType_Rectangle_1,
		PieceType_Rectangle_2,
		PieceType_Speed,
		PieceType_Speed_1,
		PieceType_Diagonal,
		PieceType_Max,
	};

	CPiece(PRIORITY type);																			// コンストラクタ
	~CPiece();																						// デストラクタ
	HRESULT Init(void);																				// 初期化処理
	void Uninit(void);																				// 終了処理
	void Update(void);																				// 更新処理
	void Draw(void);																				// 描画処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};

	bool GetCreate(void) { return m_bCreate; }														// 配置情報取得
	bool GetMove(void) { return m_bMove; }															// 移動情報取得
	bool GetPlaacement(void) { return m_bPlacement; }												// 配置情報取得
	bool GetPuzzle(int nDepth, int nWidth) { return m_bPuzzle[nDepth][nWidth]; }					// パズル情報取得

	void SetPieceType(PieceType type);																// タイプ設定
	void SetPiece(void);																			// ピース設定
	void SetMove(bool bMove) { m_bMove = bMove; }													// 移動
	void SetRelease(bool bRelease) { m_bRelease = bRelease; }										// 消すかどうか
	void SetCol(D3DXCOLOR col) { m_col = col; }														// カラー設定
	static CPiece *Create(void);																	// 生成処理
	static HRESULT Load(void);																		// テクスチャ情報ロード
	static void Unload(void);																		// テクスチャ情報アンロード

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;															// テクスチャへのポインタ
	bool m_bPuzzle[Piece_Depth][Piece_Width];														// 使用しているかどうか
	CScene2D *m_pBlock[Piece_Depth][Piece_Width];													// シーン2Dのポインタ

	D3DXVECTOR3 m_pos;																				// 位置
	D3DXCOLOR m_col;																				// カラー

	int m_nCntMove_X;																				// 移動カウントX
	int m_nCntMove_Y;																				// 移動カウントY
	PieceType m_PieceType;																			// ピースの状態
	bool m_bPlacement;																				// 配置しているかどうか
	bool m_bCreate;																					// 生成するかどうか
	bool m_bMove;																					// 動くかどうか
	bool m_bBox[Piece_Depth][Piece_Width];															// ボックスの状態
	bool m_bPut;																					// 置く
	bool m_bRelease;																				// 消す
};
#endif