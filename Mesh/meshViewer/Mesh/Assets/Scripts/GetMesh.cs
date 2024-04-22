    using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;


public class GetMesh
{
    private string ApiKey = "134a8fb6-3824-497a-a23c-1b89abe03a8a";
    private string proxyAddress = "http://localhost:8080";

    public IEnumerator ApiGetMesh(Action<string> callback)
    {
        using (UnityWebRequest req = UnityWebRequest.Get(proxyAddress + "/api/node/connections"))
            
        {
            req.SetRequestHeader("api_key", ApiKey);

            yield return req.SendWebRequest();
            callback(req.downloadHandler.text);
            if (req.result != UnityWebRequest.Result.Success)
            {
                Debug.Log(req.error);
            }
        }
    }
}