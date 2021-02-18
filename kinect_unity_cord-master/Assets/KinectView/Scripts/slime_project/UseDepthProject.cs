using UnityEngine;
using System.Collections;
using Windows.Kinect;

public class UseDepthProject : MonoBehaviour
{
    // 変数の宣言

    // game object の入手

    public GameObject DepthSourceManager;
    private DepthSourceManager _DepthManager;

    // kinect の設定

    private KinectSensor kinect = null;
    private DepthFrameReader depthFrameReader;

    // 表示サイズ

    private int depthWidth = 512;
    private int depthHeight = 424;
    private int depthBufferMax = 217088;

    // depth buffer

    private ushort[] depthBuffer;

    // depth 初期値

    private int[] firstPoints = new int[217088];

    // 受け渡し用の値

    // 0. rightHigh 1. rightLow 2. leftHigh 3. leftLow

    public bool[] swiches = new bool[4];
    public bool UnsetDataSwitch;

    // 手の判断の depth 距離

    public int maxchangedistance = 200;
    public int minchangedistance = 50;

    // オブジェクトの生成

    private int z = 2;
    public GameObject painter;

    void Start()
    {
        // kinect の取得

        kinect = KinectSensor.GetDefault();

        // depthBuffer の初期化
        depthBuffer = new ushort[kinect.DepthFrameSource.FrameDescription.LengthInPixels];

        // IndexData の取得
        IndexDatas indexDatasObj = GetComponent<IndexDatas>();

    }

    void Update()
    {
        //depthBuffer の更新

        if (DepthSourceManager == null)
        {
            return;
        }

        _DepthManager = DepthSourceManager.GetComponent<DepthSourceManager>();

        depthBuffer = _DepthManager.GetData();

        // 指の判定

        checkPoint();

        // 初期化キー待機

        if (Input.GetKey(KeyCode.I))
        {
            Invoke("setFirstPoint", 0.5f);
        }
        if (Input.GetKey(KeyCode.C))
        {
            // 球体の半径取得

            IndexDatas indexDatasObj = GetComponent<IndexDatas>();

            int radius = checkCircleLength(indexDatasObj.indexes_Y[3]);

            GameObject gameobject = Instantiate(painter, new Vector3(indexDatasObj.indexes_X[3], indexDatasObj.indexes_Y[3], 1), Quaternion.identity);
            gameobject.transform.localScale = new Vector3(radius, radius, 1);

        }
    }
    
    // 初期値の設定

    private void setFirstPoint()
    {
        // 初期値の set

        for(int i = 0; i < 217088; i++)
        {
            firstPoints[i] = depthBuffer[i];
        }

        // switch の初期化

        for(int i = 0; i < 4; i++)
        {
            swiches[i] = false;
        }

        Debug.Log("セットオワタ");
    }

    // 指の check

    private void checkPoint()
    {
        IndexDatas indexDatasObj = GetComponent<IndexDatas>();

        if (firstPoints[indexDatasObj.Indexes[0]] == 0 && firstPoints[indexDatasObj.Indexes[1]] == 0 && firstPoints[indexDatasObj.Indexes[2]] == 0 && firstPoints[indexDatasObj.Indexes[3]] == 0)
        {
            Debug.LogWarning("今すぐセットやれえぇindexDatasObj.Indexes[0] (push 'i')");
            UnsetDataSwitch = true;
        }
        else
        {
            UnsetDataSwitch = false;
        }

        // 判定

        if(maxchangedistance > Mathf.Abs(firstPoints[indexDatasObj.Indexes[0]] - depthBuffer[indexDatasObj.Indexes[0]]) && Mathf.Abs(firstPoints[indexDatasObj.Indexes[0]] - depthBuffer[indexDatasObj.Indexes[0]]) > minchangedistance && UnsetDataSwitch == false)
        {
            Debug.Log("rightHigh changing");
            swiches[0] = true;
        }
        else
        {
            swiches[0] = false;
        }
        if (maxchangedistance > Mathf.Abs(firstPoints[indexDatasObj.Indexes[1]] - depthBuffer[indexDatasObj.Indexes[1]]) && Mathf.Abs(firstPoints[indexDatasObj.Indexes[1]] - depthBuffer[indexDatasObj.Indexes[1]]) > minchangedistance && UnsetDataSwitch == false)
        {
            Debug.Log("rightLow changing");
            swiches[1] = true;
        }
        else
        {
            swiches[1] = false;
        }
        if (maxchangedistance > Mathf.Abs(firstPoints[indexDatasObj.Indexes[2]] - depthBuffer[indexDatasObj.Indexes[2]]) && Mathf.Abs(firstPoints[indexDatasObj.Indexes[2]] - depthBuffer[indexDatasObj.Indexes[2]]) > minchangedistance && UnsetDataSwitch == false)
        {
            Debug.Log("leftHigh changing");
            swiches[2] = true;  
        }
        else
        {
            swiches[2] = false;
        }
        if (maxchangedistance > Mathf.Abs(firstPoints[indexDatasObj.Indexes[3]] - depthBuffer[indexDatasObj.Indexes[3]]) && Mathf.Abs(firstPoints[indexDatasObj.Indexes[3]] - depthBuffer[indexDatasObj.Indexes[3]]) > minchangedistance && UnsetDataSwitch == false)
        {
            Debug.Log("leftLow changing");
            swiches[3] = true;
        }
        else
        {
             swiches[3] = false;
        }
    }

   int checkCircleLength(int index)
    {
        int radius = 0;

        for(int tmp = index; firstPoints[tmp] - 5 > depthBuffer[tmp]; tmp++)
        {
            radius++;
        }
        return radius;
    }

    void OnApplicationQuit()
    {
        if (depthFrameReader != null)
        {
            depthFrameReader.Dispose();
            depthFrameReader = null;
        }

        if (kinect != null)
        {
            if (kinect.IsOpen)
            {
                kinect.Close();
            }

            kinect = null;
        }
    }
}
