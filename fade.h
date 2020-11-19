//=============================================================================
//
// フェード処理 [fade.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _FADE_H_
#define _FADE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "manager.h"

//=============================================================================
// クラス定義
//=============================================================================
class CFade
{
public:
	typedef enum
	{
		FADE_NONE = 0,		// 何もない状態
		FADE_IN,			// フェードイン処理
		FADE_TELOP,			// テロップ処理
		FADE_OUT,			// フェードアウト処理
		FADE_MAX
	} FADE;

	typedef enum
	{
		FADETYPE_NORMAL = 0,		// ノーマルフェード
		FADETYPE_SLIDE,		// スライドフェード
		FADETYPE_MAX
	} FADETYPE;

	CFade();											// コンストラクタ
	~CFade();											// デストラクタ
	void Init(CManager::MODE modeNext);					// 初期化処理
	void Uninit(void);									// 開放処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	static CFade *Create();								// クリエイト処理
	static void SetFade(CManager::MODE modeNext, FADETYPE type);		// セットフェード処理

	static FADE GetFade(void) { return m_fade; }		// フェード状態の取得

private:
	void MakeVertex(LPDIRECT3DDEVICE9 pDevice);			// 頂点情報の作成
	void SetTransform(void);							// 頂点情報の更新

	void NormalFade(void);			// 通常フェード
	void SlideFade(void);			// スライドフェード

	static LPDIRECT3DVERTEXBUFFER9	m_pVtxBuffFade;		// 頂点バッファへのポインタ
	static FADE						m_fade;				// フェード状態
	static FADETYPE					m_fadeType;			// フェードタイプ
	static CManager::MODE			m_modeNext;			// 次の画面（モード）
	static D3DXCOLOR				m_colorFade;		// フェード色

	static D3DXVECTOR3				m_pos;				// 位置
	static D3DXVECTOR3				m_move;				// 移動量
	static bool						m_bReflection;		// 反発フラグ
	static int						m_nCntAction;		// アクションカウンター
};
#endif
