//=============================================================================
//
// モデル処理 [model.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _MODEL_H_
#define _MODEL_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CSceneX;

//=============================================================================
// クラス定義
//=============================================================================
class CModel
{
public:
	CModel();													// コンストラクタ
	~CModel();													// デストラクタ
	HRESULT Init(void);											// 初期化処理
	void Uninit(void);											// 開放処理
	void Update(void);											// 更新処理
	void Draw(D3DXMATRIX *mtxWorld);							// 描画処理

	static CModel *Create(char sAdd[64]);						// クリエイト処理
	HRESULT Load(char sAdd[64]);								// ロード処理

	void SetParent(CModel *pModel);								// 親の設定

	void SetIndex(int nValue);									// インデックスの設定
	void SetPosition(D3DXVECTOR3 pos);							// 位置の設定
	void SetPosPreset(D3DXVECTOR3 pos);							// 位置の設定
	void SetRotation(D3DXVECTOR3 rot);							// 回転量の設定
	void SetRotPreset(D3DXVECTOR3 rot);							// 回転量の設定
	void SetActive(bool bValue);								// 更新描画対象設定

	bool GetActive(void) { return m_bUse; }						// 更新描画対象取得
	int GetIndex(void) { return m_nIndex; }						// インデックスの取得
	CSceneX *GetScene(void) { return m_pSceneX; }				// シーンの取得
	D3DXVECTOR3 GetPosition(void) { return m_pos; }				// 位置を取得する
	D3DXVECTOR3 GetPosPreset(void) { return m_originPos; }		// プリセット位置情報を取得する
	D3DXVECTOR3 GetRotation(void) { return m_rot; }				// 回転量の取得
	D3DXVECTOR3 GetRotPreset(void) { return m_originRot; }		// 回転量のプリセット取得
	LPD3DXMESH GetMesh(void) { return m_pMesh; }				// メッシュ情報の取得
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }			// マトリックスの取得

private:
	LPDIRECT3DTEXTURE9	*m_pTexture;							// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;							// 頂点バッファへのポインタ
	LPD3DXMESH		m_pMesh;									// メッシュ情報へのポインタ
	DWORD			m_nNumMat;									// マテリアル情報の数
	LPD3DXBUFFER	m_pBuffMat;									// マテリアル情報へのポインタ
	CSceneX			*m_pSceneX;									// モデルシーンポインタ

	int m_nIndex;												// 自分のID
	CModel *m_pParent;											// 親のID

	D3DXVECTOR3 m_pos;											// ポジション
	D3DXVECTOR3 m_posOld;										// 前のポジション
	D3DXVECTOR3 m_originPos;									// ポジションのプリセット
	D3DXVECTOR3 m_size;											// 大きさ
	D3DXVECTOR3 m_rot;											// 回転量
	D3DXVECTOR3 m_originRot;									// 回転量のプリセット
	D3DXCOLOR	m_color;										// 色
	D3DXMATRIX	m_mtxWorld;										// ワールドマトリックス
	bool		m_bUse;											// 描画するかどうか
};
#endif