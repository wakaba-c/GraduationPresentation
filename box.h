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
#define Piece_Num 20																// ピース数

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

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;											// テクスチャへのポインタ
	CScene2D *m_pBlock[Box_Depth][Box_Width];										// シーン2Dのポインタ
	CPiece *m_pPiece[Piece_Num];													// ピース

	int m_nCntMove_X;																// 移動カウントX
	int m_nCntMove_Y;																// 移動カウントY
	int m_nPieceNum;																// ピース数

	static bool m_bPuzzle[Box_Depth][Box_Width];									// 使用しているかどうか
	bool m_bPuzzleStorage[Box_Depth][Box_Width];									// 保管
	bool m_bPlacement;																// 配置しているかどうか
	bool m_bCreate;																	// 生成するかどうか
	bool m_bMove;																	// 動くかどうか
	bool m_bPiece;

};
#endif