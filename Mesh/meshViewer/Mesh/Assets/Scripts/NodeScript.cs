using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static UnityEngine.ParticleSystem;
using UnityEngine.Rendering;
using System;
using System.Reflection;
using UnityEditor.MemoryProfiler;
using UnityEngine.iOS;
using static UnityEngine.GraphicsBuffer;
using Unity.VisualScripting;
using System.Linq;
using UnityEditor.Experimental.GraphView;

public class NodeScript : MonoBehaviour
{
    public int id;
    public List<GameObject> allNodes;
    public List<GameObject> connections;
    public LineRenderer lineRenderer_pref;
    public bool isReady = false;
    bool isfirst = true;
    public Dictionary<GameObject, LineRenderer> lineRenderers = new Dictionary<GameObject, LineRenderer>();

    Rigidbody rb;
    public List<Vector3> Forces;
    public LineRenderer lineRenderer;
    public int dicSize = 0;
    public bool isLocked = false;
    public InitiateMesh initiateMesh;

    void Start()
    {
        initiateMesh = GameObject.Find("Mesh").GetComponent<InitiateMesh>();
        print(initiateMesh);
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        dicSize = lineRenderers.Count;
       

        //keep current position in parameters
        Vector3 currentPosition = transform.position;
        currentPosition.x = Mathf.Clamp(currentPosition.x, -49f, 49f);
        currentPosition.y = Mathf.Clamp(currentPosition.y, -49f, 49f);
        currentPosition.z = -2f;
        
        transform.position = currentPosition;
        if (isReady)
        {
            if (isfirst)
                InitiateLine();
            UpdateLine();
        }
     
    }

     void FixedUpdate()
    {
        if (isLocked)
        {
            Vector3 currentPosition = transform.position;
            currentPosition.x = 0;
            currentPosition.y = 0;
            currentPosition.z = -2f;

        }
        else 
        {
            if(isReady)
                ApplyForces();
        }
    }

    float mass = 5f;
    //float desiredDistance = 5f;
    //float repulsiveForce = 50f;
    //float atractionForce = 20f;
    float gravity = 9f;
    Vector2 centerGravity = new Vector2(0, 0);
    void ApplyForces()
    {
        float desiredDistance = initiateMesh.desiredDistance;
        float repulsiveForce = initiateMesh.repulsiveForce;
        float atractionForce = initiateMesh.atractionForce;
        // Initialize variables
        Vector2 totalForce = Vector2.zero;
        Vector2 thisPosition = transform.position;
        for(int i = 0 ; i < allNodes.Count; i++) 
        {
            Vector2 nodePos = allNodes[i].transform.position;
            Vector2 direction = (nodePos - thisPosition).normalized;
            float distance = Vector2.Distance(transform.position, nodePos);
            if (allNodes[i].gameObject == gameObject)
                continue;

            // Repulsive Force (Node-Node)
            float repulsion = repulsiveForce / (distance * distance); // Inverse-square law
            //print(repulsion);
            Vector2 repulsionForce = -repulsion * direction;
            //print(repulsionForce);
            totalForce += repulsionForce;

            // Attractive Force (Connections)
            if (connections.Contains(allNodes[i]))
            {
                float attraction = atractionForce * (distance - desiredDistance);
                Vector2 attractionForce = attraction * direction.normalized;
                totalForce += attractionForce;
            }


        }
        rb.AddForce(totalForce, ForceMode.VelocityChange);
        // rb.velocity = totalForce;
    }
    public void InitiateLine()
    {
        print("initiating line");
        
        foreach (var connection in connections)
        {
            if (connection.GetComponent<NodeScript>().lineRenderers.ContainsKey(gameObject))
                continue;

            lineRenderer = Instantiate(lineRenderer_pref, transform.position, Quaternion.identity);

            lineRenderer.positionCount = 2;

            Vector3 vector3 = connection.GetComponent<Rigidbody>().position;
            vector3.z = -2;
            
            lineRenderer.SetPosition(0, transform.position); // Set the first point to the current position
            lineRenderer.SetPosition(1, vector3); // Set the second point to the connection position

            lineRenderers.Add(connection, lineRenderer);
        }
        isfirst = false;

    }
    public void UpdateLine()
    {
        if (lineRenderer == null)
            return;
        for(int i = 0; i< lineRenderers.Count; i++)
        {
            var item = lineRenderers.ElementAt(i);
            LineRenderer lineRenderer = item.Value;

            lineRenderer.SetPosition(0, transform.position);

            Vector3 vector3 = item.Key.transform.position;
            vector3.z = -2;
            lineRenderer.SetPosition(1, vector3);
        }

    }

}
