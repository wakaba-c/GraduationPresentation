//=============================================================================
//
// プレイヤーのUI管理処理 [playerUi.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _PLAYERUI_H_
#define _PLAYERUI_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "scene2D.h"

//=============================================================================
// 列挙型定義
//=============================================================================
typedef enum
{
	DRAWINGTYPE_HPGAUGE = 0,
	DRAWINGTYPE_DEATHGAUGE,
	DRAWINGTYPE_DAMAGE,
	DRAWINGTYPE_BACK,
	DRAWINGTYPE_HP,
	DRAWINGTYPE_DEATHBLOW,
	DRAWINGTYPE_FRONT,
	DRAWINGTYPE_CHARACTER,
	DRAWINGTYPE_MAX
} DRAWINGTYPE;

typedef enum
{
	GAUGETYPE_HP = 0,
	GAUGETYPE_DEATHBLOW,
	GAUGETYPE_MAX
} GAUGETYPE;

//=============================================================================
// 前方宣言
//=============================================================================
class CGauge2D;

//=============================================================================
// クラス定義
//=============================================================================
class CPlayerUi : public CScene2D
{
public:
	CPlayerUi(PRIORITY obj);			// コンストラクタ
	~CPlayerUi();						// デストラクタ
	HRESULT Init(void);					// 初期化処理
	void Uninit(void);					// 開放処理
	void Update(void);					// 更新処理
	void Draw(void);					// 描画処理

	static CPlayerUi *Create(void);		// クリエイト処理
	static HRESULT Load(void);			// ロード処理

	CGauge2D *GetGauge(GAUGETYPE type) { return	m_apGauge2D[type]; }		// ゲージの取得

private:
	CScene2D *m_apScene2D[DRAWINGTYPE_MAX] = {};// シーンのポインタ
	D3DXVECTOR3 m_move;							// 移動量
	D3DXVECTOR3 m_size;							// フレーム以外の大きさ
	D3DXVECTOR3 m_frameSize;					// フレームの大きさ
	float m_nDamage;							// ダメージ量
	float m_fHP;								// HP
	int		m_nPage;							// ページ数
	float m_fAngle;								// 半径
	float m_fLength;							// 長さ
	CGauge2D *m_apGauge2D[GAUGETYPE_MAX];		// ゲージのポインタ
};
#endif