//=============================================================================
//
// ボックスコライダー処理 [colliderBox.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "colliderBox.h"
#include "meshCube.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CColliderBox::CColliderBox(COLLISIONTYPE type) : CCollider(type)
{
	m_size = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// サイズの初期化

#ifdef _DEBUG
	m_pCube = NULL;										// ボックスコライダーポインタの初期化
#endif
}

//=============================================================================
// デストラクタ
//=============================================================================
CColliderBox::~CColliderBox()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CColliderBox::Init(void)
{
#ifdef _DEBUG
	m_pCube = CMeshCube::Create();								// ボックスコライダーの生成

	if (m_pCube != NULL)
	{// ボックスコライダーが存在していたとき
		m_pCube->SetPosition(GetPosition());					// 位置の設定
		m_pCube->SetSize(m_size);								// サイズの設定
		m_pCube->SetColor(D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.2f));	// 色の設定
		m_pCube->MakeVertex();									// 頂点情報の更新
	}
#endif
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CColliderBox::Uninit(void)
{
#ifdef _DEBUG
	if (m_pCube != NULL)
	{// ボックスコライダーが存在していたとき
		m_pCube->Release();
	}
#endif
}

//=============================================================================
// 更新処理
//=============================================================================
void CColliderBox::Update(void)
{
#ifdef _DEBUG
	if (m_pCube != NULL)
	{// ボックスコライダーが存在していたとき
		if (GetUse())
		{// 使われているとき
			if (!m_pCube->GetActive())
			{// アクティブではなかったとき
				m_pCube->SetActive(true);			// アクティブにする
			}
		}
		else
		{// 使われていないとき
			if (m_pCube->GetActive())
			{// アクティブだったとき
				m_pCube->SetActive(false);			// 解除する
			}
		}

		m_pCube->SetPosition(GetPosition() + GetOffset());		// オフセットを考慮した位置の設定
	}
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CColliderBox::Draw(void)
{

}

//==============================================================================
// クリエイト処理
//==============================================================================
CColliderBox *CColliderBox::Create(bool bTrigger, D3DXVECTOR3 size)
{
	CColliderBox *pColliderBox;
	pColliderBox = new CColliderBox(COLLISIONTYPE_BOX);

	if (pColliderBox != NULL)
	{// ボックスコライダーが存在していたとき
		pColliderBox->SetTrigger(bTrigger);		// トリガーの有無を設定
		pColliderBox->SetSize(size);			// サイズの設定
		pColliderBox->Init();					// 初期化処理
	}
	return pColliderBox;
}

//=============================================================================
// サイズの設定
//=============================================================================
void CColliderBox::SetSize(D3DXVECTOR3 size)
{
	m_size = size;

#ifdef _DEBUG
	if (m_pCube != NULL)
	{// スフィアコライダーが存在していたとき
		m_pCube->SetSize(size);			// 半径の設定
		m_pCube->MakeVertex();				// 頂点情報の更新
	}
#endif
}