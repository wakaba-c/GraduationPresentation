//=============================================================================
//
// 当たり判定マネージャ処理 [collider.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _COLLIDER_H_
#define _COLLIDER_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"
#include "scene.h"

//=============================================================================
// 構造体定義
//=============================================================================
typedef enum
{
	COLLISIONTYPE_NONE = -1,
	COLLISIONTYPE_SPHERE,
	COLLISIONTYPE_BOX,
	COLLISIONTYPE_MAX
} COLLISIONTYPE;

//=============================================================================
// 前方宣言
//=============================================================================
class CMeshCube;
class CMeshSphere;

//=============================================================================
// クラス定義
//=============================================================================
class CCollider
{
public:
	CCollider(COLLISIONTYPE Type);		// コンストラクタ
	virtual ~CCollider();				// デストラクタ
	virtual HRESULT Init(void) = 0;		// 初期化処理
	virtual void Uninit(void) = 0;		// 開放処理
	virtual void Update(void) = 0;		// 更新処理
	virtual void Draw(void) = 0;		// 描画処理

	static void ReleaseAll(void);		// 全ての当たり判定を破棄
	static void UpdateAll(void);		// 全ての当たり判定を更新
	static void DrawAll(void);			// 全ての当たり判定を描画

	void SetPosition(D3DXVECTOR3 pos);	// 位置の設定
	void SetPosOld(D3DXVECTOR3 pos);	// 前回位置の設定
	void SetActive(bool bValue);		// 更新描画対象設定
	void Release(void);					// 削除予約
	void SetTag(std::string sTag);		// タグの設定
	void SetScene(CScene *pScene);		// 当たり判定の持ち主を設定
	void SetTrigger(bool bValue);		// 当たり判定タイプの設定
	void SetUse(bool bValue);			// 当たり判定の使用設定
	void SetMoving(bool bValue);		// 位置修正の可否設定
	void SetOffset(D3DXVECTOR3 pos);	// オフセットの設定
	void ColliderCheck(void);			// 当たり判定チェック

	static bool SphereHitCollider(CCollider *pFirstTarget, CCollider *pSecondTarget);		// 球体の当たり判定(hit)
	static bool SphereInCollider(CCollider *pFirstTarget, CCollider *pSecondTarget);		// 球体の当たり判定(in)
	static bool BoxHitCollider(CCollider *pFirstTarget, CCollider *pSecondTarget);			// 立方体の当たり判定(Hit)
	static bool BoxInCollider(CCollider *pFirstTarget, CCollider *pSecondTarget);			// 立方体の当たり判定(in)
	static bool CollisionHitSphereAndBox(CCollider *pFirstTarget, CCollider *pSecondTarget);// 立方体と円の当たり判定(hit)
	static bool CollisionInSphereAndBox(CCollider *pFirstTarget, CCollider *pSecondTarget); // 立方体と円の当たり判定(in)
	static bool CollisionHitSphereAndPoint(D3DXVECTOR3 pos, CCollider *pSecondTarget);		// 円と点の当たり判定(in)

	COLLISIONTYPE GetColliderType(void) { return m_Obj; }	// 色の取得
	D3DXVECTOR3 GetPosition(void) { return m_pPos; }	// 位置の取得
	D3DXVECTOR3 GetPosOld(void) { return m_posOld; }	// 前の位置を取得
	bool GetActive(void) { return m_bActive; }			// 更新描画対象取得
	std::string GetTag(void) { return m_sTag; }			// タグの取得
	CScene *GetScene(void) { return m_pScene; }			// 持ち主の取得
	bool GetTrigger(void){ return m_bTrigger; }			// タイプを取得
	bool GetUse(void) { return m_bUse; }				// 当たり判定の使用状態取得
	bool GetMoving(void) { return m_bMoving; }			// 位置修正の可否取得
	D3DXVECTOR3 GetOffset(void) { return m_offset; }	// 位置を取得

private:
	void Delete(void);									// 削除処理

	static CCollider *m_apCur[COLLISIONTYPE_MAX];		// 最後尾情報
	static CCollider *m_apTop[COLLISIONTYPE_MAX];		// 先頭情報
	CCollider *m_pPrev[COLLISIONTYPE_MAX];				// 前の情報
	CCollider *m_pNext[COLLISIONTYPE_MAX];				// 次の情報

	static int m_nNumAll;					// 当たり判定の総数
	COLLISIONTYPE m_colType;				// 当たり判定の種類
	bool m_bTrigger;						// 検知だけかどうか
	bool m_bUse;							// 当たり判定の使用
	bool m_bMoving;							// 当たり判定による位置修正の可否
	COLLISIONTYPE m_Obj;					//オブジェクトの種類
	bool	m_bActive;						//描画するかどうか
	bool	m_bDie;							//Releaseするかどうか
	CScene *m_pScene;						// 対象のシーン
	std::string	m_sTag;						// タグ
	D3DXVECTOR3	m_offset;					// オフセット値
	D3DXVECTOR3 m_pPos;						// 位置のポインタ
	D3DXVECTOR3 m_posOld;					// 前回の位置
	D3DXVECTOR3 m_pMove;					// 移動量のポインタ
};
#endif