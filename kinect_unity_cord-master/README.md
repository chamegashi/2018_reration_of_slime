## スライム Kinect Unity のリポジトリ

Unity でキネクト動かす用のリポジトリ

### とりあえず最初これやっとけ

ここで Unity で kinect 使えるようになる... はず  
[Unity で kinect を動かす説明してくれてる Quita](https://qiita.com/yuzupon/items/4528f7fc09a42fb8af2e)

ここで、Unity と arduino を通信するうえで突っかかると思われることの解説してる  
[おもちゃラボ の シリアル通信ではまるとこの解説してくれてる](http://nn-hokuson.hatenablog.com/entry/2018/02/01/203114)

### 使用シーン

**MainScene**  
... 基本的な現段階のシーン。現状動いているのはこれだけ  
キャリブレーション用

**GameScene**  
... これをゲーム？プロジェクション？用にする

### 使用スプリクト

#### Unity

現状使っているスクリプトの場所は、

> **Assets/KinectView/Scripts/slime_project**

の中にある。

以下使用スクリプトのざっくりとした説明  
随時追加予定

**UseDepthPoint**  
... depth data の処理を行う。手とかの判定もここで行っている。  
... また、半径の取得なども現状はここで行っている。  
... 取り回しがきつくなってきたため、クラス分けなどして軽量化したい...(・ω・)

**SerialHander**  
... Arduino とのシリアル通信をおこなうモジュール。これが基本。

**DoSomething**  
... Arduino とのシリアル通信を行う際に使用。どんなデータを送信するかなど決めてる。

**makeStamp**  
... どこを基準にして値をとっているかの場所の表示

**Indexdatas**  
... Index の x 座標とか y 座標とかの管理するやつ

#### Ardunino

**arduino_cord**  
... Unity とのシリアル通信を行う。

****