//=============================================================================
//
// オブジェクト処理 [object.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "object.h"
#include "manager.h"
#include "renderer.h"
#include "meshField.h"
#include "colliderBox.h"
#include "colliderSphere.h"
#include "camera.h"
#include "house.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define OBJECT_SCRIPT "data/text/object.txt"

//=============================================================================
// コンストラクタ
//=============================================================================
CObject::CObject(CScene::PRIORITY obj = CScene::PRIORITY_MODEL) : CSceneX(obj)
{
	SetObjType(CScene::PRIORITY_MODEL);

	m_ColliderBox = NULL;
	m_ColliderSphere = NULL;
}

//=============================================================================
// デストラクタ
//=============================================================================
CObject::~CObject()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CObject::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	// 初期化処理
	CSceneX::Init();

	D3DXVECTOR3 pos = D3DXVECTOR3(500.0f, 0.0f, 0.0f);

	SetPosition(pos);			// 位置の設定
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CObject::Uninit(void)
{
	if (m_ColliderBox != NULL)
	{// ボックスコライダーが存在していたとき
		m_ColliderBox->Release();			// 削除予約
		m_ColliderBox = NULL;				// NULLを代入
	}

	// 開放処理
	CSceneX::Uninit();
}

//=============================================================================
// 更新処理
//=============================================================================
void CObject::Update(void)
{
	D3DXVECTOR3 pos = GetPosition();				// 位置の取得

	if (m_ColliderBox != NULL)
	{// ボックスコライダーが存在していたとき
		m_ColliderBox->SetPosition(pos);			// 位置の設定
	}
	if (m_ColliderSphere != NULL)
	{// スフィアコライダーが存在していたとき
		m_ColliderSphere->SetPosition(pos);			// 位置の設定
	}

	SetPosition(pos);		// 位置の設定
#ifdef _DEBUG
	Debug();				// デバッグ処理
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CObject::Draw(void)
{
	CSceneX::Draw();
}

//=============================================================================
// クリエイト関数
//=============================================================================
CObject *CObject::Create(void)
{
	CObject *pObject;
	pObject = new CObject(CScene::PRIORITY_MODEL);

	if (pObject != NULL)
	{
		pObject->Init();
	}
	return pObject;
}

//=============================================================================
// ロード処理
//=============================================================================
HRESULT CObject::Load(void)
{
	return S_OK;
}

//=============================================================================
// テクスチャの解放関数
//=============================================================================
void CObject::Unload(void)
{
}

//=============================================================================
// オブジェクトアセットのロード処理
//=============================================================================
void CObject::LoadScript(void)
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
	pFile = fopen(OBJECT_SCRIPT, "r");

	if (pFile != NULL)
	{
		//ポインターのリセット
		nCntPointer = 0;

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
					if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
					{//パーツモデルのアドレス情報のとき
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//アドレスの取得
						Add = sAdd;
						CManager::LoadModel(Add);
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
}

//=============================================================================
// モデルの読込
//=============================================================================
void CObject::LoadModel(char *add)
{
	FILE *pFile = NULL;											// ファイル
	char cReadText[128] = {};									// 文字
	char cHeadText[128] = {};									// 比較
	CObject *pObject = NULL;

	pFile = fopen(add, "r");									// ファイルを開くまたは作る

	if (pFile != NULL)											// ファイルが読み込めた場合
	{
		fgets(cReadText, sizeof(cReadText), pFile);				// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);				// 行を飛ばす
		fgets(cReadText, sizeof(cReadText), pFile);				// 行を飛ばす

		while (strcmp(cHeadText, "End") != 0)
		{
			std::string Data = cReadText;
			std::vector<std::string> splitData = CManager::split(Data, ',');

			pObject = CObject::Create();
			pObject->BindModel(splitData[LOADTYPE_ADD]);
			pObject->SetPosition(D3DXVECTOR3((float)atof(splitData[LOADTYPE_POS_X].c_str()), (float)atof(splitData[LOADTYPE_POS_Y].c_str()), (float)atof(splitData[LOADTYPE_POS_Z].c_str())));			// 位置の設定
			pObject->SetRotation(D3DXVECTOR3((float)atof(splitData[LOADTYPE_ROT_X].c_str()), (float)atof(splitData[LOADTYPE_ROT_Y].c_str()), (float)atof(splitData[LOADTYPE_ROT_Z].c_str())));			// 回転値を設定

			fgets(cReadText, sizeof(cReadText), pFile);									// 行を飛ばす
			sscanf(cReadText, "%s", &cHeadText);
		}

		fclose(pFile);					// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "モデル情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// モデル読み込み(テスト)
//=============================================================================
void CObject::LoadModelTest(char *add)
{
	FILE *pFile = NULL;																	// ファイル
	char cReadText[128];															// 文字
	char cHeadText[128];															// 比較
	char cDie[128];

	pFile = fopen(add, "r");				// ファイルを開くまたは作る

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
					if (strcmp(cHeadText, "MODELSET") == 0)
					{//キャラクターの初期情報のとき
						CObject *pObj;
						pObj = CObject::Create();										// 床の作成
						OutputDebugString("モデル作成");

						if (pObj != NULL)
						{
							//エンドキャラクターセットが来るまでループ
							while (strcmp(cHeadText, "END_MODELSET") != 0)
							{
								fgets(cReadText, sizeof(cReadText), pFile);
								sscanf(cReadText, "%s", &cHeadText);

								if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
								{//パーツ総数のとき
									std::string add;
									char aModelAdd[64];
									memset(&add, 0, sizeof(add));

									sscanf(cReadText, "%s %s %s", &cDie, &cDie,
										&aModelAdd);

									// charをstringに
									add = aModelAdd;

									// 位置の設定
									pObj->BindModel(add);
								}
								else if (strcmp(cHeadText, "POS") == 0)
								{//パーツ総数のとき
									D3DXVECTOR3 pos;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&pos.x,
										&pos.y,
										&pos.z);

									// 位置の設定
									pObj->SetPosition(pos);
								}
								else if (strcmp(cHeadText, "ROT") == 0)
								{//パーツ総数のとき
									D3DXVECTOR3 rot;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&rot.x,
										&rot.y,
										&rot.z);

									// 位置の設定
									pObj->SetRotation(rot);
								}
								else if (strcmp(cHeadText, "SIZE") == 0)
								{//パーツ総数のとき
									D3DXVECTOR3 size;
									sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
										&size.x,
										&size.y,
										&size.z);

									// 位置の設定
									pObj->SetSize(size);
								}
							}
						}
					}
				}
			}
		}
		fclose(pFile);				// ファイルを閉じる
	}
	else
	{
		MessageBox(NULL, "地面情報のアクセス失敗！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}
}

//=============================================================================
// モデル情報の読込
//=============================================================================
void CObject::BindModel(std::string add)
{
	m_Add = add;

	// モデルの取得
	CManager::GetModelResource(add, m_pBuffMat, m_nNumMat, m_pMesh);

	// モデルの情報をセット
	CSceneX::BindModel(m_pMesh, m_nNumMat, m_pBuffMat);
}

//=============================================================================
// インスペクターを表示
//=============================================================================
void CObject::ShowInspector(void)
{
	CSceneX::ShowInspector();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CObject::Debug(void)
{

}
#endif