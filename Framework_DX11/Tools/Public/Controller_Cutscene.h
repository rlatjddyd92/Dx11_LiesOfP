#pragma once

#include "Tools_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_Cutscene : public CBase
{
	DECLARE_SINGLETON(CController_Cutscene)

public:
	typedef struct
	{
        int mFrameMin = { 0 };
        int mFrameMax = { 50 };

	}CUTSCENE;

    static const char* SequencerItemTypeNames[5];

    struct MySequence : public ImSequencer::SequenceInterface
    {
        // interface with sequencer

        virtual int GetFrameMin() const {
            return mFrameMin;
        }
        virtual int GetFrameMax() const {
            return mFrameMax;
        }
        virtual int GetItemCount() const { return (int)myItems.size(); }

        virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
        virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
        virtual const char* GetItemLabel(int index) const
        {
            static char tmps[512];
            snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
            return tmps;
        }

        virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
        {
            MySequenceItem& item = myItems[index];
            if (color)
                *color = 0xFFAA8080; // same color for everyone, return color based on type
            if (start)
                *start = &item.mFrameStart;
            if (end)
                *end = &item.mFrameEnd;
            if (type)
                *type = item.mType;
        }
        virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
        virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
        virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

        virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

        // my datas
        MySequence() : mFrameMin(0), mFrameMax(0) {}
        int mFrameMin, mFrameMax;
        struct MySequenceItem
        {
            int mType;
            int mFrameStart, mFrameEnd;
            bool mExpanded;
        };
        std::vector<MySequenceItem> myItems;

    };

private:
	CController_Cutscene();
	virtual ~CController_Cutscene() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update(_float fTimeDelta);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
    CUTSCENE* m_CurSequence = {};
    _bool m_bFirst = { true };

private:
	void Show_CutScene_List();	//리스트 보여주기 + 새로 생성까지
	void CutScene_Detail();		//컷신 세부 설정들 확인 및 수정


    CUTSCENE m_SequenceList[8] = {};
    const char* m_CutSceneNameList[8] = { "Boss1_FirstMeet", "Boss1_Phase2", "Boss1_Dead", "Sophia_EnterRoom", "Boss2_Meet",  "Boss2_Phase2",  "Boss2_Defeat", "Sophia_Dead"};

public:
	virtual void Free() override;
};

END

const char* CController_Cutscene::SequencerItemTypeNames[] = {
    "Type1",
    "Type2",
    "Type3"
};