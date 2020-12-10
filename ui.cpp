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
#include "counter.h"

//==============================================================================
// コンストラクタ
//==============================================================================
CUi::CUi(CScene::PRIORITY obj = CScene::PRIORITY_UI) : CScene(obj)
{
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_currentPos = D3DXVECTOR3_ZERO;
	m_currentCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_nCurrentFrame = 0;
	m_nCurrentPoint = 0;
	m_nOpTypeNow = 0;

	m_vAsset = {};
	m_vOpType = {};
	m_vPoint = {};
	m_CntMap = {};
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
	Delete();
}

//=============================================================================
// 更新処理
//=============================================================================
void CUi::Update(void)
{
	if (m_vOpType.size() > m_nOpTypeNow)
	{
		switch (m_vOpType[m_nOpTypeNow])
		{
		case OPERATIONTYPE_MOVE:
			PointMove();
			break;
		case OPERATIONTYPE_DELETE:
			Delete();
			break;
		}
	}
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

	pUi = new CUi(CScene::PRIORITY_UI);		// 背景の生成

	if (pUi == NULL) { return NULL; }
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
					m_vAsset.push_back(scene);

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
				else if (strcmp(cHeadText, "COUNTERSET") == 0)
				{//カウンター情報のとき
				 // 作成
					std::string Add = {};
					std::string Tag = {};
					D3DXVECTOR3 pos = D3DXVECTOR3_ZERO;
					D3DXVECTOR3 size = D3DXVECTOR3_ZERO;
					D3DXVECTOR3 interval = D3DXVECTOR3_ZERO;
					int nLength = 1;

					while (strcmp(cHeadText, "END_COUNTERSET") != 0)
					{
						fgets(cReadText, sizeof(cReadText), pFile);
						sscanf(cReadText, "%s", &cHeadText);

						if (strcmp(cHeadText, "TEXTURE_FILENAME") == 0)
						{// アドレス情報のとき
							sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
							Add = sAdd;
						}
						else if (strcmp(cHeadText, "TAG") == 0)
						{//位置
							sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
							Tag = sAdd;
						}
						else if (strcmp(cHeadText, "LENGTH") == 0)
						{//位置
							sscanf(cReadText, "%s %s %d", &cDie, &cDie,
								&nLength);
						}
						else if (strcmp(cHeadText, "POS") == 0)
						{//位置
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&pos.x,
								&pos.y,
								&pos.z);
						}
						else if (strcmp(cHeadText, "SIZE") == 0)
						{//回転
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&size.x,
								&size.y,
								&size.z);
						}
						else if (strcmp(cHeadText, "INTERVAL") == 0)
						{//回転
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&interval.x,
								&interval.y,
								&interval.z);
						}
					}

					CCounter *pCounter = CCounter::Create(nLength);

					if (pCounter != NULL)
					{
						pCounter->BindTexture(Add);
						pCounter->SetSize(size);	// 大きさ の設定
						pCounter->SetPosition(pos);	// 位置 の設定
						pCounter->SetIntervalNum(interval);
						pCounter->SetNumber(35);
						pCounter->SetTransform();		// 頂点情報 の設定

						// マップに入れる
						m_CntMap.insert(std::map<std::string, CCounter*>::value_type(Tag, pCounter));
					}
				}
				else if (strcmp(cHeadText, "TASKSET") == 0)
				{// タスク生成
					MOVEPOINT point;
					int nType = -1;

					while (strcmp(cHeadText, "END_TASKSET") != 0)
					{
						fgets(cReadText, sizeof(cReadText), pFile);
						sscanf(cReadText, "%s", &cHeadText);

						if (strcmp(cHeadText, "TYPE") == 0)
						{// アドレス情報のとき
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nType);						//アドレスの取得
							m_vOpType.push_back((OPERATIONTYPE)nType);
						}
						else if (strcmp(cHeadText, "POS") == 0)
						{//位置
							D3DXVECTOR3 pos = D3DXVECTOR3_ZERO;
							sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
								&pos.x,
								&pos.y,
								&pos.z);

							// 位置情報を設定する
							point.pos = pos;
						}
						else if (strcmp(cHeadText, "COL") == 0)
						{//回転
							D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
							sscanf(cReadText, "%s %s %f %f %f %f", &cDie, &cDie,
								&col.r,
								&col.g,
								&col.b,
								&col.a);

							// 色情報を設定する
							point.col = col;
						}
						else if (strcmp(cHeadText, "FRAME") == 0)
						{//回転
							int nFrame = 0;
							sscanf(cReadText, "%s %s %d", &cDie, &cDie,
								&nFrame);

							// フレーム情報を設定する
							point.nFrame = nFrame;
						}
					}

					if (nType == OPERATIONTYPE_MOVE)
					{
						// 情報を格納する
						m_vPoint.push_back(point);
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
		m_vAsset.push_back(pScene2D);								// 最後尾に入れる
	}
}

//==============================================================================
// テクスチャ削除
//==============================================================================
void CUi::DeleteTexture(int nIndex)
{
	if (m_vAsset[nIndex] == NULL) { return; }			// 中身が存在していないとき

	m_vAsset[nIndex]->Release();			// 削除予約
	m_vAsset[nIndex] = NULL;				// NULLを代入
	m_vAsset.erase(m_vAsset.begin() + nIndex);	// 指定要素を削除
}

//==============================================================================
// デバッグ調整処理
//==============================================================================
void CUi::SceneDebug(void)
{
	for (unsigned int nCount = 0; nCount < m_vAsset.size(); nCount++)
	{
		char aTag[16];
		memset(&aTag, 0, sizeof(aTag));
		sprintf(aTag, "TEXTURE [%d]", nCount);
		D3DXVECTOR3 pos, rot, size;
		bool bActive;

		if (ImGui::CollapsingHeader(aTag))
		{
			pos = m_vAsset[nCount]->GetPosition();
			rot = m_vAsset[nCount]->GetRotation();
			size = m_vAsset[nCount]->GetSize();
			bActive = m_vAsset[nCount]->GetActive();

			ImGui::DragFloat3("pos", (float*)&pos);
			ImGui::DragFloat3("rot", (float*)&rot);
			ImGui::DragFloat3("size", (float*)&size);

			m_vAsset[nCount]->SetPosition(pos);
			m_vAsset[nCount]->SetRotation(rot);
			m_vAsset[nCount]->SetSize(size);

			if (bActive != m_vAsset[nCount]->GetActive())
			{
				m_vAsset[nCount]->SetActive(bActive);
			}

			m_vAsset[nCount]->SetTransform();

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

	D3DXVECTOR3 pos = GetPosition();

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

		for (unsigned int nCount = 0; nCount < m_vAsset.size(); nCount++)
		{
			pos = m_vAsset[nCount]->GetPosition() - pos;
			rot = m_vAsset[nCount]->GetRotation();
			size = m_vAsset[nCount]->GetSize();

			pWrite->Write("UISET\n");					// 頂点情報の書き込み開始宣言
			pWrite->Write("	TEXTURE_FILENAME = %s\n", m_vAsset[nCount]->GetAdd().c_str());
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

//==============================================================================
// UIの位置
//==============================================================================
void CUi::SetPosition(D3DXVECTOR3 pos)
{
	D3DXVECTOR3 assetPos = D3DXVECTOR3_ZERO;
	D3DXVECTOR3 thisPos = GetPosition();

	for (unsigned int nCount = 0; nCount < m_vAsset.size(); nCount++)
	{
		if (m_vAsset[nCount] == NULL) continue;				// ファイルが無ければ次へ

		assetPos = m_vAsset[nCount]->GetPosition() - thisPos;				// 前の位置を取得
		m_vAsset[nCount]->SetPosition(assetPos + pos);			// 新しい位置に置く
		m_vAsset[nCount]->SetTransform();
	}

	for (auto itr = m_CntMap.begin(); itr != m_CntMap.end(); itr++)
	{
		if (itr->second == NULL) continue;				// ファイルが無ければ次へ

		assetPos = itr->second->GetPosition() - thisPos;	// 前の位置を取得
		itr->second->SetPosition(assetPos + pos);			// 新しい位置に置く
		itr->second->SetTransform();

	}

	CScene::SetPosition(pos);
}

//==============================================================================
// UIの色
//==============================================================================
void CUi::SetColor(D3DXCOLOR col)
{
	for (unsigned int nCount = 0; nCount < m_vAsset.size(); nCount++)
	{
		if (m_vAsset[nCount] == NULL) continue;				// ファイルが無ければ次へ

		m_vAsset[nCount]->SetColor(col);			// 新しい位置に置く
		m_vAsset[nCount]->SetTransform();
	}
}

//==============================================================================
// 次の処理に移行
//==============================================================================
void CUi::SetNextOperation(void)
{
	m_nOpTypeNow++;
}

//==============================================================================
// UIの値移動
//==============================================================================
CCounter *CUi::GetCounter(std::string Tag)
{
	std::map<std::string, CCounter*>::const_iterator it = m_CntMap.find(Tag);

	if (it == m_CntMap.end())
	{// 見つからなかったとき
		return NULL;
	}
	else
	{
		return it->second;
	}
}

//==============================================================================
// UIの値移動
//==============================================================================
void CUi::PointMove(void)
{
	D3DXVECTOR3 pos = GetPosition();

	// 位置
	if (m_nCurrentFrame == 0)
	{
		m_currentPos = (m_vPoint[m_nCurrentPoint].pos - pos) / (float)m_vPoint[m_nCurrentPoint].nFrame;
		m_currentCol = (m_vPoint[m_nCurrentPoint].col - m_col) / (float)m_vPoint[m_nCurrentPoint].nFrame;
	}

	pos += m_currentPos;
	m_col += m_currentCol;

	SetPosition(pos);
	SetColor(m_col);

	m_nCurrentFrame++;

	if (m_vPoint[m_nCurrentPoint].nFrame <= m_nCurrentFrame)
	{
		// タスクを次へ
		m_nOpTypeNow++;
		m_nCurrentPoint++;
		m_nCurrentFrame = 0;
	}
}

//==============================================================================
// 削除
//==============================================================================
void CUi::Delete(void)
{
	for (unsigned int nCount = 0; nCount < m_vAsset.size(); nCount++)
	{
		if (m_vAsset[nCount] == NULL) continue;				// ファイルが無ければ次へ

		// 開放処理
		m_vAsset[nCount]->Release();
		m_vAsset[nCount] = NULL;
	}

	// 配列の要素を削除
	m_vAsset.clear();

	// 削除予約
	Release();
}