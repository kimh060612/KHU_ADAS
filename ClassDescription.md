## OpenCV Class Guide

> #### 0. Data Type
-----------------------------------
이 파트는 기본적으로 OpenCV에서 사용하는 데이터 타입에 관해서 다룬다. 처음 보는 사람은 언어에 관계 없이 외계어처럼 보일 수 있기 때문에, 이를 숙지하는 것이 향후 소스를 이해하는데 있어 매우 중요한 요소가 된다. 참고로 여기서 언급되는 용어는 따로 언급이 없으면 찾아볼 것. 정말 기본적인 것은 생략함(예: unsigned char, 포인터 사용법 등).

* 기본 형식: CV_(bit 수)(C/C++ 기본 데이터 타입)C(Channel 수)  
  Ex. CV_8UC1 8 bit, unsigned char, 1 Channel Matrix  
  (Bit 수)의 종류: 8, 16, 32, 64  
  (사용 가능한 기본 데이터 타입 종류): U(unsigned), S(signed), F(floating point number)  
  (channel 수): 자연수. 근데 실사용은 3 channel이 대다수. (RGB, HSV)  

부가 설명: 대학 와서 수업 전부 제낀게 아닌 이상 Bit수 <=> Byte 수 생각할 것. 굳이 데이터 타입을 나누는 관점이 아니라 진짜 그 만큼의 공간이 할당되어 데이터의 정밀도를 표현하는 수단이다.  
Ex. 8 bit 데이터이면 256가지 숫자를 표현 가능 (2진수) 음수까지 확장하면 -128 ~ 127까지 가능 (signed)
unsigend 라면 0~255까지 표현 가능.

- ps. 여기서는 unsigned char를 uchar이라고 줄여서 사용 가능하다.

<br>

> #### 1. Scalar
-----------------------------------
이 클래스는 Opencv에서 사용하는 tuple이라고 생각하면 편하다.  

* 생성자:  
  0. 기본 생성자는 있는데 쓰지 말자. ㅋ;;  
  1. Type 1 생성자 (1, 2, ... , N)  
     1, 2, ..., N 이 들어간 tuple을 만들어 낸다. N <= 4 이다.  
  2. Type 2 생성자 Scalar_<data type> (...)  
     데이터 타입을 지정해서 만듬.  

<br>

> ### 2. VecXX
-----------------------------------
이 클래스는 위의 Scalar class의 부모 class이다. 그냥 튜플이라고 생각하면 편한데, 여기서 opencv의 행렬의 원소에 접근하는 방법이 보인다.  
Opencv에서는  다체널 행렬의 원소는 다음과 같이 저장된다.  

> [ (c1, c2, c3), (c1, c2, c3), ... (c1, c2, c3),  
>   (c1, c2, c3), (c1, c2, c3), ... (c1, c2, c3),  
>   (c1, c2, c3), (c1, c2, c3), ... (c1, c2, c3) ]  
이런 식으로 3channel 행렬이면 1 channel 행렬에 Vec3X의 튜플로 행렬의 y, x 위치에 저장된다.  

이 class의 기본 형식은 다음과 같다.  
* Vec(원소의 갯수)(데이터 형식)  
  이때, 원소의 갯수는 주로 2, 3, 4(이때 1은 있어도 의미가 없어서 없다.)를 사용하며, 데이터 형식은 i, b, f, s, w, d가 있다.  
  Ex. Vec3b, Vec4f  
  * i: integer
  * b: unsigned char
  * f: float
  * s: short
  * d: double
  * w: unsigned short

> #### 3. Mat
-----------------------------------
이 클래스는 행렬을 저장하는 용도로 사용된다. numpy의 행렬 객체와 비슷한 역할을 한다고 봐도 무방하다.  
대체적으로 이미지의 픽셀 정보는 저장할때 사용되는데, 그냥 행렬 객체로 써도 딱히 아무런 상관이 없다.  

* 생성자:  
  1. 기본 생성자 (아무 것도 없어도 됨.)  
  2. Type 1 생성자 (rows, columns, Data Type, 초기값)  
     이 타입의 생성자는 모든 1 Channel의 행렬을 rows*cols 크기로 만들고 초기값으로 모든 원소를 초기화함.  
  3. Type 2 생성자 (rows, columns, Data Type, Scalar(1,2,...,N))  
     이는 Scalar tuple을 rows * cols 갯수만큼 채운다.  
     Ex. Scalar(1, 2, 3) rows: 2 cols: 2  
     [ 1 2 3 1 2 3  
       1 2 3 1 2 3 ]  
  4. 나머지는 엇비슷함. 그냥 구글링 하십셔 휴먼  
<br>
* Mat class의 Method  
  1. create(rows, columns, Data Type)  
  이는 rows, cols 크기의 Data Type의 행렬을 할당.  

  2. eye(rows, columns, Data Type)  
  대각 성분이 1인 행렬을 위의 크기만큼 만든다.  

  3. ones(rows, columns, Data Type)  
  1로 전부 채워진 행렬을 위의 크기 만큼 만든다.  

  4. zeros(rows, columns, Data Type)  
  0으로 전부 채워진 행렬을 위의 크기만큼 만든다.  
<br>

* 원소에 접근하는 방법  
1. Method at  
   기본적으로 원소에는 at 이라는 메소드로 접근한다.  
   다음과 같이 unsigned char data type의 A라는 행렬의 1channel 행렬의 y, x의 인덱스에 접근할 수 있다.  
   ``A.at<uchar>(y, x)``  
   예를 들어서 3channel의 행렬이라면 다음과 같이 접근할 수 있다.  
   ``A.at<Vec3b>(y, x)[0] // 0번째 Channel의 y,x 번째 원소``  

2. Method data  
   이는 영상을 해당 자료형의 포인터로 1차원 배열을 받아서 사용하는 것인데, 굳이 binary 파일로 저장하는 함수를 따로 만들고 싶다면 한번 구글링 해보는 것을 추천함.  

3. Method ptr 사용  
   이는 행벡터는 포인터로 꺼내는 방법이다.  
   Ex. ``Vec3b* row1 = Image.ptr<Vec3b>(y) // y 번째 행 백터를 꺼내서 포인터로 받음``  

실상, 속도를 고려하면 data >>>>>>> ptr > at 일 것이다. 하지만 사용 난이도는 data > ptr > at 일 것이다. 필요에 따라서 유동적으로 사용하자.  



더 자세한 것은 [Docs](https://docs.opencv.org/3.4/dc/d84/group__core__basic.html)를 참조하도록 하자.  
여기서 나온 것은 은근 자주 쓰이지만 의외로 정리하기는 짜증나는 내용이다. 그 외에 Point나 다른 객체들도 많지만 그렇게까지 중요한 핵은 아니고 그냥 느낌적인 느낌으로 알 수 있기 때문에 생략하도록 하겠다.  
