//=============================================================================
//
// ピースセレクト処理 [pieceSelect.cpp]
// Author : Ryouma Inoue
//
//=============================================================================
#include "pieceSelect.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "takaseiLibrary.h"
#include "box.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define	PIECE_01 100		// 車01のモデル情報アドレス
#define PIECE_FILE "data/text/piece.txt"

//==============================================================================
// コンストラクタ
//==============================================================================
CPieceSelect::CPieceSelect(PRIORITY type = CScene::PRIORITY_UI) :CScene(type)
{
	SetObjType(PRIORITY_BG);	//オブジェクトタイプの設定

}

//=============================================================================
// デストラクタ
//=============================================================================
CPieceSelect::~CPieceSelect()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CPieceSelect::Init(void)
{
	// 初期化
	m_nSelectCnt = 0;
	m_bPiece = false;

	for (int nCnt = 0; nCnt < MAX_CORE; nCnt++)
	{
		m_pPieceSelect[nCnt] = CScene2D::Create(PRIORITY_UI);
		m_bSelect[nCnt] = false;
	}

	LoadPiece();
	//SetPiece(PIECETYPE_CORE_00, D3DXVECTOR3(500.0f, 500.0f, 0.0f), D3DXVECTOR3(100.0f, 100.0f, 0.0f), "data/tex/core.png");

	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CPieceSelect::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CPieceSelect::Update(void)
{
	// キーボード取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	m_bPiece = CBox::GetPiece();

	if (m_bPiece == false)
	{
		// Zを押されたら
		if (pKeyboard->GetTriggerKeyboard(DIK_Z))
		{
			switch (m_nSelectCnt)
			{
			case 0:
				m_bSelect[PIECETYPE_CORE_16] = false;
				m_bSelect[PIECETYPE_CORE_01] = true;
				m_nSelectCnt++;
				break;
			case 1:
				m_bSelect[PIECETYPE_CORE_01] = false;
				m_bSelect[PIECETYPE_CORE_04] = true;
				m_nSelectCnt++;
				break;
			case 2:
				m_bSelect[PIECETYPE_CORE_04] = false;
				m_bSelect[PIECETYPE_CORE_12] = true;
				m_nSelectCnt++;
				break;
			case 3:
				m_bSelect[PIECETYPE_CORE_12] = false;
				m_bSelect[PIECETYPE_CORE_00] = true;
				m_nSelectCnt++;
				break;
			case 4:
				m_bSelect[PIECETYPE_CORE_00] = false;
				m_bSelect[PIECETYPE_CORE_05] = true;
				m_nSelectCnt++;
				break;
			case 5:
				m_bSelect[PIECETYPE_CORE_05] = false;
				m_bSelect[PIECETYPE_CORE_08] = true;
				m_nSelectCnt++;
				break;
			case 6:
				m_bSelect[PIECETYPE_CORE_08] = false;
				m_bSelect[PIECETYPE_CORE_14] = true;
				m_nSelectCnt++;
				break;
			case 7:
				m_bSelect[PIECETYPE_CORE_14] = false;
				m_bSelect[PIECETYPE_CORE_15] = true;
				m_nSelectCnt++;
				break;
			case 8:
				m_bSelect[PIECETYPE_CORE_15] = false;
				m_bSelect[PIECETYPE_CORE_16] = true;
				m_nSelectCnt = 0;
				break;
			}
		}
	}

	for (int nCnt = 0; nCnt < MAX_CORE; nCnt++)
	{
		if (m_bSelect[nCnt] == true)
		{
			m_pPieceSelect[nCnt]->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			m_pPieceSelect[nCnt]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void CPieceSelect::Draw(void)
{

}

//==============================================================================
// 生成処理
//==============================================================================
CPieceSelect *CPieceSelect::Create(void)
{
	CPieceSelect *pPieceSelect;		// 背景のポインタ

	pPieceSelect = new CPieceSelect(CScene::PRIORITY_BG);		// 背景の生成
	pPieceSelect->Init();							// 背景の初期化
	return pPieceSelect;
}

//==============================================================================
// アセットの生成処理
//==============================================================================
HRESULT CPieceSelect::Load(void)
{
	//テクスチャの読み込み
	CManager::Load("data/tex/back.png");
	return S_OK;
}

//==============================================================================
// ピースの設定
//==============================================================================
void CPieceSelect::SetPiece(PIECETYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 size, std::string Add)
{
	m_pPieceSelect[type]->BindTexture(Add);									// テクスチャのポインタを渡す
	m_pPieceSelect[type]->SetCenter(CScene2D::TEXTUREVTX_CENTER);			// 中心の設定
	m_pPieceSelect[type]->SetSize(D3DXVECTOR3(size.x, size.y, 0.0f));		// 大きさの設定
	m_pPieceSelect[type]->SetPosition(D3DXVECTOR3(pos.x, pos.y, 0.0f));		// 位置の設定
	m_pPieceSelect[type]->SetTransform();
}

//==============================================================================
// ピースのロード
//==============================================================================
void CPieceSelect::LoadPiece(void)
{
	FILE *pFile = NULL;																// ファイル
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	char cDie[128];
	D3DXVECTOR3 pos;
	D3DXVECTOR3 size;
	char aAdd[64];
	int type;

	pFile = fopen(PIECE_FILE, "r");				// ファイルを開くまたは作る

	if (pFile != NULL)						//ファイルが読み込めた場合
	{
		//スクリプトが来るまでループ
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//スクリプトだったら
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//エンドスクリプトが来るまで
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//改行
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "PIECESET") == 0)
					{//キャラクターの初期情報のとき

							//エンドキャラクターセットが来るまでループ
						while (strcmp(cHeadText, "END_PIECESET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							
							if (strcmp(cHeadText, "POS") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&pos.x,
									&pos.y,
									&pos.z);
							}
							else if (strcmp(cHeadText, "TYPE") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %d", &cDie, &cDie,
									&type);
							}
							else if (strcmp(cHeadText, "SIZE") == 0)
							{//パーツ総数のとき
								sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
									&size.x,
									&size.y,
									&size.z);

							}
							else if (strcmp(cHeadText, "TEX") == 0)
							{//パーツ総数のとき
								memset(&aAdd, 0, sizeof(aAdd));

								sscanf(cReadText, "%s %s %s", &cDie, &cDie,
									&aAdd);
							}
						}
						SetPiece((PIECETYPE)type, pos, size, aAdd);
					}
				}

			}
		}
		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}