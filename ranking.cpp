//=============================================================================
//
// ランキング処理 [ranking.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "ranking.h"
#include "fade.h"
#include "manager.h"
#include "scene.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "bg.h"
#include "number.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define TEXTFILE_RANKING	"data/stage/RANKING.txt"		//ランキングのデータ読み込み

//=============================================================================
// 静的メンバ変数
//=============================================================================
int CRanking::m_nResult = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CRanking::CRanking()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CRanking::~CRanking()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CRanking::Init(void)
{
	CBg::Load();				// 背景の読み込み
	CBg::Create();				// 背景の作成

	// =============== タイトル ================ //
	CScene2D *pScene2D = CScene2D::Create(CScene::PRIORITY_UI);

	if (pScene2D != NULL)
	{// pScene2Dが存在していたとき
		pScene2D->BindTexture("data/tex/ranking.png");		// 画像の読み込み
		pScene2D->SetPosition(D3DXVECTOR3(300.0f, 100.0f, 0.0f));					// 位置設定
		pScene2D->SetSize(D3DXVECTOR3(500.0f, 100.0f, 0.0f));						// 大きさ設定
		pScene2D->SetTransform();													// 頂点情報の更新
	}

	// ランキングを読み込む
	LoadRanking();

	// ランキングを整理する
	ConvertRanking(m_nResult);

	// ランキングを保存する
	SaveRanking();

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CRanking::Uninit(void)
{
	//ポリゴンの開放
	CScene::ReleaseAll();
}

//=============================================================================
// 更新処理
//=============================================================================
void CRanking::Update(void)
{
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	CInputController *pInputController = CManager::GetInputController();

	if (CFade::GetFade() == CFade::FADE_NONE)
	{//フェードが処理をしていないとき
		if (pInputKeyboard != NULL)
		{// キーボードが存在していたとき
			if (pInputKeyboard->GetTriggerKeyboard(DIK_RETURN))
			{// 指定のキーが押されたとき
				CFade::SetFade(CManager::MODE_TITLE);					//フェードを入れる
			}
		}
		if (pInputController->GetJoypadUse(0))
		{// コントローラーが生成されているとき
		 //ゲームの遷移
			if (pInputController->GetControllerTrigger(0, JOYPADKEY_A) ||			// ゲームパッドのAボダンが押されたとき
				pInputController->GetControllerTrigger(0, JOYPADKEY_START))			// ゲームパッドのSTARTボタンが押されたとき
			{
				CFade::SetFade(CManager::MODE_TITLE);					//フェードを入れる
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CRanking::Draw(void)
{

}

//=============================================================================
// 結果の設定
//=============================================================================
void CRanking::SetResultIndex(int nScore)
{
	m_nResult = nScore;
}

//=============================================================================
// 結果の整理処理
//============================================================================
void CRanking::ConvertRanking(int nNumber)
{
	int nCount, nCountA;
	int nNum01;
	m_aScore[MAX_RANK - 1] = nNumber;

	//データのソートを実行
	for (nCount = 0; nCount < MAX_RANK; nCount++)
	{
		for (nCountA = 0; nCountA < MAX_RANK - nCount; nCountA++)
		{
			if (m_aScore[nCountA] < m_aScore[nCountA + 1])
			{
				nNum01 = m_aScore[nCountA];					//小さかった数字を入れ替える
				m_aScore[nCountA] = m_aScore[nCountA + 1];	//大きかった数字を格納
				m_aScore[nCountA + 1] = nNum01;				//小さい数字を入れる
			}
		}
	}
}

//=============================================================================
// 結果の保存処理
//=============================================================================
void CRanking::SaveRanking(void)
{
	FILE *pFile;
	int nCount;

	pFile = fopen(TEXTFILE_RANKING, "w");
	if (pFile != NULL)
	{
		for (nCount = 0; nCount < 5; nCount++)
		{
			//データの保存
			fprintf(pFile, "第%d位\n", nCount + 1);
			fprintf(pFile, "%d\n", m_aScore[nCount]);
		}
		fclose(pFile);
	}
}

//=============================================================================
// 結果の読み込み処理
//=============================================================================
void CRanking::LoadRanking(void)
{
	FILE *pFile;
	int nCount;
	char x[128];

	pFile = fopen(TEXTFILE_RANKING, "r");
	if (pFile != NULL)
	{
		for (nCount = 0; nCount < 5; nCount++)
		{
			//データのロード
			fscanf(pFile, "%s", &x[0]);
			fscanf(pFile, "%d", &m_aScore[nCount]);
		}
		fclose(pFile);
	}
}