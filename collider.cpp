//==============================================================================
//
// 当たり判定マネージャ処理 [collider.cpp]
// Author : masayasu wakita
//
//==============================================================================
#include "collider.h"
#include "colliderSphere.h"
#include "colliderBox.h"
#include "debug.h"
#include "object.h"
#include "manager.h"
#include "renderer.h"

//==============================================================================
// 静的メンバ変数
//==============================================================================
CCollider *CCollider::m_apTop[COLLISIONTYPE_MAX] = {};			// 先頭情報の初期化
CCollider *CCollider::m_apCur[COLLISIONTYPE_MAX] = {};			// 最後尾情報の初期化
int CCollider::m_nNumAll = 0;									// 総数の初期化

//==============================================================================
// コンストラクタ
//==============================================================================
CCollider::CCollider(COLLISIONTYPE Type)
{
	m_offset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// オフセット値

	m_pPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 位置のポインタ
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 前回位置の初期化
	m_pMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 移動量のポインタ

	if (Type != COLLISIONTYPE_NONE)
	{
		if (m_apTop[Type] == NULL)
		{
			m_apTop[Type] = this;						// 自分 をトップに設定
		}

		if (m_apCur[Type] != NULL)
		{
			m_apCur[Type]->m_pNext[Type] = this;		// 最後尾 の次 を自分に
		}

		m_pPrev[Type] = m_apCur[Type];					// 最後尾を入れる
		m_apCur[Type] = this;							// 自身を最後尾にする
		m_pNext[Type] = NULL;							// 自分の次をNULL
		m_Obj = Type;									// オブジェクトタイプを定義
		m_nNumAll++;									// シーン数をプラスする
		m_bDie = false;

		m_mtxWorldParent = NULL;
		m_pScene = NULL;								// シーンの初期化
		m_sTag = "none";								// タグの初期化
		m_bUse = true;									// 使用中にする
		m_bMoving = true;								// 位置修正可能にする
		m_bTrigger = true;								// トリガーの初期化
	}
}

//==============================================================================
// デストラクタ
//==============================================================================
CCollider::~CCollider()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CCollider::Init(void)
{
	return S_OK;																									// 成 を返す
}

//==============================================================================
// 開放処理
//==============================================================================
void CCollider::Uninit(void)
{

}

//==============================================================================
// 更新処理
//==============================================================================
void CCollider::Update(void)
{
	if (m_pScene != NULL)
	{
		m_pPos = m_pScene->GetPosition();
	}
}

//==============================================================================
// 描画処理
//==============================================================================
void CCollider::Draw(void)
{

}

//==============================================================================
//全ての当たり判定を破棄
//==============================================================================
void CCollider::ReleaseAll(void)
{
	CCollider *pSceneNext = NULL;																			//次回削除対象
	CCollider *pSceneNow = NULL;

	for (int nCount = 0; nCount < COLLISIONTYPE_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//死亡フラグを確認
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];																//次回フラグ確認対象を控える

			pSceneNow->Delete();																					//削除

			pSceneNow = pSceneNext;																					//次回フラグ確認対象を格納
		}
	}
}

//==============================================================================
//全ての当たり判定を更新
//==============================================================================
void CCollider::UpdateAll(void)
{
	CCollider *pSceneNext = NULL;																			// 次回アップデート対象
	CCollider *pSceneNow = NULL;																			// 今回アップデート対象

	CCollider *pSceneNext2 = NULL;																			// 次回アップデート対象
	CCollider *pSceneNow2 = NULL;																			// 今回アップデート対象

	D3DXVECTOR3 first;
	D3DXVECTOR3 second;

	// 開放処理
	for (int nCount = 0; nCount < COLLISIONTYPE_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//死亡フラグを確認
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];		//次回フラグ確認対象を控える

			if (pSceneNow->m_bDie)
			{
				pSceneNow->Delete();						//削除
				pSceneNow->m_pScene = NULL;
				pSceneNow = NULL;
			}

			pSceneNow = pSceneNext;							//次回フラグ確認対象を格納
		}
	}

	// ========================= Sphere of Sphere =========================== //
	pSceneNow = m_apTop[COLLISIONTYPE_SPHERE];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[COLLISIONTYPE_SPHERE];			//次回アップデート対象を控える

		pSceneNow2 = m_apTop[COLLISIONTYPE_SPHERE];

		if (pSceneNow->GetUse())
		{// 当たり判定の計算対象かどうか

			pSceneNow->Update();

			//次がなくなるまで繰り返す
			while (pSceneNow2 != NULL)
			{
				pSceneNext2 = pSceneNow2->m_pNext[COLLISIONTYPE_SPHERE];		// 次回アップデート対象を控える

				// 当たり判定の計算
				if (pSceneNow2->GetUse())
				{// 当たり判定の計算対象かどうか
					if (pSceneNow != pSceneNow2)
					{// 対象が同一じゃないかどうか
						first = pSceneNow->GetPosition();
						second = pSceneNow2->GetPosition();

						first.y = 0.0f;
						second.y = 0.0f;
						if (pSceneNow->GetTrigger() || pSceneNow2->GetTrigger())
						{
							if (SphereHitCollider(pSceneNow, pSceneNow2))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnTriggerEnter(pSceneNow2);
								}
							}
						}
						else
						{
							if (SphereInCollider(pSceneNow, pSceneNow2))
							{
								if (pSceneNow != NULL)
								{
									if (pSceneNow->m_pScene != NULL)
									{
										pSceneNow->m_pScene->OnCollisionEnter(pSceneNow2);
									}
								}
							}
						}
					}
				}

				pSceneNow2 = pSceneNext2;			//次回アップデート対象を格納
			}
		}

		pSceneNow = pSceneNext;				//次回アップデート対象を格納
	}

	// ========================= box of box =========================== //

	pSceneNext = NULL;														// 次回アップデート対象
	pSceneNow = NULL;														// 今回アップデート対象

	pSceneNext2 = NULL;														// 次回アップデート対象
	pSceneNow2 = NULL;														// 今回アップデート対象

	pSceneNow = m_apTop[COLLISIONTYPE_BOX];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[COLLISIONTYPE_BOX];					//次回アップデート対象を控える

		pSceneNow2 = m_apTop[COLLISIONTYPE_BOX];

		if (pSceneNow->GetUse())
		{// 当たり判定の計算対象かどうか

			pSceneNow->Update();

			//次がなくなるまで繰り返す
			while (pSceneNow2 != NULL)
			{
				pSceneNext2 = pSceneNow2->m_pNext[COLLISIONTYPE_BOX];		// 次回アップデート対象を控える

				// 当たり判定の計算
				if (pSceneNow2->GetUse())
				{// 当たり判定の計算対象かどうか
					if (pSceneNow != pSceneNow2)
					{// 対象が同一じゃないかどうか
						if (pSceneNow->GetTrigger() || pSceneNow2->GetTrigger())
						{
							if (BoxInCollider(pSceneNow, pSceneNow2))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnTriggerEnter(pSceneNow2);
								}
							}
						}
						else
						{
							if (BoxHitCollider(pSceneNow, pSceneNow2))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnCollisionEnter(pSceneNow2);
								}
							}
						}
					}
				}

				pSceneNow2 = pSceneNext2;			//次回アップデート対象を格納
			}
		}

		pSceneNow = pSceneNext;				//次回アップデート対象を格納
	}

	// ========================= box of sphere =========================== //

	pSceneNext = NULL;													// 次回アップデート対象
	pSceneNow = NULL;													// 今回アップデート対象

	pSceneNext2 = NULL;													// 次回アップデート対象
	pSceneNow2 = NULL;													// 今回アップデート対象

	pSceneNow = m_apTop[COLLISIONTYPE_BOX];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[COLLISIONTYPE_BOX];				//次回アップデート対象を控える

		pSceneNow2 = m_apTop[COLLISIONTYPE_SPHERE];

		if (pSceneNow->GetUse())
		{// 当たり判定の計算対象かどうか

			pSceneNow->Update();

			//次がなくなるまで繰り返す
			while (pSceneNow2 != NULL)
			{
				pSceneNext2 = pSceneNow2->m_pNext[COLLISIONTYPE_SPHERE];		// 次回アップデート対象を控える

				// 当たり判定の計算
				if (pSceneNow2->GetUse())
				{// 当たり判定の計算対象かどうか
					if (pSceneNow != pSceneNow2)
					{// 対象が同一じゃないかどうか

						first = pSceneNow->GetPosition();
						second = pSceneNow2->GetPosition();

						first.y = 0.0f;
						second.y = 0.0f;

						if (pSceneNow->GetTrigger() || pSceneNow2->GetTrigger())
						{
							if (CollisionHitSphereAndBox(pSceneNow, pSceneNow2))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnTriggerEnter(pSceneNow2);
								}
							}
						}
						else
						{
							if (CollisionHitSphereAndBox(pSceneNow, pSceneNow2))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnCollisionEnter(pSceneNow2);
								}
							}
						}

					}
				}

				pSceneNow2 = pSceneNext2;			//次回アップデート対象を格納
			}
		}

		pSceneNow = pSceneNext;				//次回アップデート対象を格納
	}

	//逆順
	pSceneNow = m_apTop[COLLISIONTYPE_SPHERE];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[COLLISIONTYPE_SPHERE];				//次回アップデート対象を控える

		pSceneNow2 = m_apTop[COLLISIONTYPE_BOX];

		if (pSceneNow->GetUse())
		{// 当たり判定の計算対象かどうか

			pSceneNow->Update();

			//次がなくなるまで繰り返す
			while (pSceneNow2 != NULL)
			{
				pSceneNext2 = pSceneNow2->m_pNext[COLLISIONTYPE_BOX];		// 次回アップデート対象を控える

				// 当たり判定の計算
				if (pSceneNow2->GetUse())
				{// 当たり判定の計算対象かどうか
					if (pSceneNow != pSceneNow2)
					{// 対象が同一じゃないかどうか

						first = pSceneNow->GetPosition();
						second = pSceneNow2->GetPosition();

						first.y = 0.0f;
						second.y = 0.0f;

						if (pSceneNow->GetTrigger() || pSceneNow2->GetTrigger())
						{
							if (CollisionHitSphereAndBox(pSceneNow2, pSceneNow))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnTriggerEnter(pSceneNow2);
								}
							}
						}
						else
						{
							if (CollisionHitSphereAndBox(pSceneNow2, pSceneNow))
							{
								if (pSceneNow->m_pScene != NULL)
								{
									pSceneNow->m_pScene->OnCollisionEnter(pSceneNow2);
								}
							}
						}
					}
				}

				pSceneNow2 = pSceneNext2;			//次回アップデート対象を格納
			}
		}

		pSceneNow = pSceneNext;				//次回アップデート対象を格納
	}
}

//==============================================================================
//全ての当たり判定を描画
//==============================================================================
void CCollider::DrawAll(void)
{
	CCollider *pSceneNext = NULL;										//次回描画対象
	CCollider *pSceneNow = NULL;
	for (int nCount = 0; nCount < COLLISIONTYPE_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];					//次回描画対象を控える

			if (pSceneNow->GetUse())
			{
				pSceneNow->Draw();										//描画
			}
			pSceneNow = pSceneNext;										//次回描画対象を格納
		}
	}
}

//==============================================================================
// 位置の設定
//==============================================================================
void CCollider::SetPosition(D3DXVECTOR3 pos)
{
	SetPosOld(m_pPos);
	m_pPos = pos;
}

//==============================================================================
// 前回位置の設定
//==============================================================================
void CCollider::SetPosOld(D3DXVECTOR3 pos)
{
	m_posOld = pos;
}

//==============================================================================
// 更新描画対象設定
//==============================================================================
void CCollider::SetActive(bool bValue)
{
	m_bActive = bValue;					// 代入
}

//==============================================================================
// 削除予約
//==============================================================================
void CCollider::Release(void)
{
	if (!m_bDie)
	{
		m_bDie = true;
	}
}

//==============================================================================
// タグの設定
//==============================================================================
void CCollider::SetTag(std::string sTag)
{
	m_sTag = sTag;						// タグ の代入
}

//==============================================================================
// 当たり判定の持ち主を設定
//==============================================================================
void CCollider::SetScene(CScene *pScene)
{
	m_pScene = pScene;					// 生成主 のポインタ を代入
}

//==============================================================================
// マトリックスの設定
//==============================================================================
void CCollider::SetMatrix(D3DXMATRIX *mtx)
{
	m_mtxWorld = *mtx;
}

//==============================================================================
// 当たり判定タイプ
//==============================================================================
void CCollider::SetTrigger(bool bValue)
{
	m_bTrigger = bValue;				// Triggerの有無 を代入
}

//=============================================================================
// 当たり判定の使用設定
//==============================================================================
void CCollider::SetUse(bool bValue)
{
	m_bUse = bValue;
}

//=============================================================================
// 位置修正の可否設定
//==============================================================================
void CCollider::SetMoving(bool bValue)
{
	m_bMoving = bValue;
}

//==============================================================================
// オフセットの設定
//==============================================================================
void CCollider::SetOffset(D3DXVECTOR3 pos)
{
	m_offset = pos;
}

//==============================================================================
// 当たり判定チェック
//==============================================================================
void CCollider::ColliderCheck(void)
{
	COLLISIONTYPE type = this->GetColliderType();

	CCollider *pSceneNext = NULL;				//次回描画対象
	CCollider *pSceneNow = NULL;
	bool bJudge = false;

	pSceneNow = m_apTop[type];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[type];			//次回描画対象を控える

		if (pSceneNow->GetUse())
		{
			if (this != pSceneNow)
			{// 対象が同一じゃないかどうか
				if (this->GetTrigger() || pSceneNow->GetTrigger())
				{
					switch (type)
					{
					case COLLISIONTYPE_SPHERE:
						bJudge = SphereHitCollider(this, pSceneNow);
						break;
					case COLLISIONTYPE_BOX:
						bJudge = BoxInCollider(this, pSceneNow);
						break;
					}

					if (bJudge)
					{
						if (this->m_pScene != NULL)
						{
							this->m_pScene->OnTriggerEnter(pSceneNow);
						}
					}
				}
				else
				{
					switch (type)
					{
					case COLLISIONTYPE_SPHERE:
						bJudge = SphereInCollider(this, pSceneNow);
						break;
					case COLLISIONTYPE_BOX:
						bJudge = BoxHitCollider(this, pSceneNow);
						break;
					}

					if (bJudge)
					{
						if (this->m_pScene != NULL)
						{
							this->m_pScene->OnCollisionEnter(pSceneNow);
						}
					}
				}
			}
		}
		pSceneNow = pSceneNext;						//次回描画対象を格納
	}
}

//==============================================================================
// 球体と球体の当たり判定(Hit)
//==============================================================================
bool CCollider::SphereHitCollider(CCollider *pFirstTarget, CCollider *pSecondTarget)		// 球体の当たり判定
{
	D3DXVECTOR3 save = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 保存用
	D3DXVECTOR3 vec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 正規化したデータ

	CColliderSphere *pSphere = (CColliderSphere*)pFirstTarget;
	CColliderSphere *pSphere2 = (CColliderSphere*)pSecondTarget;														// 使える状態にキャスト

	D3DXVECTOR3 Target1_Pos = pSphere->GetPosition() + pSphere->GetOffset();							// Target1の位置
	D3DXVECTOR3 Target2_Pos = pSphere2->GetPosition() + pSphere2->GetOffset();							// Target2の位置

	D3DXVECTOR3 Target1_PosOld = pSphere->GetPosOld() + pSphere->GetOffset();							// Target1の前の位置
	D3DXVECTOR3 Target2_PosOld = pSphere2->GetPosOld() + pSphere2->GetOffset();							// Target2の前の位置

	if ((Target1_PosOld.x - Target2_PosOld.x) * (Target1_PosOld.x - Target2_PosOld.x) +
		(Target1_PosOld.y - Target2_PosOld.y) * (Target1_PosOld.y - Target2_PosOld.y) +
		(Target1_PosOld.z - Target2_PosOld.z) * (Target1_PosOld.z - Target2_PosOld.z) >
		(pSphere->GetRadius() + pSphere2->GetRadius()) * (pSphere->GetRadius() + pSphere2->GetRadius()))
	{// 前回もし当たっていなかったとき

		if ((Target1_Pos.x - Target2_Pos.x) * (Target1_Pos.x - Target2_Pos.x) +
			(Target1_Pos.y - Target2_Pos.y) * (Target1_Pos.y - Target2_Pos.y) +
			(Target1_Pos.z - Target2_Pos.z) * (Target1_Pos.z - Target2_Pos.z) <=
			(pSphere->GetRadius() + pSphere2->GetRadius()) * (pSphere->GetRadius() + pSphere2->GetRadius()))
		{// 今回もし当たっていたとき
			save = Target1_Pos - Target2_Pos;		//差分を求める(方向を求めるため)

			D3DXVec3Normalize(&vec, &save);			//正規化する

			if (!pSphere->GetTrigger() && !pSphere2->GetTrigger())
			{// Triggerではないとき
				if (pSphere->GetMoving())
				{
					// 食い込んだ分を求める
					Target1_Pos = Target2_Pos + vec * ((pSphere->GetRadius() + pSphere2->GetRadius()));

					// 食い込んだ分だけ戻す
					D3DXVECTOR3 offset = pSphere->GetOffset();
					pSphere->m_pScene->SetPosition(Target1_Pos - offset);
					pSphere->m_pScene->SetPosOld(Target1_Pos - offset);
				}
			}

			return true;				// フラグを立てる
		}
	}

	return false;				// フラグの状態を返す
}

//==============================================================================
// 球体と球体の当たり判定(Stay)
//==============================================================================
bool CCollider::SphereInCollider(CCollider *pFirstTarget, CCollider *pSecondTarget)
{
	D3DXVECTOR3 save = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 保存用
	D3DXVECTOR3 vec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 正規化したデータ

	CColliderSphere *pSphere = (CColliderSphere*)pFirstTarget;
	CColliderSphere *pSphere2 = (CColliderSphere*)pSecondTarget;														// 使える状態にキャスト

	D3DXVECTOR3 Target1_Pos = pSphere->GetPosition() + pSphere->GetOffset();							// Target1の位置
	D3DXVECTOR3 Target2_Pos = pSphere2->GetPosition() + pSphere2->GetOffset();							// Target2の位置
	D3DXVECTOR3 Target1_PosOld = pSphere->GetPosOld() + pSphere->GetOffset();							// Target1の前の位置
	D3DXVECTOR3 Target2_PosOld = pSphere2->GetPosOld() + pSphere2->GetOffset();							// Target2の前の位置

	if ((Target1_Pos.x - Target2_Pos.x) * (Target1_Pos.x - Target2_Pos.x) +
		(Target1_Pos.y - Target2_Pos.y) * (Target1_Pos.y - Target2_Pos.y) +
		(Target1_Pos.z - Target2_Pos.z) * (Target1_Pos.z - Target2_Pos.z) <=
		(pSphere->GetRadius() + pSphere2->GetRadius()) * (pSphere->GetRadius() + pSphere2->GetRadius()))
	{// 今回もし当たっていたとき
		save = Target1_Pos - Target2_Pos;		//差分を求める(方向を求めるため)

		D3DXVec3Normalize(&vec, &save);			//正規化する

		if (pSphere->GetMoving())
		{
			// 食い込んだ分を求める
			Target1_Pos = Target2_Pos + vec * ((pSphere->GetRadius() + pSphere2->GetRadius()));

			// 食い込んだ分だけ戻す
			D3DXVECTOR3 offset = pSphere->GetOffset();
			pSphere->m_pScene->SetPosition(Target1_Pos - offset);
			pSphere->m_pScene->SetPosOld(Target1_Pos - offset);
		}
		return true;				// フラグを立てる
	}

	return false;				// フラグの状態を返す
}

//==============================================================================
// 立方体と立方体の当たり判定(Hit)
//==============================================================================
bool CCollider::BoxHitCollider(CCollider * pFirstTarget, CCollider * pSecondTarget)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 offset;
	CColliderBox *pBox = (CColliderBox*)pFirstTarget;
	CColliderBox *pBox2 = (CColliderBox*)pSecondTarget;														// 使える状態にキャスト

	D3DXVECTOR3 Target1_pos = pBox->m_pScene->GetPosition() + pBox->GetOffset();							// Target1の位置
	D3DXVECTOR3 Target2_pos = pBox2->m_pScene->GetPosition() + pBox2->GetOffset();							// Target2の位置
	D3DXVECTOR3 Target1_posOld = pBox->m_pScene->GetPosOld() + pBox->GetOffset();							// Target1の前の位置
	D3DXVECTOR3 Target2_posOld = pBox2->m_pScene->GetPosOld() + pBox2->GetOffset();							// Target2の前の位置
	D3DXVECTOR3 Target1_size = pBox->GetSize();
	D3DXVECTOR3 Target2_size = pBox2->GetSize();

	// 左右範囲判定
	if ((Target1_posOld.x + Target1_size.x / 2 > Target2_posOld.x - Target2_size.x / 2) &&
		(Target1_posOld.x - Target1_size.x / 2 <= Target2_posOld.x + Target2_size.x / 2))
	{
		// 上下範囲判定
		if ((Target1_posOld.y + Target1_size.y / 2 > Target2_posOld.y - Target2_size.y / 2) &&
			(Target1_posOld.y - Target1_size.y / 2 <= Target2_posOld.y + Target2_size.y / 2))
		{
			// 奥行き判定
			if ((Target1_pos.z + Target1_size.z / 2 > Target2_pos.z - Target2_size.z / 2) &&
				(Target1_posOld.z + Target1_size.z / 2 <= Target2_posOld.z - Target2_size.z / 2))
			{// 手前
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;								// pFirstTarget の現在位置を取得
						offset = pBox->GetOffset();
						pos.z = Target2_pos.z - Target2_size.z / 2 - Target1_size.z / 2;		// pSecondTarget のぶつかった面の最大値分まで pFirstTarget を戻す
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}

				return true;											// フラグを立てる
			}
			else if ((Target1_pos.z - Target1_size.z / 2 < Target2_pos.z + Target2_size.z / 2) &&
				(Target1_posOld.z - Target1_size.z / 2 >= Target2_posOld.z + Target2_size.z / 2))
			{// 奥
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;								// pFirstTarget の現在位置を取得
						offset = pBox->GetOffset();
						pos.z = Target2_pos.z + Target2_size.z / 2 + Target1_size.z / 2;		// pSecondTarget のぶつかった面の最大値分まで pFirstTarget を戻す
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}
				return true;
			}
		}
	}

	//奥行き範囲判定
	if ((Target1_posOld.z + Target1_size.z / 2 > Target2_posOld.z - Target2_size.z / 2) &&
		(Target1_posOld.z - Target1_size.z / 2 <= Target2_posOld.z + Target2_size.z / 2))
	{
		//上下範囲判定
		if ((Target1_posOld.y + Target1_size.y / 2 > Target2_posOld.y - Target2_size.y / 2) &&
			(Target1_posOld.y - Target1_size.y / 2 <= Target2_posOld.y + Target2_size.y / 2))
		{
			//プレイヤーの判定(左右判定)
			if (Target1_pos.x + Target1_size.x / 2 > Target2_pos.x - Target2_size.x / 2 &&
				(Target1_posOld.x + Target1_size.x / 2 <= Target2_posOld.x - Target2_size.x / 2))
			{
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;								// pFirstTarget の現在位置を取得
						offset = pBox->GetOffset();
						pos.x = Target2_pos.x - Target2_size.x / 2 - Target1_size.x / 2;		// pSecondTarget のぶつかった面の最大値分まで pFirstTarget を戻す
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}
				return true;
			}
			else if (Target1_pos.x - Target1_size.x / 2 < Target2_pos.x + Target2_size.x / 2 &&
				(Target1_posOld.x - Target1_size.x / 2 >= Target2_posOld.x + Target2_size.x / 2))
			{
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;								// pFirstTarget の現在位置を取得
						offset = pBox->GetOffset();
						pos.x = Target2_pos.x + Target2_size.x / 2 + Target1_size.x / 2;		// pSecondTarget のぶつかった面の最大値分まで pFirstTarget を戻す
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}

				return true;
			}
		}
	}
	//左右範囲判定
	if ((Target1_posOld.x + Target1_size.x / 2 > Target2_posOld.x - Target2_size.x / 2) &&
		(Target1_posOld.x - Target1_size.x / 2 <= Target2_posOld.x + Target2_size.x / 2))
	{
		//奥行き範囲判定
		if ((Target1_posOld.z + Target1_size.z / 2 > Target2_posOld.z - Target2_size.z / 2) &&
			(Target1_posOld.z - Target1_size.z / 2 <= Target2_posOld.z + Target2_size.z / 2))
		{
			//上下判定
			if ((Target1_pos.y + Target1_size.y / 2 > Target2_pos.y - Target2_size.y / 2) &&
				(Target1_posOld.y + Target1_size.y / 2 <= Target2_posOld.y - Target2_size.y / 2))
			{// 上
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;
						offset = pBox->GetOffset();
						pos.y = Target2_pos.y - Target2_size.y / 2 - Target1_size.y / 2;
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}

				return true;
			}
			else if ((Target1_pos.y - Target1_size.y / 2 < Target2_pos.y + Target2_size.y / 2) &&
				(Target1_posOld.y - Target1_size.y / 2 >= Target2_posOld.y + Target2_size.y / 2))
			{// 下
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pBox2->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = Target1_pos;
						offset = pBox->GetOffset();
						pos.y = Target2_pos.y + Target2_size.y / 2 + Target1_size.y / 2;
						pBox->m_pScene->SetPosition(pos - offset);				// 当たり判定の位置を変える
					}
				}

				return true;
			}
		}
	}

	//結果を返す
	return false;
}

//==============================================================================
// 立方体と立方体の当たり判定(In)
//==============================================================================
bool CCollider::BoxInCollider(CCollider *pFirstTarget, CCollider *pSecondTarget)
{
	CColliderBox *pBox = (CColliderBox*)pFirstTarget;
	CColliderBox *pBox2 = (CColliderBox*)pSecondTarget;											// 使える状態にキャスト

	D3DXVECTOR3 Target1_Pos = pBox->GetPosition() + pBox->GetOffset();							// Target1の位置
	D3DXVECTOR3 Target2_Pos = pBox2->GetPosition() + pBox2->GetOffset();						// Target2の位置

	D3DXVECTOR3 Target1_Size = pBox->GetSize();
	D3DXVECTOR3 Target2_Size = pBox2->GetSize();

	//横の当たり判定 (pos1の下側)
	if (Target1_Pos.z - Target1_Size.z / 2 < Target2_Pos.z + Target2_Size.z / 2)
	{
		if (Target1_Pos.z - Target1_Size.z / 2 >= Target2_Pos.z - Target2_Size.z / 2)
		{
			if (Target1_Pos.y - Target1_Size.y / 2 < Target2_Pos.y + Target2_Size.y / 2)
			{
				if (Target1_Pos.y - Target1_Size.y / 2 >= Target2_Pos.y - Target2_Size.y / 2)
				{
					//横の当たり判定 (pos1の右側)
					if (Target1_Pos.x + Target1_Size.x / 2 < Target2_Pos.x + Target2_Size.x / 2)
					{
						if (Target1_Pos.x + Target1_Size.x / 2 >= Target2_Pos.x - Target2_Size.x / 2)
						{
							return true;
						}
					}
					//横の当たり判定 (pos1の左側)
					else if (Target1_Pos.x - Target1_Size.x / 2 < Target2_Pos.x + Target2_Size.x / 2)
					{
						if (Target1_Pos.x - Target1_Size.x / 2 >= Target2_Pos.x - Target2_Size.x / 2)
						{
							return true;
						}
					}
				}
			}
		}
	}
	//横の当たり判定 (pos1の上側)
	if (Target1_Pos.z - Target1_Size.z / 2 < Target2_Pos.z + Target2_Size.z / 2)
	{
		if (Target1_Pos.z + Target1_Size.z / 2 >= Target2_Pos.z - Target2_Size.z / 2)
		{
			if (Target1_Pos.y - Target1_Size.y / 2 < Target2_Pos.y - Target2_Size.y / 2)
			{
				if (Target1_Pos.y + Target1_Size.y / 2 >= Target2_Pos.y - Target2_Size.y / 2)
				{
					//横の当たり判定 (pos1の右側)
					if (Target1_Pos.x + Target1_Size.x / 2 < Target2_Pos.x + Target2_Size.x / 2)
					{
						if (Target1_Pos.x + Target1_Size.x / 2 >= Target2_Pos.x - Target2_Size.x / 2)
						{
							return true;
						}
					}
					//横の当たり判定 (pos1の左側)
					else if (Target1_Pos.x - Target1_Size.x / 2 < Target2_Pos.x + Target2_Size.x / 2)
					{
						if (Target1_Pos.x - Target1_Size.x / 2 >= Target2_Pos.x - Target2_Size.x / 2)
						{
							return true;
						}
					}
				}
			}
		}
	}

	//結果を返す
	return false;
}

//=============================================================================
// 球と立方体の衝突判定
//=============================================================================
bool CCollider::CollisionHitSphereAndBox(CCollider * pFirstTarget, CCollider * pSecondTarget)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 offset;
	CColliderBox *pBox = (CColliderBox*)pFirstTarget;
	CColliderSphere *pSphere = (CColliderSphere*)pSecondTarget;														// 使える状態にキャスト

	if(pBox->m_pScene == NULL || pSphere->m_pScene == NULL) { return false; }

	D3DXVECTOR3 boxPos = pBox->m_pScene->GetPosition() + pBox->GetOffset();							// Target1の位置
	D3DXVECTOR3 spherePos = pSphere->m_pScene->GetPosition() + pSphere->GetOffset();						// Target2の位置

	D3DXVECTOR3 boxPosOld = pBox->m_pScene->GetPosOld() + pBox->GetOffset();							// Target1の位置
	D3DXVECTOR3 spherePosOld = pSphere->m_pScene->GetPosOld() + pSphere->GetOffset();						// Target2の位置

	D3DXVECTOR3 boxSize = pBox->GetSize();
	float fRadius = pSphere->GetRadius();

	// 左右範囲判定
	if (((boxPosOld.x + boxSize.x / 2) > spherePosOld.x) &&
		((boxPosOld.x - boxSize.x / 2) < spherePosOld.x))
	{
		// 上下範囲判定
		if (((boxPosOld.y + boxSize.y / 2) > spherePosOld.y) &&
			((boxPosOld.y - boxSize.y / 2) < spherePosOld.y))
		{
			// 奥行き判定
			if (((boxPos.z + boxSize.z / 2) + fRadius > spherePos.z) &&
				((boxPosOld.z + boxSize.z / 2) + fRadius <= spherePosOld.z))
			{// 手前
			 //トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.z = boxPos.z + boxSize.z / 2 + fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}

				return true;											// フラグを立てる
			}
			else if (((boxPos.z - boxSize.z / 2) - fRadius < spherePos.z) &&
				((boxPosOld.z - boxSize.z / 2) - fRadius >= spherePosOld.z))
			{// 奥
			 //トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.z = boxPos.z - boxSize.z / 2 - fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}
				return true;
			}
		}
	}

	//奥行き範囲判定
	if (((boxPosOld.z + boxSize.z / 2) > spherePosOld.z) &&
		((boxPosOld.z - boxSize.z / 2) < spherePosOld.z))
	{
		//上下範囲判定
		if (((boxPosOld.y + boxSize.y / 2) > spherePosOld.y) &&
			((boxPosOld.y - boxSize.y / 2) < spherePosOld.y))
		{
			//プレイヤーの判定(左右判定)
			if (((boxPos.x + boxSize.x / 2) + fRadius > spherePos.x) &&
				((boxPosOld.x + boxSize.x / 2) + fRadius <= spherePosOld.x))
			{
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.x = boxPos.x + boxSize.x / 2 + fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}
				return true;
			}
			else if (((boxPos.x - boxSize.x / 2) - fRadius < spherePos.x) &&
				((boxPosOld.x - boxSize.x / 2) - fRadius >= spherePosOld.x))
			{
				//トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.x = boxPos.x - boxSize.x / 2 - fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}

				return true;
			}
		}
	}
	//左右範囲判定
	if (((boxPosOld.x + boxSize.x / 2) > spherePosOld.x) &&
		((boxPosOld.x - boxSize.x / 2) < spherePosOld.x))
	{
		//奥行き範囲判定
		if (((boxPosOld.z + boxSize.z / 2) > spherePosOld.z) &&
			((boxPosOld.z - boxSize.z / 2) < spherePosOld.z))
		{
			//上下判定
			if (((boxPos.y + boxSize.y / 2) + fRadius > spherePos.y) &&
				((boxPosOld.y + boxSize.y / 2) + fRadius <= spherePosOld.y))
			{// 上
			 //トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.y = boxPos.y + boxSize.y / 2 + fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}

				return true;
			}
			else if (((boxPos.y - boxSize.y / 2) - fRadius < spherePos.y) &&
				((boxPosOld.y - boxSize.y / 2) - fRadius >= spherePosOld.y))
			{// 下
			 //トリガーでは無いとき
				if (!pBox->GetTrigger() && !pSphere->GetTrigger())
				{// Triggerではないとき
					if (pBox->GetMoving())
					{
						//プレイヤーの位置を補正
						pos = spherePos;
						pos.y = boxPos.y - boxSize.y / 2 - fRadius;
						pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());
					}
				}

				return true;
			}
		}
	}

	//結果を返す
	return false;
}

//=============================================================================
// 球と立方体の範囲内判定
//=============================================================================
bool CCollider::CollisionInSphereAndBox(CCollider *pFirstTarget, CCollider *pSecondTarget)
{
	CColliderBox *pBox = (CColliderBox*)pFirstTarget;
	CColliderSphere *pSphere = (CColliderSphere*)pSecondTarget;														// 使える状態にキャスト

	D3DXVECTOR3 boxPos = pBox->GetPosition() + pBox->GetOffset();							// Target1の位置
	D3DXVECTOR3 spherePos = pSphere->GetPosition() + pSphere->GetOffset();						// Target2の位置

	D3DXVECTOR3 boxSize = pBox->GetSize();
	float fRadius = pSphere->GetRadius();

	// 左右の半径込の当たり判定
	if ((boxPos.x - boxSize.x / 2) - fRadius < spherePos.x)
	{// 左と点
		if ((boxPos.x + boxSize.x / 2) + fRadius > spherePos.x)
		{// 右と点
			if ((boxPos.y - boxSize.y / 2) - fRadius < spherePos.y)
			{// 上と点
				if ((boxPos.y + boxSize.y / 2) + fRadius > spherePos.y)
				{// 下と点
					if (boxPos.z - boxSize.z / 2 - fRadius < spherePos.z)
					{// 奥と点
						if (boxPos.z + boxSize.z / 2 + fRadius > spherePos.z)
						{// 手前と点
							D3DXVECTOR3 pos = spherePos;
							pos.z = boxPos.z + boxSize.z / 2 + fRadius;
							pSphere->m_pScene->SetPosition(pos - pSphere->GetOffset());

							return true;														// 当たり判定 のフラグを立てる
						}
					}
				}
			}
		}
	}
	// 奥行き半径込の当たり判定
	if (boxPos.z - boxSize.z / 2 - fRadius < spherePos.z)
	{// 左と点
		if (boxPos.z + boxSize.z / 2 + fRadius > spherePos.z)
		{// 右と点
			if ((boxPos.x - boxSize.x / 2) - fRadius < spherePos.x)
			{// 奥と点
				if ((boxPos.x + boxSize.x / 2) + fRadius > spherePos.x)
				{// 手前と点
					if ((boxPos.y - boxSize.y / 2) - fRadius < spherePos.y)
					{// 奥と点
						if ((boxPos.y + boxSize.y / 2) + fRadius > spherePos.y)
						{// 手前と点
							return true;													// 当たり判定 のフラグを立てる
						}
					}
				}
			}
		}
	}
	// 角の円と点による当たり判定
	if (CollisionHitSphereAndPoint(D3DXVECTOR3(0.0f, 0.0f, 0.0f), pSecondTarget))
	{// 当たり判定 のフラグが立ったとき
		return true;
	}

	return false;																		// 当たり判定 のフラグ状態を返す
}

//=============================================================================
// 球と点の当たり判定
//=============================================================================
bool CCollider::CollisionHitSphereAndPoint(D3DXVECTOR3 pos, CCollider *pSecondTarget)
{
	CColliderSphere *pSphere = (CColliderSphere*)pSecondTarget;					// 使える状態にキャスト

	D3DXVECTOR3 save = D3DXVECTOR3(0.0f, 0.0f, 0.0f);							// 保存用
	D3DXVECTOR3 vec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);							// 正規化したデータ

	D3DXVECTOR3 spherePos = pSphere->m_pScene->GetPosition() + pSphere->GetOffset();						// Target2の位置
	D3DXVECTOR3 spherePosOld = pSphere->m_pScene->GetPosOld() + pSphere->GetOffset();						// Target2の位置
	float fRadius = pSphere->GetRadius();

	D3DXVECTOR3 point = pos - pSphere->GetOffset();	// 位置

	if ((spherePosOld.x - pos.x) * (spherePosOld.x - pos.x) +
		(spherePosOld.z - pos.z) * (spherePosOld.z - pos.z) >
		pSphere->GetRadius() * pSphere->GetRadius())
	{// 前回もし当たっていなかったとき
		if ((spherePos.x - point.x) * (spherePos.x - point.x) +
			(spherePos.z - point.z) * (spherePos.z - point.z) <=
			fRadius * fRadius)
		{// 今回もし当たっていたとき
			save = spherePos - point;		//差分を求める(方向を求めるため)

			D3DXVec3Normalize(&vec, &save);			//正規化する

			if (!pSphere->GetTrigger())
			{// Triggerではないとき
				if (pSphere->GetMoving())
				{
					D3DXVECTOR3 offset = pSphere->GetOffset();

					// 食い込んだ分を求める
					spherePos = point + vec * pSphere->GetRadius();
					spherePos -= offset;
					spherePos.y = 0.0f;
					// 食い込んだ分だけ戻す
					pSphere->m_pScene->SetPosition(spherePos);
					pSphere->m_pScene->SetPosOld(spherePos);
				}
			}
			return true;				// フラグを立てる
		}
	}

	return false;
}

//==============================================================================
// 削除処理
//==============================================================================
void CCollider::Delete(void)
{
	if (this != NULL)
	{
		//終了処理及びメモリ解放
		this->Uninit();

		if (this->m_pPrev[m_Obj] == NULL)
		{//自分が先頭だったとき

			if (this->m_pNext[m_Obj] != NULL)
			{//自分の次のオブジェクトがあるとき
				m_apTop[m_Obj] = this->m_pNext[m_Obj];																//先頭を次のオブジェクトに譲渡する

				if (m_apTop[m_Obj]->m_pPrev[m_Obj] != NULL)
				{//前の人の情報がある場合
					m_apTop[m_Obj]->m_pPrev[m_Obj] = NULL;															//前にあるオブジェクト情報を NULL にする
				}
			}
			else
			{//自分の次のオブジェクトがなかったとき
				m_apTop[m_Obj] = NULL;
				m_apCur[m_Obj] = NULL;
			}
		}
		else if (this->m_pNext[m_Obj] == NULL)
		{//自分が最後尾だったとき
			m_apCur[m_Obj] = m_pPrev[m_Obj];													//最後尾を前のオブジェクトに譲渡する

			if (m_apCur[m_Obj]->m_pNext[m_Obj] != NULL)
			{
				m_apCur[m_Obj]->m_pNext[m_Obj] = NULL;											//後ろにあるオブジェクト情報を NULL にする
			}
		}
		else
		{//どちらでもない場合
			m_pNext[m_Obj]->m_pPrev[m_Obj] = m_pPrev[m_Obj];									//次のオブジェクトの前のオブジェクト情報に自分の前のオブジェクト情報を格納する
			m_pPrev[m_Obj]->m_pNext[m_Obj] = m_pNext[m_Obj];									//前のオブジェクトの次のオブジェクト情報に自分の次のオブジェクト情報を格納する
		}

		this->m_pScene = NULL;
		delete this;
		m_nNumAll--;																			//生成数を一つ減らす
	}
}



//======================================================================================================================
// レイの判定
//======================================================================================================================
bool CCollider::RayBlockCollision(D3DXVECTOR3 &pos, D3DXMATRIX *pMat, float fOffset, float fLength, VERTEX_PLANE &plane)
{
	// 地形判定 変数宣言
	BOOL				bHitFlag = false;		// 判定が出たかのフラグ
	bool				bLand = false;			// 判定が出たかのフラグ
	float				fLandDistance = 0;		// 距離
	DWORD				dwHitIndex = -1;		// インデックス
	float				fHitU = 0;		// U
	float				fHitV = 0;		// V
	D3DXMATRIX			invmat;							// 逆行列を格納する変数
	D3DXVECTOR3			m_posAfter;						// 逆行列で出した終点情報を格納する
	D3DXVECTOR3			m_posBefore;					// 終点情報を格納する
	D3DXVECTOR3			direction;						// 変換後の位置、方向を格納する変数
	std::vector<VERTEX_PLANE>	vPlane;
	std::vector<float>	vDistance;						// 長さの配列保存
	float				fData = 0.0f;		// データ
	float				fDistanceMin = 100000.0f;		// 最も近かったもの

	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;

	// modelのオブジェクトのポジションを参照
	pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, (CScene::PRIORITY_MODEL));							//次回アップデート対象を控える
		CObject *pObj = (CObject*)pSceneNow;

		//	逆行列の取得
		D3DXMatrixInverse(&invmat, NULL, &pObj->GetMtxWorld());
		//	逆行列を使用し、レイ始点情報を変換　位置と向きで変換する関数が異なるので要注意
		D3DXVec3TransformCoord(&m_posBefore, &D3DXVECTOR3(pos.x, pMat->_42+ fLength, pos.z), &invmat);
		//	レイ終点情報を変換
		D3DXVec3TransformCoord(&m_posAfter, &D3DXVECTOR3(pos.x, pos.y - 1, pos.z), &invmat);
		//	レイ方向情報を変換
		D3DXVec3Normalize(&direction, &(m_posAfter - m_posBefore));

		//Rayを飛ばす
		D3DXIntersect(pObj->GetMesh(), &m_posBefore, &direction, &bHitFlag, &dwHitIndex, &fHitU, &fHitV, &fLandDistance, NULL, NULL);
		if (bHitFlag == TRUE)
		{
			//長さの保存追加
			vDistance.emplace_back(fLandDistance);

			CObject *pObj = (CObject*)pSceneNow;

			pos.y = pos.y - fData - fOffset + fLength;
			DWORD num = pObj->GetMesh()->GetNumVertices();

			LPD3DXMESH pMesh = pObj->GetMesh();

			VERTEX_3D *pVtx;										//頂点情報へのポインタ
			WORD *pIdx;									//インデックスデータへポインタ

			pMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIdx);
			pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtx);

			D3DXVECTOR3 p0, p1, p2;
			int IndexBuff = pIdx[dwHitIndex];
			IndexBuff *= 3;

			p0 = pVtx[IndexBuff].pos;
			p1 = pVtx[IndexBuff + 1].pos;
			p2 = pVtx[IndexBuff + 2].pos;

			D3DXVECTOR3 AB = p0 - p1;
			D3DXVECTOR3 BC = p1 - p2;

			D3DXVECTOR3 norwork;

			D3DXVec3Cross(&norwork, &BC, &AB);
			D3DXVec3Normalize(&norwork, &norwork);

			CDebugProc::Log("法線 : %.2f, %.2f, %.2f\n", norwork.x, norwork.y, norwork.z);

			pMesh->UnlockVertexBuffer();
			pMesh->UnlockIndexBuffer();

			VERTEX_PLANE planeWork;
			planeWork.a = p0;
			planeWork.b = p1;
			planeWork.c = p2;
			vPlane.emplace_back(planeWork);
		}

		pSceneNow = pSceneNext;													//次回アップデート対象を格納
	}

	//Rayのヒットした物があったとき
	if (!vDistance.empty())
	{
		//最初の比較対象
		fData = vDistance[0];
		VERTEX_PLANE planeAns = vPlane[0];
		for (unsigned int nCnt = 0; vDistance.size() > nCnt; nCnt++)
		{
			if (vDistance[nCnt] < fData)
			{
				//比較対象が小さかったら代入
				planeAns = vPlane[nCnt];
				fData = vDistance[nCnt];
			}
		}
		if (fData < 30000)//Rayの長さの指定条件
		{
			pos.y = pos.y - fData - fOffset+ fLength;
			plane = planeAns;
		}
	}
	//Rayに判定がなかったらジャンプできない
	else
	{
		bLand = false;
	}


	//配列を空にしておく
	vDistance.clear();

	// 判定フラグを返す
	return bLand;
}

D3DXVECTOR3 CCollider::RayLeftWallCollision(float &fLeftLength, D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, D3DXVECTOR3 &move, D3DXMATRIX * pMat)
{
	// 地形判定 変数宣言
	BOOL				bHitFlag = false;		// 判定が出たかのフラグ
	float				fLandDistance = 0;		// 距離
	DWORD				dwHitIndex = -1;		// インデックス
	float				fHitU = 0;		// U
	float				fHitV = 0;		// V
	D3DXMATRIX			invmat;							// 逆行列を格納する変数
	D3DXVECTOR3			m_posAfter;						// 逆行列で出した終点情報を格納する
	D3DXVECTOR3			m_posBefore;					// 終点情報を格納する
	D3DXVECTOR3			direction;						// 変換後の位置、方向を格納する変数
	D3DXVECTOR3			endPoint;						// 終了地点
	std::vector<float>	vDistance;						// 長さの配列保存
	float				fData = 0.0f;		// データ

	float				fLeft;		// 壁の限界値

	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;

	fLeft = 1000.0f;

	endPoint.x = sinf(D3DX_PI * 0.5f + rot.y);
	endPoint.z = cosf(D3DX_PI * 0.5f + rot.y);

	// modelのオブジェクトのポジションを参照
	pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, (CScene::PRIORITY_MODEL));							//次回アップデート対象を控える
		CObject *pObj = (CObject*)pSceneNow;

		//	逆行列の取得
		D3DXMatrixInverse(&invmat, NULL, &pObj->GetMtxWorld());
		//	逆行列を使用し、レイ始点情報を変換　位置と向きで変換する関数が異なるので要注意
		D3DXVec3TransformCoord(&m_posBefore, &D3DXVECTOR3(pos.x, pos.y - 100.0f, pos.z), &invmat);
		//	レイ終点情報を変換
		D3DXVec3TransformCoord(&m_posAfter, &D3DXVECTOR3(pos.x + endPoint.x, pos.y - 100.0f, pos.z + endPoint.z), &invmat);
		//	レイ方向情報を変換
		D3DXVec3Normalize(&direction, &(m_posAfter - m_posBefore));
		//Rayを飛ばす
		D3DXIntersect(pObj->GetMesh(), &m_posBefore, &direction, &bHitFlag, &dwHitIndex, &fHitU, &fHitV, &fLandDistance, NULL, NULL);
		if (bHitFlag == TRUE)
		{
			//長さの保存追加
			vDistance.emplace_back(fLandDistance);
		}

		pSceneNow = pSceneNext;													//次回アップデート対象を格納
	}

	//Rayのヒットした物があったとき
	if (!vDistance.empty())
	{
		//最初の比較対象
		fData = vDistance[0];
		for (unsigned int nCnt = 0; vDistance.size() > nCnt; nCnt++)
		{
			if (vDistance[nCnt] < fData)
			{
				//比較対象が小さかったら代入
				fData = vDistance[nCnt];
			}
		}
		if (fData < 1500.0f)//Rayの長さの指定条件
		{
			fLeft = fData;
		}
	}

	//配列を空にしておく
	vDistance.clear();

	D3DXVECTOR3 testPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	fLeftLength = fLeft;
	testPos = pos + endPoint * (fLeft - 20.0f);
	testPos.y = pos.y;

#ifdef _DEBUG
	CDebugProc::Log("壁の限界値 : Left = %.2f %.2f %.2f\n", testPos.x, testPos.y, testPos.z);
#endif

	// 判定フラグを返す
	return testPos;
}

D3DXVECTOR3 CCollider::RayRightWallCollision(float &fRightLength, D3DXVECTOR3 &pos, D3DXVECTOR3 &rot, D3DXVECTOR3 &move, D3DXMATRIX * pMat)
{
	// 地形判定 変数宣言
	BOOL				bHitFlag = false;		// 判定が出たかのフラグ
	float				fLandDistance = 0;		// 距離
	DWORD				dwHitIndex = -1;		// インデックス
	float				fHitU = 0;		// U
	float				fHitV = 0;		// V
	D3DXMATRIX			invmat;							// 逆行列を格納する変数
	D3DXVECTOR3			m_posAfter;						// 逆行列で出した終点情報を格納する
	D3DXVECTOR3			m_posBefore;					// 終点情報を格納する
	D3DXVECTOR3			direction;						// 変換後の位置、方向を格納する変数
	D3DXVECTOR3			endPoint;						// 終了地点

	std::vector<float>	vDistance;						// 長さの配列保存
	float				fData = 0.0f;		// データ

	float				fRight = 1000.0f;		// 壁の限界値

	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;

	endPoint.x = sinf(-D3DX_PI * 0.5f + rot.y);
	endPoint.z = cosf(-D3DX_PI * 0.5f + rot.y);

	// modelのオブジェクトのポジションを参照
	pSceneNow = CScene::GetScene(CScene::PRIORITY_MODEL);

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = CScene::GetSceneNext(pSceneNow, (CScene::PRIORITY_MODEL));							//次回アップデート対象を控える
		CObject *pObj = (CObject*)pSceneNow;

		//	逆行列の取得
		D3DXMatrixInverse(&invmat, NULL, &pObj->GetMtxWorld());
		//	逆行列を使用し、レイ始点情報を変換　位置と向きで変換する関数が異なるので要注意
		D3DXVec3TransformCoord(&m_posBefore, &D3DXVECTOR3(pos.x, pos.y - 100.0f, pos.z), &invmat);
		//	レイ終点情報を変換
		D3DXVec3TransformCoord(&m_posAfter, &D3DXVECTOR3(pos.x + endPoint.x, pos.y - 100.0f, pos.z + endPoint.z), &invmat);
		//	レイ方向情報を変換
		D3DXVec3Normalize(&direction, &(m_posAfter - m_posBefore));
		//Rayを飛ばす
		D3DXIntersect(pObj->GetMesh(), &m_posBefore, &direction, &bHitFlag, &dwHitIndex, &fHitU, &fHitV, &fLandDistance, NULL, NULL);
		if (bHitFlag == TRUE)
		{
			//長さの保存追加
			vDistance.emplace_back(fLandDistance);
		}

		pSceneNow = pSceneNext;													//次回アップデート対象を格納
	}

	//Rayのヒットした物があったとき
	if (!vDistance.empty())
	{
		//最初の比較対象
		fData = vDistance[0];
		for (unsigned int nCnt = 0; vDistance.size() > nCnt; nCnt++)
		{
			if (vDistance[nCnt] < fData)
			{
				//比較対象が小さかったら代入
				fData = vDistance[nCnt];
			}
		}
		if (fData < 1500.0f)//Rayの長さの指定条件
		{
			fRight = fData;
		}
	}

	//配列を空にしておく
	vDistance.clear();

	D3DXVECTOR3 testPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	fRightLength = fRight;
	testPos = pos + endPoint * (fRight - 20.0f);
	testPos.y = pos.y;

#ifdef _DEBUG
	CDebugProc::Log("壁の限界値 : Right = %.2f %.2f %.2f\n", testPos.x, testPos.y, testPos.z);
#endif

	// 判定フラグを返す
	return testPos;
}

bool CCollider::IsInside(D3DXVECTOR3 * pvI, D3DXVECTOR3 * pvA, D3DXVECTOR3 * pvB, D3DXVECTOR3 * pvC)
{
	//辺ベクトル
	D3DXVECTOR3 vAB, vBC, vCA;
	vAB = *pvB - *pvA;
	vBC = *pvC - *pvB;
	vCA = *pvA - *pvC;
	//辺ベクトルと「頂点から交点へ向かうベクトル」との、それぞれの外積用
	D3DXVECTOR3 vCrossAB, vCrossBC, vCrossCA;
	//「外積結果のベクトル」と平面法線ベクトルとの、それぞれの内積用
	FLOAT fAB, fBC, fCA;
	// 法線用
	D3DXVECTOR3 vNormal;
	// まず、3頂点から平面方程式を求める。これは、同時に平面の法線を求めることでもある
	D3DXPLANE pln;
	D3DXPlaneFromPoints(&pln, pvA, pvB, pvC);
	vNormal.x = pln.a;//法線のx成分は平面方程式のx係数
	vNormal.y = pln.b;//法線のy成分は平面方程式のy係数
	vNormal.z = pln.c;//法線のz成分は平面方程式のz係数
	D3DXVec3Normalize(&vNormal, &vNormal);

	// 各頂点から交点Iに向かうベクトルをvVとする
	D3DXVECTOR3 vV;
	// 辺ABベクトル（頂点Bベクトル-頂点Aベクトル)と、頂点Aから交点Iへ向かうベクトル、の外積を求める
	vV = *pvI - *pvA;
	D3DXVec3Cross(&vCrossAB, &vAB, &vV);
	// 辺BCベクトル（頂点Cベクトル-頂点Bベクトル)と、頂点Bから交点Iへ向かうベクトル、の外積を求める
	vV = *pvI - *pvB;
	D3DXVec3Cross(&vCrossBC, &vBC, &vV);
	// 辺CAベクトル（頂点Aベクトル-頂点Cベクトル)と、頂点Cから交点Iへ向かうベクトル、の外積を求める
	vV = *pvI - *pvC;
	D3DXVec3Cross(&vCrossCA, &vCA, &vV);
	// それぞれの、外積ベクトルとの内積を計算する
	fAB = D3DXVec3Dot(&vNormal, &vCrossAB);
	fBC = D3DXVec3Dot(&vNormal, &vCrossBC);
	fCA = D3DXVec3Dot(&vNormal, &vCrossCA);

	// ３つの内積結果のうち、一つでもマイナス符号のものがあれば、交点は外にある。
	if (fAB >= 0 && fBC >= 0 && fCA >= 0)
	{
		// 交点は、面の内にある
		return true;
	}
	// 交点は面の外にある
	return false;
}
