//=============================================================================
//
// サウンド処理 [sound.h]
// Author : masayasu wakita
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================================================================
// インクルードファイル
//=============================================================================
#include "main.h"

//=============================================================================
// 構造体定義
//=============================================================================
typedef struct
{
	char *pFilename;			// ファイル名
	int nCntLoop;				// ループカウント
} SOUNDPARAM;

//=============================================================================
// サウンドファイル
//=============================================================================
typedef enum
{
	SOUND_LABEL_BGM_TiTle = 0,				// タイトル
	SOUND_LABEL_BGM_Character_Select,		// キャラクターセレクト
	SOUND_LABEL_BGM_Puzzle,					// パズル
	SOUND_LABEL_BGM_Race,					// レース
	SOUND_LABEL_BGM_Result,					// リザルト
	SOUND_LABEL_SE_Car_Near_Ring,			// 車の近くになったら鳴らす
	SOUND_LABEL_SE_Cheers,					// 歓声
	SOUND_LABEL_SE_Cursor,					// カーソル
	SOUND_LABEL_SE_CurveWarning,			// カーブ注意
	SOUND_LABEL_SE_Decision,				// 決定
	SOUND_LABEL_SE_Draft_1,					// ドラフト1
	SOUND_LABEL_SE_Draft_2,					// ドラフト2
	SOUND_LABEL_SE_Draft_3,					// ドラフト3
	SOUND_LABEL_SE_EndPuzzle_Standby,		// パズルが完了してスタンバイ
	SOUND_LABEL_SE_GearChange,				// ギアチェンジ
	SOUND_LABEL_SE_Horn,					// クラクション
	SOUND_LABEL_SE_MaxSpeed,				// マックススピード
	SOUND_LABEL_SE_PartsInset_StatusBig,	// ピースセット(ステータス大)
	SOUND_LABEL_SE_PartsInset_StatusSmall,	// ピースセット(ステータス小)
	SOUND_LABEL_SE_PartsInset_UIBig,		// ピースセット(UI大)
	SOUND_LABEL_SE_PartsInset_UISmall,		// ピースセット(UI小)
	SOUND_LABEL_SE_PullOut_1,				// 抜き去り1
	SOUND_LABEL_SE_PullOut_2,				// 抜き去り2
	SOUND_LABEL_SE_PullOut_3,				// 抜き去り3
	SOUND_LABEL_SE_PuzzleComplete_1,		// パズル完成1
	SOUND_LABEL_SE_PuzzleComplete_2,		// パズル完成2
	SOUND_LABEL_SE_RaceStart,				// レース開始
	SOUND_LABEL_SE_RaceStart_Overlapping,	// レース開始重ね
	SOUND_LABEL_SE_Speed50Km,				// スピード50キロになったら
	SOUND_LABEL_SE_Speed100Km,				// スピード100キロになったら
	SOUND_LABEL_SE_SpeedUpPanel,			// スピードパネル
	SOUND_LABEL_SE_StopHolding,				// 停止
	SOUND_LABEL_SE_WithParts,				// パーツ付け
	SOUND_LABEL_SE_kyuin,					// きゅいん
	SOUND_LABEL_MAX
} SOUND_LABEL;

//=============================================================================
// クラス定義
//=============================================================================
class CSoundCallback : public IXAudio2VoiceCallback {
public:
	CSoundCallback() { }
	~CSoundCallback() { }
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired) { }
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd(void) { }
	void STDMETHODCALLTYPE OnStreamEnd(void) { SetEvent(m_hEvent); }
	void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) { SetEvent(m_hEvent); }
	void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) { }
	void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) { }
	void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT Error) { }
private:
	HANDLE m_hEvent = NULL;
};

class CSound
{
public:
	CSound();
	~CSound();
	HRESULT Init(HWND hWnd);
	void Uninit(void);
	void Update(void);

	void SetVolume(SOUND_LABEL label, float fVolume);

	HRESULT PlaySound(SOUND_LABEL label);
	void StopSound(SOUND_LABEL label);
	void StopSound(void);

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice = NULL;			// マスターボイス
	IXAudio2SubmixVoice *m_pSubmixVoice = NULL;					// サブミックスボイス
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス

	BYTE *m_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

	// 各音素材のパラメータ
	SOUNDPARAM m_aParam[SOUND_LABEL_MAX] =
	{
		{ "data/sound/bgm/TiTle.wav", -1 },						// タイトル
		{ "data/sound/bgm/CharacterSelect.wav", -1 },			// キャラクターセレクト
		{ "data/sound/bgm/Puzzle.wav", -1 },					// パズル
		{ "data/sound/bgm/race.wav", -1 },						// レース
		{ "data/sound/bgm/Result.wav", -1 },					// リザルト
		{ "data/sound/se/car_near_Ring.wav", 0 },				// 車の近くになったら鳴らす
		{ "data/sound/se/Cheers.wav", 0 },						// 歓声
		{ "data/sound/se/Cursor.wav", 0 },						// カーソル
		{ "data/sound/se/CurveWarning.wav", 0 },				// カーブ注意
		{ "data/sound/se/Decision.wav", 0 },					// 決定
		{ "data/sound/se/draft_1.wav", 0 },						// ドラフト1
		{ "data/sound/se/draft_2.wav", 0 },						// ドラフト2
		{ "data/sound/se/draft_3.wav", 0 },						// ドラフト3
		{ "data/sound/se/EndPuzzle_Standby.wav", 0 },			// パズルが完了してスタンバイ
		{ "data/sound/se/GearChange.wav", 0 },					// ギアチェンジ
		{ "data/sound/se/Horn.wav", 0 },						// クラクション
		{ "data/sound/se/MaxSpeed.wav", -1 },					// マックススピード
		{ "data/sound/se/PartsInset_StatusBig.wav", 0 },		// ピースセット(ステータス大)
		{ "data/sound/se/PartsInset_StatusSmall.wav", 0 },		// ピースセット(ステータス小)
		{ "data/sound/se/PartsInset_UIBig.wav", 0 },			// ピースセット(UI大)
		{ "data/sound/se/PartsInset_UISmall.wav", 0 },			// ピースセット(UI小)
		{ "data/sound/se/PullOut_1.wav", 0 },					// 抜き去り1
		{ "data/sound/se/PullOut_2.wav", 0 },					// 抜き去り2
		{ "data/sound/se/PullOut_3.wav", 0 },					// 抜き去り3
		{ "data/sound/se/PuzzleComplete_1.wav", 0 },			// パズル完成1
		{ "data/sound/se/PuzzleComplete_2.wav", 0 },			// パズル完成2
		{ "data/sound/se/RaceStart.wav", 0 },					// レース開始
		{ "data/sound/se/RaceStart_Overlapping.wav", 0 },		// レース開始重ね
		{ "data/sound/se/Speed50Km.wav", 0 },					// スピード50キロになったら
		{ "data/sound/se/Speed100Km.wav", 0 },					// スピード100キロになったら
		{ "data/sound/se/SpeedUpPanel.wav", 0 },				// スピードパネル
		{ "data/sound/se/StopHolding.wav", 0 },					// 停止
		{ "data/sound/se/WithParts.wav", 0 },					// パーツ付け
		{ "data/sound/se/キュイン_01.wav", 0 },					// きゅいん							
	};
};
#endif