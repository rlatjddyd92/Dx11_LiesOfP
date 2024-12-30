#pragma once
#include "Base.h"
#include "iostream"

class CMyProfilerCallback : public physx::PxProfilerCallback
{
public:
    // zoneStart: 프로파일링 영역 시작 시 호출
    void* zoneStart(const char* eventName, bool detached, uint64_t contextId) override
    {
        // 이벤트 이름과 컨텍스트 ID를 로그에 기록
        std::cout << "Zone Started: " << eventName << " (ContextId: " << contextId << ")" << std::endl;
        // 실제 데이터를 추적할 수 있는 구조체나 변수 반환 (여기서는 예시로 NULL 반환)

        return nullptr;
    }

    // zoneEnd: 프로파일링 영역 종료 시 호출
    void zoneEnd(void* profilerData, const char* eventName, bool detached, uint64_t contextId) override
    {
        // 프로파일링 영역의 종료를 로그에 기록
        std::cout << "Zone Ended: " << eventName << " (ContextId: " << contextId << ")" << std::endl;
        // profilerData는 zoneStart에서 반환된 데이터로, 이를 통해 성능 정보를 추적할 수 있습니다.
    }
};