//=============================================================================
//
// メッシュスフィア処理 [meshSphere.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MESHSPHERE_H_
#define _MESHSPHERE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "collider.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define DIVISION 20											//分割数

//=============================================================================
// クラス定義
//=============================================================================
class CMeshSphere : public CScene
{
public:
	CMeshSphere(PRIORITY obj);								// コンストラクタ
	~CMeshSphere();											// デストラクタ
	HRESULT Init(void);										// 初期化処理
	void Uninit(void);										// 開放処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CMeshSphere *Create(float fRadius);				// クリエイト処理
	static HRESULT Load(void);								// 素材の取得

	void SetRotation(D3DXVECTOR3 rot);						// 回転を設定
	D3DXVECTOR3 GetRotation(void) { return m_rot; }			// 回転の取得

	void SetVecAxis(D3DXVECTOR3 vecAxis);					// 回転軸を設定
	D3DXVECTOR3 GetVecAxis(void) { return m_vecAxis; }		// 回転軸の取得

	void SetValueRot(float fValue);							// 回転角を設定
	float GetValueRot(void) { return m_fValueRot; }			// 回転角の取得

	void SetColor(D3DXCOLOR col);							// 色を設定
	D3DXCOLOR GetColor(void) { return m_col; }				// 色を返す

	void SetRadius(float fValue);							// 半径を設定
	float GetRadius(void) { return m_fRadius; }				// 半径を返す

	void MakeVertex(void);									// 頂点情報を作成

	void OnTriggerEnter(CCollider *col) {};
	void OnCollisionEnter(CCollider *col) {};
	void ShowInspector(void) {};

private:
#ifdef _DEBUG
	void Debug(void);										// デバッグ関数
#endif

	void MakeIndex(void);									// 頂点情報作成関数

	static LPDIRECT3DTEXTURE9	m_pTexture;					// テクスチャへのポインタ

	D3DXVECTOR3 apNor[DIVISION * DIVISION * 2];				// 1面ごとの法線ベクトルの配列

	D3DXVECTOR3				m_rot;							// ポリゴン の向き(回転)
	float					m_fRadius;						// ポリゴン の半径
	D3DXCOLOR				m_col;							// ポリゴン の色
	D3DXMATRIX				m_mtxWorld;						// ワールドマトリックス
	D3DXQUATERNION			m_quat;							// クォータニオン
	D3DXVECTOR3				m_vecAxis;						// 回転軸
	float					m_fValueRot;					// 回転角(回転量)

	int						m_nNumVertexSphere;				// 総頂点数
	int						m_nNumIndexSphere;				// インデックス数
 	int						m_nNumPolygonSphere;			// 総ポリゴン数

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuff;					// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9		m_pIdxBuff;					// インデックスバッファへのポインタ

	int m_nNumVertex;										// 総頂点数
	int m_nNumIndex;										// インデックス数
	int m_nNumPolygon;										// 総ポリゴン数
};
#endif