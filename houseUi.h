//=============================================================================
//
// おばあちゃん家のUI管理処理 [houseUi.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _HOUSEUI_H_
#define _HOUSEUI_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "scene2D.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CGauge2D;

//=============================================================================
// クラス定義
//=============================================================================
class CHouseUi : public CScene2D
{
public:
	// 種類
	typedef enum
	{
		DRAWINGTYPE_HPGAUGE = 0,
		DRAWINGTYPE_DAMAGE,
		DRAWINGTYPE_HP,
		DRAWINGTYPE_MAX
	} DRAWINGTYPE;

	CHouseUi(PRIORITY obj);									// コンストラクタ
	~CHouseUi();											// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CHouseUi *Create(void);							// クリエイト処理
	static HRESULT Load(void);								// ロード処理

	CGauge2D *GetGauge(void) { return m_pGauge2D; }			// ゲージの取得

private:
	CScene2D *m_apScene2D[DRAWINGTYPE_MAX] = {};			// 家の耐久値表示パーツ別ポインタ
	D3DXVECTOR3 m_move;										// 移動量
	D3DXVECTOR3 m_size;										// フレーム以外の大きさ
	D3DXVECTOR3 m_frameSize;								// フレームの大きさ
	float m_nDamage;										// ダメージ量
	float m_fHP;											// HP
	int	m_nPage;											// ページ数
	float m_fAngle;											// 半径
	float m_fLength;										// 長さ
	CGauge2D *m_pGauge2D;									// 耐久値表示用ゲージ
};
#endif