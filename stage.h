//=============================================================================
//
// ステージ管理処理 [stage.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _STAGE_H_
#define _STAGE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define MAX_ENEMY 4										// 敵の最大数

//=============================================================================
// 前方宣言
//=============================================================================
class CEnemy;

//=============================================================================
// クラス定義
//=============================================================================
class CStage
{
public:
	typedef enum
	{
		FADE_NONE = 0,									// 何もない状態
		FADE_IN,										// フェードイン処理
		FADE_TELOP,										// テロップ処理
		FADE_OUT,										// フェードアウト処理
		FADE_MAX
	} FADE;

	CStage();											// コンストラクタ
	~CStage();											// デストラクタ
	HRESULT Init(void);									// 初期化処理
	void Uninit(void);									// 開放処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	static CStage *Create();							// クリエイト処理
	static void LoadPhase(char *add);					// フェーズごとの敵読み込み

	static void SetStage(void);							// ステージ遷移用フェード開始処理
	static void SetPhase(void);							// フェーズ開始処理

private:
	void ChengeStage(void);
	void MakeVertexFade(LPDIRECT3DDEVICE9 pDevice);

	static LPDIRECT3DTEXTURE9		m_pTexture;			// テクスチャへのポインタ
	static LPDIRECT3DVERTEXBUFFER9	m_pVtxBuff;			// 頂点バッファへのポインタ
	static FADE						m_fade;				// フェード状態
	static bool						m_bEvent;			// イベントの有無
	static D3DXVECTOR3				m_playerPos;		// プレイヤーの立っていた場所
	static D3DXCOLOR				m_col;				// フェード色

	static CEnemy					*m_apEnemy[MAX_ENEMY];			// 敵のポインタ
};
#endif