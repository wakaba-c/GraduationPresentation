//==================================================================================================================
//
// �e�|���S��[shadow.cpp]
// Author:Seiya Takahashi
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "shadow.h"
#include "player.h"
#include "game.h"
#include "takaseiLibrary.h"
#include "debug.h"

//==================================================================================================================
// �ÓI�����o�ϐ��̏�����
//==================================================================================================================
LPDIRECT3DTEXTURE9 CShadow::m_pTexture = NULL;			// �e�N�X�`�����

//==================================================================================================================
// �R���X�g���N�^
//==================================================================================================================
CShadow::CShadow(PRIORITY type = CScene::PRIORITY_MESH) :CScene(type)
{

}

//==================================================================================================================
// �f�X�g���N�^
//==================================================================================================================
CShadow::~CShadow()
{

}

//==================================================================================================================
// ����������
//==================================================================================================================
HRESULT CShadow::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// �����_���[���擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// �f�o�C�X�̎擾

	// �ϐ��̏�����
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ʒu
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.5f);				// �傫��
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// ��]

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// ���W�̐ݒ�
	// [0]���_
	m_pVtx[0].pos.x = m_pos.x - m_size.x / 2;
	m_pVtx[0].pos.y = 0;
	m_pVtx[0].pos.z = m_pos.z - m_size.z / 2;

	// [1]���_
	m_pVtx[1].pos.x = m_pos.x + m_size.x / 2;
	m_pVtx[1].pos.y = 0;
	m_pVtx[1].pos.z = m_pos.z - m_size.z / 2;

	// [2]���_
	m_pVtx[2].pos.x = m_pos.x - m_size.x / 2;
	m_pVtx[2].pos.y = 0;
	m_pVtx[2].pos.z = m_pos.z + m_size.z / 2;

	// [3]���_
	m_pVtx[3].pos.x = m_pos.x + m_size.x / 2;
	m_pVtx[3].pos.y = 0;
	m_pVtx[3].pos.z = m_pos.z + m_size.z / 2;

	//�@���x�N�g���̐ݒ�
	m_pVtx[0].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[1].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[2].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pVtx[3].nor = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	// ���_�J���[
	m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
	m_pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

	// �e�N�X�`�����W�̐ݒ�
	m_pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	m_pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�f�[�^���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}

//==================================================================================================================
// �I������
//==================================================================================================================
void CShadow::Uninit(void)
{
	// ���_�o�b�t�@������Ƃ�
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();		// �J��
		m_pVtxBuff = NULL;			// NULL�ɂ���
	}
}

//==================================================================================================================
// �X�V����
//==================================================================================================================
void CShadow::Update(void)
{
	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		CPlayer *pPlayer = CGame::GetPlayer();
		D3DXVECTOR3 PlayerPos = ZeroVector3;
		D3DXVECTOR3 PlayerRot = ZeroVector3;

		// �v���C���[������Ƃ�
		if (pPlayer != NULL)
		{
			PlayerPos = pPlayer->GetPosition();		// �v���C���[�ʒu�擾
			PlayerRot = pPlayer->GetRotation();		// �v���C���[��]�擾
		}

		// �ʒu�ݒ�
		m_pos = D3DXVECTOR3(PlayerPos.x, PlayerPos.y + 4.0f, PlayerPos.z);

		// ��]�ݒ�
		m_rot = PlayerRot;

		// �傫���ݒ�
		m_size = D3DXVECTOR3(150.0f, 1.0f, 150.0f);
	}
}

//==================================================================================================================
// �`�揈��
//==================================================================================================================
void CShadow::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// �����_���[���擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// �f�o�C�X�̎擾
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;					// �v�Z�p�}�g���b�N�X

	// ���Z����
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScale, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, m_pTexture);

	// �\�ʂ��J�����O����
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// ����(�����)���J�����O����
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, NULL);

	// �ʏ�u�����h
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//==================================================================================================================
// ��������
//==================================================================================================================
CShadow *CShadow::Create(void)
{
	CShadow *pShadow = NULL;						// �e�̃|�C���^NULL
	pShadow = new CShadow(CScene::PRIORITY_MESH);	// ���I�m��
	pShadow->Init();								// ����������

	// �l��Ԃ�
	return pShadow;
}

//==================================================================================================================
// �e�N�X�`�����[�h
//==================================================================================================================
HRESULT CShadow::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice;

	//�f�o�C�X���擾����
	pDevice = pRenderer->GetDevice();

	m_pTexture = CManager::GetResource(SHADOW_TEX);

	return S_OK;
}
