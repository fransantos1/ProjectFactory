const bcrypt = require('bcrypt');
const pool = require("../config/database");
const auth = require("../config/utils");
const utils = require("../config/utils");
const saltRounds = 10;

//! FIX THIS
const temperature_id = 1; 
const humidity_id = 2;


class Node {
    constructor() {

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
                [data.temperature, temperature_id, date,node.node_id]);
                dbResult = await pool.query(`
                INSERT INTO data (data_value, data_dataType_id, data_time, data_node_id)
                VALUES ($1, $2,$3,$4) RETURNING data_id`,
                [data.humidity, humidity_id, date,node.node_id]);
                let data_id = dbResult.rows[0].data_id;
            }
            //if data is connections data
            return { status: 200, result: { msg: "Saved successfully" } };
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
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

            let nodeMac = utils.normalizeMAC(node.node_macaddress);
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
                let insertid = result.result.node.node_id;;
                dbResult = await pool.query(`INSERT INTO nodeConnection (nodeConnection_node_id, nodeConnection_node_id1) 
                    VALUES ($1,$2);`,[node.node_id,insertid]);
            }

            return { status: 200, result: { msg: "Inserted successfully"} };
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
    static async RegisterNode(MACAddress) {
        try {
            if(!MACAddress)
                return {
                    status: 404, result: [{
                        location: "body", param: "MAc",
                        msg: "No MacAddress Provided"
                    }]
                };
            let dbResult = await pool.query(`Select * from node where node_macaddress = $1`,[MACAddress]);
            if (!dbResult.rows.length){
                //create the node
                dbResult = await pool.query(`
                INSERT INTO node (node_MACAddress)
                VALUES ($1) RETURNING node_id`, [MACAddress]);
            }
            dbResult = dbResult.rows[0];
            let id = dbResult.node_id;
            //generate a token
            let token = utils.genToken();
            //save the token on the database
            let node = {id : id, token: token};
            let response = await this.saveNodeToken(node);
            if(response.status != 200)
                return {status: 500}
            return { status: 200, result: { msg: "Registered successfully", token : token} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    //ID, token
    static async saveNodeToken(node) {
        try {
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
            let node = dbResult.rows[0];
            if(!node){
                return { status: 404, result: { msg: "no node Found" } };
            }
            return { status: 200, result: { msg: "Saved successfully",node:node}};
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    static async getNodeByMAC(macaddress) {
        try {
            let dbResult = await pool.query(`Select * from node where node_macaddress = $1`,[macaddress]);
            let node = dbResult.rows[0];
            if(!node){
                return { status: 404, result: { msg: "no node Found" } };
            }
            return { status: 200, result: { msg: "Found Node",node:node} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    //Send desired interval between data saves
    static async AuthNode(macaddress, token){
        try {
            macaddress = utils.normalizeMAC(macaddress);
            if(!macaddress){
                return { status: 400, result: { msg: "Bad Data"}};
            }
            let result = await this.getNodeByToken(token);
            if(result.status != 404){
                if(result.result.node.node_macaddress == macaddress){
                    return { status: 200, result: { msg: "Authenthicated"}};
                }
                return { status: 401, result: { msg: "Access Denied"}};
            }
            result = await this.getNodeByMAC(macaddress);
            if(result.status == 200){
                token = utils.genToken();
                let node = {id : result.result.node.node_id, token: token};
                result = await this.saveNodeToken(node);
                if(result.status != 200){
                    return { status: 500, result: { msg: "error"}};
                }
            }else{
                result = await this.RegisterNode(macaddress);
                token = result.result.token;
            }   
            return { status: 200, result: { msg: "Saved successfully",token:token} };
        } catch (err) {
            console.log(err);
            return { status: 500};
        }
    }
    
}      
module.exports = Node;
