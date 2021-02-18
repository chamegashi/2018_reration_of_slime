using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DoSomething : MonoBehaviour {

    // 宣言

    public SerialHander serialHander;

    public delegate void SerialDataReceivedEventHandler(string message);
    public event SerialDataReceivedEventHandler OnDataReceived;

    public string portName;
    public int baudRate = 9600;

    private string message_;
    private bool isNewMessageReceived_ = false;

    void Start () {
        serialHander.OnDataReceived += OnDatareceived;
    }

    void Update () {

        UseDepthProject useDepthProjectObj = GetComponent<UseDepthProject>();

        if (Input.GetKeyDown(KeyCode.N))
        {
            serialHander.Write("\0");
            Debug.Log("Null Data send.");
        }
        if (useDepthProjectObj.swiches[0] == true)
        { 
            serialHander.Write("1");
        }
        if (useDepthProjectObj.swiches[1] == true)
        {
            serialHander.Write("2");
        }
        if (useDepthProjectObj.swiches[2] == true)
        {
            serialHander.Write("3");
        }
        if (useDepthProjectObj.swiches[3] == true)
        {
            serialHander.Write("4");
        }
        if (useDepthProjectObj.swiches[0] == false && useDepthProjectObj.swiches[1] == false && useDepthProjectObj.swiches[2] == false && useDepthProjectObj.swiches[3] == false)
        {
            serialHander.Write("\0");
        }
        if (useDepthProjectObj.UnsetDataSwitch == true)
        {
            serialHander.Write("\0");
        }
    }

    void OnDatareceived(string message)
    {
        var data = message.Split(new string[] { "\t" }, System.StringSplitOptions.None);

        if(data.Length < 2)
        {
            return;
        }

        try
        {
            
        } catch (System.Exception e)
        {
            Debug.LogWarning(e.Message);
        }
    }
}
