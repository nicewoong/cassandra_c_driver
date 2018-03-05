

## Introduction

필요에 의해 Ubuntu 에서 카산드라 C/C++ 드라이버 사용을 시도해 봤습니다. 
	한글로 따라하기가 없어서 혼자 시행착오를 겪으며 초간단 가이드를 작성해봤습니다. 
	샘플 코드, 그리고 설치 파일들도 함께 모아놨습니다. 
	자세한 시행착오 기록은 저의 블로그에서 확인할 수 있습니다. 
 
 
* [[시행착오 기록] Cassandra(카산드라) C 드라이버 설치 (클릭)](https://nicewoong.github.io/development/2018/02/24/cassandra-c-driver/)



<br><br>


## 1. Download and Install Cassandra C/C++ Driver


### 참고 링크
* [공식 설명 문서(v2.8) - DataStax C/C++ Driver for Apache Cassandra](https://docs.datastax.com/en/developer/cpp-driver/2.8/topics/)
* [cpp-drivere 우분투 v2.8 다운로드 서버](http://downloads.datastax.com/cpp-driver/ubuntu/16.04/cassandra/v2.8.1/)
* [cpp-driver 우분투 디펜던시 다운로드 서버(최신거)](http://downloads.datastax.com/cpp-driver/ubuntu/16.04/dependencies/)  


<br>


* `cpp-driver` 다운로드 서버와 `dependancies` 다운로드 서버에 접속하면 `.deb` 파일이 각 세 개씩 올라가 있다. 
* 현재Repository에서도해당파일확인가능


<br><br>


### A) .deb 파일 모두 다운받기. 

* 현재 Repository를 `Clone`하면 설치파일이 모두 포함되어 있다. 

* 때문에 별도로 다운받을 필요가없다. 하지만 버전 최신화가 안 될 수 있으니 직접 다운로드 받는 것을 추천 드린다.

* `wget` 명령어를 이용해서 다운로드 서버에서 단운로드 한다. 
  * url 은 위 다운로드 서버에서 해당 파일을 오른쪽 클릭해서 링크를 얻어왔다.     

        $ sudo wget http://downloads.datastax.com/cpp-driver/ubuntu/16.04/cassandra/v2.8.1/cassandra-cpp-driver_2.8.1-1_amd64.deb



* Cassandra-cpp-driver패키지와 cpp-driver-dependancies 에 `...-dbg` 랑 `...-dev` 파일 등을 모두 다운로드 한다. 
* 그러면 총 여섯개의 파일을 다운로드한다.

        $ ls -l
        -rw-r--r-- 1 root root  419554 Feb 15 02:48 cassandra-cpp-driver_2.8.1-1_amd64.deb
        -rw-r--r-- 1 root root 8651010 Feb 15 02:48 cassandra-cpp-driver-dbg_2.8.1-1_amd64.deb
        -rw-r--r-- 1 root root  535830 Feb 15 02:48 cassandra-cpp-driver-dev_2.8.1-1_amd64.deb
        -rw-r--r-- 1 root root   60990 Feb 15 02:48 libuv_1.18.0-1_amd64.deb
        -rw-r--r-- 1 root root  197746 Feb 15 02:48 libuv-dbg_1.18.0-1_amd64.deb
        -rw-r--r-- 1 root root   80032 Feb 15 02:48 libuv-dev_1.18.0-1_amd64.deb


<br><br>
<br><br>


#### (참고) `.deb` 파일이란?

* `.deb`은 데비안의 소프트웨어 패키지 포맷의 확장자
* 패키지 관리 시스템 
* 데비안 소프트웨어 포맷의 `바이너리 패키지`에서 가장 자주 사용되는 파일 이름이다.
* 데비안 패키지는 데비안 기반의 `GNU`/리눅스 배포판 (`우분투` 등)에서 동작한다.


##### deb 파일 설치

        # dpkg -i [deb파일]


        


<br>

### B) 다운로드 받은 .deb 파일을 모두 설치한다.
* `dpkg`를 통해 설치를 시도했는데 아래와 같은 오류가 발생

        # dpkg -i cassandra-cpp-driver_2.8.1-1_amd64.deb 
        Selecting previously unselected package cassandra-cpp-driver.
        (Reading database ... 5073 files and directories currently installed.)
        Preparing to unpack cassandra-cpp-driver_2.8.1-1_amd64.deb ...
        Unpacking cassandra-cpp-driver (2.8.1-1) ...
        dpkg: dependency problems prevent configuration of cassandra-cpp-driver:
         cassandra-cpp-driver depends on libuv; however:
          Package libuv is not installed.

* libuv 가 필요하다고 한다. 

* 그렇다. libuv 파일을 `dpkg`를 통해 설치하고나서 cpp-driver를 설치하니깐 에러 없이 순식간에 끝이났다. 

        # dpkg -i cassandra-cpp-driver_2.8.1-1_amd64.deb 
        (Reading database ... 5083 files and directories currently installed.)
        Preparing to unpack cassandra-cpp-driver_2.8.1-1_amd64.deb ...
        Unpacking cassandra-cpp-driver (2.8.1-1) over (2.8.1-1) ...
        Setting up cassandra-cpp-driver (2.8.1-1) ...
        Processing triggers for libc-bin (2.23-0ubuntu10) ...

        


<br><br>

## 2. 테스트 코드 작성해보기 `sample.c`

* `#include <cassandra.h>` 를 포함시켜서 샘플코드를 작성해 봤다. 

* Repository 의 `sample.c` 또는 `sample_modularized.c` 참고.

* `sample_modularized.c`는 `sample.c` 를 코드를 함수로 구성해 모듈화한 것이다.




<br><br>
## 3. 샘플코드 컴파일 및 실행하기


* `sample.c`를 만들어서 `#include <cassandra.h>` 를 포함시켜서 컴파일 해봤다. 

          # gcc sample.c -o test // error occur!




*  그냥 컴파일하면 오류가 발생한다. 


* 아래와 같이 Cassandra C/C++ Driver library 를 설정하며 컴파일 해야 한다. 

          # gcc sample.c -o test -L/usr/lib/ -lcassandra




* test 실행파일을 실행하면 아래와 같이 출력이 된다. 근데 connect result에 아무것도 출력 안 된다..;

        # ./test 
        Connect result:
        Query result:



* 그렇다. 이것은 성공한 것이었다. 에러 메시지가 없다는 것이다. 

* 쿼리하는 코드를 추가해서 실행하고, 카산드라서버에서 확인해 보니 잘 적용이 됐다.





-----


* Installation 설명에 아래와 같이 적혀있는데, 무슨 뜻이지!!!????

> Note: CentOS and Ubuntu use the version of OpenSSL provided with the distribution.









