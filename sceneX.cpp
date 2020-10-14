//=============================================================================
//
// sceneX処理 [sceneX.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "sceneX.h"
#include "manager.h"
#include "renderer.h"
#include "light.h"
#include "object.h"
#include "camera.h"

//=============================================================================
// 静的メンバ変数
//=============================================================================
LPD3DXEFFECT CSceneX::m_pToonShader = NULL;

//=============================================================================
// コンストラクタ
//=============================================================================
CSceneX::CSceneX(CScene::PRIORITY obj = CScene::PRIORITY_MODEL) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_MODEL);

	m_pBuffMat = NULL;
	m_pMesh = NULL;
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

//=============================================================================
// デストラクタ
//=============================================================================
CSceneX::~CSceneX()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CSceneX::Init(void)
{
	return S_OK;
}

//=============================================================================
// 開放処理
//=============================================================================
void CSceneX::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CSceneX::Update(void)
{
#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void CSceneX::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;
	D3DXVECTOR3 pos = GetPosition();

	D3DXMATRIX		mtxRot, mtxTrans;				//計算用マトリックス
	D3DXMATERIAL	*pMat;							//現在のマテリアル保存用
	D3DMATERIAL9	matDef;							//マテリアルデータへのポインタ

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);					//すべてのデータ収録

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアル情報に対するポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	D3DXMATRIX mtxView, mtxProj;

	pDevice->GetTransform(D3DTS_VIEW, &mtxView);
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProj);

	// シェーダー処理
	if (m_pToonShader != NULL)
	{
		m_pToonShader->SetTechnique("ToonShading");
		D3DXMATRIX mAll = m_mtxWorld * mtxView * mtxProj;
		m_pToonShader->SetMatrix("WVP", &mAll);

		m_pToonShader->SetMatrix("mProj", &mtxProj);
		m_pToonShader->SetMatrix("mView", &mtxView);
		m_pToonShader->SetMatrix("mWorld", &m_mtxWorld);

		m_pToonShader->SetTexture("ShadeTexture", CManager::GetResource("Shade.bmp"));
		m_pToonShader->SetTexture("LineTexture", CManager::GetResource("Outline.bmp"));

		CCamera *pCamera = CManager::GetCamera();
		CLight *pLight = CManager::GetLight();

		D3DXVECTOR4 lightPos = pLight->GetPos();

		m_pToonShader->SetVector("LightPos", (D3DXVECTOR4*)&lightPos);
		m_pToonShader->SetVector("EyePos", (D3DXVECTOR4*)&pCamera->GetPosV());

		m_pToonShader->Begin(NULL, 0);
	}

	for (int nCntMat = 0; nCntMat < (int)m_nNumMat; nCntMat++)
	{
		if (m_pToonShader != NULL)
		{
			m_pToonShader->BeginPass(0);
			m_pToonShader->SetFloatArray("Diffuse", (FLOAT*)&pMat[nCntMat].MatD3D.Diffuse, 4);

			if (pMat[nCntMat].pTextureFilename != NULL)
			{
				// テクスチャの設定
				m_pToonShader->SetTexture("DecalTexture", CManager::GetResource(pMat[nCntMat].pTextureFilename));
			}
			else
			{
				// テクスチャの設定
				m_pToonShader->SetTexture("DecalTexture", CManager::GetResource("data/tex/default.jpg"));
			}
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		if (pMat[nCntMat].pTextureFilename != NULL)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, CManager::GetResource(pMat[nCntMat].pTextureFilename));
		}

		// 描画
		m_pMesh->DrawSubset(nCntMat);

		if (m_pToonShader != NULL)
		{
			m_pToonShader->EndPass();
		}
	}

	if (m_pToonShader != NULL)
	{
		m_pToonShader->End();
	}

	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// クリエイト関数
//=============================================================================
CSceneX *CSceneX::Create(void)
{
	CSceneX *pSceneX;
	pSceneX = new CSceneX(CScene::PRIORITY_MODEL);

	if (pSceneX != NULL)
	{// シーンが存在していたとき
		pSceneX->Init();				// 初期化処理
	}
	return pSceneX;
}

//=============================================================================
// モデル情報の設定
//=============================================================================
void CSceneX::BindModel(LPD3DXMESH pMesh, DWORD nNumMat, LPD3DXBUFFER pBuffMat)
{
	m_pMesh = pMesh;
	m_nNumMat = nNumMat;
	m_pBuffMat = pBuffMat;
}

//=============================================================================
// カラーの設定
//=============================================================================
void CSceneX::SetColor(D3DXCOLOR col)
{
	m_col = col;
}

//=============================================================================
// サイズの設定
//=============================================================================
void CSceneX::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//=============================================================================
// 回転値の設定
//=============================================================================
void CSceneX::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//=============================================================================
// シェーダーの初期化処理
//=============================================================================
void CSceneX::InitShader(void)
{
	LPDIRECT3DDEVICE9 pDevice;
	CRenderer *pRenderer = CManager::GetRenderer();

	// デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//シェーダーを読み込む
	if (FAILED(D3DXCreateEffectFromFile(pDevice, "ToonShader.fx", NULL, NULL,
		0, NULL, &m_pToonShader, NULL)))
	{
		MessageBox(NULL, "シェーダーファイル読み込み失敗", "", MB_OK);
	}
}

//=============================================================================
// シェーダーの開放処理
//=============================================================================
void CSceneX::UninitShader(void)
{
	// シェーダーの開放
	if (m_pToonShader != NULL)
	{
		m_pToonShader->Release();
		m_pToonShader = NULL;
	}
}

//=============================================================================
// インスペクターを表示
//=============================================================================
void CSceneX::ShowInspector(void)
{
	D3DXVECTOR3 pos = GetPosition();
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::DragFloat3("Position", (float*)&pos, 0.1f);
		ImGui::DragFloat3("Rotation", (float*)&m_rot, 0.01f);
		ImGui::Text("");

		SetPosition(pos);
	}

	CScene::ShowInspector();
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CSceneX::Debug(void)
{

}
#endif