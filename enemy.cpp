//=============================================================================
//
// �G���� [enemy.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "enemy.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "scene3D.h"
#include "meshField.h"
#include "title.h"
#include "model.h"
#include "colliderSphere.h"
#include "meshCapsule.h"
#include "meshCube.h"
#include "inputKeyboard.h"
#include "player.h"
#include "colliderBox.h"
#include "life.h"
#include "fade.h"
#include "purpose.h"
#include "stage.h"
#include "house.h"
#include "phase.h"
#include "effect.h"
#include "result.h"
#include "sound.h"

//=============================================================================
// �}�N����`
//=============================================================================
#define SCRIPT_ENEMY "data/animation/car01.txt"		// �G�z�u���̃A�h���X
#define INTERVAL 100								// �C���^�[�o��
#define MAX_LIFE 10									// �̗͂̍ő�l

//=============================================================================
// �ÓI�����o�ϐ�
//=============================================================================
PDIRECT3DTEXTURE9 CEnemy::m_pTexture = NULL;		// �e�N�X�`�����̃|�C���^
int	CEnemy::m_nKill = 0;							// �|������

//==============================================================================
// �R���X�g���N�^
//==============================================================================
CEnemy::CEnemy(CScene::PRIORITY obj = CScene::PRIORITY_ENEMY) : CCharacter(obj)
{
	// �D��x�̐ݒ�
	SetObjType(CScene::PRIORITY_ENEMY);

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// ��]�̏�����
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ��ʂ̏�����
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���̏�����
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���̏�����
	m_bJump = false;									// �W�����v�̏�����
	m_nLife = MAX_LIFE;									// �̗͂̏�����
	m_pSphere = NULL;									// �����蔻��(��)�̏�����
	m_pLife = NULL;										// ���C�t�o�[�|�C���^�̏�����
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CEnemy::~CEnemy()
{

}

//==============================================================================
// ����������
//==============================================================================
HRESULT CEnemy::Init(void)
{
	// �L�����N�^�[�̏���������
	CCharacter::Init();

	// �A�j���[�V�����̐ݒ�
	AnimationSwitch(ANIMATIONTYPE_NONE);

	//// �G���f�����̓ǂݍ���
	//LoadScript(SCRIPT_ENEMY, ANIMATIONTYPE_MAX);

	// �ʒu�̐ݒ�
	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// �v���C���[�̓����蔻��𐶐�
	m_pSphere = CColliderSphere::Create(false, 50.0f);

	if (m_pSphere != NULL)
	{ //���̂̃|�C���^��NULL�ł͂Ȃ��Ƃ�
		m_pSphere->SetScene(this);
		m_pSphere->SetTag("enemy");										// �^�O �̐ݒ�
		m_pSphere->SetPosition(GetPosition());										// �ʒu �̐ݒ�
		m_pSphere->SetOffset(D3DXVECTOR3(0.0f, 20.0f, 0.0f));
	}

	return S_OK;
}

//=============================================================================
// �J������
//=============================================================================
void CEnemy::Uninit(void)
{
	// �L�����N�^�[�̊J������
	CCharacter::Uninit();
}

//=============================================================================
// �X�V����
//=============================================================================
void CEnemy::Update(void)
{
	// �L�����N�^�[�̍X�V����
	CCharacter::Update();

#ifdef _DEBUG
	// �f�o�b�O����
	Debug();
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void CEnemy::Draw(void)
{
	if (CManager::GetMode() == CManager::MODE_GAME)
	{
		CCharacter::Draw();
	}
}

//=============================================================================
// �N���G�C�g�֐�
//=============================================================================
CEnemy *CEnemy::Create(void)
{
	CEnemy *pEnemy;
	pEnemy = new CEnemy(CScene::PRIORITY_ENEMY);

	if (pEnemy != NULL)
	{// �G�����݂��Ă����Ƃ�
		pEnemy->Init();				// ����������
	}
	return pEnemy;
}

//=============================================================================
// ���f���̃��[�h�֐�
//=============================================================================
HRESULT CEnemy::Load(void)
{
	return S_OK;
}

//=============================================================================
// ���f���̓Ǎ�
//=============================================================================
void CEnemy::LoadEnemy(char *add)
{
	FILE *pFile = NULL;						// �t�@�C��
	char cReadText[128] = {};				// ����
	char cHeadText[128] = {};				// ��r
	float aData[3];							// ����
	CEnemy *pEnemy = NULL;

	pFile = fopen(add, "r");				// �t�@�C�����J���܂��͍��

	if (pFile != NULL)						// �t�@�C�����ǂݍ��߂��ꍇ
	{
		fgets(cReadText, sizeof(cReadText), pFile);			// �s���΂�
		fgets(cReadText, sizeof(cReadText), pFile);			// �s���΂�
		fgets(cReadText, sizeof(cReadText), pFile);			// �s���΂�

		while (strcmp(cHeadText, "End") != 0)
		{
			CManager::ConvertStringToFloat(cReadText, ",", aData);		// ���������������R���}�ŋ�؂�i�[����

			pEnemy = CEnemy::Create();			// �N���G�C�g����

			if (pEnemy != NULL)
			{// �G�����݂��Ă����Ƃ�
				pEnemy->SetPosition(D3DXVECTOR3(aData[0], aData[1], aData[2]));			// �|�W�V���������߂�
				pEnemy->SetTarget(TARGETTYPE_PLAYER);				// �U���Ώۂ̐ݒ�
			}

			fgets(cReadText, sizeof(cReadText), pFile);				// �s���΂�
			sscanf(cReadText, "%s", &cHeadText);					// �s��ǂݍ���
		}

		fclose(pFile);				// �t�@�C�������
	}
	else
	{
		MessageBox(NULL, "���f�����̃A�N�Z�X���s�I", "WARNING", MB_ICONWARNING);	// ���b�Z�[�W�{�b�N�X�̐���
	}
}

//=============================================================================
// �U���Ώۂ̐ݒ菈��
//=============================================================================
void CEnemy::SetTarget(TARGETTYPE target)
{
	m_target = target;
}

//=============================================================================
// �����蔻�� �R���C�_�[
//=============================================================================
void CEnemy::OnTriggerEnter(CCollider *col)
{
	// �A�j���[�V�������̎擾
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	std::string sTag = col->GetTag();

	if (animType != ANIMATIONTYPE_DIE)
	{
		if (col->GetScene()->GetObjType() == PRIORITY_PLAYER)
		{
			if (sTag == "weapon")
			{// �^�O�� ���� �������Ƃ�
				if (animType != ANIMATIONTYPE_DIE)
				{
					CPlayer *pPlayer = CGame::GetPlayer();				// �v���C���[�̎擾
					CSound *pSound = CManager::GetSound();				// �T�E���h�̎擾

					//pSound->PlaySoundA(SOUND_LABEL_SE_PUNCH);			// �_���[�W���̍Đ�

					if (m_target != TARGETTYPE_PLAYER)
					{// �U���Ώۂ��v���C���[�ȊO�������Ƃ�
						m_target = TARGETTYPE_PLAYER;				// �W�I���v���C���[�ɒ�߂�
					}

					if (m_pLife != NULL)
					{
						m_pLife->SetLifeBar((float)m_nLife / MAX_LIFE);
					}
					else
					{// �̗̓o�[�̐���
						m_pLife = CLife::Create();
						m_pLife->SetLifeBar((float)m_nLife / MAX_LIFE);
					}

					if (m_nLife < 1)
					{// �̗͂� 1 ����������Ƃ�
						D3DXVECTOR3 vec;

						AnimationSwitch(ANIMATIONTYPE_DIE);
						m_nKill++;

						// �̂̓����蔻��̊J��
						if (m_pSphere != NULL)
						{// �̂̓����蔻�肪���݂��Ă����Ƃ�
							m_pSphere->Release();
							m_pSphere = NULL;
						}

						// �̗̓Q�[�W�̊J��
						if (m_pLife != NULL)
						{// �̗͂̊J�������݂��Ă����Ƃ�
							m_pLife->Uninit();
							m_pLife->Release();
							m_pLife = NULL;
						}

						if (pPlayer != NULL)
						{// �v���C���[�����݂��Ă����Ƃ�
							vec = GetPosition() - pPlayer->GetPosition();		//���������߂�(���������߂邽��)
						}
						D3DXVec3Normalize(&vec, &vec);			//���K������

						m_move.x = vec.x * 25;
						m_move.z = vec.z * 25;
						m_move.y = 10;
						m_bJump = true;
					}
					else
					{
						AnimationSwitch(ANIMATIONTYPE_DAMAGE);									// �_���[�W���[�V����
					}
				}
			}
		}
	}
}

//========================================================================================
// �A�j���[�V�����t���[���̍ő吔�ɓ��B�����Ƃ��̏���
//========================================================================================
void CEnemy::BehaviorForMaxFrame(void)
{
}

//========================================================================================
// �A�j���[�V�����L�[�̍ő吔�ɓ��B�����Ƃ��̏���
//========================================================================================
void CEnemy::BehaviorForMaxKey(void)
{
	// �A�j���[�V�������̎擾
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	ANIMATION *pAnimation = GetAnimData();

	// ���[�v���邩�ǂ���
	if (animType == ANIMATIONTYPE_DIE)
	{
		Release();
	}
	else if (pAnimation[animType].nLoop)
	{
		// �L�[�̃��Z�b�g
		ResetKeyAndFrame();
	}
	else
	{
		// �j���[�g�������[�V����
		AnimationSwitch(ANIMATIONTYPE_NEUTRAL);
		// �L�[�̃��Z�b�g
		ResetKeyAndFrame();
	}
}

//=============================================================================
// �s���֐�
//=============================================================================
void CEnemy::Move(D3DXVECTOR3 &pPos)
{
	CPlayer *pPlayer = CGame::GetPlayer();
	CHouse *pHouse = CGame::GetHouse();
	D3DXVECTOR3 targetPos;

	// �A�j���[�V�������̎擾
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();

	if (m_target == TARGETTYPE_HOUSE)
	{//	���݂̑Ώۂ�[��]�������Ƃ�
		if (pHouse != NULL)
		{
			targetPos = pHouse->GetPosition();
		}
	}
	else if (m_target == TARGETTYPE_PLAYER)
	{// ���݂̍U���Ώۂ�[�v���C���[]�������Ƃ�
		if (pPlayer != NULL)
		{
			targetPos = pPlayer->GetPosition();
		}
	}

	D3DXVECTOR3 pos = pPos;

	float fDifference_x = pos.x - targetPos.x;
	float fDifference_z = pos.z - targetPos.z;
	float fDifference = sqrtf(fDifference_x * fDifference_x + fDifference_z * fDifference_z);

	// �G���U�������ǂ���
	if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE && animType != ANIMATIONTYPE_DIE)
	{
		if (m_nInterval > INTERVAL)
		{
			if (m_target == TARGETTYPE_PLAYER)
			{
			}
			else if (m_target == TARGETTYPE_HOUSE)
			{
				// �G���͈͓��ɓ����Ă������ǂ���
				if (fDifference < 3000.0f && fDifference > 450)
				{// �v���C���[�Ɍ������đ���
					D3DXVECTOR3 nor = targetPos - pos;

					D3DXVec3Normalize(&nor, &nor);

					nor.y = 0;
					m_move = nor * 10;
					m_rot.y = atan2f(fDifference_x, fDifference_z);
				}
			}

			if (m_target == TARGETTYPE_PLAYER)
			{
				// �N�[���^�C�����I���Ă��邩�ǂ���
				if (fDifference < 200)
				{// �w�肵���������߂Â��Ă����Ƃ�
					if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE)
					{
						if (CManager::GetRand(10) > 8)
						{
							AnimationSwitch(ANIMATIONTYPE_ATTACK);
							m_nInterval = 0;
						}
					}
				}
			}
			else if (m_target == TARGETTYPE_HOUSE)
			{
				// �N�[���^�C�����I���Ă��邩�ǂ���
				if (fDifference < 460)
				{// �w�肵���������߂Â��Ă����Ƃ�
					if (animType != ANIMATIONTYPE_ATTACK && animType != ANIMATIONTYPE_DAMAGE)
					{
						if (CManager::GetRand(10) > 8)
						{
							AnimationSwitch(ANIMATIONTYPE_ATTACK);
							m_nInterval = 0;
						}
					}
				}
			}
		}
		else
		{
			m_nInterval++;
		}
	}

	pPos = pos;
}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CEnemy::Collider(void)
{
	// �A�j���[�V�������̎擾
	ANIMATIONTYPE animType = (ANIMATIONTYPE)GetAnimType();
	int currentKey = GetCurrentKey();

	switch (animType)
	{
	case ANIMATIONTYPE_NEUTRAL:					// �ҋ@���[�V�����̂Ƃ�
		break;
	case ANIMATIONTYPE_ATTACK:				// �U�����[�V�����̂Ƃ�
		break;
	}
}

#ifdef _DEBUG
//=============================================================================
// �f�o�b�O����
//=============================================================================
void CEnemy::Debug(void)
{
	ImGui::Begin("System");													// �uSystem�v�E�B���h�E�ɒǉ����܂��B�Ȃ���΍쐬���܂��B
	D3DXVECTOR3 size;
	D3DXVECTOR3 pos = GetPosition();

	if (ImGui::TreeNode("enemy"))
	{//�A�j���[�V�����̃c���[�m�[�h��T���܂��B�Ȃ���΍쐬���܂��B
		ImGui::Text("pos%d = %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);							// �v���C���[�̌��݈ʒu��\��

		if (ImGui::TreeNode("collider"))
		{
			ImGui::DragFloat3("ColliderSize", (float*)&size);											// �R���C�_�[�̑傫����ݒ�

			ImGui::DragFloat3("ColliderPos", (float*)&pos);

			ImGui::TreePop();																			// �I��
		}

		ImGui::TreePop();																			// �I��
	}

	//�f�o�b�O�������I��
	ImGui::End();
}
#endif