//=============================================================================
//
// ゲージ処理 [gauge2D.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _GAUGE2D_H_
#define _GAUGE2D_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// クラス定義
//=============================================================================
class CGauge2D : public CScene2D
{
public:
	CGauge2D(PRIORITY obj);		// コンストラクタ
	~CGauge2D();				// デストラクタ
	HRESULT Init(void);			// 初期化処理
	void Uninit(void);			// 開放処理
	void Update(void);			// 更新処理
	void Draw(void);			// 描画処理

	static CGauge2D *Create(void);		// クリエイト処理
	static HRESULT Load(void);										// ロード処理

	void SetPosition(D3DXVECTOR3 pos);								// 位置の設定
	void SetLifeBar(bool bDie, float nValue);						// ゲージの表示割合設定
	void SetGaugeSize(D3DXVECTOR3 size, D3DXVECTOR3 frameSize);		// ゲージの大きさ設定

	void ShowInspector(void) {};

private:
	// ゲージの種類
	typedef enum
	{
		DRAWINGTYPE_GLAS = 0,
		DRAWINGTYPE_DAMAGE,
		DRAWINGTYPE_BACK,
		DRAWINGTYPE_FRONT,
		DRAWINGTYPE_MAX
	} DRAWINGTYPE;

	CScene2D *m_apScene2D[DRAWINGTYPE_MAX] = {};		// ゲージパーツのポインタ
	D3DXVECTOR3 m_pos;									// ポジション
	D3DXVECTOR3 m_move;									// 移動量
	D3DXVECTOR3 m_size;									// フレーム以外の大きさ
	D3DXVECTOR3 m_frameSize;							// フレームの大きさ
	float m_nDamage;									// ダメージ量
	float m_fHP;										// HP

	float m_fAngle;										// 半径
	float m_fLength;									// 長さ
};
#endif