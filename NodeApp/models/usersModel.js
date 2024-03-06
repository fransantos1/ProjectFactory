const bcrypt = require('bcrypt');
const pool = require("../config/database");
const auth = require("../config/utils");
const saltRounds = 10; 

function dbUserToUser(dbUser)  {
    let user = new User();
    user.id = dbUser.usr_id;
    user.name = dbUser.usr_name;
    user.phone = dbUser.usr_phone;
    user.email = dbUser.usr_email;
    user.type = dbUser.usr_usrtype_id;
    return user;
}

class User {
    constructor(id, name, phone, email,pass, token, type) {
        this.id = id;
        this.name = name;
        this.phone = phone;
        this.email = email;
        this.pass = pass;
        this.token = token;
        this.type = type;
    }
    export() {
        let user=new User();
        user.name = this.name;
        return user; 
    }


    
    static async register(user) {
        try {
            let dbResult =
                await pool.query("Select * from usr where usr_email=$1", [user.email]);
            let dbUsers = dbResult.rows;
            if (dbUsers.length)
                return {
                    status: 400, result: [{
                        location: "body", param: "email",
                        msg: "That email is already registered"
                    }]
                };
                
            let encpass = await bcrypt.hash(user.pass,saltRounds);      
            dbResult = await pool.query(`Insert into usr (usr_name, usr_phone, usr_email, usr_pass, usr_usrtype_id) values ($1,$2,$3,$4,$5)`,
            [user.name, user.phone, user.email,encpass,user.type]);
            return { status: 200, result: {msg:"Registered! You can now log in."}} ;
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }
    
        static async getById(id) {
        try {
            let dbResult = await pool.query("Select * from usr where usr_id=$1", [id]);
            let dbUsers = dbResult.rows;
            if (!dbUsers.length) 
                return { status: 404, result:{msg: "No user found for that id."} } ;
            let dbUser = dbUsers[0];
            return { status: 200, result: 
                new User(dbUser.usr_id,dbUser.usr_name,dbUser.usr_phone, dbUser.usr_email , dbUser.usr_pass, dbUser.usr_token, dbUser.usr_usrtype_id)} ;
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }  
    }
    

    //login
    static async checkLogin(user) {
        try {
            let dbResult =
                await pool.query("Select * from usr where usr_email=$1", [user.email]);
            let dbUsers = dbResult.rows;
            if (!dbUsers.length)
                return { status: 401, result: { msg: "Wrong username or password!"}};

            let dbUser = dbUsers[0]; 

            let isPass = await bcrypt.compare(user.pass,dbUser.usr_pass);
            if (!isPass) 
                return { status: 401, result: { msg: "Wrong username or password!"}};
            return { status: 200, result: dbUserToUser(dbUser) } ;

        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }

    // No verifications. Only to use internally
    static async saveToken(user) {
        try {
            let dbResult =
                await pool.query(`Update usr set usr_token=$1 where usr_id = $2`,
                [user.token,user.id]);
            return { status: 200, result: {msg:"Token saved!"}} ;
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }
    static async getUserByToken(token) {
        try {
            let dbResult = await pool.query(`Select * from usr where usr_token = $1`,[token]);
            let dbUsers = dbResult.rows;
            if (!dbUsers.length)
                return { status: 403, result: {msg:"Invalid authentication!"}} ;
            let user = dbUserToUser(dbUsers[0]);
            return { status: 200, result: user} ;
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }
}

module.exports = User;
