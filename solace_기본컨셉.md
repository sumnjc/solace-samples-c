# Solace 핵심 개념 정리

Solace 메시징 모델을 구성하는 5가지 주요 요소(Context, Session, Topic, Queue, Flow)에 대한 설명입니다.

---

### 1. Context (컨텍스트): "환경/그릇"
Solace API가 동작하기 위한 **실행 환경이자 최상위 컨테이너**입니다.

*   **역할:** 하나 이상의 **Session**을 생성하고 관리하며, 네트워크 통신 처리를 담당하는 스레드(Context Thread)를 제공합니다.
*   **특징:**
    *   **스레드 모델:** 기본적으로 Context 하나당 하나의 내부 스레드가 생성되어, 그 안의 모든 Session에서 발생하는 수신 메시지와 이벤트를 처리(Callback 호출)합니다.
    *   **격리:** 필요에 따라 여러 Context를 만들어 스레드와 리소스를 물리적으로 분리할 수 있습니다.
*   **C API:** `solClient_context_create()`

### 2. Session (세션): "통로"
Context 내에서 생성되며, 클라이언트 애플리케이션과 Solace 브로커 사이의 **물리적/논리적 연결**입니다.

*   **역할:** 실제 메시지를 주고받는 통로 역할을 합니다.
*   **주요 정보:** Host IP, VPN 이름, 사용자 인증 정보(ID/PW), 압축 설정 등.
*   **C API:** `solClient_session_create()`, `solClient_session_connect()`

### 3. Topic (토픽): "주소표/라벨"
메시지가 어디로 가야 할지 나타내는 **논리적인 주소**입니다.

*   **역할:** 발행(Publish) 시 메시지에 붙이는 이름표입니다.
*   **특징:**
    *   **계층 구조:** `/` 구분자를 사용한 계층적 설계 가능 (예: `SEOUL/GYEONGGI/ANYANG`).
    *   **와일드카드:** `*`(단일 레벨), `>`(멀티 레벨)를 이용한 유연한 구독.
    *   **휘발성:** 저장소가 없으므로 구독자가 없으면 메시지는 즉시 사라집니다 (Direct Messaging).

### 4. Queue (큐): "우편함/창고"
메시지를 안전하게 보관하는 **물리적인 저장소**입니다.

*   **역할:** 수신자가 오프라인일 때도 메시지를 유실 없이 보관합니다 (Guaranteed Messaging).
*   **특징:**
    *   **영속성(Persistence):** 처리 완료(Ack) 전까지 디스크/메모리에 안전하게 보관.
    *   **순서 보장:** FIFO(First-In-First-Out) 방식의 처리.
    *   **부하 분산:** 하나의 큐에 여러 수신자를 두어 메시지를 나누어 처리 가능.

### 5. Flow (플로우): "수도꼭지/흐름 제어"
세션 내에서 큐로부터 메시지를 끌어오는 **논리적인 흐름**입니다.

*   **역할:** 큐와 클라이언트를 연결하여 안정적인 메시지 전송을 제어합니다.
*   **특징:**
    *   **Ack 관리:** 메시지 수신 확인(Acknowledgement) 메커니즘 제공.
    *   **흐름 제어:** Window Size 설정을 통해 클라이언트가 처리 가능한 속도로 수신 조절.
*   **C API:** `solClient_session_createFlow()`

---

### [요약 비교] 토픽 vs 큐

| 구분 | Topic (토픽) | Queue (큐) |
| :--- | :--- | :--- |
| **메시징 타입** | Direct Messaging (고속) | Guaranteed Messaging (고신뢰) |
| **데이터 보존** | 즉시 전달 후 소멸 | 처리 완료(Ack) 전까지 보존 |
| **전달 방식** | 1:N (Broadcasting) | 1:1 또는 1:N 분할 처리 |
| **비유** | 실시간 라디오 방송 | 개인용 우편함/물류 창고 |

### [구조도]
```
Application
  └─ Context (Thread)
       └─ Session (Connection)
            ├─ Direct Message (Topic)
            └─ Flow (Queue Consumer)
```