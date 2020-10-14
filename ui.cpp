//=============================================================================
//
// ユーザーインターフェース処理 [ui.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "ui.h"
#include "manager.h"
#include "renderer.h"
#include "scene2D.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CUi::CUi()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CUi::~CUi()
{

}

//==============================================================================
// 初期化処理
//==============================================================================
HRESULT CUi::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CUi::Uninit(void)
{
	for (unsigned int nCount = 0; nCount < m_Scene.size(); nCount++)
	{
		if (m_Scene[nCount] == NULL) continue;				// ファイルが無ければ次へ

		// 開放処理
		m_Scene[nCount]->Release();
		m_Scene[nCount] = NULL;
	}

	// 配列の要素を削除
	m_Scene.clear();
}

//=============================================================================
// 更新処理
//=============================================================================
void CUi::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CUi::Draw(void)
{

}

//==============================================================================
// 生成処理
//==============================================================================
CUi *CUi::Create(const std::string &add)
{
	CUi *pUi;		// 背景のポインタ

	pUi = new CUi;		// 背景の生成
	pUi->Init();							// 背景の初期化
	pUi->LoadScript(add);					// スクリプトの読み込み
	return pUi;
}

//==============================================================================
// アセットの生成処理
//==============================================================================
HRESULT CUi::Load(void)
{
	//テクスチャの読み込み
	return S_OK;
}

//==============================================================================
// スクリプトデータロード処理
//==============================================================================
bool CUi::LoadScript(const std::string &add)
{
	FILE *pFile;
	char cReadText[128];		//文字
	char cHeadText[128];		//比較
	char cDie[128];
	int nCntPointer = 0;		//ポインターの数値

	char sAdd[64];				//モデルのアドレス
	std::string Add;

	int nCntMotion = 0;			//参照するポインタの値を初期化
	int nCntKey = 0;

	int nMaxModel = 0;

	//テキストデータロード
	pFile = fopen(add.c_str(), "r");

	if (pFile != NULL)
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
				if (strcmp(cReadText, "\n") == 0)
				{
					continue;
				}
				else if (strcmp(cHeadText, "UISET") == 0)
				{//キャラクターの初期情報のとき
					// 作成
					CScene2D *scene = CScene2D::Create(CScene2D::PRIORITY_UI);

					scene->SetCenter(CScene2D::TEXTUREVTX_CENTER);		// 中心 の設定
					scene->SetSize(D3DXVECTOR3(100.0f, 70.0f, 0.0f));	// 大きさ の設定
					scene->SetRotation(D3DXVECTOR3(-0.79f, -0.79f, 0.0f));
					scene->SetPosition(D3DXVECTOR3(320.0f, 150.0f, 0.0f));	// 位置 の設定
					scene->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色 の設定
					scene->SetTransform();		// 頂点情報 の設定

					if (scene == NULL)
					{// 生成に失敗したとき
						return false;
					}

					// 配列の最後尾に入れる
					m_Scene.push_back(scene);

					while (strcmp(cHeadText, "END_UISET") != 0)
					{
						fgets(cReadText, sizeof(cReadText), pFile);
						sscanf(cReadText, "%s", &cHeadText);

						if (strcmp(cHeadText, "TEXTURE_FILENAME") == 0)
						{// アドレス情報のとき
							sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
							Add = sAdd;
							scene->BindTexture(CManager::GetResource(Add));
						}
						else if (strcmp(cHeadText, "POS") == 0)
						{//位置
							D3DXVECTOR3 pos;
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&pos.x,
								&pos.y,
								&pos.z);

							// 位置情報を設定する
							scene->SetPosition(pos);
							scene->SetTransform();
						}
						else if (strcmp(cHeadText, "ROT") == 0)
						{//回転
							D3DXVECTOR3 rot;
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&rot.x,
								&rot.y,
								&rot.z);

							// 回転情報を設定する
							scene->SetRotation(rot);
							scene->SetTransform();
						}
						else if (strcmp(cHeadText, "SIZE") == 0)
						{//回転
							D3DXVECTOR3 size;
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&size.x,
								&size.y,
								&size.z);

							// 回転情報を設定する
							scene->SetSize(size);
							scene->SetTransform();
						}
					}
				}
			}
		}

		//ファイル閉
		fclose(pFile);
	}
	else
	{

	}

	return false;
}