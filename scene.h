//=============================================================================
//
// scene処理 [scene.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SCENE_H_
#define _SCENE_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCube;
class CMeshSphere;
class CCollider;
class CWrite;

//=============================================================================
// クラス定義
//=============================================================================
class CScene
{
public:
	typedef enum
	{
		PRIORITY_NONE = -1,
		PRIORITY_BG,
		PRIORITY_FLOOR,
		PRIORITY_WALL,
		PRIORITY_SKY,
		PRIORITY_MODEL,
		PRIORITY_PLAYER,
		PRIORITY_ENEMY,
		PRIORITY_CHARACTER,
		PRIORITY_LIFE,
		PRIORITY_TREE,
		PRIORITY_ORBIT,
		PRIORITY_EFFECT,
		PRIORITY_MESH,
		PRIORITY_UI,
		PRIORITY_MAX
	} PRIORITY;

	CScene(PRIORITY Type);						// コンストラクタ
	virtual ~CScene();							// デストラクタ
	virtual HRESULT Init(void) = 0;				// 初期化処理
	virtual void Uninit(void) = 0;				// 開放処理
	virtual void Update(void) = 0;				// 更新処理
	virtual void Draw(void) = 0;				// 描画処理
	static void ReleaseAll(void);				// 全てのオブジェクトを破開放
	static void UpdateAll(void);				// 全てのオブジェクトを更新
	static void DrawAll(void);					// 全てのオブジェクトを描画

	void Release(void);							// 削除予約処理
	void SetObjType(PRIORITY obj);				// オブジェクトタイプの設定
	void SetActive(bool bValue);				// 更新描画対象設定
	static void SetShadow(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX mtxWorld, LPD3DXBUFFER pBuffMat, DWORD nNumMat, LPD3DXMESH pMesh, D3DXVECTOR3 pos);		// 影の設定												// 影の生成
	static CScene *NowFloor(D3DXVECTOR3 pos);	// 現在立っている場所の床を取得
	static void ResetFloor(void);				// 床の高さと色をリセットする処理
	void SetPosition(D3DXVECTOR3 pos);			// 位置の設定
	void SetPosOld(D3DXVECTOR3 pos);			// 前の位置を設定

	static CScene *GetScene(PRIORITY obj);							// シーンを取得
	static CScene *GetSceneNext(CScene *pScene, PRIORITY type);		// 次のシーンを取得
	D3DXVECTOR3 GetPosition(void) { return m_pos; }					// 位置の取得
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; }				// 前の位置を取得
	bool GetActive(void) { return m_bActive; }						// 更新描画対象取得
	PRIORITY GetObjType(void) { return m_Obj; }						// オブジェクトタイプの取得

	virtual void OnTriggerEnter(CCollider *col) = 0;
	virtual void OnCollisionEnter(CCollider *col) = 0;
	virtual void ShowInspector(void);

	static void SaveEnemy(void);					// 敵情報の書き込み
	static void SaveRand(void);						// 床情報の書き込み
	static void SaveModel(void);					// 木配置情報の書き込み
	static void SaveWall(void);						// 壁情報の書き込み
	static void SavePoint(CWrite *pWrite, D3DXVECTOR3 &pos);			// ポイントモデルの書き込み
	void SaveCollider(void);						// 当たり判定の書き込み

	static char *GetSceneTypeName(const int &nIndex);

private:
	void Delete(void);								// 開放処理
	void WriteForSphereCollider(CWrite *pWrite, CCollider *pCollider);				// スフィアコライダー独自の値を書き込む
	void WriteForBoxCollider(CWrite *pWrite, CCollider *pCollider);				// スフィアコライダー独自の値を書き込む
	static void ShowHierarchy(void);				// ヒエラルキーの表示

	static int m_nNumAll;
	static CScene *m_apCur[PRIORITY_MAX];			// 現在(最後尾)オブジェクトへのポインタ
	static CScene *m_apTop[PRIORITY_MAX];			// 先頭オブジェクトへのポインタ
	CScene *m_pPrev[PRIORITY_MAX];					// 前のオブジェクトポインタ
	CScene *m_pNext[PRIORITY_MAX];					// 次のオブジェクトポインタ
	PRIORITY m_Obj;									// オブジェクトの種類
	bool	m_bActive;								// 描画するかどうか
	bool	m_bDie;									// Releaseするかどうか
	std::vector<CCollider*> m_apCollider;			// 当たり判定
	std::string m_Label;							// ラベル

	D3DXVECTOR3 m_pos;								// 位置
	D3DXVECTOR3 m_posOld;							// 前回の位置
};
#endif