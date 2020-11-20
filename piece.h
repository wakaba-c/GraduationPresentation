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

	//================================
	// ステータスのタイプ
	//================================
	enum StatusType
	{
		StatusType_None = 0,						 // 初期
		StatusType_MiniMap,							 // ミニマップ表示＆加速度上昇
		StatusType_Route,							 // ルート案内表示＆旋回速度上昇
		StatusType_Ranking,							 // 順位表示＆最高速度上昇
		StatusType_All_MediumUp,					 // 最高速度＆スピード上昇率＆旋回速度＆パワーを中アップ
		StatusType_MaxSpeed_GreatUp,				 // 最高速度大アップ
		StatusType_MaxSpeed_MediumUp,				 // 最高速度中アップ
		StatusType_MaxSpeed_SmallUp,				 // 最高速度小アップ
		StatusType_Rate_GreatUp,					 // スピード上昇率大アップ
		StatusType_Rate_MediumUp,					 // スピード上昇率中アップ
		StatusType_Rate_SmallUp,					 // スピード上昇率小アップ
		StatusType_Turning_GreatUp,					 // 旋回速度大アップ
		StatusType_Turning_MediumUp,				 // 旋回速度中アップ
		StatusType_Turning_SmallUp,					 // 旋回速度小アップ
		StatusType_Power_MediumUp,					 // パワー中アップ
		StatusType_Power_SmallUp,					 // パワー小アップ
		StatusType_Decay_Down,						 // スピード減衰率低下
		StatusType_MaxSpeed_LimitBreak,				 // 最高速度の限界突破
		StatusType_Max,								 // 最大数
	};

	//================================
	// ピースのタイプ
	//================================
	enum PieceType
	{
		PieceType_None = 0,				
		PieceType_Square,				
		PieceType_Rectangle,			
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

	bool GetMove(void) { return m_bMove; }															// 移動情報取得
	bool GetPlaacement(void) { return m_bPlacement; }												// 配置情報取得
	bool GetPuzzle(int nDepth, int nWidth) { return m_bPuzzle[nDepth][nWidth]; }					// パズル情報取得
	bool GetMap(void){ return m_bMap; }																// 
	float GetSpeed(void) { return m_fSpeed; }														// スピード取得
	float GetRate(void) { return m_fRate; }															// スピード上昇率取得
	float GetTurning(void) { return m_fTurning; }													// 旋回速度取得
	float GetDecay(void) { return m_fDecay; }														// 減衰率取得
	float GetPower(void) { return m_nPower; }														// パワー取得

	void SetPieceType(PieceType type);																// タイプ設定
	void SetStatus(void);																			// ステータス設定
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
	float m_fSpeed;																					// 速さ
	float m_fRate;																					// スピード上昇率
	float m_fTurning;																				// 旋回速度
	float m_fDecay;																					// 減衰率
	int m_nPower;																					// パワー
	PieceType m_PieceType;																			// ピースタイプ
	StatusType m_StatusType;																		// ステータスタイプ
	bool m_bPlacement;																				// 配置しているかどうか
	bool m_bMove;																					// 動くかどうか
	bool m_bBox[Piece_Depth][Piece_Width];															// ボックスの状態
	bool m_bPut;																					// 置く
	bool m_bRelease;																				// 消す
	bool m_bMap;																					// マップを表示するか
	bool m_bRoute;																					// ルートを表示するか
	bool m_bRanking;																				// ランキングを表示するか
};
#endif