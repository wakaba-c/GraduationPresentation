//=============================================================================
//
// カウンタ処理 [counter.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _COUNTER_H_
#define _COUNTER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CNumber;

//=============================================================================
// クラス定義
//=============================================================================
class CCounter : public CScene
{
public:
	CCounter(PRIORITY obj);					// コンストラクタ
	~CCounter();							// デストラクタ
	HRESULT Init(void);						// 初期化処理
	void Uninit(void);						// 開放処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static CCounter *Create(int nLength);			// クリエイト処理

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

	void SetTransform(void);
	void BindTexture(std::string Add);
	void SetNumber(int nValue);	// 数値の設定
	void SetSize(D3DXVECTOR3 size);		// サイズの設定

	void SetDistance(D3DXVECTOR3 &distance);			// 差分の設定
	void SetIntervalNum(D3DXVECTOR3 &interval);			// 数字どうしの距離

	D3DXVECTOR3 GetSize(void) { return m_size; }

private:
	D3DXVECTOR3 m_size;				// サイズ
	D3DXVECTOR3 m_distance;			// オフセット
	D3DXVECTOR3 m_IntervalNum;		// 数字どうしの距離
	unsigned int m_nLength;

	std::vector<CNumber*> m_vNumber;		// 動作タイプ
};
#endif