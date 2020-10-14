//=============================================================================
//
// 目的表示処理 [purpose.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _PURPOSE_H_
#define _PURPOSE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene2D.h"

//=============================================================================
// 列挙体定義
//=============================================================================
typedef enum
{
	MESSAGETYPE_MOVE = 0,
	MESSAGETYPE_HOUSEGUARD,
	MESSAGETYPE_MAX
} MESSAGETYPE;

//=============================================================================
// クラス定義
//=============================================================================
class CPurpose : public CScene2D
{
public:
	CPurpose(PRIORITY obj);								// コンストラクタ
	~CPurpose();										// デストラクタ
	HRESULT Init(void);									// 初期化処理
	void Uninit(void);									// 開放処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	static CPurpose *Create(void);						// クリエイト処理
	static HRESULT Load(void);							// ロード処理
	void ChangePurpose(MESSAGETYPE type);				// 目的設定

	MESSAGETYPE GetPurpose(void) { return m_type; }		// 目的を取得

private:
	typedef enum
	{
		DRAWINGTYPE_SIGNBORAD = 0,
		DRAWINGTYPE_MESSAGE,
		DRAWINGTYPE_MAX
	} DRAWINGTYPE;

	CScene2D *m_apScene2D[DRAWINGTYPE_MAX] = {};		// シーンへのポインタ
	D3DXVECTOR3 m_size;									// 大きさ
	float m_fLength;									//長さ
	MESSAGETYPE m_type;									// メッセージの種類変更
};
#endif