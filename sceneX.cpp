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
// コンストラクタ
//=============================================================================
CSceneX::CSceneX(CScene::PRIORITY obj = CScene::PRIORITY_MODEL) : CScene(obj)
{
	SetObjType(CScene::PRIORITY_MODEL);

	m_ShaderType = SHADERTYPE_LAMBERT;
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
	OutputDebugString("開放処理");
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

	D3DXMATRIX		mtxRot, mtxTrans, mtxSize;		//計算用マトリックス
	D3DXMATERIAL	*pMat;							//現在のマテリアル保存用
	D3DMATERIAL9	matDef;							//マテリアルデータへのポインタ

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズを反映
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

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

	LPD3DXEFFECT pShader = NULL;

	switch (m_ShaderType)
	{
	case SHADERTYPE_LAMBERT:
		pShader = CManager::GetShaderResource(SHADERADD_LAMBERT);

		// シェーダー処理
		if (pShader != NULL)
		{// シェーダーが存在していたとき
			pShader->SetTechnique("tecLambert");

			//ワールド・ビュー・プロジェクション行列を渡す
			pShader->SetMatrix("WVP", (D3DXMATRIX*)&(m_mtxWorld * mtxView * mtxProj));
			//ワールド行列の逆行列の転置行列を渡す
			D3DXMATRIX mWIT;
			pShader->SetMatrix("WIT", D3DXMatrixTranspose(&mWIT, D3DXMatrixInverse(&mWIT, NULL, &m_mtxWorld)));

			//ライトの方向ベクトルを渡す
			pShader->SetVector("LightDir", &D3DXVECTOR4(0.22f, -0.87f, 0.44f, 0.0f));

			//入射光（ライト）の強度を渡す　目一杯明るい白色光にしてみる
			pShader->SetVector("LightIntensity", &D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

			//拡散反射率を渡す
			pShader->SetVector("Diffuse", &D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

			// 環境光を渡す
			pShader->SetVector("Ambient", &D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

			pShader->Begin(NULL, 0);
			pShader->BeginPass(0);
		}
		break;
	case SHADERTYPE_TOON:
		pShader = CManager::GetShaderResource(SHADERADD_TOON);

		// シェーダー処理
		if (pShader != NULL)
		{
			pShader->SetTechnique("ToonShading");
			D3DXMATRIX mAll = m_mtxWorld * mtxView * mtxProj;
			pShader->SetMatrix("WVP", &mAll);

			pShader->SetMatrix("mProj", &mtxProj);
			pShader->SetMatrix("mView", &mtxView);
			pShader->SetMatrix("mWorld", &m_mtxWorld);

			pShader->SetTexture("ShadeTexture", CManager::GetResource("Shade.bmp"));
			pShader->SetTexture("LineTexture", CManager::GetResource("Outline.bmp"));

			CCamera *pCamera = CManager::GetCamera();
			CLight *pLight = CManager::GetLight();

			D3DXVECTOR4 lightPos = pLight->GetPos();

			pShader->SetVector("LightPos", (D3DXVECTOR4*)&lightPos);
			pShader->SetVector("EyePos", (D3DXVECTOR4*)&pCamera->GetPosV());

			pShader->Begin(NULL, 0);
		}
		break;
	}

	for (int nCntMat = 0; nCntMat < (int)m_nNumMat; nCntMat++)
	{
		if (pShader != NULL)
		{
			switch (m_ShaderType)
			{
			case SHADERTYPE_LAMBERT:
				if (pMat[nCntMat].pTextureFilename != NULL)
				{
					// テクスチャの設定
					pDevice->SetTexture(0, CManager::GetResource(pMat[nCntMat].pTextureFilename));

					if (pShader != NULL)
					{
						//ワールド行列の逆行列の転置行列を渡す
						D3DXMATRIX mWIT;
						pShader->SetMatrix("WIT", D3DXMatrixTranspose(&mWIT, D3DXMatrixInverse(&mWIT, NULL, &m_mtxWorld)));

						// テクスチャの設定
						pShader->SetTexture("texDecal", CManager::GetResource(pMat[nCntMat].pTextureFilename));
					}
				}
				else
				{
					// テクスチャの設定
					pShader->SetTexture("texDecal", CManager::GetResource(TEXTUREADD_DEFAULT));
				}

				pShader->SetVector("Diffuse", (D3DXVECTOR4*)&pMat[nCntMat].MatD3D.Diffuse);
				pShader->SetVector("Ambient", (D3DXVECTOR4*)&pMat[nCntMat].MatD3D.Ambient);
				pShader->CommitChanges();
				break;
			case SHADERTYPE_TOON:
				pShader->SetFloatArray("Diffuse", (FLOAT*)&pMat[nCntMat].MatD3D.Diffuse, 4);

				if (pMat[nCntMat].pTextureFilename != NULL)
				{
					// テクスチャの設定
					pShader->SetTexture("DecalTexture", CManager::GetResource(pMat[nCntMat].pTextureFilename));
				}
				else
				{
					// テクスチャの設定
					pShader->SetTexture("DecalTexture", CManager::GetResource(TEXTUREADD_DEFAULT));
				}
				pShader->BeginPass(0);
				break;
			}
		}

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// 描画
		m_pMesh->DrawSubset(nCntMat);

		if (m_ShaderType == SHADERTYPE_TOON)
		{
			if (pShader != NULL)
			{
				pShader->EndPass();
			}
		}
	}

	if (pShader != NULL)
	{
		switch (m_ShaderType)
		{
		case SHADERTYPE_LAMBERT:
			pShader->EndPass();
			pShader->End();
			break;
		case SHADERTYPE_TOON:
			// テクスチャの設定
			pShader->SetTexture("DecalTexture", CManager::GetResource("data/tex/default.jpg"));
			pShader->End();
			break;
		}
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
// シェーダーの初期化処理
//=============================================================================
void CSceneX::Load(void)
{
	//シェーダーを読み込む
	CManager::LoadShader((std::string)SHADERADD_LAMBERT);
	CManager::LoadShader((std::string)SHADERADD_TOON);
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

//=============================================================================
// シェーダーのパラメータ設定
//=============================================================================
void CSceneX::SetShaderParameter(LPD3DXEFFECT & pShader)
{
	switch (m_ShaderType)
	{
	case SHADERTYPE_LAMBERT:

		break;
	case SHADERTYPE_TOON:
		break;
	}
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CSceneX::Debug(void)
{

}
#endif