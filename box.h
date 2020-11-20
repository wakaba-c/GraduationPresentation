//==================================================================================================================
//
// box[box.h]
// Author:Ryouma Inoue
//
//==================================================================================================================
#ifndef _BOX_H_
#define _BOX_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define Box_Depth 8																	// 縦のポリゴン数
#define Box_Width 15																// 横のブロック数
#define Piece_Num 50																// ピース数

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;
class CScene2D;
class CPiece;

//==================================================================================================================
//
// クラスの定義
//
//==================================================================================================================
class CBox : public CScene
{
public:

	CBox(PRIORITY type);															// コンストラクタ
	~CBox();																		// デストラクタ
	HRESULT Init(void);																// 初期化処理
	void Uninit(void);																// 終了処理
	void Update(void);																// 更新処理
	void Draw(void);																// 描画処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};

	static CBox *Create(void);														// 生成処理
	static HRESULT Load(void);														// テクスチャ情報ロード
	static void Unload(void);														// テクスチャ情報アンロード
	static bool GetPuzzle(int nDepth, int nWidth) { return m_bPuzzle[nDepth][nWidth]; }	// パズル情報取得

	bool GetPlaacement(void){ return m_bCreate; }									// 配置情報取得
	static bool GetPiece(void) {return m_bPiece;}
	float GetSpeed(int nPieceNum) { return m_fSpeed[nPieceNum]; }					// スピード取得
	float GetRate(int nPieceNum) { return m_fRate[nPieceNum]; }						// スピード上昇率取得
	float GetTurning(int nPieceNum) { return m_fTurning[nPieceNum]; }				// 旋回速度
	float GetDecay(int nPieceNum) { return m_fDecay[nPieceNum]; }					// 減衰率
	int GetPower(int nPieceNum) { return m_nPower[nPieceNum]; }						// パワー
	int GetPieceNum(void) { return m_nPieceNum; }									// ピース数

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;											// テクスチャへのポインタ
	CScene2D *m_pBlock[Box_Depth][Box_Width];										// シーン2Dのポインタ
	CPiece *m_pPiece[Piece_Num];													// ピース

	int m_nCntChange;																// 変えるカウント
	int m_nPieceNum;																// ピース数
	int m_nSelect;																	// 選択カウント

	float m_fSpeed[Piece_Num];														// スピード
	float m_fRate[Piece_Num];														// スピード上昇率
	float m_fTurning[Piece_Num];													// 旋回速度
	float m_fDecay[Piece_Num];														// 減衰率
	int m_nPower[Piece_Num];														// パワー


	static bool m_bPuzzle[Box_Depth][Box_Width];									// 使用しているかどうか
	bool m_bPuzzleStorage[Box_Depth][Box_Width];									// 保管
	bool m_bPlacement;																// 配置しているかどうか
	bool m_bCreate;																	// 生成するかどうか
	bool m_bMove;																	// 動くかどうか
	static bool m_bPiece;
	bool m_bRelease;
};
#endif