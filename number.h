//=============================================================================
//
// 数字処理 [number.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _NUMBER_H_
#define _NUMBER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include  "scene2D.h"

//=============================================================================
// クラス定義
//=============================================================================
class CNumber : public CScene2D
{
public:
	CNumber(PRIORITY obj);					// コンストラクタ
	~CNumber();								// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 開放処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static CNumber *Create(void);			// クリエイト処理
	static HRESULT Load(void);				// ロード処理

	void SetNumber(int nNumber);			// 数字の設定
	void MakeVertex(void);					// 頂点情報の作成

	void ShowInspector(void) {};

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// 頂点バッファへのポインタ
	int m_nNumOld;							// 前までの数字
};
#endif