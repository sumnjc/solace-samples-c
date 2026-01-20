  1단계: Solace 브로커 실행 (Docker)
  먼저 메시지 중계 역할을 할 브로커를 띄웁니다. (이미 실행 중이라면 생략 가능)

   1 docker run -d -p 8080:8080 -p 55555:55555 -p 8008:8008 --shm-size=1g --env username_admin_globalaccesslevel=admin --env username_admin_password=admin --name solace-broker
     solace/solace-pubsub-standard
   * 브로커가 완전히 켜지는 데 30~60초 정도 걸릴 수 있습니다.

  2단계: 라이브러리 경로 설정
  프로그램이 Solace 라이브러리(libsolclient.so)를 찾을 수 있도록 환경 변수를 설정합니다.

   1 export LD_LIBRARY_PATH=$(pwd)/solace-samples-c/lib/linux/x64

  3단계: 구독자 (Subscriber) 실행
  메시지를 받을 준비를 합니다. 이 명령어를 실행하면 프로그램이 대기 상태에 들어갑니다.

   1 # 사용법: <IP:Port> <VPN> <User> <Password> <Topic>
   2 ./solace-samples-c/bin/TopicSubscriber localhost:55555 default default "" "tutorial/topic"

  4단계: 게시자 (Publisher) 실행
  새로운 터미널 창을 열어(2단계의 경로 설정을 다시 해야 함) 메시지를 보냅니다.

   1 export LD_LIBRARY_PATH=$(pwd)/solace-samples-c/lib/linux/x64
   2 ./solace-samples-c/bin/TopicPublisher localhost:55555 default default "" "tutorial/topic"

  성공하면 Publisher 쪽에서는 "Message sent"가 출력되고, Subscriber 쪽에서는 받은 메시지 내용("Hello world!")이 출력됩니다.