const bcrypt = require('bcrypt');
const pool = require("../config/database");
const auth = require("../config/utils");
const saltRounds = 10; 
const concertsData = {
    concerts: [
      { id: 1, band: "The Electric Tigers", time: "01:00" },
      { id: 2, band: "Lunar Groove", time: "02:00" },
      { id: 3, band: "Neon Shadows", time: "03:00" },
      { id: 4, band: "Galactic Funk", time: "04:00" },
      { id: 5, band: "Quantum Beats", time: "05:00" },
      { id: 6, band: "Cosmic Harmony", time: "06:00" },
      { id: 7, band: "Starlight Serenade", time: "07:00" },
      { id: 8, band: "Astral Echoes", time: "08:00" },
      { id: 9, band: "Celestial Melodies", time: "09:00" },
      { id: 10, band: "Nebula Rhythms", time: "10:00" },
      { id: 11, band: "Solar Symphony", time: "11:00" },
      { id: 12, band: "Interstellar Jam", time: "12:00" },
      { id: 13, band: "Galaxy Grooves", time: "13:00" },
      { id: 14, band: "Pluto Funk", time: "14:00" },
      { id: 15, band: "Mars Meltdown", time: "15:00" },
      { id: 16, band: "Saturn Serenade", time: "16:00" },
      { id: 17, band: "Jupiter Jam", time: "17:00" },
      { id: 18, band: "Uranus Uplift", time: "18:00" },
      { id: 19, band: "Mercury Mix", time: "19:00" },
      { id: 20, band: "Venus Vibes", time: "20:00" },
      { id: 21, band: "Earthly Echo", time: "21:00" },
      { id: 22, band: "Solar Salsa", time: "22:00" },
      { id: 23, band: "Astro Swing", time: "23:00" },
      { id: 24, band: "Cosmic Country", time: "24:00" },
    ],
  };
class Concert {
    constructor(id, band, play_time) {
        this.id = id;
        this.band = band;
        this.play_time = play_time
    }
    static async getNextConcert() {
        try {
            let time = new Date().getHours();
            let list = concertsData.concerts;
            for (let concert_list of list){
                let hours = concert_list.time.split(':')[0];
                if(hours > time){
                    delete concert_list["id"];
                    return { status: 200, result: {concert:concert_list}};
                }
            }
            return { status: 400, result: {msg:"No next concert"}};
            
            
        } catch (err) {
            console.log(err);
            return { status: 500, result: err };
        }
    }
}
module.exports = Concert;
