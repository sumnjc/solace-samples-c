# Solace C API 체계적 학습 플랜 (4주 완성)

이 문서는 Solace C API(`solClient`)를 마스터하기 위한 단계별 교육 과정을 담고 있습니다.

---

## 1주차: Solace 핵심 개념 및 기본기 다지기
**목표:** Solace의 3대 메시징 패턴(Direct, Persistent, Request/Reply) 이해 및 구현.

### Day 1: Direct Messaging (Fire & Forget)
- **개념:** 고속 전송, 소실 가능성 있는 메시징.
- **실습:** `TopicPublisher.c`, `TopicSubscriber.c` 분석 및 실행.
- **과제:** 토픽 계층 구조(`a/b/c`)와 와일드카드(`a/>`, `a/*/c`) 구독 테스트.

### Day 2: Request/Reply (동기 vs 비동기)
- **개념:** 요청-응답 패턴의 구현 방식 차이.
- **실습:** `BasicRequestor.c` vs `AsyncRequestor.c` 비교 분석.
- **과제:** `BasicReplier`를 수정하여 특정 요청에만 응답하거나 에러 응답 보내기.

### Day 3: Guaranteed Messaging (Persistence) 기초
- **개념:** 메시지 손실 방지, Queue 기반 메시징.
- **실습:** `QueuePublisher.c`, `QueueSubscriber.c` 실행.
- **과제:** 브로커 재시작 후에도 메시지가 유지되는지 확인.

### Day 4: Solace API 구조 이해 (Context & Session)
- **이론:** Context Threading Model, Session 관리.
- **실습:** 하나의 Context 내 멀티 세션 구성 테스트.

### Day 5: 1주차 미니 프로젝트
- **주제:** "간단한 채팅 프로그램 만들기" (Pub/Sub 활용)

---

## 2주차: 고급 기능 및 실무 패턴
**목표:** 시스템 안정성 확보 및 고급 라우팅 기능 활용.

### Day 1: Topic to Queue Mapping
- **개념:** Pub/Sub의 유연성과 Queue의 안정성 결합.
- **실습:** `TopicToQueueMapping.c` 분석.
- **과제:** 특정 Topic 메시지를 Queue로 라우팅하여 수신하기.

### Day 2: Flow & Acknowledgement (ACK)
- **개념:** 클라이언트 측 메시지 수신 확인 절차.
- **실습:** `QueueSubscriber`에서 `CLIENT_ACK` 모드 적용 및 수동 ACK 호출.
- **과제:** ACK 미발생 시 재전송(Redelivery) 동작 확인.

### Day 3: Message & User Properties
- **개념:** 표준 헤더(TTL, DMQ) 및 커스텀 메타데이터(Map/Stream).
- **실습:** 메시지에 `SenderID`, 커스텀 `OrderType` 등 삽입.

### Day 4: Error Handling & Event Callback
- **개념:** 네트워크 장애 및 재연결(Reconnect) 대응.
- **실습:** `SessionEventCallback`에서 각종 Notice/Error 이벤트 로그 분석.

### Day 5: 2주차 미니 프로젝트
- **주제:** "주문 처리 시스템" (주문 발행 -> 큐 저장 -> 작업자 처리 및 ACK)

---

## 3주차: 성능 최적화 및 특수 기능
**목표:** 대용량 처리 최적화 및 특수 기능(Replay 등) 활용.

### Day 1: Non-Blocking Send & Vector Send
- **개념:** 대량 전송 시 병목 제거 및 `WOULD_BLOCK` 처리.

### Day 2: Transacted Session
- **개념:** 여러 작업을 원자적으로 처리 (All or Nothing).
- **실습:** C API의 로컬 트랜잭션 세션 구현 방식 학습.

### Day 3: Message Replay
- **개념:** 브로커에 저장된 과거 메시지 재수신.
- **실습:** `MessageReplay.c` 실행 및 특정 시점 기반 리플레이 테스트.

### Day 4: Request/Reply with Queue
- **개념:** 응답을 Temporary Topic이 아닌 Durable Queue로 받기 (비동기 서비스 간 통신).

### Day 5: 3주차 미니 프로젝트
- **주제:** "로그 수집기" (토픽 기반 수집, 중요 로그 큐 저장, 장애 시 리플레이 복구)

---

## 4주차: 종합 프로젝트 및 아키텍처
**목표:** 실무 수준의 분산 시스템 설계 및 구현.

### Day 1~2: 프로젝트 설계
- **주제:** **"분산 작업 처리 시스템 (Distributed Job Scheduler)"**
- **구조:**
    - Client: 비동기 작업 요청.
    - Queue: 작업 대기열 (부하 분산).
    - Worker: 여러 대의 작업자가 큐에서 메시지를 가져와 처리 후 응답.

### Day 3~4: 종합 구현
- `TopicToQueueMapping` 기반 작업 분류.
- `CorrelationId`를 이용한 비동기 매칭.
- 작업 실패 시 재처리(Retry) 전략 적용.

### Day 5: 회고 및 마무리
- 코드 리뷰, 성능 테스트 결과 정리, 배운 점 문서화.
