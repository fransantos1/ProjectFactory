const express = require('express');
const router = express.Router();
const node = require("../models/nodeModel");
const utils = require("../config/utils");
const auth = require("../middleware/auth");

router.post('/data',auth.NodeAuth,async function (req, res, next) {
    try {
        node.saveData(req.node,req.body);//! NO errors have been implemented here yet
        res.status(200).send("test");
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.post('/connections',auth.NodeAuth,async function (req, res, next) {
    try {
        let result = await node.saveConn(req.node,req.body.connections);
        res.status(result.status).send({result: result.result});  
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.get('/mesh',auth.ApiKey,async function (req, res, next) {
    try {
        let result = await node.SendMesh();
        res.status(result.status).send({result: result.result.connections});  
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.get('/authenticate',auth.ApiKey, async function (req, res, next) {
    try {
        let result = await node.AuthNode(req.headers.macaddress,req.headers.auth_token);
        res.status(result.status).send({result: result.result});    
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
module.exports = router;