//=============================================================================
//
// 敵の数表示処理 [enemyUi.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _ENEMYUI_H_
#define _ENEMYUI_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_ENEMY_NUMBER 2				// 桁数

//=============================================================================
// 前方宣言
//=============================================================================
class CNumber;

//=============================================================================
// クラス定義
//=============================================================================
class CEnemyUi : public CScene2D
{
public:
	CEnemyUi(PRIORITY obj);										// コンストラクタ
	~CEnemyUi();												// デストラクタ
	HRESULT Init(void);											// 初期化処理
	void Uninit(void);											// 開放処理
	void Update(void);											// 更新処理
	void Draw(void);											// 描画処理

	static CEnemyUi *Create(void);								// クリエイト処理
	static HRESULT Load(void);									// ロード処理

	void SetNumberPos(D3DXVECTOR3 pos, float fInterval);		// 数の位置を設定
	void SetNumderSize(D3DXVECTOR3 size);						// 数のサイズを設定
	void SetNumber(int nValue);									// 数を設定
	void AddNumber(int nValue);									// 数を加算

	int GetNumber(void) { return m_nNumber; }					// 数の取得

	void ShowInspector(void) {};

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;						// テクスチャ情報のポインタ
	CNumber *m_apNumber[MAX_ENEMY_NUMBER];						// 数字のポインタ
	int m_nNumber;												// 数
};
#endif