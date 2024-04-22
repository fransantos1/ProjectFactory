const express = require('express');
const router = express.Router();
const Concert = require("../models/concertModel");
const utils = require("../config/utils");
const auth = require("../middleware/auth");
const tokenSize = 64;


//Get authenthicated user
router.get('/get',async function (req, res, next) {
    try {
        console.log(req.headers.auth_token); 
        let response = await Concert.getNextConcert();
        console.log(response);
        res.status(200).send(response.result);
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});


module.exports = router;