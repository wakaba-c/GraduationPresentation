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
#include "write.h"

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
CUi *CUi::Create(void)
{
	CUi *pUi;		// 背景のポインタ

	pUi = new CUi;		// 背景の生成

	if(pUi == NULL) { return NULL; }
	pUi->Init();							// 背景の初期化

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
							scene->BindTexture(Add);
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

//==============================================================================
// テクスチャ生成
//==============================================================================
void CUi::CreateTexture(std::string Add)
{
	CScene2D *pScene2D = CScene2D::Create(CScene2D::PRIORITY_UI);

	if (pScene2D != NULL)
	{
		pScene2D->BindTexture(Add);			// テクスチャの設定
		if (pScene2D->GetActive())
		{
			pScene2D->SetActive(true);
		}
		m_Scene.push_back(pScene2D);								// 最後尾に入れる
	}
}

//==============================================================================
// テクスチャ削除
//==============================================================================
void CUi::DeleteTexture(int nIndex)
{
	if (m_Scene[nIndex] == NULL) { return; }			// 中身が存在していないとき

	m_Scene[nIndex]->Release();			// 削除予約
	m_Scene[nIndex] = NULL;				// NULLを代入
	m_Scene.erase(m_Scene.begin() + nIndex);	// 指定要素を削除
}

//==============================================================================
// デバッグ調整処理
//==============================================================================
void CUi::SceneDebug(void)
{
	for (unsigned int nCount = 0; nCount < m_Scene.size(); nCount++)
	{
		char aTag[16];
		memset(&aTag, 0, sizeof(aTag));
		sprintf(aTag, "TEXTURE [%d]", nCount);
		D3DXVECTOR3 pos, rot, size;
		bool bActive;

		if (ImGui::CollapsingHeader(aTag))
		{
			pos = m_Scene[nCount]->GetPosition();
			rot = m_Scene[nCount]->GetRotation();
			size = m_Scene[nCount]->GetSize();
			bActive = m_Scene[nCount]->GetActive();

			ImGui::DragFloat3("pos", (float*)&pos);
			ImGui::DragFloat3("rot", (float*)&rot);
			ImGui::DragFloat3("size", (float*)&size);

			m_Scene[nCount]->SetPosition(pos);
			m_Scene[nCount]->SetRotation(rot);
			m_Scene[nCount]->SetSize(size);

			if (bActive != m_Scene[nCount]->GetActive())
			{
				m_Scene[nCount]->SetActive(bActive);
			}

			m_Scene[nCount]->SetTransform();

			if (ImGui::Button("delete"))
			{
				DeleteTexture(nCount);
			}
		}
	}
}

//==============================================================================
// スクリプトの書き出し処理
//==============================================================================
void CUi::SaveScript(std::string Add)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	//変数宣言
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;

	std::string Full = "data/text/";
	Full += Add;
	Full += ".txt";

	//開けた
	if (pWrite->Open(Full))
	{
		strcpy(text, "# UIデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");			// スクリプトの終了宣言
		pWrite->Write("\n");

		// モデルの情報 //
		pWrite->IndexWrite("UIの情報\n");

		D3DXVECTOR3 pos, rot, size;

		for (unsigned int nCount = 0; nCount < m_Scene.size(); nCount++)
		{
			pos = m_Scene[nCount]->GetPosition();
			rot = m_Scene[nCount]->GetRotation();
			size = m_Scene[nCount]->GetSize();

			pWrite->Write("UISET\n");					// 頂点情報の書き込み開始宣言
			pWrite->Write("	TEXTURE_FILENAME = %s\n", m_Scene[nCount]->GetAdd().c_str());
			pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);		// 中心位置の書き込み
			pWrite->Write("	ROT = %.2f %.2f %.2f\n", rot.x, rot.y, rot.z);		// 中心位置の書き込み
			pWrite->Write("	SIZE = %.2f %.2f %.2f\n", size.x, size.y, size.z);		// 中心位置の書き込み
			pWrite->Write("END_UISET\n");				// 頂点情報の書き込み開始宣言
			pWrite->Write("\n");							// 改行
		}

		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();

		MessageBox(NULL, "当たり判定の書き込み終了しました！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}