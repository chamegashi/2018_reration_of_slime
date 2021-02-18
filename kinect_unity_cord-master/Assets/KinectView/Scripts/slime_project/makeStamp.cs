using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class makeStamp : MonoBehaviour {

    // 宣言

    private int z = 1;
    public GameObject pointer;

    void Start () {

        // index の取得

        IndexDatas indexDatasObj = GetComponent<IndexDatas>();

        // x, y の取得

        for(int i = 0; i < 4; i++)
        {
            Instantiate(pointer, new Vector3(indexDatasObj.indexes_X[i], indexDatasObj.indexes_Y[i], z), Quaternion.identity);
        }
    }
}