const pool = require("../config/database");
module.exports.genToken = function genToken(length) {
   let token = '';
   let characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
   for (let i = 0; i < length; i++) {
       token += characters.charAt(Math.floor(Math.random() * characters.length));
   }
   return token;
}
module.exports.normalizeMAC =function normalizeMAC(mac) {
    // Remove non-hex characters and convert to lowercase
    const normalizedMAC = mac.toLowerCase().replace(/[^0-9a-f]/g, '');
    // Insert colons every two characters
    return normalizedMAC.replace(/(.{2})(?=.)/g, '$1:');
}
