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
#include "object.h"
#include "player.h"
#include "game.h"
#include "takaseiLibrary.h"

//==============================================================================
// マクロ定義
//==============================================================================
#define DIVISION (0.00622f)

//==============================================================================
// コンストラクタ
//==============================================================================
CDistanceNext::CDistanceNext()
{
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
	CUi *pUi = CUi::Create();

	if (pUi != NULL)
	{
		pUi->LoadScript("data/text/ui/TargetDistance.txt");
		pUi->SetPosition(D3DXVECTOR3(200.0f, 80.0f, 0.0f));
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

	// 最大人数までカウント
	for (int nCnt = 0; nCnt < MAX_PLAYER; nCnt++)
	{
		m_nRank[nCnt] = 0;
	}

	// 位置関係の設定
	SetTransform();
	SetNumber(0);

	m_nNowRound = 0;
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
	CNetwork *pNetwork = CManager::GetNetwork();				// ネットワークの取得
	std::vector<CObject*> pointObj = CObject::GetPointObj();	// オブジェクト情報取得
	unsigned int pointNum = CObject::GetPointNum();				// 現在のポイント番号取得
	D3DXVECTOR3 distance;										// 二点間の差
	CPlayer *pPlayer = CGame::GetPlayer();						// プレイヤー情報取得
	D3DXVECTOR3 Target = D3DXVECTOR3_ZERO;						// 敵の位置
	int nRound = 0;												// 敵の周回回数
	unsigned int nFlag = 0;												// 敵のフラグ番号
	float fDistance = 0.0f;										// 距離

	// 現在の順位取得
	int nRank = pNetwork->GetRank(pNetwork->GetId());

	// 順位が2位以下の時
	if (nRank > 1)
	{
		// 最大人数までカウント
		for (int nCnt = 0; nCnt < MAX_PLAYER; nCnt++)
		{
			if(nCnt == pNetwork->GetId()) { continue; }

			// 順位より上の順位取得
			if (pNetwork->GetRank(nCnt) == nRank - 1)
			{
				Target = pNetwork->GetPosition(nCnt);	// 敵の位置取得
				nFlag = pNetwork->GetFlag(nCnt);		// 敵のフラグ番号取得
				nRound = pNetwork->GetRound(nCnt);		// 敵の周回数取得
				break;
			}
		}

		//// 周回遅れの時
		//if (nRound > 0)
		//{
		//	// 最大チェックポイントまでカウント
		//	for (int nCnt = 0; nCnt < pointObj.size(); nCnt++)
		//	{
		//
		//	}
		//}

		//if(nFlag < 0) { return; }

		//if (pNetwork->GetFlag(pNetwork->GetId()) != nFlag)
		//{
		//	if (pointNum < pointObj.size() - 1)
		//	{
		//		// プレイヤーと次のチェックポイントの距離計算
		//		fDistance = CTakaseiLibrary::OutputDistance(pointObj[pointNum + 1]->GetPosition(), pPlayer->GetPosition());

		//		// 敵までのチェックポイントの数まで加算
		//		for (int nCnt = pointNum; nCnt < nFlag; nCnt++)
		//		{
		//			// 距離計算
		//			fDistance = fDistance + CTakaseiLibrary::OutputDistance(pointObj[nCnt]->GetPosition(), pointObj[nCnt + 1]->GetPosition());
		//		}

		//		// 敵と前のチェックポイントの距離計算
		//		fDistance = fDistance + CTakaseiLibrary::OutputDistance(pointObj[nFlag]->GetPosition(), Target);
		//	}
		//}
		//else
		//{
		//	// 敵と前のチェックポイントの距離計算
		//	fDistance = fDistance + CTakaseiLibrary::OutputDistance(pPlayer->GetPosition(), Target);
		//}

		float fPlayerLength = CalculateCourseRange(3, pointNum, pPlayer->GetPosition());

		float fTargetLength = CalculateCourseRange(3, nFlag, Target);

		fDistance = fPlayerLength - fTargetLength;

		fDistance *= DIVISION;

		if (fDistance > 999)
		{
			fDistance = 999;
		}

		// 数字設定
		SetNumber((int)fDistance);
	}
	else
	{
		// ゼロにする
		SetNumber(0);
	}
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
// 現在の周回回数に加算する
//=============================================================================
void CDistanceNext::SetNowRound(void)
{
	m_nNowRound++;
}

//=============================================================================
// 位置関係の更新
//=============================================================================
void CDistanceNext::SetTransform(void)
{
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

//=============================================================================
// コースの残り距離を求める処理
//=============================================================================
float CDistanceNext::CalculateCourseRange(int nRound, unsigned int &nPointNum, D3DXVECTOR3 &pos)
{
	std::vector<CObject*> pointObj = CObject::GetPointObj();	// オブジェクト情報取得
	float fDistance = 0.0f;										// 距離

	// 周回回数
	for (int nCount = 0; nCount < MAX_ROUND - (nRound + 1); nCount++)
	{
		for (unsigned int nCntCource = 1; nCntCource < pointObj.size(); nCntCource++)
		{
			// プレイヤーと次のチェックポイントの距離計算
			fDistance += CTakaseiLibrary::OutputDistance(pointObj[nCntCource]->GetPosition(), pointObj[nCntCource - 1]->GetPosition());
		}
	}

	// 残りチェックポイント数分
	for (unsigned int nCount = nPointNum; nCount < pointObj.size() - 1; nCount++)
	{
		// プレイヤーと次のチェックポイントの距離計算
		fDistance += CTakaseiLibrary::OutputDistance(pointObj[nCount]->GetPosition(), pointObj[nCount + 1]->GetPosition());
	}

	// プレイヤーと次のチェックポイントの距離計算
	fDistance += CTakaseiLibrary::OutputDistance(pointObj[nPointNum]->GetPosition(), pos);

	return fDistance;
}