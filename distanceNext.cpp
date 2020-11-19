//=============================================================================
//
// 次の距離を求める処理 [distanceNext.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "distanceNext.h"
#include "manager.h"
#include "renderer.h"
#include "ui.h"
#include "number.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CDistanceNext::CDistanceNext()
{
	m_pUi = NULL;
	m_pos = D3DXVECTOR3_ZERO;
	m_distance = D3DXVECTOR3_ZERO;
	m_IntervalNum = D3DXVECTOR3_ZERO;
}

//=============================================================================
// デストラクタ
//=============================================================================
CDistanceNext::~CDistanceNext()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CDistanceNext::Init(void)
{
	// 初期化
	m_pUi = CUi::Create();

	if (m_pUi != NULL)
	{
		m_pUi->LoadScript("data/text/ui/TargetDistance.txt");
	}

	for (int nCount = 0; nCount < DISTANCE_MAXNUM; nCount++)
	{
		m_apNumber[nCount] = CNumber::Create();

		if (m_apNumber[nCount] != NULL)
		{
			m_apNumber[nCount]->SetSize(D3DXVECTOR3(50, 90, 0));						// 大きさ設定
			m_apNumber[nCount]->BindTexture("data/tex/number_rank.png");
		}
	}

	// 位置関係の設定
	SetTransform();
	SetNumber(0);
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CDistanceNext::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CDistanceNext::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CDistanceNext::Draw(void)
{

}

//==============================================================================
// 生成処理
//==============================================================================
CDistanceNext *CDistanceNext::Create(void)
{
	CDistanceNext *pDistanceNext;		// 背景のポインタ

	pDistanceNext = new CDistanceNext();		// 背景の生成
	if (pDistanceNext == NULL) { return NULL; }	// NULLだったら返す

	pDistanceNext->Init();				// 背景の初期化
	return pDistanceNext;
}

//==============================================================================
// アセットの生成処理
//==============================================================================
HRESULT CDistanceNext::Load(void)
{
	return S_OK;
}

//=============================================================================
// 位置の設定
//=============================================================================
void CDistanceNext::SetPosition(D3DXVECTOR3 &pos)
{
	m_pos = pos;
	SetTransform();
}

//=============================================================================
// 差分の設定
//=============================================================================
void CDistanceNext::SetDistance(D3DXVECTOR3 &distance)
{
	m_distance = distance;
	SetTransform();
}

//=============================================================================
// 数字どうしの距離
//=============================================================================
void CDistanceNext::SetIntervalNum(D3DXVECTOR3 & interval)
{
	m_IntervalNum = interval;
	SetTransform();
}

//=============================================================================
// 位置関係の更新
//=============================================================================
void CDistanceNext::SetTransform(void)
{
	if (m_pUi != NULL)
	{
		m_pUi->SetPosition(m_pos);
	}

	for (int nCount = 0; nCount < DISTANCE_MAXNUM; nCount++)
	{
		if (m_apNumber[nCount] != NULL)
		{
			m_apNumber[nCount]->SetPosition((m_distance + m_pos) + m_IntervalNum * (float)nCount);
			m_apNumber[nCount]->SetTransform();
		}
	}
}

//=============================================================================
// タイムの変更
//=============================================================================
void CDistanceNext::SetNumber(int nValue)
{
	int nNumber;
	int nLength = CManager::LengthCalculation(nValue);

	// 最大桁数までカウント
	for (int nCount = 0; nCount < DISTANCE_MAXNUM; nCount++)
	{
		// 表示する数字計算
		nNumber = nValue % (int)powf(10.0f, (float)DISTANCE_MAXNUM - nCount) / (int)powf(10.0f, DISTANCE_MAXNUM - 1.0f - nCount);

		// 数字設定
		m_apNumber[nCount]->SetNumber(nNumber);

		if (DISTANCE_MAXNUM - nCount <= nLength)
		{// 最大桁数より小さい桁だったとき
			if (!m_apNumber[nCount]->GetActive())
			{
				m_apNumber[nCount]->SetActive(true);
			}
		}
		else
		{// 最大桁数より大きい桁だったとき
			if (m_apNumber[nCount]->GetActive())
			{
				m_apNumber[nCount]->SetActive(false);
			}
		}
	}
}