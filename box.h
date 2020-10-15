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

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;

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
	void SetPos(D3DXVECTOR3 pos);													// 位置設定処理
	void SetMove(D3DXVECTOR3 move);													// 移動量設定処理

	D3DXVECTOR3 GetPos(void);														// 位置取得処理
	D3DXVECTOR3 GetMove(void);														// 移動量取得処理
	float GetHeight(D3DXVECTOR3 pos);												// 高さ算出処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};

	static CBox *Create(void);														// 生成処理
	static HRESULT Load(void);														// テクスチャ情報ロード
	static void Unload(void);														// テクスチャ情報アンロード
	static CBox *GetBox(void);														// メッシュフィールド情報取得

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;											// テクスチャへのポインタ
	static CBox *m_pBox;															// メッシュフィールドのポインタ
	static CPlayer *m_pPlayer;														// プレイヤーの情報ポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;												// バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;												// インデックスバッファへのポインタ
	VERTEX_2D *m_pVtx;																// 頂点格納
	D3DXMATRIX m_mtxWorld;															// ワールドマトリックス
	D3DXVECTOR3 m_pos;																// 位置
	D3DXVECTOR3 m_rot;																// 回転
	D3DXVECTOR3 m_move;																// 移動
	int m_nNumVertex;																// 総頂点数
	int m_nNumIndex;																// 総インデックス数
	int m_nNumPolygon;																// 総ポリゴン数
	int m_nNumIndexPolygon;															// インデックスのときのポリゴン数
	D3DXVECTOR3 m_aVecA[Box_Depth * Box_Width];										// 法線ベクトルを面の数分一時的に格納
	D3DXVECTOR3 m_aVecB[Box_Depth * Box_Width];										// 法線ベクトルを面の数分一時的に格納
	D3DXVECTOR3 m_vectorA;															// Aベクトル
	D3DXVECTOR3 m_vectorB;															// Bベクトル
	D3DXVECTOR3 m_vectorC;															// Cベクトル
	int nNumber;																	// 三角形の配列の番号
	int StartBox;																	// 始まる箱
	int EndBox;																		// 引かれる箱
	float fDivide;																	// sinの中身を割る変数

};
#endif