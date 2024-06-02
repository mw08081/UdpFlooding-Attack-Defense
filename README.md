
<h1>방어하지 않는 udp서버</h1>

![game_server](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/2c3976bf-e755-468e-a2c8-2f9a507d772b) 

↓ 공격당하면 이렇게 됨
![attacked_game_server](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/ad347c8a-9e79-4ea3-9ac4-a1d8743a3e6f) 

<h1>방어기법이 적용된 udp서버</h1>
<p>(아이디어 : 특정 시간동안 n회이상 패킷을 보내면 통과)</p>

↓ 다이나믹 화이트리스트 업데이트
![enhanced_server](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/0964804f-7ab8-40e9-b488-6500074e3bcb) 

↓ 다이나믹 화이트리스트 업데이트2
![enhanced_server2](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/92708b1b-5274-4f35-95fb-e3be030e4ced)
↓ 방어기법이 적용되어 통신 끊김이 없음
![enhanced_server_attacked](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/4ed64c73-7afa-4749-9d1a-e26ee66a94ac)


![그림1](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/4436fde8-8ab1-4f71-8c48-045ab009da84)
![그림2](https://github.com/mw08081/UdpFlooding-Attack-Defense/assets/58582985/1102d16c-4f69-40eb-b601-9d47bf6c93f9)

동일한 아이피와 포트번호로 계속 보낸다면? → 레이트 리미팅 기법 적용
