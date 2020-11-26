//=============================================================================
//
// 数字処理 [number.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "counter.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CCounter::CCounter(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene(obj)
{
	m_vNumber = {};

	m_distance = D3DXVECTOR3_ZERO;
	m_IntervalNum = D3DXVECTOR3_ZERO;
	m_nLength = 0;
}

//=============================================================================
// デストラクタ
//=============================================================================
CCounter::~CCounter()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CCounter::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CCounter::Uninit(void)
{
	for (unsigned int nCount = 0; nCount < m_vNumber.size(); nCount++)
	{
		if (m_vNumber[nCount] != NULL)
		{
			m_vNumber[nCount]->Uninit();
			m_vNumber[nCount]->Release();
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CCounter::Update(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void CCounter::Draw(void)
{
}

//=============================================================================
// クリエイト処理
//=============================================================================
CCounter *CCounter::Create(int nLength)
{
	CCounter *pCounter;
	pCounter = new CCounter(PRIORITY_UI);

	if (pCounter != NULL)
	{// 数字が存在していたとき
		pCounter->m_nLength = nLength;		// 最大桁数

		// 桁数分生成する
		for (int nCount = 0; nCount < nLength; nCount++)
		{
			CNumber *pNumber = CNumber::Create();		// 数字の作成

			if (pNumber != NULL)
			{// 数字が存在していたとき
				pNumber->SetNumber(0);
				pCounter->m_vNumber.push_back(pNumber);
			}
		}

		pCounter->Init();					// 初期化処理
	}
	return pCounter;
}

//=============================================================================
// 差分の設定
//=============================================================================
void CCounter::SetDistance(D3DXVECTOR3 &distance)
{
	m_distance = distance;
	SetTransform();
}

//=============================================================================
// 数字どうしの距離
//=============================================================================
void CCounter::SetIntervalNum(D3DXVECTOR3 & interval)
{
	m_IntervalNum = interval;
	SetTransform();
}

//=============================================================================
// 位置関係の更新
//=============================================================================
void CCounter::SetTransform(void)
{
	D3DXVECTOR3 pos = GetPosition();

	for (unsigned int nCount = 0; nCount < m_vNumber.size(); nCount++)
	{
		if (m_vNumber[nCount] != NULL)
		{
			m_vNumber[nCount]->SetPosition((m_distance + pos) + m_IntervalNum * (float)nCount);
			m_vNumber[nCount]->SetTransform();
		}
	}
}

//=============================================================================
// テクスチャの設定
//=============================================================================
void CCounter::BindTexture(std::string Add)
{
	for (unsigned int nCount = 0; nCount < m_vNumber.size(); nCount++)
	{
		if (m_vNumber[nCount] != NULL)
		{
			// テクスチャの設定
			m_vNumber[nCount]->BindTexture(Add);
		}
	}
}

//=============================================================================
// タイムの変更
//=============================================================================
void CCounter::SetNumber(int nValue)
{
	int nNumber;
	unsigned int nLength = CManager::LengthCalculation(nValue);

	// 最大桁数までカウント
	for (unsigned int nCount = 0; nCount < m_vNumber.size(); nCount++)
	{
		// 表示する数字計算
		nNumber = nValue % (int)powf(10.0f, (float)m_vNumber.size() - nCount) / (int)powf(10.0f, m_vNumber.size() - 1.0f - nCount);

		// 数字設定
		m_vNumber[nCount]->SetNumber(nNumber);

		if (m_vNumber.size() - nCount <= nLength)
		{// 最大桁数より小さい桁だったとき
			if (!m_vNumber[nCount]->GetActive())
			{
				m_vNumber[nCount]->SetActive(true);
			}
		}
		else
		{// 最大桁数より大きい桁だったとき
			if (m_vNumber[nCount]->GetActive())
			{
				m_vNumber[nCount]->SetActive(false);
			}
		}
	}
}

//=============================================================================
// サイズの設定
//=============================================================================
void CCounter::SetSize(D3DXVECTOR3 size)
{
	m_size = size;

	for (unsigned int nCount = 0; nCount < m_vNumber.size(); nCount++)
	{
		if (m_vNumber[nCount] != NULL)
		{
			m_vNumber[nCount]->SetSize(m_size);						// 大きさ設定
			m_vNumber[nCount]->SetTransform();
		}
	}
}