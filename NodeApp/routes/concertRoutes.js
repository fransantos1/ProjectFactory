const express = require('express');
const router = express.Router();
const User = require("../models/concertModel");
const utils = require("../config/utils");
const auth = require("../middleware/auth");
const tokenSize = 64;


//Get authenthicated user
router.get('/get',async function (req, res, next) {
    try {
        console.log("TEST");
        res.status(result.status).send("hai:3");
    } catch (err) {
        console.log(err);
        res.status(500).send(err);
    }
});

module.exports = router;