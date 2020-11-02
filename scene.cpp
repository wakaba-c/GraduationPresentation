//=============================================================================
//
// scene処理 [scene.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "scene.h"
#include "game.h"
#include "renderer.h"
#include "manager.h"
#include "light.h"
#include "meshField.h"
#include "meshCube.h"
#include "meshSphere.h"
#include "enemy.h"
#include "player.h"
#include "camera.h"
#include "debug.h"
#include "object.h"
#include "write.h"
#include "colliderBox.h"
#include "colliderSphere.h"
#include "wall.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
CScene *CScene::m_apTop[CScene::PRIORITY_MAX] = {};
CScene *CScene::m_apCur[CScene::PRIORITY_MAX] = {};
int CScene::m_nNumAll = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CScene::CScene(CScene::PRIORITY Type)
{
	if (m_apTop[Type] == NULL)
	{
		m_apTop[Type] = this;																	//自分をトップに設定
	}

	if (m_apCur[Type] != NULL)
	{
		m_apCur[Type]->m_pNext[Type] = this;													//最後尾の次を自分に
	}

	m_pPrev[Type] = m_apCur[Type];																//自分の前に最後尾を入れる
	m_apCur[Type] = this;																		//最後尾を自分にする
	m_pNext[Type] = NULL;																		//自分の次をNULL
	m_Obj = Type;																				//オブジェクトタイプを定義
	m_nNumAll++;																				//シーン数をプラスする
	m_bActive = true;
	m_bDie = false;

	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//=============================================================================
// デストラクタ
//=============================================================================
CScene::~CScene()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CScene::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CScene::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CScene::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CScene::Draw(void)
{

}

//====================================================================
// 全てのオブジェクトを破開放
//====================================================================
void CScene::ReleaseAll(void)
{
	CScene *pSceneNext = NULL;																	//次回削除対象
	CScene *pSceneNow = NULL;

	CCollider::ReleaseAll();

	for (int nCount = 0; nCount < PRIORITY_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//死亡フラグを確認
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];											//次回フラグ確認対象を控える

			pSceneNow->Delete();																//削除
			pSceneNow = NULL;

			pSceneNow = pSceneNext;																//次回フラグ確認対象を格納
		}
	}
}

//====================================================================
// 全てのオブジェクトを更新
//====================================================================
void CScene::UpdateAll(void)
{
	CScene *pSceneNext = NULL;																	//次回アップデート対象
	CScene *pSceneNow = NULL;

	for (int nCount = 0; nCount < PRIORITY_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];											// 次回アップデート対象を控える

			if (pSceneNow->GetActive())
			{
				pSceneNow->Update();																// アップデート
			}
			pSceneNow = pSceneNext;																// 次回アップデート対象を格納
		}
	}

	// 当たり判定
	CCollider::UpdateAll();

	for (int nCount = 0; nCount < PRIORITY_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//死亡フラグを確認
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[nCount];											// 次回フラグ確認対象を控える

			if (pSceneNow->m_bDie)
			{
				pSceneNow->Delete();															// 削除
				pSceneNow = NULL;																// NULLを代入
			}

			pSceneNow = pSceneNext;																// 次回フラグ確認対象を格納
		}
	}
}

//====================================================================
// 全てのオブジェクトを描画
//====================================================================
void CScene::DrawAll(void)
{
	CScene *pSceneNext = NULL;																	// 次回描画対象
	CScene *pSceneNow = NULL;
	for (int nCount = 0; nCount < PRIORITY_MAX; nCount++)
	{
		pSceneNow = m_apTop[nCount];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			CCamera *pCamera = CManager::GetCamera();
			pSceneNext = pSceneNow->m_pNext[nCount];											//次回描画対象を控える

			if (pSceneNow->GetActive())
			{
				pSceneNow->Draw();																	//描画
			}
			pSceneNow = pSceneNext;																//次回描画対象を格納
		}
	}
}

//====================================================================
// 自分の削除予約
//====================================================================
void CScene::Release(void)
{
	if (!m_bDie)
	{
		m_bDie = true;
	}
}

//====================================================================
// オブジェクトタイプの設定
//====================================================================
void CScene::SetObjType(PRIORITY obj)
{
	m_Obj = obj;
}

//====================================================================
// 更新描画対象設定
//====================================================================
void CScene::SetActive(bool bValue)
{
	m_bActive = bValue;
}

//=============================================================================
// 影の設定
//=============================================================================
void CScene::SetShadow(LPDIRECT3DDEVICE9 pDevice, D3DXMATRIX mtxWorld, LPD3DXBUFFER pBuffMat, DWORD nNumMat, LPD3DXMESH pMesh, D3DXVECTOR3 pos)
{
	D3DXMATRIX mtxShadow;
	D3DMATERIAL9 matShadow;
	D3DXPLANE planeField;
	D3DXVECTOR4 vecLight;
	D3DXVECTOR3 workPos, normal;

	D3DXMATERIAL	*pMat;							//現在のマテリアル保存用
	D3DMATERIAL9	matDef;							//マテリアルデータへのポインタ

	CLight *pLight = CManager::GetLight();
	D3DLIGHT9 light = pLight->GetLight(0);
	D3DXVECTOR3 lightDef = light.Direction;

	lightDef *= -1;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxShadow);

	vecLight = D3DXVECTOR4(lightDef.x, lightDef.y, lightDef.z, 0.0f);

	workPos = D3DXVECTOR3(pos.x, 85.0f, pos.z);
	normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXPlaneFromPointNormal(&planeField, &workPos, &normal);
	D3DXMatrixShadow(&mtxShadow, &vecLight, &planeField);

	// ポリゴンにあった高さ分影を計算する
	D3DXMatrixMultiply(&mtxShadow, &mtxWorld, &mtxShadow);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxShadow);					//すべてのデータ収録

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアル情報に対するポインタを取得
	pMat = (D3DXMATERIAL*)pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)nNumMat; nCntMat++)
	{
		matShadow = pMat[nCntMat].MatD3D;

		matShadow.Diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

		// マテリアルの設定
		pDevice->SetMaterial(&matShadow);

		// 描画
		pMesh->DrawSubset(nCntMat);
	}

	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);
}

//====================================================================
// 現在立っている床の面
//====================================================================
CScene *CScene::NowFloor(D3DXVECTOR3 pos)
{
	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;

	pSceneNow = m_apTop[PRIORITY_FLOOR];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[PRIORITY_FLOOR];							//次回アップデート対象を控える

		CMeshField *pField = (CMeshField*)pSceneNow;								// クラスチェンジ(床)

		if (pField->CollisionRange(pos))
		{// プレイヤー が床に乗っていたとき
			return pField;
		}

		pSceneNow = pSceneNext;														//次回アップデート対象を格納
	}

	return NULL;
}

//====================================================================
// 床の高さをリセット
//====================================================================
void CScene::ResetFloor(void)
{
	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;

	pSceneNow = m_apTop[PRIORITY_FLOOR];

	//次がなくなるまで繰り返す
	while (pSceneNow != NULL)
	{
		pSceneNext = pSceneNow->m_pNext[PRIORITY_FLOOR];							//次回アップデート対象を控える

		CMeshField *pField = (CMeshField*)pSceneNow;								// クラスチェンジ(床)

		if (pField != NULL)
		{// プレイヤー が床に乗っていたとき
			pField->Reset();
		}

		pSceneNow = pSceneNext;														//次回アップデート対象を格納
	}
}

//====================================================================
// 位置の取得
//====================================================================
void CScene::SetPosition(D3DXVECTOR3 pos)
{
	SetPosOld(m_pos);
	m_pos = pos;
}

//====================================================================
// 前回位置の取得
//====================================================================
void CScene::SetPosOld(D3DXVECTOR3 pos)
{
	m_posOld = pos;
}

//====================================================================
// シーンを取得
//====================================================================
CScene *CScene::GetScene(CScene::PRIORITY obj)
{
	return m_apTop[obj];
}

//====================================================================
// 次のシーンを取得
//====================================================================
CScene *CScene::GetSceneNext(CScene *pScene, PRIORITY type)
{
	return pScene->m_pNext[type];
}

//=============================================================================
// 当たり判定 トリガー
//=============================================================================
void CScene::OnTriggerEnter(CCollider *col)
{

}

//=============================================================================
// 当たり判定 コリジョン
//=============================================================================
void CScene::OnCollisionEnter(CCollider *col)
{

}

//=============================================================================
// インスペクターを表示
//=============================================================================
void CScene::ShowInspector(void)
{
	if (ImGui::BeginMenuBar())
	{// メニューバーの生成
		if (ImGui::BeginMenu("File"))
		{// ファイルタブの生成
			if (ImGui::MenuItem("Save"))
			{// 2D物理特性マテリアルメニュー
				SaveCollider();
			}

			ImGui::EndMenu();			// メニューの更新終了
		}
		if (ImGui::BeginMenu("Add Component"))
		{// ファイルタブの生成
			if (ImGui::BeginMenu("Physic 2D"))
			{// 2D物理特性マテリアルメニュー
				if (ImGui::MenuItem("Box Collider"))
				{// 四角形の当たり判定生成
					CCollider *pCollider = CColliderBox::Create(false, D3DXVECTOR3(550.0f, 120.0f, 50.0f));		// 生成

					if (pCollider != NULL)
					{// ボックスコライダーが存在していたとき
						pCollider->SetScene(this);									// 持ち主を設定
						pCollider->SetTag("none");									// fenceを設定
						pCollider->SetPosition(m_pos);						// 位置の設定
						pCollider->SetOffset(D3DXVECTOR3(0.0f, 60.0f, 0.0f));		// オフセット値の設定
						m_apCollider.push_back(pCollider);							// 作ったコライダーを配列の最後に追加する
					}
				}
				else if (ImGui::MenuItem("Circle Collider"))
				{// 円の当たり判定生成
				}
				else if (ImGui::MenuItem("RigitBody"))
				{// 物理演算の適応
				}

				ImGui::EndMenu();			// メニューの更新終了
			}

			if (ImGui::BeginMenu("Physic"))
			{// 物理特性マテリアルメニュー
				if (ImGui::MenuItem("Box Collider"))
				{// 立方体の当たり判定生成
					CCollider *pCollider = CColliderBox::Create(false, D3DXVECTOR3(550.0f, 120.0f, 50.0f));		// 生成

					if (pCollider != NULL)
					{// ボックスコライダーが存在していたとき
						pCollider->SetScene(this);									// 持ち主を設定
						pCollider->SetTag("none");									// fenceを設定
						pCollider->SetPosition(m_pos);								// 位置の設定
						pCollider->SetOffset(D3DXVECTOR3(0.0f, 60.0f, 0.0f));		// オフセット値の設定
						m_apCollider.push_back(pCollider);							// 作ったコライダーを配列の最後に追加する
					}
				}
				else if (ImGui::MenuItem("Sphere Collider"))
				{// 球体の当たり判定生成
					CCollider *pCollider = CColliderSphere::Create(false, 100.0f);		// 生成

					if (pCollider != NULL)
					{// ボックスコライダーが存在していたとき
						pCollider->SetScene(this);									// 持ち主を設定
						pCollider->SetTag("none");									// fenceを設定
						pCollider->SetPosition(m_pos);								// 位置の設定
						pCollider->SetOffset(D3DXVECTOR3(0.0f, 60.0f, 0.0f));		// オフセット値の設定
						m_apCollider.push_back(pCollider);							// 作ったコライダーを配列の最後に追加する
					}
				}
				else if (ImGui::MenuItem("RigitBody"))
				{// 物理演算の適応
				}

				ImGui::EndMenu();			// メニューの更新終了
			}
			ImGui::EndMenu();			// メニューの更新終了
		}
		ImGui::EndMenuBar();		// メニューバーの更新終了
	}

	std::string Title;

	for (int nCount = 0; nCount < (int)m_apCollider.size(); nCount++)
	{
		switch (m_apCollider[nCount]->GetColliderType())
		{
		case COLLISIONTYPE_BOX:
			Title = "Box Collider";
			Title += nCount;

			if (ImGui::CollapsingHeader(Title.c_str()))
			{
				CColliderBox *pColliderBox = (CColliderBox*)m_apCollider[nCount];

				// 衝突判定の有無
				bool bTrigger = pColliderBox->GetTrigger();
				ImGui::Checkbox("Trigger", &bTrigger);
				pColliderBox->SetTrigger(bTrigger);

				// オフセット値の設定
				D3DXVECTOR3 offset = pColliderBox->GetOffset();
				ImGui::DragFloat3("Center", (float*)&offset);
				pColliderBox->SetOffset(offset);

				// サイズの設定
				D3DXVECTOR3 size = pColliderBox->GetSize();
				ImGui::DragFloat3("Size", (float*)&size);
				pColliderBox->SetSize(size);
				ImGui::Text("");
			}
			break;
		case COLLISIONTYPE_SPHERE:
			Title = "Sphere Collider";
			Title += nCount;

			if (ImGui::CollapsingHeader(Title.c_str()))
			{
				CColliderSphere *pColliderSphere = (CColliderSphere*)m_apCollider[nCount];

				// 衝突判定の有無
				bool bTrigger = pColliderSphere->GetTrigger();
				ImGui::Checkbox("Trigger", &bTrigger);
				pColliderSphere->SetTrigger(bTrigger);

				// オフセット値の設定
				D3DXVECTOR3 offset = pColliderSphere->GetOffset();
				ImGui::DragFloat3("Center", (float*)&offset);
				pColliderSphere->SetOffset(offset);

				// サイズの設定
				float radius = pColliderSphere->GetRadius();
				ImGui::DragFloat("Radius", (float*)&radius);
				pColliderSphere->SetRadius(radius);
				ImGui::Text("");
			}
			break;
		}
	}
}

//====================================================================
// 自分の削除
//====================================================================
void CScene::Delete(void)
{
	if (this != NULL)
	{
		//終了処理及びメモリ解放
		this->Uninit();

		if (this->m_pPrev[m_Obj] == NULL)
		{//自分が先頭だったとき

			if (this->m_pNext[m_Obj] != NULL)
			{//自分の次のオブジェクトがあるとき
				m_apTop[m_Obj] = this->m_pNext[m_Obj];											//先頭を次のオブジェクトに譲渡する

				if (m_apTop[m_Obj]->m_pPrev[m_Obj] != NULL)
				{//前の人の情報がある場合
					m_apTop[m_Obj]->m_pPrev[m_Obj] = NULL;										//前にあるオブジェクト情報を NULL にする
				}
			}
			else
			{//自分の次のオブジェクトがなかったとき
				m_apTop[m_Obj] = NULL;
				m_apCur[m_Obj] = NULL;
			}
		}
		else if (this->m_pNext[m_Obj] == NULL)
		{//自分が最後尾だったとき
			m_apCur[m_Obj] = m_pPrev[m_Obj];													//最後尾を前のオブジェクトに譲渡する

			if (m_apCur[m_Obj]->m_pNext[m_Obj] != NULL)
			{
				m_apCur[m_Obj]->m_pNext[m_Obj] = NULL;											//後ろにあるオブジェクト情報を NULL にする
			}
		}
		else
		{//どちらでもない場合
			m_pNext[m_Obj]->m_pPrev[m_Obj] = m_pPrev[m_Obj];									//次のオブジェクトの前のオブジェクト情報に自分の前のオブジェクト情報を格納する
			m_pPrev[m_Obj]->m_pNext[m_Obj] = m_pNext[m_Obj];									//前のオブジェクトの次のオブジェクト情報に自分の次のオブジェクト情報を格納する
		}

		delete this;
		m_nNumAll--;																			//生成数を一つ減らす
	}
}

//=============================================================================
// スフィアコライダーの情報を書き込む処理
//=============================================================================
void CScene::WriteForSphereCollider(CWrite *pWrite ,CCollider *pCollider)
{
	CColliderSphere *pSphere = (CColliderSphere*)pCollider;
	float fRadius = pSphere->GetRadius();
	D3DXVECTOR3 center = pSphere->GetOffset();
	pWrite->Write("	CENTER = %.2f %.2f %.2f\n", center.x, center.y, center.z);			// 位置情報を書き込む
	pWrite->Write("	RADIUS = %.2f\n", fRadius);											// 大きさ情報を書き込む
}

//=============================================================================
// ボックスコライダーの情報を書き込む処理
//=============================================================================
void CScene::WriteForBoxCollider(CWrite *pWrite, CCollider *pCollider)
{
	CColliderBox *pBox = (CColliderBox*)pCollider;
	D3DXVECTOR3 size = pBox->GetSize();
	D3DXVECTOR3 center = pBox->GetOffset();
	pWrite->Write("	CENTER = %.2f %.2f %.2f\n", center.x, center.y, center.z);			// 位置情報を書き込む
	pWrite->Write("	SIZE = %.2f %.2f %.2f\n", size.x, size.y, size.z);		// 大きさ情報を書き込む
}

//=============================================================================
// 敵配置保存関数
//=============================================================================
void CScene::SaveEnemy(void)
{
	FILE *pFile;																	// ファイル
	char cWriteText[128];															// 文字として書き込み用
	CScene *pSceneNext = NULL;														//次回アップデート対象
	CScene *pSceneNow = NULL;
	D3DXVECTOR3 pos;

	pFile = fopen("data/stage/enemy.csv", "w");									// ファイルを開くまたは作る

	if (pFile != NULL)																// ファイルが読み込めた場合
	{
		sprintf(cWriteText, "enemy\n");
		fputs(cWriteText, pFile);													// 書き込み
		sprintf(cWriteText, "位置 X軸, Y軸, Z軸\n");
		fputs(cWriteText, pFile);													// 書き込み

		// treeのオブジェクトのポジションを参照
		pSceneNow = m_apTop[PRIORITY_ENEMY];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[PRIORITY_ENEMY];							//次回アップデート対象を控える
			CEnemy *pEnemy = (CEnemy*)pSceneNow;
			pos = pEnemy->GetPosition();
			sprintf(cWriteText, "%.2f, %.2f, %.2f\n", pos.x, pos.y, pos.z);
			fputs(cWriteText, pFile);												// 書き込み
			pSceneNow = pSceneNext;													//次回アップデート対象を格納
		}

		sprintf(cWriteText, "End");
		fputs(cWriteText, pFile);												// 書き込み

		//ファイル閉
		fclose(pFile);

		MessageBox(NULL, "敵情報の書込に成功！", "SUCCESS", MB_ICONASTERISK);
	}
	else
	{
		MessageBox(NULL, "敵ファイルのアクセス失敗！", "WARNING", MB_ICONWARNING);
	}
}

//=============================================================================
// 床起伏書き込み処理
//=============================================================================
void CScene::SaveRand(void)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	//変数宣言
	int nCntLoad = 0;			//ロードカウント
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;


	//開けた
	if (pWrite->Open("data/text/rand.txt"))
	{
		strcpy(text, "# シーンデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");			// スクリプトの終了宣言
		pWrite->Write("\n");

		// 床の情報 //
			// treeのオブジェクトのポジションを参照
		pSceneNow = m_apTop[PRIORITY_FLOOR];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[PRIORITY_FLOOR];						//次回アップデート対象を控える
			CMeshField *pMeshField = (CMeshField*)pSceneNow;

			if (!pMeshField->GetDebugRand())
			{// デバッグ用の床ではないとき
				pWrite->Write("FIELDSET\n");					// 頂点情報の書き込み開始宣言

				D3DXVECTOR3 pos = pMeshField->GetPosition();
				pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);		// 中心位置の書き込み

				pWrite->Write("	VERTEXINFO\n");					// 頂点情報の書き込み開始宣言
				pMeshField->SaveRand(pWrite);							// 頂点情報の書き込み
				pWrite->Write("	END_VERTEXINFO\n");				// 頂点情報の書き込み終了宣言
				pWrite->Write("END_FIELDSET\n");					// 頂点情報の書き込み開始宣言
				pWrite->Write("\n");							// 改行
			}

			pSceneNow = pSceneNext;													// 次回アップデート対象を格納
		}
		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();
	}
}

//=============================================================================
// モデル情報書き込み処理
//=============================================================================
void CScene::SaveModel(void)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	CWrite *pPointWrite = new CWrite;
	if (pPointWrite == NULL) return;

	if(!pPointWrite->Open("data/text/point.txt"))
	{
		return;
	}

	//変数宣言
	int nCntLoad = 0;			//ロードカウント
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;

	//開けた
	if (pWrite->Open("data/text/model.txt"))
	{
		strcpy(text, "# シーンデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");			// スクリプトの終了宣言
		pWrite->Write("\n");

		// モデルの情報 //
		pWrite->IndexWrite("モデルの情報\n");

		// objectのオブジェクトのポジションを参照
		pSceneNow = m_apTop[PRIORITY_MODEL];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[PRIORITY_MODEL];						//次回アップデート対象を控える
			CObject *pObject = (CObject*)pSceneNow;

			if (pObject->GetAdd() == "data/model/stick.x")
			{
				SavePoint(pPointWrite, pObject->GetPosition());
			}
			else
			{
				pWrite->Write("MODELSET\n");					// 頂点情報の書き込み開始宣言

				D3DXVECTOR3 pos = pObject->GetPosition();		// 位置情報を書き込む
				D3DXVECTOR3 rot = pObject->GetRotation();		// 回転情報を書き込む
				D3DXVECTOR3 size = pObject->GetSize();			// 大きさ情報を書き込む

				pWrite->Write("	MODEL_FILENAME = %s\n", pObject->GetAdd().c_str());		// アドレス情報を書き込む
				pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);			// 位置情報を書き込む
				pWrite->Write("	ROT = %.2f %.2f %.2f\n", rot.x, rot.y, rot.z);			// 回転情報を書き込む
				pWrite->Write("	SIZE = %.2f %.2f %.2f\n", size.x, rot.y, rot.z);		// 大きさ情報を書き込む

				pWrite->Write("END_MODELSET\n");				// 頂点情報の書き込み開始宣言
				pWrite->Write("\n");							// 改行
			}

			pSceneNow = pSceneNext;													// 次回アップデート対象を格納
		}
		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();
		pPointWrite->End();
	}

	delete pWrite;
	delete pPointWrite;
}

//=============================================================================
// 壁情報書き込み処理
//=============================================================================
void CScene::SaveWall(void)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	//変数宣言
	int nCntLoad = 0;			//ロードカウント
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;


	//開けた
	if (pWrite->Open("data/text/wall.txt"))
	{
		strcpy(text, "# シーンデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");			// スクリプトの終了宣言
		pWrite->Write("\n");

		// 壁の情報 //
		// treeのオブジェクトのポジションを参照
		pSceneNow = m_apTop[PRIORITY_WALL];

		//次がなくなるまで繰り返す
		while (pSceneNow != NULL)
		{
			pSceneNext = pSceneNow->m_pNext[PRIORITY_WALL];						//次回アップデート対象を控える
			CMeshWall *pMeshWall = (CMeshWall*)pSceneNow;

			if (!pMeshWall->GetDebugRand())
			{// デバッグ用の壁ではないとき
				pWrite->Write("FIELDSET\n");					// 頂点情報の書き込み開始宣言

				D3DXVECTOR3 pos = pMeshWall->GetPosition();
				pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);		// 中心位置の書き込み

				pWrite->Write("	VERTEXINFO\n");					// 頂点情報の書き込み開始宣言
				pMeshWall->SaveWall(pWrite);							// 頂点情報の書き込み
				pWrite->Write("	END_VERTEXINFO\n");				// 頂点情報の書き込み終了宣言
				pWrite->Write("END_FIELDSET\n");					// 頂点情報の書き込み開始宣言
				pWrite->Write("\n");							// 改行
			}

			pSceneNow = pSceneNext;													// 次回アップデート対象を格納
		}
		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();
	}
}

//=============================================================================
// ポイント情報書き込み処理
//=============================================================================
void CScene::SavePoint(CWrite *pWrite, D3DXVECTOR3 &pos)
{
	pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, 3000.0f, pos.z);		// 中心位置の書き込み
	pWrite->Write("	POS = %.2f %.2f %.2f\n", pos.x, -500.0f, pos.z);		// 中心位置の書き込み
}

//=============================================================================
// 当たり判定情報書き込み処理
//=============================================================================
void CScene::SaveCollider(void)
{
	CWrite *pWrite = new CWrite;
	if (pWrite == NULL) return;

	//変数宣言
	char text[64];				// テキスト
	char cEqual[8] = { "=" };	//イコール用
	CScene *pSceneNext = NULL;	//次回アップデート対象
	CScene *pSceneNow = NULL;

	//開けた
	if (pWrite->Open("data/text/collider.txt"))
	{
		strcpy(text, "# コライダーデータスクリプト\n");
		strcat(text, "# Author : masayasu wakita\n");

		pWrite->TitleWrite(text);				// タイトルの形式で書き込む
		pWrite->Write("SCRIPT\n");			// スクリプトの終了宣言
		pWrite->Write("\n");

		// モデルの情報 //
		pWrite->IndexWrite("モデルの情報\n");

		for (unsigned int nCount = 0; nCount < m_apCollider.size(); nCount++)
		{
			pWrite->Write("COLLISIONSET\n");					// 頂点情報の書き込み開始宣言

			// コライダーの種類を書き込む
			switch (m_apCollider[nCount]->GetColliderType())
			{
			case COLLISIONTYPE_BOX:
				pWrite->Write("	COLLISIONTYPE_BOX\n");			// 位置情報を書き込む
				WriteForBoxCollider(pWrite, m_apCollider[nCount]);
				break;
			case COLLISIONTYPE_SPHERE:
				pWrite->Write("	COLLISIONTYPE_SPHERE\n");			// 位置情報を書き込む
				WriteForSphereCollider(pWrite, m_apCollider[nCount]);
				break;
			}

			pWrite->Write("END_COLLISIONSET\n");				// 頂点情報の書き込み開始宣言
			pWrite->Write("\n");							// 改行
		}

		pWrite->Write("END_SCRIPT\n");			// スクリプトの終了宣言
		pWrite->End();

		MessageBox(NULL, "当たり判定の書き込み終了しました！", "WARNING", MB_ICONWARNING);	// メッセージボックスの生成
	}

	delete pWrite;
}