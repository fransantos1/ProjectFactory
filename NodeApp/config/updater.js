let olddate;
let ctr = 0;
//! VERIFY IF CARS SERVICES ARE NEARBY
//! VERIFY RENTS 
async function verify() {
    if(ctr == 0){
        olddate= new Date();
        //
    }else{
        let newdate = new Date();
        if(newdate.getDay() - olddate.getDay() === 1){
            //
            console.log("changed day");
        }    
    }
    olddate = new Date();
    ctr++;
}
module.exports = {verify};