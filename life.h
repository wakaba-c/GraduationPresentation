//=============================================================================
//
// 体力ゲージ処理 [life.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _LIFE_H_
#define _LIFE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CScene3D;

//=============================================================================
// クラス定義
//=============================================================================
class CLife : public CScene
{
public:
	CLife(PRIORITY obj);				// コンストラクタ
	~CLife();							// デストラクタ
	HRESULT Init(void);					// 初期化処理
	void Uninit(void);					// 開放処理
	void Update(void);					// 更新処理
	void Draw(void);					// 描画処理

	static CLife *Create(void);			// クリエイト処理
	static HRESULT Load(void);			// ロード処理

	void SetLifeBar(float fValue);		// ゲージの取得

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
	void MakeVertex(void);

	typedef enum
	{
		DRAWINGTYPE_FRONT = 0,
		DRAWINGTYPE_FRAME,
		DRAWINGTYPE_BACK,
		DRAWINGTYPE_MAX
	} DRAWINGTYPE;

	D3DXVECTOR3 m_size;								// フレーム以外の大きさ
	D3DXVECTOR3 m_frameSize;						// フレームの大きさ
	CScene3D *m_apScene3D[DRAWINGTYPE_MAX];			// タイプ

	LPDIRECT3DVERTEXBUFFER9	m_pVtxBuff;				// 頂点バッファへのポインタ
	LPD3DXMESH m_pMesh;								// メッシュ情報へのポインタ
	DWORD m_nNumMat;								// マテリアル情報の数
	LPD3DXBUFFER m_pBuffMat;						// マテリアル情報へのポインタ
	D3DXMATRIX m_mtxWorld;							// ワールドマトリックス

	float m_fAngle;									// 半径
	float m_fLength;								// 長さ
	D3DXVECTOR3	m_rot;								// 回転
};
#endif