STM32 SPI with STM32CubeMX and STM32F407VET6
--------------------------------------------

### SPI란?

#### SPI(Serial Peripheral Interface)

: Motorola에 의해 개발된 **full-duplex 통신** 이 가능한 동기 통신 규격

```
  full-duplex 통신 : 전이중 통신. 데이터의 송신과 수신이 동시에 가능한 통신
```

-	Master-Slave 방식으로 동작하며 하나의 Master와 하나 이상의 Slave가 연결되는 구조
-	Master에 의해 클럭이 출력되며 클럭에 의해 동기화된 직렬 통신 방식
-	4개의 신호로 구성(SCLK, MOSI, MISO, NSS)

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_structure.JPG" width="300" height="200"></img>

-	SPI의 구성 핀 설명
	-	SCLK : Master에서 출력하는 Slave와 동기화하기 위한 클럭 핀
	-	MOSI : Master Output Slave Input. Master에서는 출력, Slave에서는 입력으로 사용하는 핀
	-	MISO : Master Input Slave Output. Master에서는 입력, Slave에서는 출력으로 사용하는 핀
	-	NSS : Master에서 통신할 하나의 Slave를 선택하는 핀<br><br>

#### STM32L0 SPI 통신의 특징

-	Master 또는 Slave 모드로 동작
-	Full-duplex / Half-duplex / Simplex synchronous 전송 가능
-	8-bit / 16-bit transfer frame format 선택
-	MSB-first 또는 LSB-first shifting으로 Data 전송 순서 설정 가능
-	**Clock polarity** 와 **phase** 설정 가능<br><br>

#### Clock Polarity와 Phase 제어

-	CPOL(Clock Polarity)

	-	CPOL - 0 : Idle 상태에서 clock의 극성이 LOW
	-	CPOL - 1 : Idle 상태에서 clock의 극성이 HIGH

-	CPHA(Clock Phase)

	-	CPHA - 0 : clock의 첫번째 edge에서 전송된 첫번째 data를 capture
	-	CPHA - 1 : clock의 두번째 edge에서 전송된 첫번째 data를 capture<br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/clock_polarity_phase.JPG" weight="700" height="300"></img><br><br>

#### STM32F407VET6 보드를 이용한 SPI 통신 구현

-	SPI 채널 세팅
	-	SPI2 채널과 SPI3 채널 사이의 통신 구현
	-	SPI2의 모드를 전이중 통신이 가능한 Full-Duplex Master로 세팅
	-	SPI3의 모드를 전이중 통신이 가능한 Full-Duplex Slave로 세팅

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_mode_set.JPG" weight="500" height="200"></img><br><br>

-	SPI 채널 세팅 결과 설정된 핀

	-	SPI2
		-	SPI2_MISO : PC2
		-	SPI2_MOSI : PC3
		-	SPI2_SCLK : PB10
	-	SPI3
		-	SPI3_MISO : PC11
		-	SPI3_MOSI : PC12
		-	SPI3_SCLK : PC10

	※ Slave는 하나이므로 NSS 핀은 필요 없음

	<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_mode_set_result.JPG" weight="500" height="300"></img><br><br>

-	SPI Configuration

	-	Configuration 탭에서 SPI2, SPI3에 대한 Configuration하기
	-	SPI2, SPI3 Configuration
	-	DMA Settings 탭에서 Add 버튼 클릭하여 각각 RX, TX 핀 DMA_REQUEST에 추가

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_spi2_configuration.JPG" weight="500" height="300"></img><br><br>

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_spi3_configuration.JPG" weight="500" height="300"></img><br><br>

-	Hardware 설정
	-	SPI2와 SPI3의 각각의 MOSI, MISO, SCLK 핀을 서로 연결해준다.
	-	PC2(SPI2_MISO) - PC11(SPI3_MISO)
	-	PC3(SPI2_MOSI) - PC12(SPI3_MOSI)
	-	PB10(SPI2_SCLK) - PC10(SPI3_SCLK)

<img src="https://github.com/SongKJ00/Drone/blob/master/TIL/TIL_SongKJ/0924_spi_loopback/image/spi_hardware.jpg" weight="500" height="300"></img><br><br>

-	Code

Master와 Slave에서 전송할 데이터인 Tx Buffer

<pre>
/* Master, Slave Tx Buffer*/
uint8_t master_buffer_tx[] = "STM32F4 SPI Master Tx";
uint8_t slave_buffer_tx[] = "STM32F4 SPI Slave Tx";
</pre><br>


Tx Buffer의 Size

<pre>
\# define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(\*(__BUFFER__)))
\# define SLAVE_TX_SIZE (COUNTOF(slave_buffer_tx))
\# define MASTER_TX_SIZE (COUNTOF(master_buffer_tx))
</pre><br>


Master와 Slave에서 수신할 데이터인 Rx Buffer

<pre>
/* Master, Slave Rx Buffer*/
uint8_t master_buffer_rx[SLAVE_TX_SIZE];
uint8_t slave_buffer_rx[MASTER_TX_SIZE];
</pre><br>


시작을 위한 시작 버튼(PE4) 폴링 체크

<pre>
/* Wait for the button event*/
while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) != GPIO_PIN_RESET);
</pre><br>


Master에서 Transmit, Slave에서 Receive하기

<pre>
/* Receive the data from SPI2-Master*/
if(HAL_SPI_Receive_DMA(&hspi3, (uint8_t*)slave_buffer_rx, MASTER_TX_SIZE) != HAL_OK)
{
  /* Error Handler*/
}

/* Send the data to SPI3-Slave*/
if(HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)master_buffer_tx, MASTER_TX_SIZE) != HAL_OK)
{
  /* Error Handler*/
}
</pre><br>


데이터 송수신이 완료되었는지 확인

<pre>
/* Check the SPI tx, rx state*/
while(HAL_SPI_GetState(&hspi3) != HAL_SPI_STATE_READY);
while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
</pre><br>


Slave에서 Transmit, Master에서 Receive하기

<pre>
/* Send the data to SPI2-master*/
if(HAL_SPI_Transmit_DMA(&hspi3, (uint8_t*)slave_buffer_tx, SLAVE_TX_SIZE) != HAL_OK)
{
  /* Error Handler*/
}

/* Receive the data to SPI3-slave*/
if(HAL_SPI_Receive_DMA(&hspi2, (uint8_t*)master_buffer_rx, SLAVE_TX_SIZE) != HAL_OK)
{
  /* Error Handler*/
}
</pre><br>

데이터 송수신이 완료되었는지 확인

<pre>
/* Check the SPI tx, rx state*/
while(HAL_SPI_GetState(&hspi3) != HAL_SPI_STATE_READY);
while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
</pre><br>

데이터 송수신 후 LED(PA6) Toggle

<pre>
  /* Infinite loop*/
  /* USER CODE BEGIN WHILE*/
  while(1)
  {
    /* USER CODE END WHILE*/

    /* USER CODE  BEGIN 3*/
    /* verify the transmission completion using LED*/
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
    HAL_Delay(500);
  }
  /* USER CODE END 3*/
</pre>

사용한 ToolChain은 IAR 사의 EWARM입니다.
