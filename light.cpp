//=============================================================================
//
// ライト処理 [light.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "light.h"
#include "renderer.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "player.h"
#include "game.h"
#include "object.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CLight::CLight()
{
	m_pos = D3DXVECTOR3(-3000.0f, 5000.0f, 0.0f);
}

//=============================================================================
// デストラクタ
//=============================================================================
CLight::~CLight()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CLight::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	//デバイスを取得する
	pDevice = pRenderer->GetDevice();
	D3DXVECTOR3 vecDir[MAX_LIGHT];						//設定用ベクトル

	// ライトをクリアする
	for (int nCount = 0; nCount < MAX_LIGHT; nCount++)
	{
		ZeroMemory(&m_aLight[nCount], sizeof(D3DLIGHT9));
	}

	// ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;
	// ライトの種類を設定
	m_aLight[1].Type = D3DLIGHT_DIRECTIONAL;
	// ライトの種類を設定
	m_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライトの拡散光を設定
	m_aLight[1].Diffuse = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.0f);

	// ライトの拡散光を設定
	m_aLight[2].Diffuse = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);

	// ライトの方向の設定
	vecDir[0] = D3DXVECTOR3(0.22f, -0.87f, 0.44f);
	D3DXVec3Normalize(&vecDir[0], &vecDir[0]);			//正規化する
	m_aLight[0].Direction = vecDir[0];

	// ライトの方向の設定
	vecDir[1] = D3DXVECTOR3(-0.18f, 0.88f, -0.44f);
	D3DXVec3Normalize(&vecDir[1], &vecDir[1]);			//正規化する
	m_aLight[1].Direction = vecDir[1];

	// ライトの方向の設定
	vecDir[2] = D3DXVECTOR3(0.0f, -0.11f, -0.11f);
	D3DXVec3Normalize(&vecDir[2], &vecDir[2]);			//正規化する
	m_aLight[2].Direction = vecDir[2];

	// ライトを設定する
	pDevice->SetLight(0, &m_aLight[0]);

	// ライトを設定する
	pDevice->SetLight(1, &m_aLight[1]);

	// ライトを設定する
	pDevice->SetLight(2, &m_aLight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);

	// ライトを有効にする
	pDevice->LightEnable(1, TRUE);

	// ライトを有効にする
	pDevice->LightEnable(2, TRUE);

#ifdef _DEBUG
	m_pDirectionalLight = CObject::Create();

	if (m_pDirectionalLight != NULL)
	{
		m_pDirectionalLight->BindModel("data/model/DirectionalLight.x");
	}
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CLight::Uninit(void)
{

}

//=============================================================================
// 更新処理
//=============================================================================
void CLight::Update(void)
{
#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// ライトの取得
//=============================================================================
D3DLIGHT9 CLight::GetLight(const int nIndex)
{
	return m_aLight[nIndex];					//指定されたライトを返す
}

#ifdef _DEBUG
//=============================================================================
// デバッグ処理
//=============================================================================
void CLight::Debug(void)
{
	ImGui::Begin("Light");			// Systemウィンドウ の生成またはアクセス

	ImGui::DragFloat3("pos", (float*)&m_pos);

	if (m_pDirectionalLight != NULL)
	{
		m_pDirectionalLight->SetPosition(m_pos);
	}

	//デバッグ処理を終了
	ImGui::End();
}
#endif