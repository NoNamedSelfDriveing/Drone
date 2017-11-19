STM32 PWM with STM32CubeMX and STM32F407VET6
--------------------------------------------

### STM32F407VET6 블록 다이어그램 중 APB1/APB2 Clock - peripheral, timer

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/block_diagram.JPG" width="500" height="300"></img>

[그림 1]<br><br>

APB1와 연결된 Timer 채널 : TIM2, TIM3, TIM4, TIM5, TIM12, TIM13, TIM14

APB2와 연결된 Timer 채널 : TIM1, TIM8, TIM9, TIM10. TIM11<br><br>

TIM1 채널을 통한 PWM Pulse 출력
-------------------------------

### CubeMX

#### TIM1 Pin, Mode Setting

-	내부 클럭을 사용할 것이므로 Clock Source를 Internal Clock으로 세팅
-	Channel 1에 PWM Pulse를 출력할 것이므로 PWM Generation CH1으로 세팅<br><br>

	<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/tim1_mode_set.JPG" width="300" height="300"></img>

	[그림 2]<br><br>

TIM1 Configuration 상단의 Configuration 탭을 클릭하면 설정한 TIM1에 대한 세부 설정을 할 수 있는 화면이 나온다.

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/configuration_tab.jpg" width="500" height="300"></img>

[그림 3]<br><br>

강조된 부분을 클릭하면 다음과 같은 창이 뜬다.

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/tim1_configuration.JPG" width="400" height="400"></img>

[그림 4]<br><br>

이 부분이 이제 바로 TIM1에 대해 세부 설정을 할 수 있는 곳인데 기본적인 PWM Pulse 출력을 위해서는 Counter Setting 탭에 **Prescaler** , **Counter Period** 를 필수적으로 설정해주어야 한다.

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/counter_settings.JPG" width="800" height="200"></img>

[그림 5]<br><br><br>

##### Prescaler

Prescaler란 클럭에 대한 분주비이다. Timer1를 사용하기 위해서는 클럭이 필요한데 현재 사용하려는 TIM1은 위에서 설명했던 거와 같이 APB2와 연결되어 있다. <br><br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/apb2_timer_clock.JPG" width="400" height="150"></img>

[그림 6]

(이 내용은 상단의 Clock Configuration 탭을 클릭하여 볼 수 있다.)<br><br>

TIM1가 사용하는 클럭은 APB2 timer clocks라는 곳에서 출력하는 클럭인데 클럭의 주파수가 무려 16MHz이다. 이는 TIM1이 사용하기에는 너무 빠른 클럭이며 제대로 된 PWM Pulse를 출력하기 어렵다. (이유를 과장해서 말하면 PWM Pulse의 주기가 1/16M 초라는 것은 핵폭탄급 PWM Pulse이기 때문이다.(CPU도 이런 PWM Pulse를 출력하다가 어지러워 죽지 않을까....) )

따라서, 우리가 원하는 PWM Pulse를 사용하기 위해서는 APB2 timer clocks에서 출력되는 클럭을 조금 줄여서 사용할 필요가 있다. 이때 사용하는 것이 바로 **Prescaler** 이다. APB2 timer clocks에서 출력되는 클럭을 Prescaler 값만큼으로 나눠서(Divide) TIM1이 더욱 효율적인 클럭을 사용할 수 있게 된다.

위에서 필자는 Prescaler 값을 16으로 설정했는데 이는 입력되는 16MHz 클럭을 16으로 나눠서 사용하니 실제 TIM1이 사용하는 클럭의 주파수는 1MHz이다.<br><br><br>

##### Counter Period

Counter Period를 설정한다는 것은 어떤 기준점(한계점)을 설정한다는 것과 똑같다. 다른 컨트롤러에서도 마찬가지로 PWM 설정을 할 떄 첫번째로 Prescaler 값을 결정하고 두번째로 하는 것이 Timer 레지스터 값 설정이다. Timer 레지스터는 클럭 펄스가 한 번 실행될 때마다 자동으로 레지스터 값이 1만큼 증가된다.

현재 TIM1에 Channel 1을 PWM Generation CH1으로 설정하였는데 이는 Timer 레지스터가 점점 증가하다가 그 값이 Counter Period의 값과 똑같아지면 다시 Timer 레지스터가 0으로 초기화된다. 실제로 [그림 5]를 보면 Counter Period를 보면 옆에 (AutoReload Register)이라고 적혀있는데 이는 Timer 레지스터 값이 Counter Period 값과 같아지면 자동으로 0으로 초기화된다는 것을 의미한다.

한편, 이 순간 즉, Timer 레지스터 값과 Counter Period 값이 같아져 AutoReload되는 순간 새로운 PWM Pulse Cycle이 출력된다. 즉, LOW에서 HIGH로 변하는 **Rising Edge** 가 실행된다는 말이다.

현재 [그림 5]와 같이 Counter Period 값을 1000으로 설정해 놓았는데 이는 클럭 펄스에 맞춰 증가하던 Timer 레지스터 값이 1000이 되는 순간 다시 AutoReload된다.<br><br><br>

##### Prescaler와 Counter Period로 PWM Pulse 주파수 구하기

현재 Prescaler 값을 16으로 두었으니 TIM1이 사용하는 클럭의 주파수는 1MHz이며 Counter Period 값을 1000으로 두었으니 Timer 레지스터 값이 1000이 되면 다시 0으로 AutoReload된다는 것을 위에서 설명했었다.

그렇다면 PWM Pulse의 주파수를 어떻게 하면 구할 수 있을까?

단순히 공식부터 쓰자면 이렇다.

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/pwm_pulse_freq.JPG"></img>

[그림 7]

위 수식에 대한 이해는 별로 어렵지 않다. 위에서 사용했던 수치를 그대로 예를 들어보겠다. 일단 기존의 클럭 주파수는 16MHz이다. 이것을 16 Prescaler로 나눴으니 실제 사용하는 클럭은 1MHz이다. Timer 레지스터는 하나의 클럭 펄스 당 값이 1씩 증가하는데, Counter Period 값을 1000으로 설정하여 Timer 레지스터 값이 1000이 되면 Rising Edge가 실행되므로 새로운 PWM Pulse Cycle을 위해서는 1000개의 클럭 펄스가 필요하다는 것이다. 따라서, 1MHz 클럭에서 1000개의 클럭 펄스가 있어야 하나의 PWM Pulse Cycle이 생기므로 PWM Pulse의 주파수는 1MHz / 1000 = 1KHz 가 된다.

**주파수 = 1 / 주기** 이므로 PWM Pulse의 주기도 구할 수 있다(주파수와 주기는 서로 역수 관계이다). 즉, 몇 초마다 PWM Pulse Cycle이 생기는지 알 수 있다는 것이다. 위에서 구한 주파수는 1KHz이므로 주기는 1 / 1000 초, 즉 1ms이다.<br><br><br><br>

##### Duty Cycle 설정

PWM Pulse의 주기를 설정했다면 이제 남은 작업은 Duty Cycle 설정이다(매우 길어져서 점점 타이핑이 힘들어진다...) Duty Cycle이란 한 사이클 내에서 HIGH의 비중을 얼마만큼 두느냐는 것이다. Duty Cycle을 잘 설명할 수 있는 사진은 아래에 나와있다.

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/duty_cycle.JPG"></img>

[그림 8]

PWM이란 개념은 본디 펄스 폭 변조(Pulse Width Modulation)이다(너무 늦게 설명한 것인가...) PWM은 아날로그 신호를 디지털 형태로 나타낸 것이다. 그래서 PWM은 LED의 밝기나 모터의 속도 제어에 많이 사용된다(LED나 모터를 단순히 디지털 적으로 ON/OFF 시키는 것이 아닌 아날로그적으로 출력값을 조정하는 것이다.)

[그림 8]에 1번 파형을 보면 HIGH의 비중이 10%이기 때문에 10% Duty Cycle이라 표현되어 있고, 2번 파형을 보면 HIGH의 비중이 50%이기 때문에 50% Duty Cycle, 3번 파형은 HIGH의 비중이 90%이기 때문에 90% Duty Cycle이라고 적혀있다(LED의 밝기를 조절할 때 당연히 10% Duty Cycle보다는 50%, 더 넘어서는 90% Duty Cycle이 더 밝게 나오겠죠?)

그럼 CubeMX에서 Duty Cycle을 설정해보자. [그림 4]를 보면 맨 밑에 PWM Generation Channel 1이라고 다른 탭이 있는데 그 부분을 주목해야 한다. 이번 실습에는 30% Duty Cycle으로 PWM Pulse를 출력해보도록 하겠다.<br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/duty_cycle_settings.jpg"></img>

[그림 9]

30% Duty Cycle을 위해 **Pulse** 값을 300으로 설정했다. 30% Duty Cycle이라면 값을 30으로 해야 되는 것이 아닌가라고 생각할 수 있지만 사실 이 값은 **Counter Period** 의 영향을 받아 결정되는 값이다. [그림 5]에서 Counter Period를 1000으로 설정해놓았는데 이 의미는 1000개의 클럭 펄스가 들어오면 새로운 PWM Pulse Cycle을 출력하는 것이었다. 여기서 [그림 9]의 Pulse 값이 의미하는 것은 Counter Period 값만큼의 클럭 펄스가 필요할 떄 몇 개의 클럭 펄스까지 PWM Pulse에 HIGH를 유지할 것인가를 의미한다. 따라서 현재 Counter Period 값을 1000으로 설정했으므로 300개의 클럭 펄스가 출력되는 동안은 TIM1 Channer 1에서 HIGH를 출력하고 나머지 700개의 클럭 펄스가 출력되는 동안은 LOW를 출력한다.

1000개의 클럭 펄스에서 300개의 클럭 펄스가 출력되는 동안 HIGH가 출력되므로 30% Duty Cycle을 구현할 수 있게 되었다.<br><br><br>

### ToolChain Code

위와 같은 CubeMX 세팅을 모두 마쳤다면 이제 남은 건 동작 코드이다. CubeMX의 놀라운 **generate code** 기술 덕분에 우리가 작성할 코드는 별로 없다.(CubeMX에서 Project 탭에서 Generate Code 버튼을 누르면 프로젝트 저장 후 자동으로 코드가 생성된다.)

딱 한 줄이면 된다.

<pre>
/* USER CODE BEGIN 2 \*/
HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
/* USER CODE END 2 \*/
</pre>

<br>

##### HAL_TIM_PWM_Start(TIM_HandleTydeDef \*htim, uint32_t Channel)

이 함수는 PWM 파형을 출력하기 시작하는 함수이다.

-	함수 인자
	-	TIM_HandleTydeDef \*htim : Generate Code로 자동적으로 생성되는 TIM Handler이다.  
	-	uint32_t Channel : PWM을 출력하는 채널이다.<br><br>

코드 작성이 끝났다면 Make 후 보드에 Program한 다음 오실로스코프로 결과를 확인해보자(만약 오실로스코프가 없다면 LED를 통해서라도 결과를 확인해보자.)<br><br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/oscilloscope_1.jpg"></img>

[그림 10] PWM Pulse가 출력되고 있는 오실로스코프와 STM32F407VET6 보드<br><br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0925_pwm_tutorial/image/oscilloscope_2.jpg"></img>

[그림 11] PWM Pulse가 출력되고 있는 오실로스코프<br><br>

오실로스코프를 보면 가로로 한 칸의 크기가 1ms이므로 우리가 원했던 1ms 주기의 PWM Pulse가 제대로 출력되는 것을 확인할 수 있고, HIGH의 비중으로 보아 30% Duty Cycle이 제대로 형성된 것을 확인할 수 있다(LED로 실험하신다면 매우 약하게 켜지는 모습만 확인하실 수 있을 겁니다).
