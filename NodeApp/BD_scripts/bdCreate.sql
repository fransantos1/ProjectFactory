create table node (
    node_id SERIAL not null,
    node_MACAddress macaddr not null,
    node_token VARCHAR(255), 
    primary key (node_id)
);

create table data(
    data_id SERIAL not null,
    data_temperature DECIMAL,
    data_Humidity DECIMAL,
    data_time timestamp,
    data_node_id int not null,
    primary key(data_id)
);
alter table data add constraint node_fk_data
foreign key (data_node_id) references node(node_id) 
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