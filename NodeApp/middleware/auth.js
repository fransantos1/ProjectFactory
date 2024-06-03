const User = require("../models/usersModel");
const Node = require("../models/nodeModel");
const refreshPer =  1440e3; // 1440e3 milliseconds = 1 hour
module.exports.verifyAuth = async function (req, res, next) {
    try {
        let token = req.session.token;
        if (!token) {
            res.status(401).send({msg: "Please log in." });
            return;
        }
        let result = await User.getUserByToken(token);
        if (result.status != 200) {
            res.status(401).send(result.result);
            return;
        }
         req.user = result.result;
        next()
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
} 
module.exports.ApiKey = async function (req, res, next) {
    let APIKey = req.headers.api_key;
    if (!APIKey) {
        res.status(401).send({msg: "Not Authorized" });
        return;
    }
    if(APIKey != process.env.API_KEY){
        res.status(401).send({msg: "Not Authorized" });
        return;
    }
    next()
} 
module.exports.AdminKey = async function (req, res, next) {
    let AdminKey = req.headers.adminkey;
    if (!AdminKey) {
        res.status(401).send({msg: "Not Authorized" });
        return;
    }
    if(AdminKey != "IEIJjGZz7zmzkjV"){
        res.status(401).send({msg: "Not Authorized" });
        return;
    }
    next()
} 
//verify the node token
module.exports.NodeAuth = async function (req, res, next) {
    try {
        let token = req.headers.auth_token;
        if (!token) {
            res.status(401).send({msg: "No Token Given" });
            return;
        }
        let result = await Node.getNodeByToken(token);
        if (result.status != 200) {
            res.status(401).send(result.result);
            return;
        }
        req.node = result.result.node;
        next()
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
} 
module.exports.UserToken = async function (req, res, next) {
    try {
        let token = req.headers.usertoken;
        if (!token) {
            res.status(401).send({msg: "No Token Given" });
            return;
        }
        let result = await Node.getNodeByUser(token);
        if (result.status != 200) {
            res.status(401).send(result.result);
            return;
        }
        req.node = result.result.node;
        next()
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
} 
//verify apikey to save a node or get the token


