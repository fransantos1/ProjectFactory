--! Only user with an history is the driver and with one car. that is the only history yet



INSERT INTO usrtype VALUES (1,'Driver');
INSERT INTO usrtype VALUES (2,'Owner');
INSERT INTO usrtype VALUES (3,'Mechanic');

INSERT INTO rentstate VALUES (1,'Scheduled');
INSERT INTO rentstate VALUES (2,'Started');
INSERT INTO rentstate VALUES (3,'Finished');

INSERT INTO repairtype VALUES (1,'Paint job');
INSERT INTO repairtype VALUES (2,'Mechanical');
INSERT INTO repairtype VALUES (3,'body Work');

INSERT INTO carstate VALUES (1,'available');
INSERT INTO carstate VALUES (2,'rented');
INSERT INTO carstate VALUES (3,'workshop');
INSERT INTO carstate VALUES (4,'unavailable');

--password:1234
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id) VALUES ('driver','123456', 'driver','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id) VALUES ('admin','234512541', 'admin','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'2');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id) VALUES ('mech','234512541', 'mech','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'3');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id)  VALUES ('rent0','234512541', 'rent0','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id)  VALUES ('rent1','234512541', 'rent1','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id)  VALUES ('rent2','234512541', 'rent2','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id)  VALUES ('rent3','234512541', 'rent3','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
INSERT INTO usr (usr_name, usr_phone, usr_email, usr_pass,usr_token, usr_usrtype_id)  VALUES ('rent4','234512541', 'rent4','$2b$10$Tp7f5YzlN4f3bH/.4hWbU.jmb7wPxmQ0luFpV/io.FnBxaMzIj3Zi',null,'1');
--car & images
INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('54-23-AV','Volkswagen', 'Passat','2001','130hp','1.9TDI','Diesel','5M','FWD','5','5','495L','AC;Heated Seats;Bluetooth Radio','120','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210455838408764/Passa_image1.jpg?width=1246&height=701',1);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210456354295868/Passat_image2.jpg?width=1246&height=701',1);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2023-12-04',1);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2023-12-25',1);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 1);

INSERT INTO rent(rent_data_inicio, rent_data_final,rent_price, rent_car_id, rent_usr_id, rent_rentstate_id) VALUES ('2023-08-3','2023-08-12','(1080€)',1,4,1);


INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('23-GA-21','Audi', 'A4','2001','130hp','1.9TDI','Diesel','5M','FWD','5','5','490L','AC;Heated Seats;Bluetooth Radio','250','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210454517203024/Audi_image1.jpg?width=1246&height=701',2);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210455263776858/Audi_image2.jpg?width=1246&height=701',2);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2024-04-13',2);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2024-03-25',2);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 2);

INSERT INTO rent(rent_data_inicio, rent_data_final,rent_price, rent_car_id, rent_usr_id, rent_rentstate_id) VALUES ('2023-07-20','2023-08-5','(4000€)',2,5,1);

INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('61-PO-41','Volkswagen', 'Pollo','2001','75hp','1.4TDI','Diesel','5m','FWD','5','5','100L','AC;Heated Seats;Bluetooth Radio','80','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210454223597578/Polo_image2.jpg?width=1246&height=701',3);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1094210456916344882/Polo_image1.jpg?width=1246&height=701',3);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2023-11-21',3);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2023-11-10',3);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 3);

INSERT INTO rent(rent_data_inicio, rent_data_final,rent_price, rent_car_id, rent_usr_id, rent_rentstate_id) VALUES ('2023-07-20','2023-08-12','(1840€)',3,6,1);

INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('23-45-AB','Audi', 'A1','2019','95hp','25TFSI','Petrol','5m','FWD','5','5','335L','AC;Heated Seats;Bluetooth Radio; Led Headlights;Cruise Controll','120','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095432056999268352/Audi_A1_2.jpg?width=1246&height=701',4);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095432057632608316/Audi_A1_1.jpg?width=1246&height=701',4);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2023-10-04',4);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2023-10-25',4);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 4);

INSERT INTO rent(rent_data_inicio, rent_data_final,rent_price, rent_car_id, rent_usr_id, rent_rentstate_id) VALUES ('2023-07-25','2023-07-31','(840€)',4,7,1);

INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('85-IT-23','Ford', 'Smax','2012','140hp','2.0TDCi','Diesel','6m','FWD','5','5','465L','AC;Heated Seats;Bluetooth Radio; Led Headlights;Cruise Controll','80','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095442367668035644/ford_Smax2.jpg?width=1246&height=701',5);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095442368116838511/ford_Smax1.jpg?width=1246&height=701',5);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2023-09-12',5);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2023-09-13',5);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 5);


INSERT INTO car (car_licenseplate, car_brand, car_model, car_year, car_bhp, car_engine, car_fuel, car_gearbox, car_drivetrain, car_doors, car_seats, car_bootcapacity, car_equi_ext, car_priceday, car_carstate_id, car_usr_id)
VALUES ('72-PO-08','Opel', 'Astra','2004','100hp','1.7CDTI','Diesel','5m','FWD','5','5','380L','AC;Heated Seats;Bluetooth Radio; Led Headlights;Cruise Controll','60','1','2');
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095444619552108635/opel_astra2.jpg?width=1246&height=701',6);
INSERT INTO carimage (carimage_link, carimage_car_id) VALUES ('https://media.discordapp.net/attachments/1094210400469397614/1095444620047032440/opel_astra1.jpg?width=1246&height=701',6);

INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('inspection','2023-07-25',6);
INSERT INTO carservices(carservices_type, carservices_due,carservices_car_id) VALUES ('insurance','2023-07-25',6);
insert into carloc(cl_geom, cl_car_id) values ('SRID=4326;POINT(0 0)', 6);


--FINISHED RENTS
-- uma rent tem vários pontos em cada dia
--create rent
INSERT INTO rent(rent_data_inicio, rent_data_final, rent_price, rent_car_id, rent_usr_id, rent_rentstate_id) VALUES ('2023-02-01', '2023-02-04','(360€)', 1, 1, 3);
--create routes
--day 1
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.152453257636921 38.70662814386117)'),'2023-02-01 12:00:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.152539197374665 38.707008514815186)'),'2023-02-01 12:01:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.152652617217 38.707408539012874)'),'2023-02-01 12:01:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.152747250394526 38.70771066011443)'),'2023-02-01 12:02:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.152880173936012 38.70826114122471)'),'2023-02-01 12:02:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.15297086394284 38.70861528673814)'),'2023-02-01 12:03:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.153063043764348 38.70899307988074)'),'2023-02-01 12:03:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.15332221774878 38.708839634720846)'),'2023-02-01 12:04:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.153741973528156 38.70864297238526)'),'2023-02-01 12:04:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.154215886813716 38.708411318770175)'),'2023-02-01 12:05:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.154587678947252 38.70822931836551)'),'2023-02-01 12:05:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.15498364123195 38.7080443316226)'),'2023-02-01 12:06:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.155431547150357 38.707977169405325)'),'2023-02-01 12:06:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.155874402881807 38.70782044858868)'),'2023-02-01 12:07:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.15633494240845 38.70746474983682)'),'2023-02-01 12:07:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.156464853319079 38.707570724068376)'),'2023-02-01 12:08:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.156577731247864 38.70775871331148)'),'2023-02-01 12:08:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.1562501513163 38.70801001607839)'),'2023-02-01 12:09:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.156312513436944 38.70812677030847)'),'2023-02-01 12:09:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES
(ST_GeographyFromText('SRID=4326;POINT(-9.156312513436944 38.70812677030847)'),'2023-02-01 12:10:00',5);
--day 2
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161583771021242 38.70942419075206)'),'2023-02-02 10:00:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161197099863301 38.709610525434925)'),'2023-02-02 10:00:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160818982213215 38.70981188817885)'),'2023-02-02 10:01:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160421401837795 38.710015247239056)'),'2023-02-02 10:01:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160209892974564 38.709973788381575)'),'2023-02-02 10:02:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159948609182237 38.7096520724229)'),'2023-02-02 10:02:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159720346460006 38.709339484158704)'),'2023-02-02 10:03:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159528920452829 38.70911816493941)'),'2023-02-02 10:03:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.15932195345303 38.708853941842165)'),'2023-02-02 10:04:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159245850306462 38.70876783229983)'),'2023-02-02 10:04:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159143111059791 38.70865202964811)'),'2023-02-02 10:05:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.15893763256517 38.70841151584946)'),'2023-02-02 10:05:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158699333616767 38.70814456755616)'),'2023-02-02 10:06:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158633299218735 38.70802635299259)'),'2023-02-02 10:06:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158549785126155 38.70784145289761)'),'2023-02-02 10:07:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158446849153194 38.70762624071037)'),'2023-02-02 10:07:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158343078925583 38.707401641147726)'),'2023-02-02 10:08:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158239678067304 38.707173918989554)'),'2023-02-02 10:08:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158169018691979 38.70701746669147)'),'2023-02-02 10:09:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158077351102861 38.70689979164814)'),'2023-02-02 10:09:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158040987043734 38.70680898557774)'),'2023-02-02 10:10:00',5);
-----estacionado
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158007584759332 38.70678062956921)'),'2023-02-02 10:30:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158121523532657 38.70670874254688)'),'2023-02-02 10:30:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158407640758725 38.70652810417147)'),'2023-02-02 10:31:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.158717546342444 38.70633717944679)'),'2023-02-02 10:31:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.15903475886239 38.70613656335786)'),'2023-02-02 10:32:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.159485826223118 38.70591122241228)'),'2023-02-02 10:32:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160160481178224 38.70558807328899)'),'2023-02-02 10:33:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160625760456753 38.70536658821507)'),'2023-02-02 10:33:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161016595180001 38.70520682798761)'),'2023-02-02 10:34:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161239929234256 38.705475515309814)'),'2023-02-02 10:34:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161360901967868 38.705667952862285)'),'2023-02-02 10:35:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160700205391947 38.70589669876594)'),'2023-02-02 10:35:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.160960761787578 38.70636145009516)'),'2023-02-02 10:36:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161142220706694 38.7066809648814)'),'2023-02-02 10:36:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161325267071476 38.707213386664165)'),'2023-02-02 10:37:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161512430016785 38.707810959742886)'),'2023-02-02 10:37:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161656278880372 38.708220199697195)'),'2023-02-02 10:38:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161707459600876 38.708394476135595)'),'2023-02-02 10:38:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.161791209871268 38.708648628514425)'),'2023-02-02 10:39:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.16191218248369 38.70904800899754)'),'2023-02-02 10:39:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES  (ST_GeographyFromText('SRID=4326;POINT(-9.16161044629311 38.70942069323948)'),'2023-02-02 10:40:00',5);
--day3
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161621840634893 38.709397099540894)'),'2023-02-03 12:00:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161419632814727 38.70948942247722)'),'2023-02-03 12:00:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161058240115864 38.70966735361898)'),'2023-02-03 12:01:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.160772137561338 38.709810033931745)'),'2023-02-03 12:01:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.160483883861104 38.70996278547989)'),'2023-02-03 12:02:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.160130589761309 38.710145661488184)'),'2023-02-03 12:02:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.160156403525605 38.71031519810529)'),'2023-02-03 12:03:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.160517796225918 38.71039073409213)'),'2023-02-03 12:03:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161077094450661 38.71040584127971)'),'2023-02-03 12:04:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161503021562396 38.710253090678236)'),'2023-02-03 12:04:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161995634229953 38.71037562690083)'),'2023-02-03 12:05:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.16248669657682 38.70999630384176)'),'2023-02-03 12:05:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.162605009664048 38.70988887426583)'),'2023-02-03 12:06:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.162635125722659 38.709722693822556)'),'2023-02-03 12:06:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.16248024313677 38.7095900849086)'),'2023-02-03 12:07:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.16210379240789 38.70930640172301)'),'2023-02-03 12:07:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.162050013732141 38.70924093621275)'),'2023-02-03 12:08:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161970421292096 38.70918722087717)'),'2023-02-03 12:08:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161847805911975 38.70926443665951)'),'2023-02-03 12:09:00',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.161656353826146 38.7093852959764)'),'2023-02-03 12:09:30',5);
INSERT INTO rentroute (rr_geom,rr_time, rr_rent_id) VALUES(ST_GeographyFromText('SRID=4326;POINT(-9.16161044629311 38.70942069323948)'),'2023-02-03 12:10:00',5);











INSERT INTO Allowed_map (am_geom) VALUES
(ST_GeographyFromText(
    'SRID=4326;
    POLYGON((
        -9.1600575423017 38.70357496264694,
        -9.1561866290659 38.70540004468239,
        -9.152795382859352 38.705767006961594,
        -9.151932385393167 38.70589354992566,
        -9.152170786746723 38.70718591383758,
        -9.15339017549087 38.71125897200574,
        -9.154040982939847 38.712281110618534,
        -9.156257337535209 38.71276133477909,
        -9.16227635838672 38.70914837832751,
        -9.161536324998451 38.70714615258041,
        -9.162433107613538 38.70704050063898,
        -9.161991353661875 38.70584524902077,
        -9.1600575423017 38.70357496264694
    ))'));

--nostopZones
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.158268835834804 38.70661055116773,
        -9.157437537153044 38.70704827819546,
        -9.156028456963753 38.70751325296894,
        -9.15451664938064 38.707918482236664,
        -9.152939100841394 38.70835676110465
        )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.158694513051842 38.710876117883544,
        -9.157700242064692 38.710238662572806,
        -9.156492245211922 38.70956065533886
    )'));

Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.157122773424874 38.709950951288846,
        -9.15696052463204 38.7101126118512,
        -9.156956049450258 38.71018460704542,
        -9.15708968419682 38.71026871535122,
        -9.156751185428163 38.71068807820453,
        -9.156647245499585 38.71067899498988,
        -9.155722735687135 38.70988223367726,
        -9.15576699056058 38.70957253353714
    )'));
    Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.157684746451906 38.71020222391397,
        -9.15868188397792 38.709642415549496,
        -9.159841929772938 38.709018669455816,
        -9.161417039945917 38.70821755096358,
        -9.16158615810076 38.70813966676934
    )'));
        Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.162238228812441 38.70691249482118,
        -9.161855975434435 38.70597212594501,
        -9.161690140043476 38.705548728260226,
        -9.161352671329865 38.70503765837745,
        -9.159694272263778 38.7058170245341,
        -9.159315092782663 38.70600756448192,
        -9.159136091523067 38.70607032229199,
        -9.158264572655668 38.70661253792659
    )'));
Insert into no_stopzones(NS_geom)VALUES
    (ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
        -9.162238228812441 38.70691249482118,
        -9.161855975434435 38.70597212594501,
        -9.161690140043476 38.705548728260226,
        -9.161352671329865 38.70503765837745,
        -9.159694272263778 38.7058170245341,
        -9.159315092782663 38.70600756448192,
        -9.159136091523067 38.70607032229199,
        -9.158264572655668 38.70661253792659
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.160290048596181 38.71006732375022,
		-9.158672808971545 38.70811336637854
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.15648342505088 38.70955467615099,
		-9.155756025590023 38.70957259771683,
		-9.15518908624847 38.7095224291146,
		-9.154778371082955 38.70943651099978
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.158662078312432 38.708093572528014,
		-9.158297534458626 38.70730153166619,
		-9.158145325266474 38.70698819095779,
		-9.157892051315457 38.706670279217434,
		-9.157732493201877 38.706561604570965,
		-9.156942838946634 38.706529581688585,
		-9.15655829708615 38.70655474056446,
		-9.15607029401113 38.70662572024057,
		-9.155808042746571 38.70668533982601,
		-9.153890280887651 38.707671958792304,
		-9.153775040528643 38.70767427616988
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.155295477635178 38.7068999903301,
		-9.155073094093467 38.706593025339146,
		-9.15497020417601 38.706519601200654,
		-9.154875908146863 38.70642846792498
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.15517067663086 38.70680610657175,
		-9.155023046140059 38.706681361103534,
		-9.1549515283005 38.70660936512098,
		-9.15495343602629 38.706511453540315
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.15526575256635 38.70679136258423,
		-9.155273807618272 38.706658379311364,
		-9.155364365023928 38.70654808292173
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.152892319341134 38.70661359378022,
		-9.153498058114565 38.70662431244685,
		-9.153904631078603 38.706642534176865,
		-9.154430703529016 38.70665218097295,
		-9.154966215994563 38.70661040144719,
		-9.155488167772802 38.70652036457986,
		-9.155993636862405 38.70637994971415,
		-9.156465152259983 38.706176402091614,
		-9.15692254684464 38.70596309898539,
		-9.157565258999938 38.705649588932744,
		-9.158022653583743 38.70542985294864,
		-9.158484521428647 38.7052171358591,
		-9.158943289570715 38.70500275800026,
		-9.159402088124438 38.70478783170984,
		-9.159845747135279 38.70455415840888,
		-9.159944643261696 38.704503779202724,
		-9.159568288558376 38.704625975514205,
		-9.159108146859865 38.704840354502636,
		-9.15819181476715 38.705276111566235,
		-9.15663799491736 38.70599539816732,
		-9.15618109709061 38.70621319032881,
		-9.155689363573998 38.70637289919077,
		-9.155163069893547 38.70648066733909,
		-9.154640968243314 38.70654741640331,
		-9.153566846426145 38.70655813507986,
		-9.153020826091392 38.70654990699495,
		-9.152691172337398 38.70654454765665
    )'));
Insert into no_stopzones(NS_geom)VALUES
(ST_GeographyFromText(
    'SRID=4326;
    LINESTRING(
		-9.154775348461158 38.70943655996646,
		-9.155091371097598 38.70927733701356,
		-9.155993390244078 38.7088258402772,
		-9.156214854925281 38.70871224735839,
		-9.156462447462815 38.708622926304145,
		-9.156953899987712 38.70845399270067,
		-9.157250012729662 38.70839379874579,
		-9.157718365608247 38.70830755920338,
		-9.157800481725928 38.70829979211524,
		-9.157896283864034 38.70829785034306,
		-9.157992086023285 38.70828619970317,
		-9.158087549480427 38.70827291608305,
		-9.158293292707157 38.7082096317213,
		-9.158660462003695 38.70808363873951,
		-9.158881926685723 38.708012763810444,
		-9.159151914526035 38.70789431514024,
		-9.160105235231754 38.70748190846962,
		-9.161078220349054 38.707058344255785,
		-9.161197661991253 38.70702630444737,
		-9.161268933610557 38.70702242083482,
		-9.162237614217588 38.706907348164066
    )'));