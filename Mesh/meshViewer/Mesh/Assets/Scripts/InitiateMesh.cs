using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Xml.Linq;
using UnityEngine;
using System.Linq;
using UnityEditor.Experimental.GraphView;
using Unity.VisualScripting;

public class InitiateMesh : MonoBehaviour
{
    public GameObject prefab;
    public GetMesh Apimesh = new();


    public float desiredDistance = 10f;
    public float repulsiveForce = 20f;
    public float atractionForce = 10f;



    void Start()
    {
        
        StartCoroutine(Apimesh.ApiGetMesh(GetData));
    }

    void GetData(string result)
    {
        JsonResult jobj = new JsonResult();
        jobj = JsonUtility.FromJson<JsonResult>(result);

        List<Node> nodes_list = jobj.result.nodes.ToList();

        List<GameObject> nodes_obj = new List<GameObject>();


        List<int> obj_id = new List<int>(); 

        foreach (var node in nodes_list)
        {

            Vector2 position = new Vector2(0,0);

            GameObject instantiatedObject = Instantiate(prefab, position, Quaternion.identity);



            instantiatedObject.name = "node ["+node.id+"]";

            NodeScript nodeScript = instantiatedObject.GetComponent<NodeScript>();

            if (nodeScript == null)
                return;
            nodeScript.id = node.id;
            nodes_obj.Add(instantiatedObject);
            obj_id.Add(node.id);
        }

        foreach (var node_Obj in nodes_obj)
        {
            NodeScript tempNodeScript = node_Obj.GetComponent<NodeScript>();
            Node nodeClass = nodes_list[nodes_obj.IndexOf(node_Obj)];

            foreach (var connection in nodeClass.connections)
            {
                GameObject conn_obj = nodes_obj[obj_id.IndexOf(connection.id)];
                float randomAngle = UnityEngine.Random.Range(0f, Mathf.PI * 2);
                float x = desiredDistance * Mathf.Sin(randomAngle);
                float y = desiredDistance * Mathf.Cos(randomAngle);
                conn_obj.transform.position = new Vector2(node_Obj.transform.position.x+x, node_Obj.transform.position.y+ y);
                tempNodeScript.connections.Add(nodes_obj[obj_id.IndexOf(connection.id)]);
                NodeScript connNodeScript = nodes_obj[obj_id.IndexOf(connection.id)].GetComponent<NodeScript>();
                connNodeScript.connections.Add(node_Obj);
            }
            if(node_Obj == nodes_obj[0])
            {
                //tempNodeScript.isLocked = true;
            }
            tempNodeScript.allNodes = nodes_obj;
            tempNodeScript.isReady = true;

        }
        
    }
}







[Serializable]
public class JsonResult
{
    public NodeList result;
}

[Serializable]
public class NodeList
{
    public Node[] nodes;
}

[Serializable]
public class Node
{
    public int id;
    public Connection[] connections;
}
[Serializable]
public class Connection
{
    public int id;
}