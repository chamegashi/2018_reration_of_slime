using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IndexDatas : MonoBehaviour {

    // Index の取得

    public int rightHighIndex = 0;
    public int rightLowIndex = 0;
    public int leftHighIndex = 0;
    public int leftLowIndex = 0;

    // 配列の宣言

    // 0. rightHigh 1. rightLow 2. leftHigh 3. leftLow

    public int[] Indexes = new int[4];
    public int[] indexes_X = new int[4];
    public int[] indexes_Y = new int[4];

    // Use this for initialization
    void Start () {

        Indexes[0] = rightHighIndex;
        Indexes[1] = rightLowIndex;
        Indexes[2] = leftHighIndex;
        Indexes[3] = leftLowIndex;

        for (int i = 0; i < 4; i++)
        {
            int x = Indexes[i];
            int y;
            for (y = 0; x > 512; y++)
            {
                x = x - 512;
            }

            indexes_X[i] = x;
            indexes_Y[i] = y;
        }
    }
}
