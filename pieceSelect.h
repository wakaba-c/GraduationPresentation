//=============================================================================
//
// ピースセレクト処理 [pieceSelect.h]
// Author : Ryouma Inoue
//
//=============================================================================
#ifndef _BG_H_
#define _BG_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"
#include "box.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	MAX_CORE 17		// コアの最大数

//=============================================================================
// 前方宣言
//=============================================================================
class CScene2D;
class CPiece;

//=============================================================================
// クラス定義
//=============================================================================
class CPieceSelect : public CScene
{
public:
	//コアタイプ
	typedef enum
	{
		PIECETYPE_NONE = -1,		// なし
		PIECETYPE_CORE_00,			// コア
		PIECETYPE_CORE_01,			// コア
		PIECETYPE_CORE_02,			// コア
		PIECETYPE_CORE_03,			// コア
		PIECETYPE_CORE_04,			// コア
		PIECETYPE_CORE_05,			// コア
		PIECETYPE_CORE_06,			// コア
		PIECETYPE_CORE_07,			// コア
		PIECETYPE_CORE_08,			// コア
		PIECETYPE_CORE_09,			// コア
		PIECETYPE_CORE_10,			// コア
		PIECETYPE_CORE_11,			// コア
		PIECETYPE_CORE_12,			// コア
		PIECETYPE_CORE_13,			// コア
		PIECETYPE_CORE_14,			// コア
		PIECETYPE_CORE_15,			// コア
		PIECETYPE_CORE_16,			// コア
		PIECETYPE_CORE_17,			// コア
		PIECETYPE_MAX				// 最大数
	} PIECETYPE;					// タイプ

	CPieceSelect(PRIORITY obj);		// コンストラクタ
	~CPieceSelect();				// デストラクタ
	HRESULT Init(void);				// 初期化処理
	void Uninit(void);				// 開放処理
	void Update(void);				// 更新処理
	void Draw(void);				// 描画処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};

	static CPieceSelect *Create(void);	// クリエイト処理
	static HRESULT Load(void);			// ロード処理
	static bool GetPuzzle(int nDepth, int nWidth) { return m_bPuzzle[nDepth][nWidth]; }	// パズル情報取得

	int GetPieceNum(void) { return m_nPieceNum; }									// ピース数
	float GetSpeed(int nPieceNum) { return m_fSpeed[nPieceNum]; }					// スピード取得
	float GetRate(int nPieceNum) { return m_fRate[nPieceNum]; }						// スピード上昇率取得
	float GetTurning(int nPieceNum) { return m_fTurning[nPieceNum]; }				// 旋回速度
	float GetDecay(int nPieceNum) { return m_fDecay[nPieceNum]; }					// 減衰率
	int GetPower(int nPieceNum) { return m_nPower[nPieceNum]; }						// パワー
	bool GetRoute(int nPieceNum) { return m_bRoute[nPieceNum]; }					// ルート
	bool GetRank(int nPieceNum) { return m_bRank[nPieceNum]; }						// ランキング

	void ShowInspector(void) {};
	void SetPiece(PIECETYPE type,D3DXVECTOR3 pos,D3DXVECTOR3 size, std::string Add);
	void LoadPiece(void);

private:
	CScene2D *m_pPieceSelect[MAX_CORE];				// Scene2Dのポインタ
	CScene2D *m_pPieceExplanation;					// Scene2Dのポインタ
	CPiece *m_pPiece[Piece_Num];					// ピース

	PIECETYPE m_type;								// ピースのタイプ

	bool m_bSelect[MAX_CORE];						// 選ばれているかどうか
	bool m_bPiece;									// 置いてるかどうか

	int m_nPieceNum;								// ピース数
	int m_nSelect;									// 選択カウント
	int m_nSelectCnt;								// セレクトカウント
	float m_fSpeed[Piece_Num];						// スピード
	float m_fRate[Piece_Num];						// スピード上昇率
	float m_fTurning[Piece_Num];					// 旋回速度
	float m_fDecay[Piece_Num];						// 減衰率
	int m_nPower[Piece_Num];						// パワー
	bool m_bRoute[Piece_Num];						// ルートを表示するか
	bool m_bRank[Piece_Num];						// ランキングを表示するか
	bool m_bPlacement;																// 配置しているかどうか

	static bool m_bPuzzle[Box_Depth][Box_Width];	// 使用しているかどうか
	bool m_bPuzzleStorage[Box_Depth][Box_Width];	// 保管
	bool m_bRelease;

};
#endif