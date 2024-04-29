const express = require('express');
const router = express.Router();
const Node = require("../models/nodeModel");
const utils = require("../config/utils");
const auth = require("../middleware/auth");

router.post('/data',auth.NodeAuth,async function (req, res, next) {
    try {
        Node.saveData(req.node,req.body);//! NO errors have been implemented here yet
        res.status(200).send("test");
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.post('/connections',auth.NodeAuth,async function (req, res, next) {
    try {
        let result = await Node.saveConn(req.node,req.body.connections);
        res.status(result.status).send({result: result.result});  
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.get('/mesh',auth.ApiKey,async function (req, res, next) {
    try {
        let result = await Node.SendMesh();
        res.status(result.status).send({result: result.result.connections});  
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.get('/authenticate',auth.ApiKey, async function (req, res, next) {
    try {
        let node = new Node();
        node.apiToken = req.headers.apitoken
        node.macaddress = req.headers.macaddress
        node.ip = req.headers.ip
        node.token = req.headers.auth_token;
        let result = await Node.AuthNode(node);
        res.status(result.status).send({result: result.result});    
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
router.post('/Led',auth.ApiKey,async function (req, res, next) {
    try {
        let result = await Node.ControlLed(req.body.value,req.body.macaddress, req.body.activate);
        res.status(result.status).send({result: result.result});  
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});
module.exports = router;