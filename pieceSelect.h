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

//=============================================================================
// マクロ定義
//=============================================================================
#define	MAX_CORE 17		// コアの最大数

//=============================================================================
// 前方宣言
//=============================================================================
class CScene2D;

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

	void ShowInspector(void) {};
	void SetPiece(PIECETYPE type,D3DXVECTOR3 pos,D3DXVECTOR3 size, std::string Add);
	void LoadPiece(void);

private:
	CScene2D *m_pPieceSelect[MAX_CORE];

	PIECETYPE m_type;
};
#endif