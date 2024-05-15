create table node (
    node_id SERIAL not null,
    node_MACAddress macaddr not null,
    node_token VARCHAR(255), 
    node_apiToken VARCHAR(255) not null,
    node_isEmergency boolean DEFAULT FALSE,
    node_isOnline boolean DEFAULT FALSE,
    node_ip inet not null,
    primary key (node_id)
);

create table data(
    data_id SERIAL not null,
    data_value DECIMAL,
    data_dataType_id int not null,
    data_time timestamp,
    data_node_id int not null,
    primary key(data_id)
);
alter table data add constraint node_fk_data
foreign key (data_node_id) references node(node_id) 
ON DELETE NO ACTION ON UPDATE NO ACTION;


create table dataType(
    dataType_id SERIAL not null,
    dataType_name VARCHAR(255),
    primary key(dataType_id)
);
INSERT INTO dataType (dataType_name) VALUES ('temperature');
INSERT INTO dataType (dataType_name) VALUES ('humidity');

alter table data add constraint data_fk_dataType
foreign key (data_dataType_id) references dataType(dataType_id) 
ON DELETE NO ACTION ON UPDATE NO ACTION;




create table nodeConnection (
    nodeConnection_id SERIAL not null,
    nodeConnection_node_id int not null,
    nodeConnection_node_id1 int not null,
    primary key(nodeConnection_id)
);

alter table nodeConnection add constraint node_fk_connection
foreign key (nodeConnection_node_id) references node(node_id) 
ON DELETE NO ACTION ON UPDATE NO ACTION;
alter table nodeConnection add constraint node_fk_connection1
foreign key (nodeConnection_node_id1) references node(node_id) 
ON DELETE NO ACTION ON UPDATE NO ACTION;