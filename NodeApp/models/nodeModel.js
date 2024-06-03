const bcrypt = require('bcrypt');
const pool = require("../config/database");
const auth = require("../config/utils");
const utils = require("../config/utils");
const saltRounds = 10;
const nodeApi_port = 80;
//! FIX THIS
const temperature_id = 1; 
const humidity_id = 2;

function dbNodetoNode(dbnode)  {
    let node = new Node();
    node.id = dbnode.node_id;
    node.token = dbnode.node_token;
    node.apiToken = dbnode.node_apitoken;
    node.ip = dbnode.node_ip;
    node.isEmergency = dbnode.node_isemergency;
    node.macaddress = dbnode.node_macaddress;
    node.isOnline = dbnode.node_isOnline;
    node.userToken = dbnode.node_usertoken;
    return node;
}

class Node {
    constructor(id, token, apiToken, ip,macaddress, isEmergency,isOnline, userToken) {
        this.id = id
        this.token = token
        this.apiToken = apiToken
        this.ip = ip
        this.macaddress = macaddress
        this.isEmergency = isEmergency
        this.isOnline = isOnline,
        this.userToken = userToken
    }
    export() {
    }
    static async saveData(node, datas) {
        try {
            /*    data_value DECIMAL,
                data_dataType_id DECIMAL, 
                */
            //if data is sensor data

            for (let data of datas) {
                let date = new Date(data.timestamp * 1000); // Convert seconds to milliseconds
                let dbResult = await pool.query(`
                INSERT INTO data (data_value, data_dataType_id, data_time, data_node_id)
                VALUES ($1, $2,$3,$4) RETURNING data_id`,
                [data.temperature, temperature_id, date,node.id]);
                dbResult = await pool.query(`
                INSERT INTO data (data_value, data_dataType_id, data_time, data_node_id)
                VALUES ($1, $2,$3,$4) RETURNING data_id`,
                [data.humidity, humidity_id, date,node.id]);
                let data_id = dbResult.rows[0].data_id;
            }
            //if data is connections data
            return { status: 200, result: { msg: "Saved successfully" } };
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }
    static async getLastData(node){
        try{
            console.log(node);
            let dbResult = await pool.query(`
                    WITH ranked_data AS (
                        SELECT
                            data_id,
                            data_value,
                            data_dataType_id,
                            data_time,
                            data_node_id,
                            ROW_NUMBER() OVER (PARTITION BY data_dataType_id ORDER BY data_time DESC) AS index
                        FROM
                            data
                        where data_node_id =$1
                    )
                    SELECT
                        data_value,
                        dataType_name,
                        data_time
                    FROM
                        ranked_data
                    
                        inner join datatype on datatype_id = data_dataType_id
                        
                    WHERE
                        index = 1;`,[node.id]);
            let dbvalues = dbResult.rows;
            if (!dbvalues.length)
                return {status:500};
            let result={}
            for(let value of dbvalues){
                if(value.datatype_name == "temperature"){
                    result.temperature = value.data_value;
                }else{
                    result.humidity = value.data_value;
                }

            }
            console.log(result);
            return{status: 200, result: result}
        }catch(err){
            console.log(err);
            return { status: 500 };
        }


    }
    static async saveConn(node,connections){
        try{
            if(!connections)
            return {
                status: 404, result: [{
                    msg: "No connections Provided"
                }]
            };
            //every node connection needs to be deleted before putting the new list, and the opposite also deletes, because, if a node doesnt detect the other
            //? not sure If I should delete both sides of the connection, of just the connections of the node that's inserting (or nodeconnection_node_id1 = (select node_id from node where node_macaddress = $1))

            let nodeMac = utils.normalizeMAC(node.macaddress);
            let dbResult = await pool.query(`
                delete from nodeconnection 
                where nodeconnection_node_id = (select node_id from node where node_macaddress = $1)`,[nodeMac]);
            
            for(let conn of connections){
                let insertMac = utils.normalizeMAC(conn);
                //avoid repeated connections on the database so we search for the oposite connection
                dbResult = await pool.query(`
                    select * from nodeconnection 
                    where nodeconnection_node_id = (select node_id from node where node_macaddress = $1) 
                    and nodeconnection_node_id1 = (select node_id from node where node_macaddress = $2)`,[insertMac,nodeMac]);
                let rows = dbResult.rows;
                if(rows.length != 0)
                    continue;
                let result = await this.getNodeByMAC(insertMac);
                if(result.status != 200)
                    return {status: 500}
                let insertid = result.result.node.id;;
                dbResult = await pool.query(`INSERT INTO nodeConnection (nodeConnection_node_id, nodeConnection_node_id1) 
                    VALUES ($1,$2);`,[node.id,insertid]);
            }

            return { status: 200, result: { msg: "Inserted successfully"} };
        }catch(err){
            console.log(err);
            return { status: 500};
        }

    }
    static async ResetNodeUser(macaddress){
        macaddress = utils.normalizeMAC(macaddress);
        let result = await this.getNodeByMAC(macaddress);
        if(result.status != 200){
            return { status: 404, result: { msg: "no node Found" } };
        }
        let node = result.result.node;
        const data = {
            token: node.apiToken,
            userToken: utils.genToken(15)
        };
        try{
            result = await fetch('http://' + node.ip + ':' + nodeApi_port + '/reset', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            });  
            const responseBodyJson = await result.json();
            if (result.status === 403) { //if its not possible
            
            return { status: result.status, result: { msg: responseBodyJson.msg } };
            } else if(result.status != 200){
                console.log(responseBodyJson);
                return { status: 500};
                // handle other status codes or successful response
            
            }
            console.log(result.body);
            let dbResult = await pool.query(`UPDATE node SET node_usertoken = $1 WHERE node_macaddress = $2;`,[data.userToken, macaddress]);
            console.log(data.userToken);


            return { status: 200 };

            
        }catch(err){
            console.log(err);
            return { status: 500};
        }
    }
    
    //! Experimental way to controll led
    static async AmbientLed(node){
        let result;
        const data = {
            token: node.apiToken
        };
        try{
            result = await fetch('http://' + node.ip + ':' + nodeApi_port + '/setLed', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            });  
            const responseBodyJson = await result.json();
            if (result.status === 403) { //if its not possible
            
            return { status: result.status, result: { msg: responseBodyJson.msg } };
            } else if(result.status != 200){
                console.log(responseBodyJson);
                return { status: 500};
                
                // handle other status codes or successful response
            
            }
            console.log(result.body);
            return { status: 200 };

            
        }catch(err){
            console.log(err);
            return { status: 500};
        }
    }
    static async ControlLed(value, macaddress, activate){   
        macaddress = utils.normalizeMAC(macaddress);
        let result = await this.getNodeByMAC(macaddress);
        if(result.status != 200){
            return { status: 404, result: { msg: "no node Found" } };
        }
        let node = result.result.node;
        const data = {
            token: node.apiToken,
            value: value,
            activate: activate};
        try{
            result = await fetch('http://' + node.ip + ':' + nodeApi_port + '/setRGB', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            });  

            if (result.status === 403) { //if its not possible
               const responseBodyJson = await result.json();
               return { status: result.status, result: { msg: responseBodyJson.msg } };
            } else if(result.status != 200){
                
                return { status: 500};
                // handle other status codes or successful response
               
            }
            return { status: 200 };

            
        }catch(err){
            console.log(err);
            return { status: 500};
        }

    }
    static async LedShow_node(){
        /*
        send node:
        {
            token: apiToken,
            value: [
                values:{
                    value : {
                                r:  ,
                                g:  ,
                                b:  ,
                            }, 
                    duration: duration(ms)}
            ],
            time: epoch_startTime
        }
        */
    }
    static async BlinkNode_Find(node){      
        let result; 
        const data = {
            token: node.apiToken
        };
        try{
            result = await fetch('http://' + node.ip + ':' + nodeApi_port + '/locate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(data)
            });  
            const responseBodyJson = await result.json();
            if (result.status === 403) { //if its not possible
            return { status: result.status, result: { msg: responseBodyJson.msg } };
            } else if(result.status != 200){
                return { status: 500};
                
                // handle other status codes or successful response
            
            }
            console.log(result.body);
            return { status: 200 };

            
        }catch(err){
            console.log(err);
            return { status: 500};
        }


    }
    static async GetEmergencyNode(){

    }
    static async toggleEmergency(node,isEmergency){
        try{
            
            let dbResult = await pool.query(`
            UPDATE node SET node_isemergency = $1 WHERE node_id = $2`,[isEmergency,node.id]);
            //change status of emergency on db
            return { status: 200, result: { msg:"success"} };
        }catch(err){
            console.log(err);
            return { status: 500};
        }
    }

    static async SendMeshData(data_type, interval){
        /*
        SELECT  d.data_node_id, data_value
        FROM data d
        JOIN (
            SELECT data_node_id, MAX(data_time) AS max_time
            FROM data
            WHERE data_time <= to_timestamp(1704067200+10)
            GROUP BY data_node_id
        ) AS sub
        ON d.data_node_id = sub.data_node_id AND d.data_time = sub.max_time
        where d.data_dataType_id =1
        will return:
        data :[
            {
                id: node_id,
                data: data_id
            }
        ]
         
         */
        
    }
    static async SendMesh(){
        try{
            let data = ["temperature", "humidity"];
            let interval = 60 //in seconds, forcing interval because it will make it easier
            let connections = {nodes:[]}
            let mesh ={};
            //will return something like:
            /*

                mesh :{
                    beginning: beginning_date,
                    end: end_date,
                    interval: data_interval,
                    data_types: [
                        temperature, humidity, ...
                    ],
                    nodes: [{
                        id: id,
                        conn:[],
                        state: node_state,
                        n_occupants: n_occupants,
                        },{...}
                    ]
                }
            */

            //get all nodes id of nodes that have connections
            let dbResult = await pool.query(`
                select * from node 
                where
                EXISTS(
                    select * 
                    from nodeconnection 
                    where nodeconnection_node_id = node_id
                    or nodeconnection_node_id1 = node_id
                )`);
            let nodes = dbResult.rows
            //foreach node onde nodes

            for(let node of nodes){
                let insert = {"id":node.node_id}
                //get all associated connections
                dbResult = await pool.query(`
                    select nodeconnection_node_id1 as id from nodeconnection 
                    where nodeconnection_node_id = $1`,[node.node_id]);
                let rows = dbResult.rows;
                insert.connections = rows
                connections.nodes.push(insert)
            }
            //get min and max date
            dbResult = await pool.query(`select min(data_time) as beginning, max(data_time) as end from data`);
            //if(dbResult.rows.length)
            mesh.beginning = dbResult.rows[0].beginning;
            mesh.end = dbResult.rows[0].end;

            dbResult = await pool.query(`select min(data_time) as beginning, max(data_time) as end from data`);


            
            return { status: 200, result: { connections} };
        }catch(err){
            console.log(err);
            return { status: 500};
        }
    }
    /*
    SELECT 
        MIN(EXTRACT(EPOCH FROM (t1.data_time - t2.data_time))) AS min_time_difference
    FROM 
        data t1
    JOIN 
        data t2 ON t1.data_node_id = t2.data_node_id AND t1.data_time > t2.data_time;

     */
    static async RegisterNode(in_node) {
        try {
            if(!in_node.macaddress)
                return {
                    status: 404, result: [{
                        location: "body", param: "MAc",
                        msg: "No MacAddress Provided"
                    }]
                };
                if(!in_node.ip)
                return {
                    status: 404, result: [{
                        location: "body", param: "MAc",
                        msg: "No MacAddress Provided"
                    }]
                };
                if(!in_node.apiToken)
                return {
                    status: 404, result: [{
                        location: "body", param: "MAc",
                        msg: "No MacAddress Provided"
                    }]
                };
            let dbResult = await pool.query(`Select * from node where node_macaddress = $1`,[in_node.MACAddress]);
            if (!dbResult.rows.length){
                //create the node
                dbResult = await pool.query(`
                INSERT INTO node (node_MACAddress, node_ApiToken, node_ip)
                VALUES ($1, $2, $3) RETURNING node_id`, [in_node.macaddress,in_node.apiToken, in_node.ip]);
            }
            dbResult = dbResult.rows[0];
            let id = dbResult.node_id;
            let node = new Node();
            node.id = id;
            return { status: 200, result: { msg: "Registered successfully", node : node} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    //ID, token
    static async saveNodeToken(node) {
        try {
            if(!node.id || !node.token)
                return { status: 400, result: { msg: "Bad Data"}};
            
            let dbResult =
                await pool.query(`Update node set node_token=$1 where node_id = $2`,
                [node.token,node.id]);
            
            return { status: 200, result: {msg:"Token saved!"}} ;
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }






    static async getNodeByToken(token) {
        try {
            let dbResult = await pool.query(`Select * from node where node_token = $1`,[token]);
            let dbnode = dbResult.rows[0];
            if(!dbnode){
                return { status: 404, result: { msg: "no node Found" } };
            }
            let node = dbNodetoNode(dbnode);
            return { status: 200, result: { msg: "Saved successfully",node:node}};
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    static async getNodeByid(id) {
        try {
            let dbResult = await pool.query(`Select * from node where node_id = $1`,[id]);
            let dbnode = dbResult.rows[0];
            if(!dbnode){
                return { status: 404, result: { msg: "no node Found" } };
            }
             
            return { status: 200, result: { msg: "Found Node",node: dbNodetoNode(dbnode)} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }

    static async getNodeByUser(token) {
        try {
            let dbResult = await pool.query(`Select * from node where node_usertoken= $1`,[token]);
            let dbnode = dbResult.rows[0];
            if(!dbnode){
                return { status: 404, result: { msg: "no node Found" } };
            }
             
            return { status: 200, result: { msg: "Found Node",node: dbNodetoNode(dbnode)} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }

    static async getNodeByMAC(macaddress) {
        try {
            let dbResult = await pool.query(`Select * from node where node_macaddress = $1`,[macaddress]);
            let dbnode = dbResult.rows[0];
            if(!dbnode){
                return { status: 404, result: { msg: "no node Found" } };
            }
             
            return { status: 200, result: { msg: "Found Node",node: dbNodetoNode(dbnode)} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    static async updateNodeInfo(new_node, old_node){
        let dbResult
        //verify apiToken
        if(new_node.apiToken != old_node.apiToken){
            dbResult = await pool.query(`UPDATE node SET node_apiToken = $1 WHERE node_id = $2;`,[new_node.apiToken, old_node.id]);
        }
        //verify ip
        if(new_node.ip != old_node.ip){
            dbResult = await pool.query(`UPDATE node SET node_ip = $1 WHERE node_id = $2;`,[new_node.ip, old_node.id]);
        }
        //verify userToken
        if(!new_node.userToken)
            return;
        if(new_node.userToken != old_node.userToken){
            dbResult = await pool.query(`UPDATE node SET node_usertoken = $1 WHERE node_id = $2;`,[new_node.userToken, old_node.id]);
        }
    }
    //Send desired interval between data saves
    static async AuthNode(new_node){
        try {
            //in node has: macaddress, ApiToken, ip, token.


            new_node.macaddress = utils.normalizeMAC(new_node.macaddress);

            if(!new_node.macaddress || !new_node.apiToken || !new_node.ip){
                return { status: 400, result: { msg: "Bad Data"}};
            }

            //find node by token 
            let result = await this.getNodeByToken(new_node.token);
            if(result.status != 404){
                if(result.result.node.macaddress != new_node.macaddress)
                    return { status: 401, result: { msg: "Access Denied"}};
                
                //the node has all the correct information so just return with authenthicated
                //User Token

                let response = {msg:"Authenthicated"}
                if(result.result.node.userToken == null){
                    let usrtoken = utils.genToken(15);
                    while(1){
                        let tempresult = await this.getNodeByUser(usrtoken);
                        if(tempresult.status == 404)
                            break;
                        usrtoken = utils.genToken(15);    
                    }
                    new_node.userToken = usrtoken;
                    response.userToken = usrtoken;
                }

                await this.updateNodeInfo(new_node, result.result.node);
                return { status: 200, result:response};
            }
            let response = { msg: "Saved successfully"}

            result = await this.getNodeByMAC(new_node.macaddress);
            if(result.status != 200){
                result = await this.RegisterNode(new_node);
                if(result.status != 200){
                    return result;
                }
            }
            if(result.result.node.userToken == null){
                let usrtoken = utils.genToken(15);
                while(1){
                    let tempresult = await this.getNodeByUser(usrtoken);
                    if(tempresult.status == 404)
                        break;
                    usrtoken = utils.genToken(15);    
                }
                new_node.userToken = usrtoken;
                response.userToken = usrtoken;
            }
            await this.updateNodeInfo(new_node, result.result.node);
            let token = utils.genToken(62);
            response.token = token;


            new_node.id = result.result.node.id;
            
            new_node.token = token;
            console.log(new_node);
            result = await this.saveNodeToken(new_node);
            if(result.status != 200){
                return { status: 500, result: { msg: "error"}};
            }


        return { status: 200, result: response};
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    
}      
module.exports = Node;
