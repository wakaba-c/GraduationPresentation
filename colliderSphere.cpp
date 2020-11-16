//=============================================================================
//
// スフィアコライダー処理 [colliderSphere.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "colliderSphere.h"
#include "meshSphere.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CColliderSphere::CColliderSphere(COLLISIONTYPE type) : CCollider(type)
{
	m_fRadius = 0.0f;					// 半径の初期化

#ifdef _DEBUG
	m_pSphere = NULL;					// スフィアコライダーのポインタを初期化
#endif
}

//=============================================================================
// デストラクタ
//=============================================================================
CColliderSphere::~CColliderSphere()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CColliderSphere::Init(void)
{
#ifdef _DEBUG
	m_pSphere = CMeshSphere::Create(m_fRadius);						// スフィアコライダーの生成

	if (m_pSphere != NULL)
	{// スフィアコライダーが存在していたとき
		m_pSphere->SetPosition(GetPosition());						// 位置の設定
		m_pSphere->SetColor(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.2f));		// 色の設定
		m_pSphere->MakeVertex();									// 頂点情報の更新
	}
#endif

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CColliderSphere::Uninit(void)
{
#ifdef _DEBUG
	if (m_pSphere != NULL)
	{// スフィアコライダーが存在していたとき
		m_pSphere->Release();
	}
#endif
}

//=============================================================================
// 更新処理
//=============================================================================
void CColliderSphere::Update(void)
{
	CCollider::Update();

#ifdef _DEBUG
	if (m_pSphere != NULL)
	{// スフィアコライダーが存在していたとき
		if (GetUse())
		{// 使われているとき
			if (!m_pSphere->GetActive())
			{// アクティブではなかったとき
				m_pSphere->SetActive(true);				// アクティブにする
			}
		}
		else
		{// 使われていないとき
			if (m_pSphere->GetActive())
			{// アクティブだったとき
				m_pSphere->SetActive(false);			// 解除する
			}
		}

		m_pSphere->SetPosition(GetPosition() + GetOffset());			// オフセットを考慮した位置の設定
	}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CColliderSphere::Draw(void)
{

}

//==============================================================================
// クリエイト処理
//==============================================================================
CColliderSphere *CColliderSphere::Create(bool bTrigger, float fRadius)
{
	CColliderSphere *pColliderSphere;
	pColliderSphere = new CColliderSphere(COLLISIONTYPE_SPHERE);

	if (pColliderSphere != NULL)
	{
		pColliderSphere->SetTrigger(bTrigger);							// トリガーの有無を設定
		pColliderSphere->Init();										// 初期化
		pColliderSphere->SetRadius(fRadius);							// 半径の設定
	}
	return pColliderSphere;
}

//=============================================================================
// 半径の設定
//==============================================================================
void CColliderSphere::SetRadius(float fValue)
{
	m_fRadius = fValue;				// 半径の代入

#ifdef _DEBUG
	if (m_pSphere != NULL)
	{// スフィアコライダーが存在していたとき
		m_pSphere->SetRadius(fValue);			// 半径の設定
		m_pSphere->MakeVertex();				// 頂点情報の更新
	}
#endif
}