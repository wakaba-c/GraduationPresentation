//=============================================================================
//
// �L�����N�^�[���� [character.cpp]
// Author : masayasu wakita
//
//=============================================================================
#include "character.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "inputController.h"
#include "scene3D.h"
#include "meshField.h"
#include "model.h"
#include "meshCapsule.h"
#include "colliderSphere.h"
#include "meshCube.h"
#include "enemy.h"
#include "meshOrbit.h"
#include "camera.h"
#include "colliderBox.h"
#include "stage.h"
#include "sound.h"
#include "scene2D.h"
#include "effect.h"
#include "result.h"
#include "fade.h"
#include "takaseiLibrary.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CCharacter::CCharacter(CScene::PRIORITY obj) : CScene(obj)
{
	m_pAnimation = NULL;
	m_apCurrentPos = NULL;								// ���݈ʒu �̏�����
	m_apCurrentRot = NULL;								// ���݉�] �̏�����
	m_pModel = NULL;									// ���f�����̏�����

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// ��]�̏�����
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ��ʂ̏�����
	m_dest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���̏�����
	m_difference = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���̏�����
	m_nNumParts = 0;									// �p�[�c�����̏�����
	m_nCurrentFrame = 0;								// �t���[�����̏�����
	m_nCurrentKey = 0;									// �L�[���̏�����
	m_bAnimSwitch = false;								// �A�j���[�V�����X�C�b�`�̏�����
	m_bAnimation = true;								// �A�j���[�V�����t���O�̏�����
	m_nActionCount = 0;									// �A�N�V�����J�E���^�̏�����
	m_nAnimationType = -1;								// �A�j���[�V�����^�C�v�̏�����
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CCharacter::~CCharacter()
{

}

//=============================================================================
// ����������
//=============================================================================
HRESULT CCharacter::Init(void)
{
	return S_OK;
}

//=============================================================================
// �J������
//=============================================================================
void CCharacter::Uninit(void)
{
	if (m_apCurrentPos != NULL)
	{// �p�[�c��1�t���[�����Ƃ̈ړ��ʂ��i�[����ϐ���NULL����Ȃ������Ƃ�
		delete[] m_apCurrentPos;
		m_apCurrentPos = NULL;
	}

	if (m_apCurrentRot != NULL)
	{// �p�[�c��1�t���[�����Ƃ̉�]�ʂ��i�[����ϐ���NULL����Ȃ������Ƃ�
		delete[] m_apCurrentRot;
		m_apCurrentRot = NULL;
	}

	if (m_pModel != NULL)
	{
		for (int nCount = 0; nCount < m_nNumParts; nCount++)
		{
			m_pModel[nCount].Uninit();
		}

		delete[] m_pModel;
		m_pModel = NULL;
	}

	if (m_pAnimation != NULL)
	{
		for (int nCount = 0; nCount < m_nMaxAnimationType; nCount++)
		{
			if (m_pAnimation[nCount].apKeyInfo != NULL)
			{//NULL�ł͂Ȃ��Ƃ�
				delete[] m_pAnimation[nCount].apKeyInfo;										//�p�[�c�̑��������������擾����
				m_pAnimation[nCount].apKeyInfo = NULL;											//�p�[�c���̃A�h���X��NULL������
			}
		}

		delete[] m_pAnimation;
		m_pAnimation = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void CCharacter::Update(void)
{
	// �A�j���[�V�����Đ���~�̃t���O�������Ă�����
	if (m_bAnimation) Animation();

	for (int nCount = 0; nCount < m_nNumParts; nCount++)
	{
		m_pModel[nCount].Update();
	}
#ifdef _DEBUG
	Debug();
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void CCharacter::Draw(void)
{
	// �J�����̎擾
	//CCamera *pCamera = CManager::GetCamera();

	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();
	D3DXMATRIX		mtxRot, mtxTrans;				//�v�Z�p�}�g���b�N�X

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �L�����N�^�[�p���s��Z�o
	//CTakaseiLibrary::CalcLookAtMatrix(&m_mtxWorld, &GetPosition(), &pCamera->GetPosR(), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);

	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	for (int nCount = 0; nCount < m_nNumParts; nCount++)
	{
		if (m_pModel != NULL)
		{//�p�[�c�����݂���ꍇ
			m_pModel[nCount].Draw(&m_mtxWorld);							//�`��
		}
	}
}

//=============================================================================
// ���[�h����
//=============================================================================
HRESULT CCharacter::Load(void)
{
	return S_OK;
}

//=============================================================================
// �����蔻��(trigger)
//=============================================================================
void CCharacter::OnTriggerEnter(CCollider *col)
{

}

//=============================================================================
// �����蔻��(collider)
//=============================================================================
void CCharacter::OnCollisionEnter(CCollider *col)
{

}

//========================================================================================
// �p�[�c�̃��[�h
//========================================================================================
void CCharacter::LoadScript(std::string add, const int nMaxAnim)
{
	FILE *pFile;
	char cReadText[128];		//����
	char cHeadText[128];		//��r
	char cDie[128];
	int nCntPointer = 0;			//�|�C���^�[�̐��l

	char sAdd[64];					//���f���̃A�h���X
	int nIndex;
	int nParent;
	int nCntMotion = 0;				//�Q�Ƃ���|�C���^�̒l��������
	int nCntKey = 0;
	int nMaxModel = 0;
	int nCntMaya = 0;				//maya�̃f�[�^�Q��

	if (!m_pAnimation)
	{// �������擾
		m_pAnimation = new ANIMATION[nMaxAnim];
		m_nMaxAnimationType = nMaxAnim;

		if (m_pAnimation != NULL)
		{
			// �A�j���[�V�������̏�����
			for (int nCount = 0; nCount < nMaxAnim; nCount++)
			{
				m_pAnimation[nCount].apKeyInfo = NULL;			// �A�j���[�V�������̏�����
			}
		}
	}

	//�e�L�X�g�f�[�^���[�h
	pFile = fopen(add.c_str(), "r");

	if (pFile != NULL)
	{
		//�|�C���^�[�̃��Z�b�g
		nCntPointer = 0;

		//�X�N���v�g������܂Ń��[�v
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);
			sscanf(cReadText, "%s", &cHeadText);
		}

		//�X�N���v�g��������
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			//�G���h�X�N���v�g������܂�
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);
				sscanf(cReadText, "%s", &cHeadText);

				//���s
				if (strcmp(cReadText, "\n") != 0)
				{
					if (strcmp(cHeadText, "NUM_MODEL") == 0)
					{//�p�[�c�����̂Ƃ�
						if (m_pModel == NULL)
						{
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nMaxModel);					//�p�[�c�̐����擾

							const int nNumParts = nMaxModel;											//�p�[�c�̐���萔�Ƃ��Ē�`

							if (m_pModel == NULL)
							{//���f�����i�[����ϐ���NULL�������Ƃ�
								m_pModel = new CModel[nNumParts];											//�p�[�c�̑��������������擾����
							}

							if (m_apCurrentPos == NULL)
							{// �p�[�c��1�t���[�����Ƃ̈ړ��ʂ��i�[����ϐ���NULL�������Ƃ�
								m_apCurrentPos = new D3DXVECTOR3[nNumParts];
							}

							if (m_apCurrentRot == NULL)
							{// �p�[�c��1�t���[�����Ƃ̉�]�ʂ��i�[����ϐ���NULL�������Ƃ�
								m_apCurrentRot = new D3DXVECTOR3[nNumParts];
							}

							for (int nCount = 0; nCount < nMaxAnim; nCount++)
							{
								if (m_pAnimation[nCount].apKeyInfo == NULL)
								{
									m_pAnimation[nCount].apKeyInfo = new KEY_INFO[nNumParts];			//�p�[�c�̑��������������擾����
								}
							}

							m_nNumParts = nNumParts;													//�p�[�c�̑������i�[����
						}
					}
					else if (strcmp(cHeadText, "MODEL_FILENAME") == 0)
					{//�p�[�c���f���̃A�h���X���̂Ƃ�
						sscanf(cReadText, "%s %s %s", &cDie, &cDie, &sAdd[0]);						//�A�h���X�̎擾

						if (m_pModel != NULL)
						{
							m_pModel[nCntPointer].Load(sAdd);										//�p�[�c���f���̃��[�h
							m_pModel[nCntPointer].Init();											//�p�[�c���f���̏�����
							nCntPointer++;																//�|�C���^�[��4B�v���X
						}
					}

					else if (strcmp(cHeadText, "CHARACTERSET") == 0)
					{//�L�����N�^�[�̏������̂Ƃ�
						nCntPointer = 0;															//�Q�Ƃ���|�C���^�̒l��������

						// �G���h�L�����N�^�[�Z�b�g������܂Ń��[�v
						while (strcmp(cHeadText, "END_CHARACTERSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							//�p�[�c�Z�b�g��������
							if (strcmp(cHeadText, "PARTSSET") == 0)
							{
								//�G���h�p�[�c�Z�b�g������܂Ń��[�v
								while (strcmp(cHeadText, "END_PARTSSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									//�C���f�b�N�X
									if (strcmp(cHeadText, "INDEX") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nIndex);			//�C���f�b�N�X�̒l���擾
										m_pModel[nCntPointer].SetIndex(nIndex);							//�C���f�b�N�X�̒l�����ݎQ�ƒ��̃p�[�c���f���Ɋi�[
									}

									//�e
									if (strcmp(cHeadText, "PARENT") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &nParent);			//�e�̒l���擾

										if (nParent == -1)
										{//�e�̒l��-1�������Ƃ�
											m_pModel[nCntPointer].SetParent(NULL);				//NULL���i�[����
										}
										else
										{//�e�̒l��-1�ł͂Ȃ��Ƃ�
											if (m_pModel != NULL)
											{//���f���̒��g��NULL�ł͂Ȃ��Ƃ�
											 //���݃��[�h����Ă��郂�f�����̒�����Ώۂ�������܂ŉ�
												for (int nCount = 0; nCount < nMaxModel; nCount++)
												{
													if (nParent == m_pModel[nCount].GetIndex())
													{//�p�[�c���f���̃C���f�b�N�l�Ǝ����̐e�̒l����v�����Ƃ�
														m_pModel[nCntPointer].SetParent(&m_pModel[nCount]);				//�e�̃|�C���^���i�[����
														break;
													}
												}
											}
										}
									}

									if (strcmp(cHeadText, "POS") == 0)
									{//�ʒu
										D3DXVECTOR3 originPos;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originPos.x,
											&originPos.y,
											&originPos.z);

										m_pModel[nCntPointer].SetPosPreset(originPos);
										m_pModel[nCntPointer].SetPosition(originPos);
									}

									if (strcmp(cHeadText, "ROT") == 0)
									{//��]
										D3DXVECTOR3 originRot;
										sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
											&originRot.x,
											&originRot.y,
											&originRot.z);

										m_pModel[nCntPointer].SetRotPreset(originRot);
										m_pModel[nCntPointer].SetRotation(originRot);
									}
								}

								nCntPointer++;											//�Q�Ƃ���|�C���^�̒l��i�߂�
							}
						}
					}
					else if (strcmp(cHeadText, "MOTIONSET") == 0)
					{//�L�����N�^�[�Z�b�g��������

					 //�G���h�L�����N�^�[�Z�b�g������܂Ń��[�v
						while (strcmp(cHeadText, "END_MOTIONSET") != 0)
						{
							fgets(cReadText, sizeof(cReadText), pFile);
							sscanf(cReadText, "%s", &cHeadText);

							if (strcmp(cHeadText, "LOOP") == 0)
							{//���[�v���邩�ǂ���
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nLoop);
							}
							else if (strcmp(cHeadText, "NUM_KEY") == 0)
							{//�L�[��
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nMaxKey);
							}
							else if (strcmp(cHeadText, "SWITCHFRAME") == 0)
							{// �t���[���Ԃ̈ړ��t���[����
								sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].nSwitchFrame);
							}

							if (strcmp(cHeadText, "KEYSET") == 0)
							{//�L�[�Z�b�g��������

							 //�Q�Ƃ���|�C���^�̒l��������
								nCntPointer = 0;
								//�G���h�L�[�Z�b�g������܂Ń��[�v
								while (strcmp(cHeadText, "END_KEYSET") != 0)
								{
									fgets(cReadText, sizeof(cReadText), pFile);
									sscanf(cReadText, "%s", &cHeadText);

									//�t���[����
									if (strcmp(cHeadText, "FRAME") == 0)
									{
										sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_pAnimation[nCntMotion].apKeyInfo[nCntKey].nFrame);
									}

									//�L�[��������
									if (strcmp(cHeadText, "KEY") == 0)
									{
										//�G���h�L�[������܂Ń��[�v
										while (strcmp(cHeadText, "END_KEY") != 0)
										{
											fgets(cReadText, sizeof(cReadText), pFile);
											sscanf(cReadText, "%s", &cHeadText);

											//�ʒu
											if (strcmp(cHeadText, "POS") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].pos.z);
											}

											//��]
											if (strcmp(cHeadText, "ROT") == 0)
											{
												sscanf(cReadText, "%s %s %f %f %f", &cDie, &cDie,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.x,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.y,
													&m_pAnimation[nCntMotion].apKeyInfo[nCntPointer].aKey[nCntKey].rot.z);
											}
										}

										//�p�[�c�̃L�[�����i�[����ꏊ��i�߂�
										nCntPointer++;
									}
								}

								//key�̃J�E���^�[��1�v���X
								nCntKey++;
								nCntPointer = 0;
							}
						}

						//�A�j���[�V�����̎�ނ�1�v���X����
						nCntMotion++;
						nCntKey = 0;
					}
				}
			}
		}

		//�t�@�C����
		fclose(pFile);
	}
	else
	{

	}
}

//=============================================================================
// �A�j���[�V��������
//=============================================================================
void CCharacter::Animation(void)
{
	if (m_nAnimationType > -1)
	{
		//�ŏ��̃t���[���̎�
		if (m_nCurrentFrame == 0)
		{
			for (int nCount = 0; nCount < m_nNumParts; nCount++)
			{
				D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
				D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

				if (m_bAnimSwitch)
				{
					//pos�̌v�Z((�ڕW��key + �v���Z�b�g�̔z�u) - ���݂̃L�[)
					m_apCurrentPos[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].pos + m_pModel[nCount].GetPosPreset()) - pos) / 0.1f * (float)m_pAnimation[m_nAnimationType].nSwitchFrame;

					//rot�̌v�Z((�ڕW��key + �v���Z�b�g�̔z�u) - ���݂̃L�[)
					m_apCurrentRot[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].rot + m_pModel[nCount].GetRotPreset()) - rot) / 0.1f * (float)m_pAnimation[m_nAnimationType].nSwitchFrame;
				}
				else
				{
					//pos�̌v�Z((�ڕW��key + �v���Z�b�g�̔z�u) - ���݂̃L�[)
					m_apCurrentPos[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].pos + m_pModel[nCount].GetPosPreset()) - pos) / (float)m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame;

					//rot�̌v�Z((�ڕW��key + �v���Z�b�g�̔z�u) - ���݂̃L�[)
					m_apCurrentRot[nCount] = ((m_pAnimation[m_nAnimationType].apKeyInfo[nCount].aKey[m_nCurrentKey].rot + m_pModel[nCount].GetRotPreset()) - rot) / (float)m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame;
				}
			}
		}
		else
		{//����ȊO�̃t���[��
			for (int nCount = 0; nCount < m_nNumParts; nCount++)
			{
				D3DXVECTOR3 pos = m_pModel[nCount].GetPosition();
				D3DXVECTOR3 rot = m_pModel[nCount].GetRotation();

				//rot�̈ړ�
				m_pModel[nCount].SetRotation(rot + m_apCurrentRot[nCount]);

				//pos�̈ړ�
				m_pModel[nCount].SetPosition(pos + m_apCurrentPos[nCount]);
			}
		}

		if (m_pAnimation != NULL)
		{
			if (m_pAnimation[m_nAnimationType].apKeyInfo != NULL)
			{// �L�[���NULL�ł͂Ȃ��Ƃ�
			 // �t���[���̍ő吔�ɖ����Ȃ��ꍇ
				if (m_nCurrentFrame < m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame)
				{
					m_nCurrentFrame++;
				}
				//�t���[�����̍ő�l�ɒB�����ꍇ
				else if (m_nCurrentFrame >= m_pAnimation[m_nAnimationType].apKeyInfo[m_nCurrentKey].nFrame)
				{
					// �ő�t���[�����B���̋���
					BehaviorForMaxFrame();

					m_nCurrentKey++;			// �L�[�̒l��1�v���X
					m_nCurrentFrame = 0;		// �t���[���̃��Z�b�g
				}
			}

			// �L�[���̍ő�l�ɒB�����ꍇ
			if (m_nCurrentKey >= m_pAnimation[m_nAnimationType].nMaxKey)
			{
				BehaviorForMaxKey();
			}
		}
	}
}

//=============================================================================
// �A�j���[�V�����̐؂�ւ�
//=============================================================================
void CCharacter::AnimationSwitch(int nType)
{
	m_nAnimationTypeOld = m_nAnimationType;								// �O�̃A�j���[�V�����ɍ��̃A�j���[�V����������
	m_nAnimationType = nType;												// �A�j���[�V�����̐؂�ւ�
	m_nCurrentFrame = 0;												// ���݂̃t���[���������Z�b�g
	m_nCurrentKey = 0;													// ���݂̃L�[�������Z�b�g
	m_bAnimation = true;
}

//=============================================================================
// �L�[���ƃt���[�����̃��Z�b�g
//=============================================================================
void CCharacter::ResetKeyAndFrame(void)
{
	m_nCurrentKey = 0;
	m_nCurrentFrame = 0;
}

#ifdef _DEBUG
//=============================================================================
// �f�o�b�O����
//=============================================================================
void CCharacter::Debug(void)
{

}
#endif