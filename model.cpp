//=============================================================================
//
// モデル処理 [model.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "player.h"
#include "light.h"
#include "sceneX.h"
#include "camera.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CModel::CModel()
{
	m_pBuffMat = NULL;
	m_pMesh = NULL;
	m_nNumMat = 0;
	m_pTexture = NULL;
	m_bUse = true;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CModel::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	//値の初期化
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//=============================================================================
// 更新処理
//=============================================================================
void CModel::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void CModel::Draw(D3DXMATRIX *mtxWorld)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXEFFECT ToonShader = CManager::GetShaderResource((std::string)SHADERADD_TOON);
	CCamera* pCamera = CManager::GetCamera();

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
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//親子関係が成立している場合
	if (m_pParent == NULL)
	{
		//マトリックスの合成
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, mtxWorld);
	}
	else
	{
		//マトリックスの合成
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &m_pParent->m_mtxWorld);
	}

	//CScene::SetShadow(pDevice, m_mtxWorld, m_pBuffMat, m_nNumMat, m_pMesh, m_pos);

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
	if (ToonShader != NULL)
	{
		ToonShader->SetTechnique("ToonShading");
		D3DXMATRIX mAll = m_mtxWorld * mtxView * mtxProj;
		ToonShader->SetMatrix("WVP", &mAll);

		ToonShader->SetMatrix("mProj", &mtxProj);
		ToonShader->SetMatrix("mView", &mtxView);
		ToonShader->SetMatrix("mWorld", &m_mtxWorld);

		ToonShader->SetTexture("ShadeTexture", CManager::GetResource("Shade.bmp"));
		ToonShader->SetTexture("LineTexture", CManager::GetResource("Outline.bmp"));

		CCamera *pCamera = CManager::GetCamera();
		CLight *pLight = CManager::GetLight();

		D3DXVECTOR4 lightPos = pLight->GetPos();

		ToonShader->SetVector("LightPos", (D3DXVECTOR4*)&lightPos);
		ToonShader->SetVector("EyePos", (D3DXVECTOR4*)&pCamera->GetPosV());

		ToonShader->Begin(NULL, 0);
	}

	if (m_bUse)
	{
		for (int nCntMat = 0; nCntMat < (int)m_nNumMat; nCntMat++)
		{
			if (ToonShader != NULL)
			{
				ToonShader->BeginPass(0);
				ToonShader->SetFloatArray("Diffuse", (FLOAT*)&pMat[nCntMat].MatD3D.Diffuse, 4);

				if (pMat[nCntMat].pTextureFilename != NULL)
				{
					// テクスチャの設定
					ToonShader->SetTexture("DecalTexture", CManager::GetResource(pMat[nCntMat].pTextureFilename));
				}
				else
				{
					// テクスチャの設定
					ToonShader->SetTexture("DecalTexture", CManager::GetResource("data/tex/default.jpg"));
				}
			}

			// 情報の変更
			ToonShader->CommitChanges();

			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			if (pMat[nCntMat].pTextureFilename != NULL)
			{
				// テクスチャの設定
				pDevice->SetTexture(0, CManager::GetResource(pMat[nCntMat].pTextureFilename));
			}

			// 描画
			m_pMesh->DrawSubset(nCntMat);

			if (ToonShader != NULL)
			{
				ToonShader->EndPass();
				// テクスチャの設定
				ToonShader->SetTexture("DecalTexture", CManager::GetResource("data/tex/default.jpg"));
			}
		}
	}

	if (ToonShader != NULL)
	{
		ToonShader->End();
		// テクスチャの設定
		ToonShader->SetTexture("DecalTexture", CManager::GetResource("data/tex/default.jpg"));
	}

	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);

	// テクスチャをデフォルトに戻す
	pDevice->SetTexture(0, NULL);
}

//=============================================================================
// 開放処理
//=============================================================================
void CModel::Uninit(void)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CModel::~CModel()
{

}

//=============================================================================
// クリエイト処理
//=============================================================================
CModel *CModel::Create(char sAdd[64])
{
	CModel *pModel;
	pModel = new CModel();

	if (pModel != NULL)
	{// モデルが存在していたとき
		pModel->Load(sAdd);				// モデルのロード
		pModel->Init();					// 初期化処理

	}
	return pModel;
}

//=============================================================================
// モデルのロード関数
//=============================================================================
HRESULT CModel::Load(char sAdd[64])
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = NULL;

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	// Xファイルの読み込み
	D3DXLoadMeshFromX(sAdd, D3DXMESH_SYSTEMMEM, pDevice, NULL, &m_pBuffMat, NULL, &m_nNumMat, &m_pMesh);

	std::string add = sAdd;
	CManager::GetModelResource(add, m_pBuffMat, m_nNumMat, m_pMesh);

	D3DXMATERIAL	*pMat;							//現在のマテリアル保存用

		// マテリアル情報に対するポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	// マテリアルのテクスチャ読み込み
	for (int nCntMat = 0; nCntMat < (int)m_nNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャがあったとき
			char *pFileName = NULL;
			pFileName = pMat[nCntMat].pTextureFilename;			// テクスチャのアドレスを取得
			CManager::GetResource(pFileName);		// テクスチャの読み込み
		}
	}

	return S_OK;
}

//=============================================================================
// 親の設定
//=============================================================================
void CModel::SetParent(CModel *pModel)
{
	m_pParent = pModel;							//親のポインタを格納
}

//=============================================================================
// IDの設定
//=============================================================================
void CModel::SetIndex(int nValue)
{
	m_nIndex = nValue;							//値の格納
}

//=============================================================================
// ポジションセット
//=============================================================================
void CModel::SetPosition(D3DXVECTOR3 pos)
{
	m_pos = pos;		//ポジションの値を更新
}

//=============================================================================
// ポジションプリセット設定
//=============================================================================
void CModel::SetPosPreset(D3DXVECTOR3 pos)
{
	m_originPos = pos;		//ポジションの値を更新
}

//=============================================================================
// 回転量の設定
//=============================================================================
void CModel::SetRotation(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//=============================================================================
// 回転量のプリセットを設定
//=============================================================================
void CModel::SetRotPreset(D3DXVECTOR3 rot)
{
	m_originRot = rot;
}

//=============================================================================
// 更新描画対象設定
//=============================================================================
void CModel::SetActive(bool bValue)
{
	m_bUse = bValue;
}